from pathlib import Path
import hashlib,json,re
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/HandbookLevels20261031'
rel='Server/script/phongthan/test/event_handbook.lua'
live=ROOT/'PhongThanRuntime-Loose'/rel
before=live.read_bytes();marker=b'\n-- Handbook selectable level milestones.'
data=before.split(marker,1)[0]
row=b'EBNodes[1][3][1]={"'+encode_tcvn3('Nh\u1eadn C\u1ea5p')+b'","EBChooseLevel",""}'
data,count=re.subn(rb'(?m)^EBNodes\[1\]\[3\]\[1\]=[^\r\n]+',lambda m:row,data)
assert count==1
append='''
-- Handbook selectable level milestones.
function EBChooseLevel()
 EBShow("Chọn cấp muốn nhận. Chỉ nâng cấp, không hạ cấp nhân vật.",{
 "50/EBReceiveLevel(50)","70/EBReceiveLevel(70)","90/EBReceiveLevel(90)",
 "150/EBReceiveLevel(150)","200/EBReceiveLevel(200)",
 EBLabels[3].."/EBMenu(1,1)"})
end
function EBReceiveLevel(level)
 local result=EventHandbookAction(2,level)
 local text=EBLabels[5]
 if result>0 then text=EBLabels[4] end
 EBShow(text,{"Chọn cấp khác/EBChooseLevel",EBLabels[3].."/EBMenu(1,1)"})
end
'''
data+=encode_tcvn3(append)
out=PACKAGE/'compiled'/rel;out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(data)
(PACKAGE/'manifest.json').write_text(json.dumps(dict(scope='LOOSE_ONLY',levels=[50,70,90,150,200],files=[dict(path=rel,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(before).hexdigest())]),indent=2),'utf8')
source=ROOT/'PhongThanSource/Sources/Core/Src/PhongThanEventHandbook.inl'
raw=source.read_bytes()
if b'static int PtHandbookLevel200(int player)' in raw:
 raw=raw.replace(b'static int PtHandbookLevel200(int player)',b'static int PtHandbookLevel(int player,int target)')
 start=raw.index(b'static int PtHandbookLevel(int player,int target)');end=raw.index(b'\n#endif',start)
 part=raw[start:end]
 part=part.replace(b'if(!PtHandbookAllowed(player))return 0;',b'if(!PtHandbookAllowed(player) || (target!=50 && target!=70 && target!=90 && target!=150 && target!=200))return 0;')
 part=part.replace(b'if(actor.m_Level>=200)return 1;',b'if(actor.m_Level>=target)return 1;')
 part=part.replace(b'while(actor.m_Level<200)owner.LevelUp();',b'while(actor.m_Level<target){int previous=actor.m_Level;owner.LevelUp();if(actor.m_Level<=previous)return 0;}')
 part=part.replace(b'return actor.m_Level==200;',b'return actor.m_Level==target;')
 raw=raw[:start]+part+raw[end:]
 needle=b'else if(op==2 && Lua_GetTopIndex(L)==1)result=PtHandbookLevel200(player);'
 assert raw.count(needle)==1
 raw=raw.replace(needle,b'''else if(op==2){
            if(Lua_GetTopIndex(L)==1)result=PtHandbookLevel(player,200);
            else if(Lua_GetTopIndex(L)==2 && Lua_IsNumber(L,2)){
                double target=Lua_ValueToNumber(L,2);
                if(target==50 || target==70 || target==90 || target==150 || target==200)result=PtHandbookLevel(player,(int)target);
            }
        }''')
 source.write_bytes(raw)
print('STAGED handbook level choices 50/70/90/150/200 and validated native level action')

start=raw.index(b'static int PtHandbookLevel(int player,int target)');end=raw.index(b'\n#endif',start)
(ROOT/'PhongThanSource/Output/Tools/HandbookLevelActual.inl').write_bytes(raw[start:end])

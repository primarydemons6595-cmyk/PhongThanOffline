"""Stage UI aliases and connected smith/roulette services; no live writes."""
import json,re,hashlib
from restore_original_world import ROOT,OUT,Original
import build_offline_loose as b
from stage_event_npc_services import ROULETTE_APPENDIX
from roulette_level_policy import apply_level_20

BATCH=OUT/'city_service_repair';server=ROOT/'PhongThanRuntime-Loose/Server'
stage=BATCH/'staged/Server';client_stage=BATCH/'staged/Client'
plan=json.loads((BATCH/'plan.json').read_text('utf8'))
ledger_path=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
ledger=json.loads(ledger_path.read_text('utf8'))
changes={f['relative']:(stage/f['relative']).read_bytes() for f in plan['files']}
metadata={}
smiths=[]
for npc in ledger['ready']+plan['ready']:
    relative=npc['script_relative']
    if npc['name']=='铜匠':
        payload=changes.get(relative) or (server/relative).read_bytes()
        payload+=f'''
-- Connected city smith: retain original shop/quest branch and offer crafting.
pt_smith_shop=main
function main()
    if pt_runtime_world_pos()~={npc['world']} then CloseDialog();return end
    Say("Tho Dong",3,"Mua vu khi, trang bi/pt_smith_shop","Ep vu khi, trang bi/pt_smith_compose","Thoat/no")
end
function pt_smith_compose()
    if pt_runtime_world_pos()~={npc['world']} then CloseDialog();return end
    CloseDialog()
    EnchaseItem()
end
'''.encode('ascii')
        changes[relative]=payload;npc['lua_sha256']=b.sha(payload)
        npc['business_status']='ORIGINAL_SHOP_AND_PROJECT_COMPOSE_SERVICE'
        smiths.append(npc)
    if npc['name']=='太岁师':
        payload=(server/relative).read_bytes()
        assert payload.endswith(ROULETTE_APPENDIX)
        payload=payload[:-len(ROULETTE_APPENDIX)]+b'''
-- Persist an unfinished spin so disconnect/restart does not consume its reward.
pt_native_Roulette=Roulette
pt_roulette_rolling=rolling
pt_roulette_main=main
pt_original_Finished=Finished
function Roulette(outcome)
    SetTask(4910,1)
    return pt_native_Roulette(outcome)
end
function rolling()
    if GetTask(4910)==1 then return Roulette(GetTask(72)) end
    return pt_roulette_rolling()
end
function main()
    if pt_runtime_world_pos()==1020 or pt_runtime_world_pos()==1021 then
        if GetTask(4910)==1 then return Roulette(GetTask(72)) end
        return pt_roulette_main()
    end
    CloseDialog()
end
function Finished()
    if ConsumeRouletteResult()~=1 or GetTask(4910)~=1 then return end
    SetTask(4910,0)
    return pt_original_Finished()
end
'''
        payload=apply_level_20(payload)
        changes[relative]=payload;npc['lua_sha256']=b.sha(payload)
        npc['business_status']='ORIGINAL_REWARDS_WITH_ROULETTE_UI_TOKEN_AND_PERSISTENT_RESUME'

# Preserve already restored Bào Thương scripts if the authored generator runs.
ledger.setdefault('service_overrides',[])
for world in (1002,1003,1004,1020,1021):
    authored=json.loads((ROOT/'PhongThanSource/Docs/NPC_RESTORATION_DEPLOYMENT.json').read_text('utf8'))['rows']
    npc=next(n for n in authored if n['world']==world and n['name']=='Thuong Diem')
    relative=npc['lua']['relative'];payload=(server/relative).read_bytes()
    if not any(n['script_relative']==relative for n in ledger['service_overrides']):
        ledger['service_overrides'].append(dict(script_relative=relative,lua_sha256=b.sha(payload)))
metadata['Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json']=json.dumps(ledger,ensure_ascii=False,indent=2).encode('utf8')

for relative,payload in changes.items():
    p=stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(payload)
plan['files']=[dict(relative=p,sha256=b.sha(v),before_sha256=b.sha((server/p).read_bytes()) if (server/p).exists() else None) for p,v in changes.items()]
plan['metadata']=[]
for relative,payload in metadata.items():
    p=BATCH/'staged/Project'/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(payload)
    plan['metadata'].append(dict(relative=relative,sha256=b.sha(payload),before_sha256=b.sha((ROOT/'PhongThanSource'/relative).read_bytes())))
(BATCH/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')

header=ROOT/'PhongThanSource/Sources/Core/Src/PhongThanXichTungTu.h'
text=header.read_text('ascii')
if 'PhongThanIsComposeService' in text:text=text[:text.index('inline bool PhongThanIsComposeService')]
else:text=text.replace('#endif','')
text+='inline bool PhongThanIsComposeService(int world, int npcTemplate, DWORD scriptId)\n{\n    if(PhongThanIsXichTungTu(world,npcTemplate,scriptId))return true;\n'
for npc in smiths:
    escaped=npc['lua'].replace('\\','\\\\')
    text+=f'    if(world=={npc["world"]} && npcTemplate=={npc["template"]} && scriptId==g_FileName2Id("{escaped}"))return true;\n'
text+='    return false;\n}\n#endif\n'
header.write_text(text,'ascii')
path=ROOT/'PhongThanSource/Sources/Core/Src/PhongThanComposeServer.inl'
payload=path.read_bytes().replace(b'PhongThanIsXichTungTu(reply.MapId',b'PhongThanIsComposeService(reply.MapId');path.write_bytes(payload)

client=ROOT/'PhongThanRuntime-Loose/Client'
client_files={}
for scheme in ('ui3','ui4'):
    folder=client/'ui'/scheme
    # Warehouse implementation expects this ASCII name, while original uses GBK.
    source=folder/'储物箱.ini'.encode('gbk').decode('latin1')
    if not source.exists():source=client/'ui/ui3'/'储物箱.ini'.encode('gbk').decode('latin1')
    if source.exists():
        payload=source.read_bytes()
        payload=payload.replace(b'[Main]',b'[Main]\r\nWidth=256\r\nHeight=430',1)
        payload+=b'\r\n[CurPage]\r\nLeft=103\r\nTop=31\r\nWidth=42\r\nHeight=16\r\nFont=14\r\nColor=255,217,78\r\n'
        # Native storage currently has page navigation rather than five tab widgets.
        for section,x,sprite in (('LeftBtn',34,'上一页'),('RightBtn',154,'下一页')):
            payload+=f'\r\n[{section}]\r\nLeft={x}\r\nTop=30\r\nWidth=55\r\nHeight=14\r\nUp=0\r\nDown=1\r\n'.encode()
            payload+=b'Image=\\spr\\Ui4\\'+ ('买卖/'+sprite+'.spr').replace('/','\\').encode('gbk')+b'\r\n'
        client_files[f'ui/{scheme}/UiStoreBox.ini']=payload
    source=folder/'太岁彩.ini'.encode('gbk').decode('latin1')
    if not source.exists():source=client/'ui/ui3'/'太岁彩.ini'.encode('gbk').decode('latin1')
    if source.exists():
        payload=source.read_bytes().replace(b'[Main]',b'[Main]\r\nWidth=380\r\nHeight=365\r\nMoveable=1',1)
        payload+=b'\r\n[CloseBtn]\r\nLeft=350\r\nTop=4\r\nWidth=20\r\nHeight=16\r\nUp=0\r\nDown=1\r\nOver=1\r\n'
        payload+=b'Image=\\spr\\Ui4\\'+'按钮/关闭键.spr'.replace('/','\\').encode('gbk')+b'\r\n'
        payload+=b'[Status]\r\nLeft=60\r\nTop=310\r\nWidth=280\r\nHeight=40\r\nFont=14\r\nColor=255,217,78\r\n'
        client_files[f'ui/{scheme}/UiRoulette.ini']=payload
    source=folder/'UiMsgSel.ini'
    if not source.exists():source=client/'ui/ui3/UiMsgSel.ini'
    if source.exists():
        payload=source.read_bytes().replace(b'MaxMsgCount=20',b'MaxMsgCount=21')
        payload+=b'\r\n[CloseBtn]\r\nLeft=319\r\nTop=2\r\nWidth=20\r\nHeight=16\r\nUp=0\r\nDown=1\r\nOver=1\r\n'
        payload+=b'Image=\\spr\\Ui4\\'+'按钮/关闭键.spr'.replace('/','\\').encode('gbk')+b'\r\n'
        client_files[f'ui/{scheme}/UiMsgSel.ini']=payload
        client_files[f'ui/{scheme}/UiMsgSelPic.ini']=payload
    # Preserve original nine-slot composition artwork with an explicit hit area.
    source=folder/'UiVngCompose.ini'
    if not source.exists():source=client/'ui/ui3/UiVngCompose.ini'
    if source.exists():client_files[f'ui/{scheme}/UiVngCompose.ini']=source.read_bytes().replace(b'[Main]',b'[Main]\r\nWidth=156\r\nHeight=335',1)
for relative,payload in client_files.items():
    p=client_stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(payload)
(BATCH/'client_plan.json').write_text(json.dumps([dict(relative=p,sha256=b.sha(v),before_sha256=b.sha((client/p).read_bytes()) if (client/p).exists() else None) for p,v in client_files.items()],indent=2),'utf8')
print('Staged city NPCs:',len(plan['ready']),'smith services:',len(smiths),'client UI files:',len(client_files))
fixtures=(OUT/'event_services/service_fixtures.txt').read_text('ascii').splitlines()
lines=[]
for line in fixtures:
    if '828485f4' in line:line=line.replace('event_services/staged/Server','city_service_repair/staged/Server')
    lines.append(line)
(BATCH/'service_ui_fixtures.txt').write_text('\n'.join(lines)+'\n','ascii')

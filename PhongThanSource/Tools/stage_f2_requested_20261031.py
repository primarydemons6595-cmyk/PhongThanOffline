from pathlib import Path
import json,sys,hashlib
R=Path.cwd();sys.path.insert(0,str(R/'PhongThanSource/Tools'));from extract_client_coordinate_dictionary import encode_tcvn3 as enc
P=R/'PhongThanSource/Deploy/ProjectContent/F2Requested20261031';P.mkdir(parents=True,exist_ok=True)
assert not (P/'publication.json').exists()
files=[]
def stage(role,rel,data):
 live=R/'PhongThanRuntime-Loose'/role/rel;out=P/'compiled'/role/rel;out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(data)
 snap=P/'source'/role/rel;snap.parent.mkdir(parents=True,exist_ok=True)
 if not snap.exists():snap.write_bytes(live.read_bytes() if live.exists() else b'')
 files.append(dict(path=role+'/'+rel,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(live.read_bytes()).hexdigest() if live.exists() else None))
spec={295:[(3,41,0,0,9)],213:[(3,3,0,0,100)],288:[(3,i,0,0,5) for i in range(15,22)],285:[(3,i,0,0,100) for i in range(8,14)],286:[(3,i,0,0,100) for i in range(22,26)]}
for detail,start,end in [(287,0,6),(448,6,10),(449,10,14),(1408,14,18)]:spec[detail]=[(0,4,i,1,1) for i in range(start,end)]
s='#ifndef PT_F2_GIFT_DATA_H\n#define PT_F2_GIFT_DATA_H\nstruct PTF2Reward{int genre,detail,part,level,count;};\nstruct PTF2Gift{int detail,count;PTF2Reward rewards[7];};\nstatic const PTF2Gift ptF2Gifts[]={\n'
for d,rewards in spec.items():s+='{%d,%d,{%s}},\n'%(d,len(rewards),','.join('{'+','.join(map(str,r))+'}' for r in rewards))
s+='};\nenum{PT_F2_GIFT_COUNT=sizeof(ptF2Gifts)/sizeof(ptF2Gifts[0])};\n'
for key,text in {'Success':'Đã nhận đủ vật phẩm trong túi.','Full':'Hành trang không đủ chỗ hoặc chưa thể mở túi. Vật phẩm được giữ nguyên.'}.items():s+='static const char ptF2Gift'+key+'[]="'+''.join('\\%03o'%c for c in enc(text))+'";\n'
s+='#endif\n';(R/'PhongThanSource/Headers/PhongThanF2GiftData.h').write_text(s,'ascii')
for role in ['Server','Client']:
 raw=(R/'PhongThanRuntime-Loose'/role/'settings/item/001/ibitem.txt').read_bytes()
 original=(R/'analysis/f2_requested_20261031/original/settings/item/001/ibitem.txt').read_bytes().splitlines()
 for identity in [[b'8',b'9444',b'2'],[b'8',b'9445',b'0']]:
  if not any(l.split(b'\t')[1:4]==identity for l in raw.splitlines()[1:]):
   row=next(l for l in original[1:] if l.split(b'\t')[1:4]==identity);raw=raw.rstrip(b'\r\n')+b'\r\n'+row+b'\r\n'
 lines=raw.splitlines(keepends=True)
 for i,line in enumerate(lines):
  c=line.rstrip(b'\r\n').split(b'\t')
  if len(c)<13 or c[1]!=b'8':continue
  d=int(c[2]);p=int(c[3]);changed=False
  if p==2 and d in spec:
   c[12]=b'\\script\\phongthan\\items\\f2_gifts.lua';changed=True
   if d==1408:c[8]=enc('#Mở nhận 4 Pháp Bảo cấp 70, mỗi loại 1 cái.')
  if p==2 and d==9444:c[12]=b'\\script\\phongthan\\items\\experience_lock.lua';changed=True
  if p==2 and d in [30,391]:c[12]=b'\\script\\phongthan\\items\\compass.lua';changed=True
  if changed:lines[i]=b'\t'.join(c)+(b'\r\n' if line.endswith(b'\r\n') else b'\n')
 stage(role,'settings/item/001/ibitem.txt',b''.join(lines))
 if role=='Server':
  stage(role,'script/phongthan/items/f2_gifts.lua',b'function main(item) return UseF2Gift(item) end\n')
  stage(role,'script/phongthan/items/experience_lock.lua',b'function main(item) return UseOriginalBuffItem(item) end\n')
 rel='settings/ibshop/ibshopgoods.txt';goods=(R/'PhongThanRuntime-Loose'/role/rel).read_bytes();rows=goods.splitlines()
 assert not any(l.split(b'\t')[1:4]==[b'8',b'9444',b'2'] for l in rows[1:])
 index=len(rows)-1
 new=[enc('H?a Th?n T?a'),b'8',b'9444',b'2',b'0',b'0',b'1',b'10',b'10']+[b'']*9
 special=[enc('T?i Ph?p B?o ??c bi?t'),b'8',b'1408',b'2',b'0',b'0',b'1',b'6',b'6']+[b'']*9
 assert not any(l.split(b'\t')[1:4]==[b'8',b'1408',b'2'] for l in rows[1:])
 stage(role,rel,goods.rstrip(b'\r\n')+b'\r\n'+b'\t'.join(new)+b'\r\n'+b'\t'.join(special)+b'\r\n')
 rel='settings/ibshop/micsgoods.txt';raw=(R/'PhongThanRuntime-Loose'/role/rel).read_bytes();stage(role,rel,raw.rstrip(b'\r\n')+b'\r\n'+str(index).encode()+b'\r\n'+str(index+1).encode()+b'\r\n')
 rel='settings/phongthan/ConsumableEffects.txt';raw=(R/'PhongThanRuntime-Loose'/role/rel).read_bytes()
 rows=[b'133\t0\t5\t3600\t111\t20\t0\t0\t1',b'330\t0\t5\t3600\t187\t100\t0\t0\t1',b'334\t0\t5\t86400\t181\t50\t187\t100\t1',b'782\t0\t5\t3600\t187\t100\t0\t0\t1']
 existing={l.split(b'\t')[0] for l in raw.splitlines()[1:]};assert not existing.intersection([b'133',b'330',b'334',b'782'])
 stage(role,rel,raw.rstrip(b'\r\n')+b'\r\n'+b'\r\n'.join(rows)+b'\r\n')
rel='Ui/ui3/PtNativeTimedItems.ini';raw=(R/'PhongThanRuntime-Loose/Client'/rel).read_bytes();table=(R/'PhongThanRuntime-Loose/Client/settings/item/001/ibitem.txt').read_bytes().splitlines()
for detail in [133,330,334,782]:
 c=next(l.split(b'\t') for l in table[1:] if l.split(b'\t')[1:4]==[b'8',str(detail).encode(),b'0']);raw+=b'\r\n[BuffInfo_'+str(detail).encode()+b']\r\nImage='+c[4]+b'\r\nTitle=$'+c[0].lstrip(b'#')+b'\r\nDesc=$'+c[8].lstrip(b'#')+b'\r\n'
stage('Client',rel,raw)
# Shared compass UI keeps labels from the original dictionary, full runtime IDs in native tasks.
import re
original=(R/'analysis/f2_requested_20261031/original/30.lua').read_bytes()
names=re.findall(rb'\[(\d+)\]\s*=\s*"([^"\r\n]*)"',re.split(rb'function\s+main',original)[0])
lua=b'PT_COMPASS_MAPS={'+b','.join(b'['+str(1000+int(k)).encode()+b']=\"'+name+b'\"' for k,name in names)+b'}\n'
text="""
function no() CloseDialog() end
function main(item)
 if TravelItem(0,item)~=12 then return end
 Say("Ch? Nam Ch?u: ch?n ch?c n?ng.",4,{"Ghi nh? v? tr?/pt_compass_save","Chuy?n ??n v? tr?/pt_compass_go","X?a v? tr?/pt_compass_clear","K?t th?c/no"})
end
function pt_compass_menu(action)
 local choices={}
 for i=1,5 do
  local ok,map,x,y=CompassItem(0,i)
  local label="V? tr? "..i
  if ok==1 then label=label..": "..(PT_COMPASS_MAPS[map] or tostring(map)).." ("..x..","..y..")" end
  tinsert(choices,label.."/pt_compass_"..action..i)
 end
 tinsert(choices,"K?t th?c/no")
 Say("Ch?n v? tr? ?? l?u:",getn(choices),choices)
end
function pt_compass_save() pt_compass_menu("save") end
function pt_compass_go() pt_compass_menu("go") end
function pt_compass_clear() pt_compass_menu("clear") end
function pt_compass_apply(op,slot)
 local ok=CompassItem(op,slot)
 if ok~=1 then Msg2Player("Kh?ng th? th?c hi?n t?i ??y. V? tr? v? l??t s? d?ng ???c gi? nguy?n.");return end
 if op==1 then Msg2Player("?? ghi nh? v? tr?. Kh?ng ti?u hao l??t s? d?ng.") end
 if op==3 then Msg2Player("?? x?a v? tr?. Kh?ng ti?u hao l??t s? d?ng.") end
 CloseDialog()
end
"""
for i in range(1,6):
 text+=f'function pt_compass_save{i}() local ok=CompassItem(0,{i});if ok==1 then MsgBox("Ghi ?? v? tr? ?? l?u?","pt_compass_confirm{i}","no") else pt_compass_apply(1,{i}) end end\n'
 text+=f'function pt_compass_confirm{i}() pt_compass_apply(1,{i}) end\nfunction pt_compass_go{i}() pt_compass_apply(2,{i}) end\nfunction pt_compass_clear{i}() pt_compass_apply(3,{i}) end\n'
stage('Server','script/phongthan/items/compass.lua',lua+enc(text))
rel='script/phongthan/travel_items.lua';raw=(R/'PhongThanRuntime-Loose/Server'/rel).read_bytes().replace(b'\r\n',b'\n')
raw=raw.replace(b' if GetMorphType()==364 or IsPlayerInsideWeapon(PlayerIndex)>0 then return end\n local profile=TravelItem(0,item)',b' if IsPlayerInsideWeapon(PlayerIndex)>0 then return end\n local profile=TravelItem(0,item)')
raw=raw.replace(b' if profile==0 then return end',b' if profile==0 or (GetMorphType()==364 and profile~=11) then return end')
raw=raw.replace(b' if GetMorphType()==364 or IsPlayerInsideWeapon(PlayerIndex)>0 then return end\n if TravelItem(1,option)',b' if IsPlayerInsideWeapon(PlayerIndex)>0 then return end\n if TravelItem(1,option)')
stage('Server',rel,raw)

rel='Ui/ui3/PtConsumableBuff.ini';raw=(R/'PhongThanRuntime-Loose/Client'/rel).read_bytes()
assert b'[BuffInfo_9445]' not in raw
c=next(l.split(b'\t') for l in table[1:] if l.split(b'\t')[1:4]==[b'8',b'9445',b'0'])
raw+=b'\r\n[BuffInfo_9445]\r\nImage='+c[4]+b'\r\nTitle=$'+enc('H?a Th?n T?a')+b'\r\nDesc=$'+enc('Kh?ng th? nh?n th?m kinh nghi?m nh?n v?t trong 7 ng?y.')+b'\r\n'
stage('Client',rel,raw)
(P/'manifest.json').write_text(json.dumps(dict(scope='LOOSE_ONLY',files=files,gifts=spec),indent=2),'utf8')
print('Staged',len(files),'files, nine atomic gift packs and four timed consumable identities')

from pathlib import Path
import sys,re,json,hashlib,struct
R=Path(__file__).resolve().parents[2];sys.path.insert(0,str(R/'PhongThanSource/Tools'))
from extract_client_coordinate_dictionary import encode_tcvn3 as strict_enc,decode_tcvn3 as dec
def enc(text):
 # The recovered font has no slots for some uppercase accented letters.
 # Use their supported lowercase glyph locally; never substitute question marks.
 while True:
  try:return strict_enc(text)
  except KeyError as error:
   glyph=error.args[0];assert glyph.isupper() and glyph.lower()!=glyph
   strict_enc(glyph.lower());text=text.replace(glyph,glyph.lower())
import build_offline_loose as pak
from prepare_npc_restoration import split_region,npc_records,merge_region,standalone_npcs
from build_authored_npcs import barrier_at,blocked,place_in_tile
P=R/'PhongThanSource/Deploy/ProjectContent/ItemQuestFollowup20261031';S=R/'PhongThanRuntime-Loose/Server';C=R/'PhongThanRuntime-Loose/Client';A=R/'analysis/item_quest_followup_20261031'
assert not (P/'publication.json').exists()
files=[]
def stage(role,rel,raw):
 live=R/'PhongThanRuntime-Loose'/role/rel;dest=P/'compiled'/role/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(raw)
 old=P/'source'/role/rel;old.parent.mkdir(parents=True,exist_ok=True)
 if live.exists() and not old.exists():old.write_bytes(live.read_bytes())
 files.append(dict(path=role+'/'+rel,before=hashlib.sha256(old.read_bytes()).hexdigest() if old.exists() else None,after=hashlib.sha256(raw).hexdigest()))
client=R/'Client_Original';packs=[client/'data'/s.split('=',1)[1].strip() for s in (client/'package.ini').read_text().splitlines() if '=' in s and s.split('=',1)[0].isdigit()];index,_=pak.pak_index([p for p in packs if p.is_file()],strict=False)
def icon(original):
 path=b'\\'+original.lstrip(b'\\');alias='spr/pt_test/'+hashlib.sha256(path).hexdigest()[:12]+'.spr'
 if alias not in staged_icons:
  existing=C/Path(path.decode('latin1').lstrip('\\'))
  if existing.is_file():raw=existing.read_bytes()
  else:
   entry=index.get(pak.pak_id(path));assert entry,repr(path);raw,_=pak.entry_payload(entry,{},None)
  assert raw.startswith(b'SPR');stage('Client',alias,raw);staged_icons.add(alias)
 return '\\'+alias.replace('/','\\')
staged_icons=set()
# Recovered original PT seal catalogue, preserving all row identities/levels.
rows=(A/'original/001/signet.txt').read_bytes().splitlines();seals=[]
for row,line in enumerate(rows[1:]):
 c=line.split(b'\t');assert c[1:3]==[b'0',b'13'];alias=icon(c[4]);c[4]=alias.encode();rows[row+1]=b'\t'.join(c);seals.append(dict(row=row,part=int(c[3]),level=int(c[11]),name=c[0].lstrip(b'#'),image=alias))
for role in ['Server','Client']:stage(role,'settings/item/001/signet.txt',b'\r\n'.join(rows)+b'\r\n')
header=R/'PhongThanSource/Sources/Core/Src/PhongThanEventHandbookItems.inl';raw=header.read_bytes();B=A/'source_backup/Sources/Core/Src/PhongThanEventHandbookItems.inl';B.parent.mkdir(parents=True,exist_ok=True)
if not B.exists():B.write_bytes(raw)
raw=B.read_bytes();entries=re.findall(rb'\{(-?\d+),(-?\d+),(-?\d+),(-?\d+),(-?\d+),(-?\d+)\}',raw);first=len(entries)
add=b''.join(('{0,13,%d,%d,%d,0},\n'%(s['part'],s['level'],s['row'])).encode() for s in seals);at=raw.rfind(b'};');assert at>0;header.write_bytes(raw[:at]+add+raw[at:])
rel='script/phongthan/test/event_handbook.lua';lua=(S/rel).read_bytes();node=max(map(int,re.findall(rb'EBNodes\[(\d+)\]=',lua)))+1;root_options=max(map(int,re.findall(rb'EBNodes\[1\]\[3\]\[(\d+)\]',lua)))+1
def quote(raw):return b'"'+b''.join(('\\%03d'%c).encode() if c>=128 else bytes([c]) if c not in [34,92] else b'\\'+bytes([c]) for c in raw)+b'"'
lua+=b'\nEBNodes[1][3]['+str(root_options).encode()+b']={'+quote(enc('Thần ấn'))+b',"EBMenu('+str(node).encode()+b',1)",""}\n'
lua+=b'EBNodes['+str(node).encode()+b']={'+quote(enc('Thần ấn'))+b',1,{}}\n'
for i,seal in enumerate(seals):
 label=seal['name']+enc(' [cấp ')+str(seal['level']).encode()+b']'
 lua+=('EBNodes[%d][3][%d]={'%(node,i+1)).encode()+quote(label)+(',"EBItem(%d,%d)",'%(first+i,node)).encode()+quote(seal['image'].encode())+b'}\n'
stage('Server',rel,lua)
# Correct the real effect as well as both tooltip descriptions.
old=enc('Tấn công tập trung: -5%');new=enc('Giảm sát thương tấn công tập trung: +5%')
for role,base in [('Server',S),('Client',C)]:
 rel='settings/item/001/ibitem.txt';rows=(base/rel).read_bytes().splitlines()
 for i,line in enumerate(rows[1:],1):
  c=line.split(b'\t')
  if c[1:3] in [[b'8',b'7446'],[b'8',b'7447']]:assert old in c[8];c[8]=c[8].replace(old,new);assert c[33:35]==[b'146',b'-5'];c[33:35]=[b'291',b'5'];rows[i]=b'\t'.join(c)
 stage(role,rel,b'\r\n'.join(rows)+b'\r\n')
 rel='settings/item/001/magicscript.txt';rows=(base/rel).read_bytes().splitlines()
 for i,line in enumerate(rows[1:],1):
  c=line.split(b'\t');sprite=next((n for n,v in enumerate(c) if v.startswith(b'\\spr\\')),None)
  if sprite is not None and c[sprite-1]==b'110':
   if sprite==3:c=[enc('#Mật đồ da dê'),b'6']+c[1:]
   c[0]=enc('#Mật đồ da dê');c[8]=enc('#Thầy Bói trao để tìm kho báu. Chuột phải xem manh mối; chỉ mất khi mở kho báu thành công.')
   script=next(n for n,v in enumerate(c) if v.startswith(b'\\script\\'));c[script]=b'\\script\\phongthan\\items\\treasure_map.lua';rows[i]=b'\t'.join(c)
 stage(role,rel,b'\r\n'.join(rows)+b'\r\n')
rel='Ui/ui3/PtConsumableBuff.ini';raw=(C/rel).read_bytes();assert raw.count(old)==2;stage('Client',rel,raw.replace(old,new))
# Point centres/radius are recovered verbatim from the original fortune-teller.
original=(A/'fortune-original-unicode.txt').read_text('utf8');points=[]
for trigram in range(1,8):
 block=original[original.index('function yv'+str(trigram)+'()'):].split('\nfunction ',1)[0]
 for mapid,x,y,radius in re.findall(r'\{mapid=(\d+),x=(\d+),y=(\d+),r=(\d+)\}',block):
  # The guide identifies the riddle region: use its live destination map.
  target={1:1016,2:1065,3:1008,4:1015,5:1014,6:1011,7:1065}[trigram]
  points.append((trigram,target,int(x),int(y)))
assert len(points)==144
data='#ifndef PT_TREASURE_DATA_H\n#define PT_TREASURE_DATA_H\nstruct PTTreasurePoint{int trigram,map,x,y;};\nstatic const PTTreasurePoint ptTreasurePoints[]={\n'+''.join('{%d,%d,%d,%d},\n'%p for p in points)+'};\nenum{PT_TREASURE_POINT_COUNT=sizeof(ptTreasurePoints)/sizeof(ptTreasurePoints[0])};\n#endif\n'
(R/'PhongThanSource/Headers/PhongThanTreasureData.h').write_text(data,'ascii')
map_lua='''function no() CloseDialog() end
PT_TREASURE_CLUES={[1016]="Thạch Lâm Tiêu Bích - Ải Tam Sơn",[1065]="Vùng đất ẩm ướt / Phong Yên - Ải Trần Đường",[1008]="Mang Mang Tuyết Nguyên - chân núi Côn Lôn",[1015]="Thạch Kiều Tiểu Kính - Mạnh Tân",[1014]="Phong Diệp Thụ Lâm - Đồng Quan",[1011]="Lục Lâm Thâm Sứ - Ải Du Hồn"}
function main(item)
 local ok,map,distance,last=TreasureItem(0,item)
 if ok~=1 then Talk(1,"no","Mật đồ chưa có manh mối đang thực hiện. Hãy đến Thầy Bói ở Triều Ca (212,187).");return end
 if distance<0 then Talk(1,"no","Manh mối trên mật đồ: <color=green>"..(PT_TREASURE_CLUES[map] or tostring(map)).."<color>.");return end
 local light="ánh sáng yếu"
 if distance<=2500 then light="ánh sáng mờ ảo" end
 if distance<=400 then light="ánh sáng mạnh" end
 if distance<=25 then SayButtons("Mật đồ phát ra ánh sáng chói chang. Kho báu ở rất gần!",2,{"Mở kho báu/pt_treasure_dig","Kết thúc/no"});return end
 local direction="Bạn đang tiến gần kho báu."
 if last>0 and distance>last then direction="Bạn đang đi xa kho báu." end
 Talk(1,"no","Mật đồ phát ra <color=yellow>"..light.."<color>. "..direction)
end
function pt_treasure_dig()
 if TreasureItem(2,0)==1 then Talk(1,"no","Đã mở kho báu và nhận phần thưởng.") else Talk(1,"no","Chưa thể mở kho báu: hãy đến đúng vị trí, giữ mật đồ trong hành trang hoặc ô phím nhanh và chừa chỗ nhận quà. Mật đồ vẫn được giữ nguyên.") end
end
'''
stage('Server','script/phongthan/items/treasure_map.lua',enc(map_lua))
npc_lua='''function no() CloseDialog() end
function main()
 SayTask("Thầy Bói: mang một trong bảy quẻ đến đây, ta sẽ đổi cho ngươi Mật đồ da dê để tìm kho báu.",{{"Tìm bảo vật","pt_treasure_options"},{"Mật đồ da dê","pt_treasure_about"},{"Nhận lại mật đồ bị mất","pt_treasure_restore"}})
end
function pt_treasure_options()
 SayButtons("Chọn quẻ muốn đổi lấy Mật đồ da dê:",8,{"Quẻ Khôn/pt_treasure_1","Quẻ Khảm/pt_treasure_2","Quẻ Ly/pt_treasure_3","Quẻ Cấn/pt_treasure_4","Quẻ Đoài/pt_treasure_5","Quẻ Tốn/pt_treasure_6","Quẻ Chấn/pt_treasure_7","Kết thúc/no"})
end
function pt_treasure_accept(n)
 if TreasureItem(1,n)==1 then Talk(1,"no","Đã đổi quẻ lấy Mật đồ da dê. Chuột phải để đọc manh mối; có thể đặt vào ô phím nhanh 1-4.") else Talk(1,"no","Chưa thể đổi: ngươi cần có quẻ đã chọn, hành trang còn chỗ và chưa có nhiệm vụ tìm kho báu đang thực hiện.") end
end
function pt_treasure_about() Talk(1,"no","Mật đồ da dê sẽ sáng dần khi ngươi đến gần kho báu. Đọc mật đồ không làm mất vật phẩm; chỉ mở kho báu thành công mới dùng hết mật đồ.") end
function pt_treasure_restore() if TreasureItem(3,0)==1 then Talk(1,"no","Đã cấp lại Mật đồ da dê cho nhiệm vụ đang thực hiện.") else Talk(1,"no","Không có mật đồ cần cấp lại, vật phẩm vẫn còn trong túi/kho, hoặc hành trang chưa đủ chỗ.") end end
'''
for i in range(1,8):npc_lua+=f'function pt_treasure_{i}() pt_treasure_accept({i}) end\n'
stage('Server','script/phongthan/npc/fortune_teller.lua',enc(npc_lua))
# Reuse the validated town-astrologer appearance; keep its other instances intact.
meta=dict(re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(S/'settings/WorldSet.ini').read_bytes()));mid=1021;physical=meta[b'1021'].decode('latin1');x,y=212*256+128,187*512+256;rx,ry=x//512,y//1024
rel=f'settings/phongthan/npc_regions/{mid}/v_{ry:03d}/{rx:03d}_region_s.dat';existing=S/rel
if not existing.exists():existing=S/'maps'/(physical+'_S')/f'v_{ry:03d}/{rx:03d}_region_s.dat'
base=existing.read_bytes() if existing.exists() else None
if base is None:
 rel=rel.replace('_region_s.dat','_npc_s.dat');existing=S/rel;old=npc_records(existing.read_bytes()) if existing.exists() else []
else:old=npc_records(split_region(base)[2])
client_region=C/'maps'/physical/f'v_{ry:03d}/{rx:03d}_region_c.dat'
x,y=place_in_tile(base if base else client_region.read_bytes(),x,y,[struct.unpack_from('<ii',rec,4) for rec in old]);assert x//256==212 and y//512==187 and not blocked(barrier_at(client_region.read_bytes(),x,y),x,y)
script=b'\\script\\phongthan\\npc\\fortune_teller.lua';label=b'ThayBoiBatQuai';record=struct.pack('<iiiB3x32shhhhBBH',150,x,y,0,label,1,0,0,3,6,0,len(script))+script
assert all(label not in rec for rec in old)
stage('Server',rel,merge_region(base,[record]) if base else standalone_npcs(old+[record]))
for role,folder in [('Server',S),('Client',C)]:
 rel='settings/phongthan/NpcDisplayNames.txt';raw=(folder/rel).read_bytes();stage(role,rel,raw.rstrip(b'\r\n')+b'\r\n'+label+b'\t'+enc('Thầy Bói')+b'\r\n')
(P/'manifest.json').write_text(json.dumps(dict(scope='LOOSE_ONLY',files=files,seals=len(seals),seal_first_id=first,fortune=dict(map=mid,template=150,x=x,y=y)),indent=2),'utf8')
print('Staged',len(files),'files;',len(seals),'original seals;',len(points),'original treasure centres')

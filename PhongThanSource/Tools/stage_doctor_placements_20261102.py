"""Add missing doctors only at checked walkable cells; preserve NPC sections."""
import json,re,struct,hashlib
from pathlib import Path
from prepare_npc_restoration import split_region,npc_records,merge_region,standalone_npcs,pak_id
from publish_original_world import parse_index
from build_authored_npcs import barrier_at,blocked
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2];server=ROOT/'PhongThanRuntime-Loose/Server';out=ROOT/'analysis/doctors_thuongchu_20261102';stage=out/'staged/Server'
audit=json.loads((out/'full-audit.json').read_text('utf8'));index=parse_index((server/'loose_index.bin').read_bytes())
worlds={int(a):b for a,b in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes())}
existing=set(audit['maps_with_doctors']);entries=[];changes={}
def load(relative):
 p=server/relative
 if p.is_file():return p.read_bytes()
 raw=('\\'+relative.replace('/','\\')).encode('latin1');bound=index.get(pak_id(raw))
 if bound:
  p=server/bound.decode('latin1').lstrip('\\').replace('\\','/')
  if p.is_file():return p.read_bytes()
 return None
def region(world,x,y):
 folder=worlds[world].decode('latin1');cell=f'v_{y//1024:03d}/{x//512:03d}'
 base=load(f'maps/{folder}_S/{cell}_region_s.dat')
 if base is None:return None
 current=f'settings/phongthan/npc_regions/{world}/{cell}_region_s.dat'
 standalone=f'settings/phongthan/npc_regions/{world}/{cell}_npc_s.dat'
 replacement=load(current)
 if replacement is not None:base=replacement
 return base,current,standalone
def add(world,tid,name,script,ax,ay,why):
 occupied=[(r['x'],r['y']) for r in audit['npcs'] if r['world']==world]
 candidates=[(ax+dx,ay+dy) for radius in (64,128,256,512,1024,2048) for dx in range(-radius,radius+1,32) for dy in (-radius,radius)]
 candidates.sort(key=lambda p:(p[0]-ax)**2+(p[1]-ay)**2)
 for x,y in candidates:
  r=region(world,x,y)
  if not r or any(abs(x-a)<64 and abs(y-b)<64 for a,b in occupied):continue
  base,current,standalone=r
  if blocked(barrier_at(base,x,y),x,y):continue
  logical=('\\'+script.replace('/','\\')).encode('ascii')
  record=bytearray(60);struct.pack_into('<iii',record,0,tid,x,y);encoded=encode_tcvn3(name);assert len(encoded)<32;record[16:16+len(encoded)]=encoded
  struct.pack_into('<h',record,48,1);struct.pack_into('<h',record,54,3);struct.pack_into('<H',record,58,len(logical)+1);record=bytes(record)+logical+b'\0'
  old=load(current);baseNpc=npc_records(split_region(base)[2]);oldstand=load(standalone)
  if old is not None or baseNpc:
   relative=current;before=old;after=merge_region(changes.get(relative,base),[record])
  else:
   relative=standalone;before=oldstand;rows=npc_records(changes.get(relative,oldstand)) if (relative in changes or oldstand) else [];after=standalone_npcs(rows+[record])
  changes[relative]=after;target=stage/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(after)
  entries.append(dict(world=world,name=name,x=x,y=y,display_x=x//256,display_y=y//512,script=script,relative=relative,before_sha256=hashlib.sha256(before).hexdigest() if before else None,reason=why));return
 raise ValueError(f'No verified walkable doctor position map={world} anchor={ax},{ay}')
travel={int(m):(int(x)*32,int(y)*32) for m,x,y in re.findall(r'\{\d+,(\d+),(\d+),(\d+),\d+\}',(ROOT/'PhongThanSource/Headers/PhongThanTravelDestinations.h').read_text('ascii'))}
loaded={int(m) for m in re.findall(rb'(?m)^World\d+=(\d+)',(server/'maps/WorldSet.ini').read_bytes())}
eligible=[m for m in sorted(worlds) if (1001<=m<=1057 or m==1065 or 1073<=m<=1078 or m==1099) and m in loaded]
for world in eligible:
 if world in existing:continue
 actors=[r for r in audit['npcs'] if r['world']==world and r['kind']==3]
 if world in travel:ax,ay=travel[world]
 elif actors:ax,ay=actors[0]['x'],actors[0]['y']
 else:
  wor=load('maps/'+worlds[world].decode('latin1')+'.wor');rect=re.search(rb'(?im)^rect\s*=\s*(\d+),(\d+),(\d+),(\d+)',wor or b'')
  if not rect:raise ValueError('Missing map geometry '+str(world))
  l,t,r,b=map(int,rect.groups());ax=(l+r)*256;ay=(t+b)*512
 script=f'script/phongthan/services/doctor_{world}.lua'
 text='''function main() Say("Đại Phu",3,"Cửa hàng thuốc/pt_doctor_shop","Trị thương/pt_doctor_heal","Kết thúc/pt_doctor_close") end
function pt_doctor_shop() CloseDialog();Sale(23) end
function pt_doctor_heal()
 if SystemTime()-GetTask(763)<1800 then Talk(1,"pt_doctor_close","Ta chưa hồi phục nguyên khí. Hãy quay lại sau.");return end
 RestoreLife();RestoreMana();SetTask(763,SystemTime());CloseDialog();Msg2Player("Sinh lực và nội lực đã hoàn toàn hồi phục.")
end
function pt_doctor_close() CloseDialog() end
'''
 target=stage/script;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(encode_tcvn3(text));add(world,149,'Đại Phu',script,ax,ay,'Authored missing service near existing arrival/NPC, verified original terrain; not claimed original coordinate')
# The other faction's original entry coordinate; keep the existing Shang stele.
add(1001,173,'Bia Thần phe Chu','script/phongthan/thuong_chu/stele_zhou.lua',212*256,215*512,'VNG documented Zhou entry coordinate')
for side,name,x,y in [(1,'Văn Trọng',178,203),(2,'Khương Thượng',290,206)]:add(1071,190,name,'script/phongthan/thuong_chu/mission_npc.lua',x*256,y*512,'VNG mission service at faction base')
# Remove authored guide-only marshal/general placeholders; actual battle units
# spawn with entity ownership when a session starts, never as permanent dialogers.
for row in audit['battle']:
 if row['template'] not in (190,211):continue
 relative=row['effective'];before=load(relative);data=changes.get(relative,before);record=bytes.fromhex(row['record_hex'])
 if relative.endswith('_npc_s.dat'):after=standalone_npcs([r for r in npc_records(data) if r!=record])
 else:
  head,sections,payload=split_region(data);rows=[r for r in npc_records(payload) if r!=record];new=standalone_npcs(rows);after=bytearray(data);struct.pack_into('<II',after,20,len(data)-head,len(new));after=bytes(after)+new
 changes[relative]=after;target=stage/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(after)
 entries.append(dict(world=1071,relative=relative,before_sha256=hashlib.sha256(before).hexdigest(),reason='Remove dialog-only combat placeholder; real session owns combat units'))
(out/'placement-plan.json').write_text(json.dumps(dict(entries=entries,files=[dict(relative=k,sha256=hashlib.sha256(v).hexdigest()) for k,v in changes.items()]),ensure_ascii=False,indent=2),'utf8')
print('Missing doctor maps', [r['world'] for r in entries if r.get('name')=='Đại Phu'],'NPC region files',len(changes))

"""Stage real doctor Lua, safe entrance placements and minimap metadata aliases."""
import json,re,struct
from restore_original_world import ROOT,Original
from prepare_npc_restoration import split_region,npc_records,merge_region
from build_authored_npcs import barrier_at,blocked
import build_offline_loose as b
folder=ROOT/'analysis/vtt_client_services_20261019';folder.mkdir(exist_ok=True)
if (folder/'publication.json').exists():raise ValueError('Already published')
runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server';o=Original()
worlds=dict((int(k),v) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
changes={'Server':{},'Client':{}};doctors=[]
def add(side,relative,data,evidence):
 target=runtime/side/relative
 if target.exists() and target.read_bytes()==data:return
 changes[side][relative]=dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha(target.read_bytes()) if target.exists() else None,bytes=len(data),evidence=evidence)
 p=folder/'staged'/side/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
doctor,proof=o.get('script/万仙阵/医生.lua'.encode('gbk'))
doctor_rel='script/phongthan/van_tien/doctor.lua'
# Retain the exact original task transition 408:1 -> 100000 and original Sale(24).
add('Server',doctor_rel,doctor,'Exact original doctor Lua '+proof['id'])
lines=o.get(b'settings/Npcs.txt')[0].splitlines();header=lines[0].split(b'\t')
for tid in range(53,61):
 row=dict(zip(header,lines[tid+1].split(b'\t')));got=o.get(row[b'DeathScript']);assert got
 add('Server',f'script/phongthan/van_tien/mob{tid}.lua',got[0]+b'\n-- Explicit runtime/content map identity compatibility.\nGetWorldPos=GetContentWorldPos\n','Exact original normal VTT monster death/quest script '+got[1]['id'])
# Water previously had only one loose server cell. Recover missing collision
# grids with the same explicit obstacle-only format as the other three arenas.
name=worlds[1068];wor=o.get(b'maps/'+name+b'.wor')[0]
left,top,right,bottom=map(int,re.search(rb'rect=(\d+),(\d+),(\d+),(\d+)',wor).groups())
for ry in range(top,bottom+1):
 for rx in range(left,right+1):
  relative=(b'maps/'+name+f'_S/v_{ry:03d}/{rx:03d}_region_s.dat'.encode()).decode('latin1')
  if (server/relative).exists():continue
  got=o.get(b'maps/'+name+f'/v_{ry:03d}/{rx:03d}_region_c.dat'.encode())
  if not got:continue
  head,sections,_=split_region(got[0]);off,size=sections[0];assert size in (0,2048)
  obstacle=got[0][head+off:head+off+size]
  data=struct.pack('<I',6)+b''.join(struct.pack('<II',a,z) for a,z in [(0,size)]+[(size,0)]*5)+obstacle
  add('Server',relative,data,'Authored server container; exact original water client collision only '+got[1]['id'])
for ident in range(1,5):
 world=1066+ident;name=worlds[world]
 wor=o.get(b'maps/'+name+b'.wor')[0]
 relative=(b'settings/phongthan/minimap/'+name+b'.wor').decode('latin1')
 add('Client',relative,wor,'Alias exact original WOR for existing native minimap loader; original JPG unchanged')
 mission_rel=f'script/phongthan/van_tien/mission{ident:02d}.lua';data=(server/mission_rel).read_bytes()
 x=int(re.search(rb'PT_ENTRY_X=(\d+)',data)[1])*32;y=int(re.search(rb'PT_ENTRY_Y=(\d+)',data)[1])*32
 candidates=sorted(((x+dx,y+dy) for dx in range(-192,193,32) for dy in range(-192,193,32) if abs(dx)+abs(dy)>=96),key=lambda q:(q[0]-x)**2+(q[1]-y)**2)
 for px,py in candidates:
  raw=b'maps/'+name+f'_S/v_{py//1024:03d}/{px//512:03d}_region_s.dat'.encode()
  relative=raw.decode('latin1');target=server/relative
  if relative in changes['Server']:target=folder/'staged/Server'/relative
  if not target.exists():target=server/raw.replace(name+b'_S',name).decode('latin1')
  base=target.read_bytes() if target.exists() else None
  if base is not None and not blocked(barrier_at(base,px,py),px,py):break
 else:raise ValueError(f'No walkable doctor point world={world} entry={x},{y} path={target} exists={target.exists()}')
 # Authored point near the original entrance; do not label it an original coordinate.
 relative=f'settings/phongthan/npc_regions/{world}/v_{py//1024:03d}/{px//512:03d}_region_s.dat'
 current=server/relative
 if current.exists():base=current.read_bytes()
 script=('\\'+doctor_rel.replace('/','\\')).encode('ascii')
 record=struct.pack('<iiiB3x32shhhhBBH',766,px,py,0,b'Dai Phu',1,0,0,3,0,0,len(script))+script
 assert len(record)==60+len(script)
 result=merge_region(base,[record]);assert npc_records(split_region(result)[2]).count(record)==1
 add('Server',relative,result,'Authored walkable doctor placement near original entry; original template 766 and original Lua 7A4FF11E')
 doctors.append(dict(map=world,x=px,y=py,display=[px//256,py//512],template=766,original_coordinate=False,script=doctor_rel,override=relative))
 # Countdown arrives on entry and at combat start; the native API checks membership.
 data=data.replace(b'function pt_fight() SetFightState(1) end',f'function pt_fight() SetFightState(1);ShowMissionCountdown(PT_ID,{3000+ident*2}) end'.encode())
 match=re.search(rb'PT_MOBS=(\{[^\r\n]+)',data);assert match
 counter=[0]
 def normal_mob(m):
  tid=51+ident*2+counter[0]%2;counter[0]+=1
  return b'{'+str(tid).encode()+b','+m[2]+b','+m[3]+b'}'
 mobs=re.sub(rb'\{(\d+),(\d+),(\d+)\}',normal_mob,match[1]);assert counter[0]==24
 data=data[:match.start(1)]+mobs+data[match.end(1):]
 data=data.replace(b'pt_spawn(p[1],p[2],p[3],nil)',b'pt_spawn(p[1],p[2],p[3],"\\\\script\\\\phongthan\\\\van_tien\\\\mob"..p[1]..".lua")')
 add('Server',mission_rel,data,'Send native mission countdown when combat starts')
 portal_rel={1:'1002_f57647b5',2:'1004_281c1583',3:'1003_479d2a84',4:'1001_41e3ecb2'}[ident]
 relative='script/phongthan/npc_recovered_batch/'+portal_rel+'.lua';data=(server/relative).read_bytes()
 needle=b'    if state==1 then SetFightState(0) else SetFightState(1) end\n'
 assert needle in data
 data=data.replace(needle,needle+f'    if state==1 then ShowMissionCountdown(PT_ID,{2999+ident*2}) else ShowMissionCountdown(PT_ID,{3000+ident*2}) end\n'.encode(),1)
 add('Server',relative,data,'Send registration/combat native countdown after successful entry and membership')
# Native APIs in the rebuilt server are used explicitly; no invented reward substitutions.
ledger=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
content=json.loads(ledger.read_text('utf8'))
for row in content['ready']:
 rel=row['script_relative']
 if rel in changes['Server']:row['lua_sha256']=changes['Server'][rel]['sha256']
p=folder/'staged/Project/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json';p.parent.mkdir(parents=True,exist_ok=True);p.write_text(json.dumps(content,ensure_ascii=False,indent=2),'utf8')
plan=dict(ready=[],blocked=[],files=list(changes['Server'].values()),client_files=list(changes['Client'].values()),doctors=doctors,doctor_original=proof,
 metadata=[dict(relative='Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json',sha256=b.sha(p.read_bytes()),before_sha256=b.sha(ledger.read_bytes()))])
(folder/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
print(json.dumps(dict(server_files=len(changes['Server']),client_files=len(changes['Client']),doctors=doctors),ensure_ascii=False))

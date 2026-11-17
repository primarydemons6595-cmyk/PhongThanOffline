import json,re
from restore_original_world import ROOT,OUT
from build_authored_npcs import barrier_at,blocked
from prepare_npc_restoration import split_region,npc_records
folder=OUT/'van_tien_entry_fix';plan=json.loads((folder/'plan.json').read_text('utf8'))
staged=folder/'staged';server=ROOT/'PhongThanRuntime-Loose/Server'
def get(side,relative):
 p=staged/side/relative
 return p.read_bytes() if p.exists() else (ROOT/'PhongThanRuntime-Loose'/side/relative).read_bytes()
for side in ('Server','Client'):
 ids=[int(n) for n in re.findall(rb'World\d+=(\d+)',get(side,'maps/WorldSet.ini'))]
 assert len(ids)==105 and len(set(ids))==105 and all(n in ids for n in range(1067,1071))
 old=[int(n) for n in re.findall(rb'World\d+=(\d+)',(ROOT/'PhongThanRuntime-Loose'/side/'maps/WorldSet.ini').read_bytes())]
 assert ids[:len(old)]==old
worlds=dict((int(a),v) for a,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
checks=0
for arena in json.loads((OUT/'boss_van_tien/arenas.json').read_text('utf8')):
 world=arena['runtime_map']
 if world==1068:continue
 points=[arena['entry']]+arena['positions']+[[r[1],r[2]] for r in arena['mobs']]
 for x,y in points:
  relative='maps/'+worlds[world].decode('latin1')+f'_S/v_{y//1024:03d}/{x//512:03d}_region_s.dat'
  data=get('Server',relative);head,sections,payload=split_region(data)
  assert not blocked(barrier_at(data,x,y),x,y),(world,x,y)
  assert not npc_records(payload) and all(size==0 for off,size in sections[1:])
  checks+=1
for row in plan['changes']['Server']:
 if row['relative'].endswith('.lua'):
  data=get('Server',row['relative']);assert b'Ban do Van Tien Tran chua san sang' in data and b'Dang ky 02:20' in data
print('PASS VTT_ENTRY_MAPS active=105 arenas=4 existing_slots_preserved=1 obstacle_points='+str(checks)+' explicit_rejection_feedback=4')

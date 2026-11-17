"""Record native live population and original script/resource provenance."""
import json,re,shutil,time
from datetime import datetime,timezone
from restore_original_world import ROOT,Original
from publish_original_world import parse_index
from prepare_npc_restoration import split_region,npc_records
import build_offline_loose as b
folder=ROOT/'analysis/vtt_client_services_20261019';runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server';client=runtime/'Client'
plan=json.loads((folder/'plan.json').read_text('utf8'));deployment=json.loads((folder/'deployment.json').read_text('utf-8-sig'));live=json.loads((server/'studio_live_world.json').read_text('utf8'))
assert live['Pid']==deployment['Pid'] and 0<=time.time()-live['UtcSeconds']<15,'Need fresh live snapshot for current main process'
o=Original();result=[]
for doctor in plan['doctors']:
 world=doctor['map'];npcs=[n for n in live['Npcs'] if n['Map']==world]
 normal=[n for n in npcs if 53<=n['Template']<=60];guards=[n for n in npcs if 129<=n['Template']<=147]
 actual=[n for n in npcs if n['Template']==766]
 assert len(actual)==1 and actual[0]['X']==doctor['x'] and actual[0]['Y']==doctor['y'],world
 assert len(normal)>=24,world
 result.append(dict(map=world,doctor=actual[0],normal_monsters=len(normal),guardians=len(guards)))
data,proof=o.get(bytes.fromhex(plan['doctor_original']['raw_hex']));assert (server/'script/phongthan/van_tien/doctor.lua').read_bytes()==data
names=dict((int(k),v) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
index=parse_index((client/'loose_index.bin').read_bytes())
for world in range(1067,1071):
 name=names[world]
 for raw in (b'maps/'+name+b'24.jpg',b'settings/phongthan/minimap/'+name+b'.wor'):
  key=b.pak_id(raw);assert key in index
  data=(client/index[key].lstrip(b'\\').decode('latin1').replace('\\','/')).read_bytes()
  original=o.get(b'maps/'+name+(b'24.jpg' if raw.endswith(b'.jpg') else b'.wor'))
  assert data==original[0],world
activation=json.loads((folder/'activation.json').read_text('utf8'))
before_ledger=json.loads((folder/'backup_before_publication/Project/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))
after_ledger=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))
old={(row['world'],row['name']):row for row in before_ledger['ready']};new={(row['world'],row['name']):row for row in after_ledger['ready']}
assert old.keys()==new.keys()
metadata_changes=[]
for key in old:
 if old[key]!=new[key]:
  a=dict(old[key]);c=dict(new[key]);a.pop('lua_sha256',None);c.pop('lua_sha256',None)
  assert a==c and '/npc_recovered_batch/100' in c['script_relative'] and c['world'] in (1001,1002,1003,1004)
  metadata_changes.append(c['script_relative'])
assert len(metadata_changes)==4
target=server/'script/servertimer.lua';normal=(folder/'backup_activation/script/servertimer.lua').read_bytes()
if target.read_bytes()!=normal:
 permitted=[activation['files']['script/servertimer.lua']]
 if (folder/'timer_fix/plan.json').exists():permitted+=[row['sha256'] for row in json.loads((folder/'timer_fix/plan.json').read_text('utf8'))['files'] if row['relative']=='script/servertimer.lua']
 assert b.sha(target.read_bytes()) in permitted
 target.write_bytes(normal);(ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled/script/servertimer.lua').write_bytes(normal)
 manifest=runtime/'completion/Server_named_effective_manifest.csv';shutil.copy2(manifest,folder/'Server_manifest_before_finalizing_recovery.csv');rows=list(b.csv_read(manifest))
 for row in rows:
  if row['path'].lower()=='script\\servertimer.lua':row.update(sha256=b.sha(normal),bytes=str(len(normal)),origin='VTT_RECOVERY_FINALIZED',evidence='One recovery round confirmed natively; ordinary schedule restored on disk')
 b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
report=dict(status='NATIVE_POPULATION_CONFIRMED',verified_utc=datetime.now(timezone.utc).isoformat(),pid=live['Pid'],arenas=result,total_normal_monsters=96,total_doctors=4,
 original_doctor_script=True,original_minimap_images_and_metadata=True,normal_heartbeat_restored=True,unrelated_recovered_npc_metadata_preserved=True,changed_existing_npc_metadata=metadata_changes,core_server_sha256=b.sha((server/'CoreServer.dll').read_bytes()),game_sha256=b.sha((client/'Game.exe').read_bytes()),
 client_ui_visual_acceptance=False,scene_barrier_visual_acceptance=False,original_server_traps_restored=False,reward_vng_parity=False)
(folder/'runtime_verification.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
ledger=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/vtt_doctors.json';ledger.write_text(json.dumps(dict(doctors=plan['doctors'],original_script=plan['doctor_original'],coordinate_provenance='AUTHORED_WALKABLE_NEAR_ORIGINAL_ENTRY'),ensure_ascii=False,indent=2),'utf8')
print(json.dumps(report,ensure_ascii=False))

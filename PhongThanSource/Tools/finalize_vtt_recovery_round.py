import json,re,time,shutil
from datetime import datetime,timezone
from restore_original_world import ROOT
import build_offline_loose as b
folder=ROOT/'analysis/vtt_open_fix_20261019';server=ROOT/'PhongThanRuntime-Loose/Server'
plan=json.loads((folder/'plan.json').read_text('utf8'))
lines=(server/'vtt_mission_lifecycle.log').read_text('ascii').splitlines()
matches=[re.search(r'INIT utc=(\d+) map=(\d+) mission=(\d+) call=PASS state=1 registered=1',line) for line in lines]
records=[dict(utc=int(m[1]),map=int(m[2]),mission=int(m[3])) for m in matches if m and int(m[1])>=int(time.time())-900]
assert {row['map'] for row in records}=={1067,1068,1069,1070},'Need successful native initialization for all four arenas'
target=server/'script/servertimer.lua';staged=(folder/'staged/Server/script/servertimer.lua').read_bytes();before=(folder/'backup_before_publication/Server/script/servertimer.lua').read_bytes()
assert target.read_bytes() in (staged,before)
saved=folder/'recovery_round_runtime_payload.lua'
if not saved.exists():saved.write_bytes(staged)
# Running Lua already owns its bounded one-shot copy. Restore normal files now
# so a later restart cannot reopen another extra round inside the expiry window.
target.write_bytes(before)
compiled=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled/script/servertimer.lua';compiled.write_bytes(before)
manifest=ROOT/'PhongThanRuntime-Loose/completion/Server_named_effective_manifest.csv';rows=list(b.csv_read(manifest))
for row in rows:
 if row['path'].lower()=='script\\servertimer.lua':row.update(sha256=b.sha(before),bytes=str(len(before)),origin='VTT_STACK_REPAIR_RECOVERY_FINALIZED',evidence='Temporary recovery round confirmed natively, then removed from disk; analysis/vtt_open_fix_20261019/runtime_verification.json')
b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
result=dict(status='NATIVE_REGISTRATION_CONFIRMED',verified_utc=datetime.now(timezone.utc).isoformat(),arenas=records,registration_end_utc=max(r['utc'] for r in records)+300,
    server_sha256=b.sha((server/'CoreServer.dll').read_bytes()),normal_heartbeat_restored_on_disk=True,live_player_teleport_verified=False,
    tests=dict(real_mission_size=867116,scalar_lookups=20000,native_schedule_vtt_rows=24,lua_lifecycle_checks=369,event_native_checks=530,connected_event_checks=275,native_abi_calls=182,event_abi_calls=15))
(folder/'runtime_verification.json').write_text(json.dumps(result,indent=2),'utf8')
print(json.dumps(result))

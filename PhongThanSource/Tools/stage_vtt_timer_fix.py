import json,time
from restore_original_world import ROOT
import build_offline_loose as b
folder=ROOT/'analysis/vtt_client_services_20261019';batch=folder/'timer_fix';batch.mkdir(exist_ok=True)
if (batch/'publication.json').exists():raise ValueError('Timer binary already published')
server=ROOT/'PhongThanRuntime-Loose/Server';live=json.loads((server/'studio_live_world.json').read_text('utf8'));deployment=json.loads((folder/'deployment.json').read_text('utf-8-sig'))
assert live['Pid']==deployment['Pid'] and time.time()-live['UtcSeconds']<15
evidence=[]
for world in range(1067,1071):
 expected=list(range(129+(world-1067)*5,133+(world-1067)*5))
 guards=[n for n in live['Npcs'] if n['Map']==world and n['Template'] in expected]
 assert all(any(n['Template']==tid for n in guards) for tid in expected),world
 evidence.append(dict(map=world,required_guardians=expected,actual_guardians=guards))
(folder/'native_combat_proof.json').write_text(json.dumps(dict(pid=live['Pid'],utc=live['UtcSeconds'],arenas=evidence,scope='Native phase transition created all four guardians per arena; native summon skills may create additional NPCs'),indent=2),'utf8')
rel='script/servertimer.lua';normal=(server/rel).read_bytes();assert normal==(folder/'backup_activation'/rel).read_bytes()
expires=int(time.time())+3600
data=normal+f'''
-- One recovery round after the countdown ID repair; normal daily windows unchanged.
pt_vtt_timer_previous=main
pt_vtt_timer_done=0
function main()
    pt_vtt_timer_previous()
    if pt_vtt_timer_done==1 or SystemTime()>{expires} then return end
    pt_vtt_timer_done=1
    local old=SubWorld
    for id=1,4 do
        SubWorld=SubWorldID2Idx(66+id)
        if SubWorld>=0 and IsMission(id)==0 then OpenMission(id) end
    end
    SubWorld=old
end
'''.encode('ascii')
p=batch/'staged/Server'/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
plan=dict(ready=[],blocked=[],files=[dict(relative=rel,sha256=b.sha(data),before_sha256=b.sha(normal))],binaries=[dict(relative='CoreServer.dll',sha256=b.sha((ROOT/'PhongThanSource/Output/Server/CoreServer.dll').read_bytes()),before_sha256=b.sha((server/'CoreServer.dll').read_bytes()))])
(batch/'plan.json').write_text(json.dumps(plan,indent=2),'utf8');print(json.dumps(plan))

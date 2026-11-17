import json,hashlib,urllib.request,time
from restore_original_world import ROOT
runtime=ROOT/'PhongThanRuntime-Loose';out=ROOT/'analysis/event_service_20261019'
deployment=json.loads((out/'deployment.json').read_text('utf8'));manifest=json.loads((out/'publish_manifest.json').read_text('utf8'))
endpoint=json.loads((runtime/'Server/EventService/endpoint.json').read_text('utf8'))
request=urllib.request.Request(endpoint['Url']+'v1/status',headers={'X-PhongThan-Event-Token':(runtime/'Server/EventService/access.token').read_text().strip()})
with urllib.request.urlopen(request,timeout=10) as reply:status=json.load(reply)
assert status['ServicePid']==deployment['EventServicePid']
native=status['Native']
if native is None:
    print(json.dumps(dict(state='WAIT_NATIVE_READY',service=status['ServicePid'],server=deployment['GameServerPid'])));raise SystemExit(2)
assert native['Pid']==deployment['GameServerPid'],('Unexpected server',native['Pid'])
assert native['LeaseValid'] and native['Revision']==status['Configuration']['Revision']
assert len(status['Catalog'])==18 and len(native['Events'])==18
assert {d['Id'] for d in native['Events']}=={d['Id'] for d in status['Catalog']}
assert all(d['Phase']>=0 for d in native['Events'])
for row in manifest['files']:
    assert hashlib.sha256((runtime/row['relative']).read_bytes()).hexdigest()==row['sha256'],row['relative']
for row in manifest['protected_drop_config']:
    assert hashlib.sha256((runtime/row['relative']).read_bytes()).hexdigest()==row['sha256'],('Drop config changed',row['relative'])
snapshot=json.loads((runtime/'Server/studio_live_world.json').read_text('utf8'))
assert snapshot['Pid']==native['Pid'] and time.time()-snapshot['UtcSeconds']<15
positions=[]
for npc in manifest['npcs']:
    rows=[r for r in snapshot['Npcs'] if r['Map']==npc['world'] and r['Template']==npc['template'] and r['X']==npc['x'] and r['Y']==npc['y']]
    assert len(rows)==1,('NPC missing/duplicate',npc['name'],rows)
    assert rows[0]['Kind']==3
    positions.append(dict(name=npc['name'],map=npc['world'],display=npc['display'],native_index=rows[0]['Index']))
deployment.update(State='NATIVE_READY_VERIFIED',VerifiedUtcSeconds=int(time.time()),Revision=native['Revision'],CatalogEvents=18,NewNpcsObserved=positions,NativeDropConfigChanged=False)
(out/'deployment.json').write_text(json.dumps(deployment,ensure_ascii=False,indent=2),'utf8')
(out/'live_status_verified.json').write_text(json.dumps(dict(ServicePid=status['ServicePid'],Native=native,NewNpcsObserved=positions,TokenRecorded=False),ensure_ascii=False,indent=2),'utf8')
print(json.dumps(dict(state=deployment['State'],server=native['Pid'],service=status['ServicePid'],events=18,revision=native['Revision'],new_npcs=positions,drop_configs_unchanged=True),ensure_ascii=False))

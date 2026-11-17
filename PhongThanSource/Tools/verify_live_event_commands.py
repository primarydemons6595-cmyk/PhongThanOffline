import json,time,uuid,urllib.request,urllib.error
from restore_original_world import ROOT
runtime=ROOT/'PhongThanRuntime-Loose';out=ROOT/'analysis/event_service_20261019'
endpoint=json.loads((runtime/'Server/EventService/endpoint.json').read_text('utf8'));token=(runtime/'Server/EventService/access.token').read_text().strip()
def call(method,path,body=None):
    request=urllib.request.Request(endpoint['Url']+path,data=json.dumps(body).encode() if body is not None else None,method=method,headers={'X-PhongThan-Event-Token':token,'Content-Type':'application/json'})
    with urllib.request.urlopen(request,timeout=10) as reply:return json.load(reply)
status=call('GET','v1/status');rule=next(r for r in status['Configuration']['Events'] if r['Id']==2001)
assert status['Native']['LeaseValid'] and status['Native']['Revision']==status['Configuration']['Revision']
saved=call('PUT','v1/event',dict(ExpectedRevision=status['Configuration']['Revision'],Rule=rule));revision=saved['Configuration']['Revision']
assert saved['State']=='pending_native_ack'
for _ in range(100):
    status=call('GET','v1/status')
    if status['Native'] and status['Native']['Revision']==revision and status['Native']['LeaseValid']:break
    time.sleep(.1)
else:raise RuntimeError('Native did not acknowledge configuration')
receipts=[]
for eventId in (2001,2002):
    requestId=str(uuid.uuid4());body=dict(EventId=eventId,Operation='open',RequestId=requestId)
    queued=call('POST','v1/command',body);id=queued['Command']['Id']
    for _ in range(100):
        receipt=call('GET','v1/receipt/'+id)
        if receipt['State'] in ('applied','rejected','expired','unknown'):break
        time.sleep(.1)
    assert receipt['State']=='applied',receipt
    assert receipt['Pid']==status['Native']['Pid'] and receipt['BootUtc']==status['Native']['BootUtc']
    retry=call('POST','v1/command',body);assert retry['Command']['Id']==id and retry['Receipt']['State']=='applied'
    receipts.append(dict(EventId=eventId,Receipt=receipt,RetrySameId=True))
(out/'live_commands_verified.json').write_text(json.dumps(dict(ConfigurationRevision=revision,SettingsChanged=False,NativePid=status['Native']['Pid'],Receipts=receipts,TokenRecorded=False),indent=2),'utf8')
print(json.dumps(dict(state='LIVE_CONFIG_AND_COMMAND_ACK_PASS',revision=revision,settings_changed=False,events=[2001,2002],native_pid=status['Native']['Pid'],idempotent_retry=True)))

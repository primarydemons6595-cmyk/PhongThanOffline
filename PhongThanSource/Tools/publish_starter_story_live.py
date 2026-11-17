"""Publish the reviewed scripts to Loose and reload each existing Lua state."""
from pathlib import Path
import hashlib,json,time,uuid,shutil
ROOT=Path(__file__).resolve().parents[2]
RUNTIME=ROOT/'PhongThanRuntime-Loose/Server'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/StarterStory20261029'
REPORT=ROOT/'analysis/starter_story_20261029'
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
manifest=json.loads((PACKAGE/'manifest.json').read_text('utf-8'))
status=json.loads((RUNTIME/'EventService/campaigns.status.json').read_text('utf-8-sig'))
world=json.loads((RUNTIME/'studio_live_world.json').read_text('utf-8'))
assert status['pid']==world['Pid'] and time.time()-world['UtcSeconds']<15
assert status['state'] not in ('running','waiting_for_players_to_leave')
pid=world['Pid']
plans=[]
for e in manifest['files']:
    assert e['role']=='Server' and e['relative'].startswith('script/') and '..' not in e['relative']
    src=PACKAGE/'compiled/Server'/e['relative'];dst=RUNTIME/e['relative']
    assert sha(src)==e['sha256'] and sha(dst) in (e['before'],e['sha256']),e['relative']
    plans.append((src,dst,e))
backup=REPORT/('backup-'+time.strftime('%Y%m%d_%H%M%S',time.gmtime()))
copied=[]
try:
    for src,dst,e in plans:
        if sha(dst)!=e['sha256']:
            saved=backup/e['relative'];saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(dst,saved)
            copied.append((dst,saved));shutil.copy2(src,dst)
        assert sha(dst)==e['sha256']
except Exception:
    for dst,saved in copied:shutil.copy2(saved,dst)
    raise
receipts=[]
for src,dst,e in plans:
    current=json.loads((RUNTIME/'studio_live_world.json').read_text('utf-8'));assert current['Pid']==pid
    command=uuid.uuid4().hex
    raw=('\\'+e['relative'].replace('/','\\')).encode('cp1252')
    payload=f'{command}\t{pid}\t{int(time.time())}\tscript\t'.encode()+raw
    temp=RUNTIME/'EventService/reload.command.tmp';temp.write_bytes(payload);temp.replace(RUNTIME/'EventService/reload.command')
    deadline=time.time()+30
    while time.time()<deadline:
        state=json.loads((RUNTIME/'EventService/campaigns.status.json').read_text('utf-8-sig'))
        if state['command']==command and state['state']=='completed':
            assert state['failed']==0 and state['processed']==1
            receipts.append(dict(path=e['relative'],command=command,pid=pid,failed=0));break
        if state['command']==command and state['state']=='completed_with_errors':raise RuntimeError(state)
        time.sleep(.3)
    else:raise TimeoutError(e['relative'])
result=dict(result='LOOSE_SCRIPTS_PUBLISHED_AND_RELOADED',world_pid=pid,world_not_restarted=True,
            files=len(plans),backup=str(backup),reloads=receipts)
(REPORT/'publication.json').write_text(json.dumps(result,indent=2)+'\n','utf-8')
print(json.dumps(dict(result=result['result'],files=len(plans),pid=pid,failed=0)),flush=True)

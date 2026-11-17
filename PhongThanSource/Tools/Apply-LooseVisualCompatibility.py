"""Deploy/rollback the reviewed client visual table overlay, never player data."""
from pathlib import Path
import argparse,json,shutil,struct,subprocess
from datetime import datetime,timezone
import build_offline_loose as b

root=Path(__file__).resolve().parents[2];client=(root/'PhongThanRuntime-Loose/Client').resolve()
out=root/'analysis/spr_visual_compat_20261018';parser=argparse.ArgumentParser();parser.add_argument('--rollback',action='store_true');args=parser.parse_args()
plan=json.loads((out/'plan.json').read_text('utf8'));index=client/'loose_index.bin'
if args.rollback:
    proc=subprocess.check_output(['powershell','-NoProfile','-Command','Get-Process Game -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path'],text=True,errors='replace')
    if str(client/'Game.exe').lower() in proc.lower():raise SystemExit('Save/logout and close main client before rollback')
    receipt=json.loads((out/'applied.json').read_text('utf8'))
    if b.sha(index.read_bytes())!=receipt['index_after_sha256']:raise SystemExit('Index changed since overlay; refuse rollback')
else:
    if (out/'applied.json').exists():raise SystemExit('Already applied')
for row in plan['files']:
    path=(client/row['path']).resolve()
    if not path.is_relative_to(client):raise ValueError('Outside client')
    expected=row['after_sha256'] if args.rollback else row['before_sha256']
    if path.exists():
        if b.sha(path.read_bytes())!=expected:raise ValueError('File changed: '+row['path'])
    elif expected:raise ValueError('Missing file: '+row['path'])
blob=index.read_bytes();assert blob[:8]==b'PTLOOSE1';pos=12;entries={}
for _ in range(struct.unpack_from('<I',blob,8)[0]):
    eid,size=struct.unpack_from('<IH',blob,pos);pos+=6;entries[eid]=blob[pos:pos+size];pos+=size
assert pos==len(blob)
additions=[]
if not args.rollback:
    backup=out/'backup';backup.mkdir(exist_ok=True);shutil.copy2(index,backup/'loose_index.bin')
    for row in plan['files']:
        path=client/row['path'];raw=('\\'+row['path']).encode('latin1');eid=b.pak_id(raw)
        if eid in entries and entries[eid].lower()!=raw.lower():raise ValueError('Index alias collision: '+row['path'])
        if eid not in entries:entries[eid]=raw;additions.append('%08X'%eid)
        if row['before_exists']:
            save=backup/row['path'];save.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(path,save)
    # Backups complete before the first table mutation. Existing client caches
    # its resource tables; new clients see the complete overlay after restart.
    for row in plan['files']:
        path=client/row['path'];source=out/'prepared'/row['path'];assert b.sha(source.read_bytes())==row['after_sha256']
        temp=path.with_suffix('.visual.tmp');temp.parent.mkdir(parents=True,exist_ok=True);temp.write_bytes(source.read_bytes());temp.replace(path)
    payload=b'PTLOOSE1'+struct.pack('<I',len(entries))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(entries.items()))
    temp=index.with_suffix('.visual.tmp');temp.write_bytes(payload);temp.replace(index)
    receipt=dict(utc=datetime.now(timezone.utc).isoformat(),files=len(plan['files']),cells=plan['summary']['cells'],
        index_before_sha256=b.sha(blob),index_after_sha256=b.sha(payload),index_additions=additions,
        client_restart_required=True,server_modified=False,player_data_modified=False,status='DEPLOYED_NATIVE_AND_VISUAL_VERIFICATION_PENDING')
    (out/'applied.json').write_text(json.dumps(receipt,indent=2),encoding='utf8');print(json.dumps(receipt))
else:
    backup=out/'backup';data=(backup/'loose_index.bin').read_bytes()
    if b.sha(data)!=receipt['index_before_sha256']:raise ValueError('Backup index changed')
    for row in plan['files']:
        path=client/row['path']
        if row['before_exists']:
            data=(backup/row['path']).read_bytes();assert b.sha(data)==row['before_sha256']
            temp=path.with_suffix('.rollback.tmp');temp.write_bytes(data);temp.replace(path)
        else:path.unlink()
    temp=index.with_suffix('.rollback.tmp');temp.write_bytes((backup/'loose_index.bin').read_bytes());temp.replace(index)
    print('Restored client visual tables/index; no player data restored')

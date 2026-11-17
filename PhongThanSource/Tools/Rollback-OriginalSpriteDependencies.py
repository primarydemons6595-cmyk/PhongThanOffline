"""Rollback only the second, original-table-proven sprite additions."""
from pathlib import Path
import hashlib
import json
import subprocess

root=Path(__file__).resolve().parents[2]
client=(root/'PhongThanRuntime-Loose/Client').resolve()
out=root/'analysis/spr_recovery_20261018/original_dependency_recovery'
j=json.loads((out/'applied.json').read_text('utf8'));sha=lambda data:hashlib.sha256(data).hexdigest()
processes=subprocess.check_output(['powershell','-NoProfile','-Command',
    'Get-Process Game -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path'],
    text=True,errors='replace')
if str(client/'Game.exe').lower() in processes.lower():raise SystemExit('Save/logout and close main client first')
index=client/'loose_index.bin';backup=out/'loose_index.before.bin'
if sha(index.read_bytes())!=j['summary']['index_after_sha256']:raise SystemExit('Index changed after recovery; preserve later edits')
if sha(backup.read_bytes())!=j['summary']['index_before_sha256']:raise SystemExit('Backup changed')
paths=[]
for row in j['records']:
    if row['status']!='RECOVERED':continue
    path=(client/row['path'].lstrip('\\').replace('\\','/')).resolve()
    if not path.is_relative_to(client) or not path.is_file() or sha(path.read_bytes())!=row['sha256']:
        raise SystemExit('Recovered payload changed: '+str(path))
    paths.append(path)
temp=index.with_suffix('.original-rollback.tmp');temp.write_bytes(backup.read_bytes());temp.replace(index)
for path in paths:path.unlink()
print('Restored previous index and removed %d unchanged additions; no player data touched'%len(paths))

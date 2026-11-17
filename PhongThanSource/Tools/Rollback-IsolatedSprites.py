"""Revert only journalled sprite additions; never restore character data."""
import json
from pathlib import Path
import subprocess
import hashlib

root = Path(__file__).resolve().parents[2]
client = (root / 'PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_05d188ae7f02/Client').resolve()
report = root / 'analysis/spr_recovery_20261018'
assert (client.parent / 'p0_isolated.flag').is_file()
processes = subprocess.check_output(
    ['powershell', '-NoProfile', '-Command',
     'Get-Process Game -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path'],
    text=True, errors='replace')
if str(client / 'Game.exe').lower() in processes.lower():
    raise SystemExit('Save/logout and close the isolated client before rollback')
journal = json.loads((report / 'applied.json').read_text('utf8'))
sha = lambda data: hashlib.sha256(data).hexdigest()
current = client / 'loose_index.bin'
integrity = json.loads((report / 'integrity.json').read_text('utf8'))
if sha(current.read_bytes()) != integrity['index_after_sha256']:
    raise SystemExit('Index changed after recovery; refuse overwriting later updates')
targets = []
for row in journal['records']:
    if row['status'] != 'RECOVERED':
        continue
    path = (client / row['path'].replace('\\', '/')).resolve()
    if not path.is_relative_to(client) or not path.is_file() or sha(path.read_bytes()) != row['sha256']:
        raise SystemExit('Recovered asset changed/missing; refuse rollback: ' + str(path))
    targets.append(path)
backup = report / 'loose_index.before.bin'
if sha(backup.read_bytes()) != integrity['index_before_sha256']:
    raise SystemExit('Backup index hash changed')
temp = current.with_suffix('.rollback.tmp')
temp.write_bytes(backup.read_bytes()); temp.replace(current)
for path in targets:
    path.unlink()
print('Restored index and removed %d unchanged additions; character data untouched' % len(targets))

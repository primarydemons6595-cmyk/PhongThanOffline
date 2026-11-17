"""Revert this verified main Loose SPR addition set, never player saves."""
from pathlib import Path
import hashlib
import json
import subprocess

root = Path(__file__).resolve().parents[2]
client = (root / 'PhongThanRuntime-Loose/Client').resolve()
out = root / 'analysis/loose_shared_update_20261018'
receipt = json.loads((out / 'sprites_applied.json').read_text('utf8'))
processes = subprocess.check_output(['powershell', '-NoProfile', '-Command',
    'Get-Process Game -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path'],
    text=True, errors='replace')
if str(client / 'Game.exe').lower() in processes.lower():
    raise SystemExit('Save and close main Loose client before sprite rollback')
sha = lambda data: hashlib.sha256(data).hexdigest()
index = client / 'loose_index.bin'
if sha(index.read_bytes()) != receipt['index_after_sha256']:
    raise SystemExit('Main index changed after deployment; preserve later edits')
backup = out / 'backup/Client/loose_index.bin'
if sha(backup.read_bytes()) != receipt['index_before_sha256']:
    raise SystemExit('Backup index hash mismatch')
targets = []
for row in receipt['files']:
    path = (client / row['path']).resolve()
    if not path.is_relative_to(client) or not path.is_file() or sha(path.read_bytes()) != row['sha256']:
        raise SystemExit('Added sprite changed/missing; refuse rollback: ' + str(path))
    targets.append(path)
temp = index.with_suffix('.rollback.tmp')
temp.write_bytes(backup.read_bytes()); temp.replace(index)
for path in targets:
    path.unlink()
print('Restored index and removed %d unchanged added sprites; no character data restored' % len(targets))

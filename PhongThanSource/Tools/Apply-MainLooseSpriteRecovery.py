"""Promote verified sprite additions only; preserve main Loose state and bindings."""
from pathlib import Path
import hashlib
import json
import shutil
import struct
from datetime import datetime, timezone
import build_offline_loose as codec

root = Path(__file__).resolve().parents[2]
main = root / 'PhongThanRuntime-Loose/Client'
isolated = root / 'PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_05d188ae7f02/Client'
out = root / 'analysis/loose_shared_update_20261018'
out.mkdir(exist_ok=True)
if (out / 'sprites_applied.json').exists():
    raise SystemExit('Already applied; preserve first deployment journal')
journal = json.loads((root / 'analysis/spr_recovery_20261018/applied.json').read_text('utf8'))
sha = lambda data: hashlib.sha256(data).hexdigest()
assert (main / 'loose_only.flag').exists()
blob = (main / 'loose_index.bin').read_bytes()
assert blob[:8] == b'PTLOOSE1'
entries = {}; pos = 12
for _ in range(struct.unpack_from('<I', blob, 8)[0]):
    eid, size = struct.unpack_from('<IH', blob, pos); pos += 6
    entries[eid] = blob[pos:pos+size]; pos += size
assert pos == len(blob)
files = []; conflicts = []; additions = []
for rec in journal['records']:
    if rec['status'] not in ('RECOVERED', 'PRESENT'):
        continue
    rel = Path(codec.logical(rec['path']).replace('\\', '/'))
    target = main / rel
    if rec['status'] == 'RECOVERED':
        data = (isolated / rel).read_bytes()
        if sha(data) != rec['sha256']:
            raise ValueError('Isolated recovered payload changed: ' + str(rel))
        if target.exists() and sha(target.read_bytes()) != rec['sha256']:
            conflicts.append(dict(path=str(rel), reason='EXISTING_MAIN_ASSET_DIFFERENT'))
            continue
        if not target.exists():
            files.append(dict(path=str(rel), sha256=rec['sha256'], bytes=len(data)))
    elif not target.is_file():
        continue
    raw = ('\\' + rec['path']).encode('latin1'); eid = codec.pak_id(raw)
    if eid not in entries:
        entries[eid] = raw; additions.append('%08X' % eid)
    elif entries[eid].lower() != raw.lower():
        old = main / entries[eid].decode('latin1').lstrip('\\').replace('\\', '/')
        new = isolated / rel if not target.exists() else target
        if not old.is_file() or sha(old.read_bytes()) != sha(new.read_bytes()):
            conflicts.append(dict(path=str(rel), id='%08X' % eid,
                                  binding=entries[eid].decode('latin1'), reason='EXISTING_ID_COLLISION_RETAINED'))
backup = out / 'backup/Client'
backup.mkdir(parents=True, exist_ok=True)
shutil.copy2(main / 'loose_index.bin', backup / 'loose_index.bin')
# Save the complete reviewed addition set before any mutation.
receipt = dict(utc=datetime.now(timezone.utc).isoformat(), files=files, index_additions=additions,
               conflicts=conflicts, index_before_sha256=sha(blob), player_data_modified=False,
               scope='MAIN_LOOSE_CLIENT_VERIFIED_SPR_ONLY', client_restart_required=True)
(out / 'sprites_plan.json').write_text(json.dumps(receipt, ensure_ascii=True, indent=2), encoding='utf8')
for rec in files:
    rel = Path(rec['path']); target = main / rel
    target.parent.mkdir(parents=True, exist_ok=True)
    data = (isolated / rel).read_bytes()
    with target.open('xb') as stream:
        stream.write(data)
    assert sha(target.read_bytes()) == rec['sha256']
payload = b'PTLOOSE1' + struct.pack('<I', len(entries)) + b''.join(
    struct.pack('<IH', eid, len(raw)) + raw for eid, raw in sorted(entries.items()))
temp = main / 'loose_index.spr-update.tmp'; temp.write_bytes(payload); temp.replace(main / 'loose_index.bin')
receipt.update(index_after_sha256=sha(payload), total_ids=len(entries), files_verified=len(files))
(out / 'sprites_applied.json').write_text(json.dumps(receipt, ensure_ascii=True, indent=2), encoding='utf8')
print(json.dumps(dict(files=len(files), index_additions=len(additions), total_ids=len(entries), conflicts=len(conflicts))))

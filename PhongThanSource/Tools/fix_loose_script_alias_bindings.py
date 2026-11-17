"""Bind existing transition aliases to identical already-loaded native names."""
import json
from pathlib import Path
import build_offline_loose as b
import build_named_loose_index as index

root = Path(__file__).resolve().parents[2] / 'PhongThanRuntime-Loose'
server = root / 'Server'
manifest = root / 'completion/Server_named_effective_manifest.csv'
rows = list(b.csv_read(manifest))
native = {}
for row in rows:
    if row['status'] != 'RESOLVED' or not row['path'].lower().startswith('script\\') or not row['path'].lower().endswith('.lua'):
        continue
    try:
        raw = ('\\' + row['path']).encode('mbcs', errors='strict')
        if raw.decode('mbcs') != '\\' + row['path']:
            continue
    except UnicodeError:
        continue
    native.setdefault(b.pak_id(raw), []).append((raw, row))
registry = server / 'script/phongthan/mapexit/registry.tsv'
original = registry.read_bytes()
lines = original.splitlines(keepends=True)
changes = []
moves = []
for number, line in enumerate(lines):
    if not line.startswith(b'0x'):
        continue
    fields = line.rstrip(b'\r\n').split(b'\t')
    assert len(fields) == 5
    eid = int(fields[0], 16)
    matches = native.get(eid, [])
    if not matches:
        continue  # Existing operational alias is still necessary.
    expected = fields[4].decode('ascii').lower()
    if any(row['sha256'].lower() != expected for _, row in matches):
        # Preserve the existing adapted transition selected by the registry.
        # Keep original payloads available by ID, but outside eager Lua scanning.
        if fields[3] != b'ADAPTED':
            raise ValueError('Unexpected differing byte-identical alias: %08X' % eid)
        for raw, row in matches:
            old = server / Path(row['path'].replace('\\', '/'))
            newname = 'recovered_original_scripts\\' + row['path']
            new = server / Path(newname.replace('\\', '/'))
            data = old.read_bytes()
            assert b.sha(data) == row['sha256'].lower()
            if new.exists() and new.read_bytes() != data:
                raise ValueError('Quarantine destination conflict')
            moves.append((old, new, data))
            changes.append(dict(id='%08X' % eid, old=row['path'], new=newname,
                                sha256=row['sha256'], action='PRESERVE_EXISTING_ADAPTED_ALIAS_EXCLUDE_DUPLICATE_FROM_EAGER_SCAN'))
            row.update(path=newname, evidence='Original bytes retained outside eager script scan; original ID mapping retained; existing ADAPTED transition preserved')
        continue
    raw, row = min(matches, key=lambda pair: pair[0])
    file = server / Path(row['path'].replace('\\', '/'))
    if b.sha(file.read_bytes()) != expected:
        raise ValueError('Native script bytes differ from registry provenance')
    if raw != fields[1]:
        changes.append(dict(id='%08X' % eid, old=fields[1].decode('mbcs'),
                            new=raw.decode('mbcs'), sha256=expected))
        fields[1] = raw
        newline = b'\r\n' if line.endswith(b'\r\n') else b'\n'
        lines[number] = b'\t'.join(fields) + newline
result = b''.join(lines)
backup = root / 'completion/Server_mapexit_registry.before_native_binding.tsv'
if not backup.exists():
    backup.write_bytes(original)
for old, new, data in moves:
    new.parent.mkdir(parents=True, exist_ok=True)
    new.write_bytes(data)
    assert new.read_bytes() == data
    old.unlink()  # Recoverable identical bytes at the recorded new path.
registry.write_bytes(result)
registry_path = 'script\\phongthan\\mapexit\\registry.tsv'
for row in rows:
    if row['path'].lower() == registry_path:
        row.update(sha256=b.sha(result), bytes=str(len(result)),
                   evidence='LOOSE_ONLY: alias rebind to already-loaded hash-identical original script path; Lua bytes unchanged')
b.csv_write(manifest, rows, ['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
index.run(root)
(root / 'completion/Server_script_alias_binding_repair.json').write_text(json.dumps(changes, indent=2), encoding='utf-8')
print('HASH_IDENTICAL_NATIVE_ALIAS_BINDINGS', len(changes))

"""Recover only three hash-proven dynamically named active item tables."""
import argparse
import json
import struct
from pathlib import Path
import build_offline_loose as b
import build_named_loose_index as index

TABLES = (
    ('TownPortal.txt', 0x0DAB985B, 'serverlist.pak', 116, 242),
    ('potion.txt', 0x814BC927, 'serverlist.pak', 1216, 4839),
    ('skillbook.txt', 0x268A85C9, 'vng00.pak', 642, 13512),
)


def run(workspace):
    workspace = workspace.resolve()
    root = workspace / 'PhongThanRuntime-Loose'
    plans = []
    for role in ('Client', 'Server'):
        winners, _ = b.pak_index(b.package_chain(workspace / 'PhongThanRuntime-Staging' / role), strict=False)
        manifest = root / 'completion' / (role + '_named_effective_manifest.csv')
        rows = list(b.csv_read(manifest))
        for name, eid, pak, number, size in TABLES:
            logical = 'settings\\item\\001\\' + name
            assert b.pak_id(('\\' + logical).encode('ascii')) == eid
            entry = winners[eid]
            if (entry['pak'].name.lower(), entry['entry'], entry['size']) != (pak, number, size):
                raise ValueError('Active PAK winner changed: ' + logical)
            data, method = b.entry_payload(entry, {}, None)
            if b'\t' not in data or b'\n' not in data or b'\0' in data:
                raise ValueError('Not a plain tabular payload: ' + logical)
            target = root / role / Path(logical.replace('\\', '/'))
            if target.exists() and target.read_bytes() != data:
                raise ValueError('Refuse to overwrite different existing data: ' + str(target))
            row = dict(path=logical, id='%08X' % eid, status='RESOLVED',
                       origin='ACTIVE_PAK_DYNAMIC_PATH_RECOVERY', pak=pak,
                       entry=str(number), sha256=b.sha(data), bytes=str(len(data)),
                       evidence='KBasPropTbl.CPP: active item version 001 + authentic filename; signed ANSI path hash matches native winning entry', error='')
            existing = [r for r in rows if r['path'].lower() == logical.lower()]
            if existing and any(r['sha256'].lower() != row['sha256'] for r in existing):
                raise ValueError('Manifest conflict: ' + logical)
            rows = [r for r in rows if r['path'].lower() != logical.lower()] + [row]
            plans.append((target, data, dict(role=role, source=str(entry['pak']),
                                           compression=method, **row)))
        plans.append((manifest, rows, None))
    provenance = []
    fields = ['path', 'id', 'status', 'origin', 'pak', 'entry', 'sha256', 'bytes', 'evidence', 'error']
    for target, data, record in plans:
        if record is None:
            backup = target.with_suffix(target.suffix + '.before_required_item_tables')
            if not backup.exists():
                backup.write_bytes(target.read_bytes())
            b.csv_write(target, data, fields)
        else:
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
            if b.sha(target.read_bytes()) != record['sha256']:
                raise ValueError('Post-write hash mismatch')
            provenance.append(record)
            print(record['role'], record['path'], record['sha256'], flush=True)
    for role in ('Client', 'Server'):
        target = root / role / 'loose_index.bin'
        backup = target.with_suffix('.bin.before_required_item_tables')
        if not backup.exists():
            backup.write_bytes(target.read_bytes())
    index.run(root)
    for role in ('Client', 'Server'):
        data = (root / role / 'loose_index.bin').read_bytes()
        count = struct.unpack_from('<I', data, 8)[0]
        offset = 12
        entries = {}
        for _ in range(count):
            eid, length = struct.unpack_from('<IH', data, offset)
            offset += 6
            entries[eid] = data[offset:offset + length]
            offset += length
        for name, eid, _, _, _ in TABLES:
            assert entries[eid].lower() == ('\\settings\\item\\001\\' + name).encode('ascii').lower()
    (root / 'completion' / 'required_item_tables_recovery.json').write_text(
        json.dumps(dict(records=provenance, hash_and_index_verified=True,
                        runtime_startup_verified=False), indent=2), encoding='utf-8')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, required=True)
    run(parser.parse_args().workspace)

"""Recover source-named SPR dependencies into the existing isolated client only."""
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import shutil
import struct
import build_offline_loose as b

ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / 'PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_05d188ae7f02/Client'
OUT = ROOT / 'analysis/spr_recovery_20261018'


def run(apply=False):
    assert (CLIENT.parent / 'p0_isolated.flag').exists()
    OUT.mkdir(exist_ok=True)
    if apply and (OUT / 'applied.json').exists():
        raise ValueError('Recovery already applied; retain original journal and rollback backup')
    refs = {}

    def add(raw, evidence):
        try:
            name = b.logical(raw.decode('latin1'))
        except (ValueError, UnicodeError):
            return
        if name.lower().endswith('.spr'):
            refs.setdefault(name.lower(), {'path': name, 'evidence': set()})['evidence'].add(evidence)

    settings = CLIENT / 'settings/npcres'
    kind = settings / bytes.fromhex('c8cbceefc0e0d0cd2e747874').decode('latin1')
    normal = settings / bytes.fromhex('c6d5cda86e7063d7cad4b42e747874').decode('latin1')
    normal_rows = {r.split(b'\t')[0]: r.split(b'\t')[1:]
                   for r in normal.read_bytes().splitlines()[1:] if r}
    for line in kind.read_bytes().splitlines()[1:]:
        cells = line.split(b'\t')
        if len(cells) < 3:
            continue
        for cell in normal_rows.get(cells[0], []):
            if cell.lower().endswith(b'.spr'):
                add(cells[2].rstrip(b'\\') + b'\\' + cell, 'NormalNpc:' + cells[0].decode('latin1'))
    # All original player part tables compose their filenames under human ResFilePath.
    normal_names = {normal.name, 'pu_tong_npc_zi_yuan.txt'}
    for file in settings.glob('*.txt'):
        if file.name in normal_names:
            continue
        for cell in re.split(rb'[\t\r\n]', file.read_bytes()):
            cell = cell.strip()
            if cell.lower().endswith(b'.spr'):
                add(cell if b'\\' in cell else b'spr\\npcres\\human\\' + cell,
                    'PlayerPartTable:' + file.name)
    # Exact absolute/SPR-root paths in item catalog, effects and runtime failures.
    for file in (CLIENT / 'settings').rglob('*'):
        if file.is_file() and file.suffix.lower() in ('.txt', '.ini'):
            for raw in re.findall(rb'(?:\\?spr\\)[^\t\r\n"=]*?\.spr', file.read_bytes(), re.I):
                add(raw, 'Settings:' + str(file.relative_to(CLIENT)))
    log = CLIENT / 'loose_missing_files.log'
    for raw in re.findall(rb'path=(.*?) result=MISSING', log.read_bytes()):
        if b'npcres\\' in raw.lower() or b'\\item\\' in raw.lower():
            add(raw, 'RuntimeMissing')

    original = ROOT / 'Client_Original'
    listed = [original / 'data' / x for x in re.findall(
        r'^\d+=([^\r\n]+)', (original / 'package.ini').read_text('ascii'), re.M)]
    winners, rejected = b.pak_index([p for p in listed if p.is_file()], strict=False)
    records = []
    for n, rec in enumerate(refs.values()):
        name = rec['path']
        rec['evidence'] = sorted(rec['evidence'])
        target = CLIENT / name.replace('\\', '/')
        if target.is_file():
            rec.update(status='PRESENT', bytes=target.stat().st_size)
        else:
            raw = ('\\' + name).encode('latin1')
            eid = b.pak_id(raw)
            rec['id'] = '%08X' % eid
            entry = winners.get(eid)
            # Main Loose is a read-only named recovery source, never the deploy target.
            loose = ROOT / 'PhongThanRuntime-Loose/Client' / name.replace('\\', '/')
            try:
                if entry:
                    data, method = b.entry_payload(entry, None, None)
                    rec.update(source=str(entry['pak']), entry=entry['entry'], method=method)
                elif loose.is_file():
                    data = loose.read_bytes()
                    rec.update(source=str(loose), method='EXACT_NAMED_MAIN_LOOSE_FALLBACK')
                else:
                    rec['status'] = 'SOURCE_PAYLOAD_MISSING'
                    records.append(rec)
                    continue
                if not (b.sprite_blob_valid(data, True) or b.sprite_blob_valid(data, False)):
                    raise ValueError('SPR header/frame/RLE validation failed')
                rec.update(sha256=b.sha(data), bytes=len(data), frames=struct.unpack_from('<H', data, 12)[0],
                           status='RECOVERED' if apply else 'RECOVERABLE')
                if apply:
                    target.parent.mkdir(parents=True, exist_ok=True)
                    # Never replace an existing asset or write through a hard link.
                    with target.open('xb') as handle:
                        handle.write(data)
                    assert b.sha(target.read_bytes()) == rec['sha256']
            except Exception as exc:
                rec.update(status='DECODE_BLOCKED', error=str(exc))
        records.append(rec)
        if n % 500 == 0:
            print(n, dict(Counter(r['status'] for r in records)), flush=True)
    # Retain every existing binding; add only proven missing IDs, refuse collisions.
    index_path = CLIENT / 'loose_index.bin'
    blob = index_path.read_bytes()
    assert blob[:8] == b'PTLOOSE1'
    count = struct.unpack_from('<I', blob, 8)[0]
    entries = {}; pos = 12
    for _ in range(count):
        eid, length = struct.unpack_from('<IH', blob, pos); pos += 6
        entries[eid] = blob[pos:pos+length]; pos += length
    assert pos == len(blob)
    additions = []
    for rec in records:
        if rec['status'] not in ('RECOVERED', 'PRESENT'):
            continue
        raw = ('\\' + rec['path']).encode('latin1')
        eid = b.pak_id(raw)
        if eid not in entries:
            entries[eid] = raw; additions.append(eid)
        elif entries[eid].lower() != raw.lower():
            old = CLIENT / entries[eid].decode('latin1').lstrip('\\').replace('\\', '/')
            new = CLIENT / rec['path'].replace('\\', '/')
            if not old.is_file() or b.sha(old.read_bytes()) != b.sha(new.read_bytes()):
                rec['index_conflict'] = entries[eid].decode('latin1')
    if apply and additions:
        backup = OUT / 'loose_index.before.bin'
        if not backup.exists():
            shutil.copy2(index_path, backup)
        payload = b'PTLOOSE1' + struct.pack('<I', len(entries)) + b''.join(
            struct.pack('<IH', eid, len(raw)) + raw for eid, raw in sorted(entries.items()))
        temp = index_path.with_suffix('.spr-recovery.tmp'); temp.write_bytes(payload); temp.replace(index_path)
    summary = dict(counts=dict(Counter(r['status'] for r in records)), references=len(records),
                   index_additions=len(additions), apply=apply,
                   missing_original_packages=[str(p) for p in listed if not p.is_file()],
                   client_restart_required=bool(apply and additions),
                   acceptance='ASSET_RECOVERY_ONLY_REAL_CLIENT_VISUAL_TEST_PENDING')
    summary['index_after_sha256'] = b.sha(index_path.read_bytes())
    (OUT / ('applied.json' if apply else 'plan.json')).write_text(
        json.dumps(dict(summary=summary, records=records), ensure_ascii=True, indent=2), encoding='utf8')
    print(json.dumps(summary), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    run(parser.parse_args().apply)

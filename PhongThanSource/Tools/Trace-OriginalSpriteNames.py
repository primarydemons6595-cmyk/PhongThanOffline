"""Read original PAK tables, preserve filename bytes, then resolve exact SPR IDs."""
from pathlib import Path
from collections import Counter
import json
import re
import build_offline_loose as b

root = Path(__file__).resolve().parents[2]
original = root / 'Client_Original'
out = root / 'analysis/spr_recovery_20261018/original_name_trace'
out.mkdir(exist_ok=True)
listed = [original / 'data' / n for n in re.findall(
    r'^\d+=([^\r\n]+)', (original / 'package.ini').read_text('ascii'), re.M)]
diagnostics = []
winners, all_entries = b.pak_index([p for p in listed if p.is_file()], strict=False, diagnostics=diagnostics)
tables = {}; table_errors = []; references = {}


def load(raw):
    raw = b'\\' + raw.lstrip(b'\\')
    if raw in tables:
        return tables[raw]['data']
    eid = b.pak_id(raw); entry = winners.get(eid)
    if not entry:
        table_errors.append(dict(path_hex=raw.hex(), id='%08X' % eid, status='ORIGINAL_TABLE_NOT_FOUND'))
        return None
    try:
        data, method = b.entry_payload(entry, None, None)
    except Exception as exc:
        table_errors.append(dict(path_hex=raw.hex(), error=str(exc)))
        return None
    tables[raw] = dict(data=data, id='%08X' % eid, pak=entry['pak'].name,
                       entry=entry['entry'], sha256=b.sha(data))
    return data


def add(base, cell, table, row, column, actor):
    if not cell.lower().endswith(b'.spr'):
        return
    # ComposePathAndName joins the row's ResFilePath and cell, even when a
    # cell begins with a slash. Retain this exact source contract here.
    raw = b'\\' + base.strip(b'\\') + b'\\' + cell
    eid = b.pak_id(raw)
    key = raw.hex()
    record = references.setdefault(key, dict(path=raw.decode('latin1'), path_hex=key,
        id='%08X' % eid, evidence=[]))
    record['evidence'].append(dict(table_hex=table.hex(), row=row, column=column,
                                  actor_hex=actor.hex(), base_hex=base.hex(), cell_hex=cell.hex()))


prefix = b'\\settings\\npcres\\'
kind_name = prefix + bytes.fromhex('c8cbceefc0e0d0cd2e747874')
normal_name = prefix + bytes.fromhex('c6d5cda86e7063d7cad4b42e747874')
kind = load(kind_name); normal = load(normal_name)
if kind is None or normal is None:
    raise SystemExit('Required original tables absent; no loose-derived inference')
normal_rows = {}
for rowno, line in enumerate(normal.splitlines()[1:], 2):
    cells = line.split(b'\t')
    normal_rows.setdefault(cells[0], (rowno, cells))
for rowno, line in enumerate(kind.splitlines()[1:], 2):
    cells = line.split(b'\t')
    if len(cells) < 3:
        continue
    actor, base = cells[0], cells[2]
    special = len(cells) > 1 and cells[1] == bytes.fromhex('ccd8cae26e7063')
    if not special:
        entry = normal_rows.get(actor)
        if entry:
            for col, cell in enumerate(entry[1][1:], 2):
                add(base, cell, normal_name, entry[0], col, actor)
        continue
    # Character part filenames are explicitly named in original kind columns
    # 8..16. No prefix inferred from sprite spelling or translated filenames.
    for name in cells[7:16]:
        if not name.lower().endswith(b'.txt'):
            continue
        table_name = prefix + name
        data = load(table_name)
        if data is None:
            continue
        for partrow, partline in enumerate(data.splitlines()[1:], 2):
            for col, cell in enumerate(partline.split(b'\t')[1:], 2):
                add(base, cell, table_name, partrow, col, actor)

old = json.loads((out.parent / 'applied.json').read_text('utf8'))
old_missing = {r['path'].replace('/', '\\').lstrip('\\').lower(): r
               for r in old['records'] if r['status'] == 'SOURCE_PAYLOAD_MISSING'}
by_id = {}
for entry in all_entries:
    by_id.setdefault(entry['id'], []).append(entry)
for rec in references.values():
    eid = int(rec['id'], 16); winner = winners.get(eid)
    rec['status'] = 'ORIGINAL_NAME_ID_FOUND' if winner else 'ORIGINAL_NAME_ID_NOT_IN_AVAILABLE_CHAIN'
    if winner:
        rec.update(pak=winner['pak'].name, entry=winner['entry'])
    rec['previously_reported_missing'] = rec['path'].lstrip('\\').lower() in old_missing
    rec['all_archive_hits'] = [dict(pak=e['pak'].name, entry=e['entry']) for e in by_id.get(eid, [])]

table_manifest = []
for raw, rec in tables.items():
    table_manifest.append(dict(path_hex=raw.hex(), **{k:v for k,v in rec.items() if k != 'data'}))
    target = out / 'original_tables' / (rec['id'] + '.bin')
    target.parent.mkdir(exist_ok=True); target.write_bytes(rec['data'])
previous = [r for r in references.values() if r['previously_reported_missing']]
original_paths = {r['path'].lstrip('\\').lower() for r in references.values()}
summary = dict(original_tables=len(tables), original_references=len(references),
    counts=dict(Counter(r['status'] for r in references.values())),
    previous_missing_confirmed_by_original=len(previous),
    previous_missing_confirmed_but_id_found=sum(r['status']=='ORIGINAL_NAME_ID_FOUND' for r in previous),
    previous_missing_not_in_this_original_graph=sum(n not in original_paths for n in old_missing),
    missing_packages=[p.name for p in listed if not p.is_file()], table_errors=len(table_errors),
    note='Not-in-graph includes settings outside NPC/player tables and runtime names; not proof of invalid name.')
(out / 'trace.json').write_text(json.dumps(dict(summary=summary,tables=table_manifest,
    table_errors=table_errors,package_diagnostics=diagnostics,records=list(references.values())),
    ensure_ascii=True,indent=2),encoding='utf8')
print(json.dumps(summary),flush=True)

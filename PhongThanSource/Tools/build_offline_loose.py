"""Offline, read-only input resolver and byte-exact loose content exporter.

No game binaries, source, live runtime, PAKs or character stores are modified.
Unknown names stay unknown. A partial export is NEVER certified PAK-free.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import shutil
import struct
import ctypes
from pathlib import Path, PureWindowsPath

ROOTS = ('settings', 'script', 'ui', 'spr', 'maps', 'font', 'sound',
         'you_xi_zi_yuan', '\u00d3\u00ce\u00cf\u00b7\u00d7\u00ca\u00d4\u00b4')


def sha(data):
    return hashlib.sha256(data).hexdigest()


def logical(path):
    p = PureWindowsPath(path.replace('/', '\\'))
    if p.drive or '..' in p.parts or any(':' in x for x in p.parts):
        raise ValueError('Unsafe content path: ' + path)
    parts = [x for x in p.parts if x not in ('\\', '.', '')]
    if not parts or any(x.endswith((' ', '.')) or any(c in x for c in '<>"|?*\0') for x in parts):
        raise ValueError('Invalid Windows content path: ' + path)
    if any(x.split('.')[0].upper() in {'CON', 'PRN', 'AUX', 'NUL',
            *('COM'+str(n) for n in range(1, 10)), *('LPT'+str(n) for n in range(1, 10))} for x in parts):
        raise ValueError('Reserved Windows content path: ' + path)
    return '\\'.join(parts)


def pak_id(raw):
    value = 0
    raw = b'\\' + raw.replace(b'/', b'\\').lstrip(b'\\')
    for i, b in enumerate(raw, 1):
        if 65 <= b <= 90:
            b += 32
        signed = b if b < 128 else b - 256
        value = (((value + i * signed) & 0xffffffff) % 0x8000000b * 0xffffffef) & 0xffffffff
    return value ^ 0x12345678


def ids_for(path):
    result = set()
    for enc in ('latin1', 'cp936', 'cp1252'):
        try:
            result.add(pak_id(path.encode(enc)))
        except UnicodeEncodeError:
            pass
    return result


def csv_read(path):
    with path.open(encoding='utf-8-sig', newline='') as f:
        yield from csv.DictReader(f)


def csv_write(path, rows, fields):
    with path.open('w', encoding='utf-8-sig', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fields, extrasaction='ignore')
        writer.writeheader()
        writer.writerows(rows)


def package_chain(root):
    text = (root / 'package.ini').read_text('ascii')
    section = re.search(r'(?ims)^\[Package\]\s*$(.*?)(?=^\[|\Z)', text)
    if not section:
        raise ValueError('Missing Package section')
    values = dict(re.findall(r'(?m)^([^=\r\n]+)=(.*)$', section[1]))
    folder = logical(values['Path'].strip())
    result = []
    n = 0
    while str(n) in values and values[str(n)].strip():
        name = logical(values[str(n)].strip())
        path = root / folder / name
        if not path.is_file():
            raise ValueError('Listed package missing: ' + str(path))
        result.append(path)
        n += 1
    if not result:
        raise ValueError('Empty package chain')
    return result


def pak_index(chain, strict=True, diagnostics=None):
    winners, all_entries = {}, []
    for pak in chain:
        with pak.open('rb') as f:
            header = f.read(32)
            if len(header) != 32:
                if strict:
                    raise ValueError('Invalid PAK header: ' + str(pak))
                if diagnostics is not None:
                    diagnostics.append(dict(pak=pak.name, error='NATIVE_OPEN_HEADER_REJECT'))
                continue
            magic, count, offset, data_offset = struct.unpack_from('<4sIII', header)
            length = pak.stat().st_size
            if (magic != b'PACK' or not 0 < count <= min(2000000, length // 16) or
                    not 32 <= offset < length or not 32 <= data_offset < length or offset + count * 16 > length):
                if strict:
                    raise ValueError('Invalid PAK index: ' + str(pak))
                if diagnostics is not None:
                    diagnostics.append(dict(pak=pak.name, error='NATIVE_OPEN_INDEX_REJECT'))
                continue
            f.seek(offset)
            table = f.read(count * 16)
            descriptors = list(struct.iter_unpack('<IIII', table))
            if any(not eid or not 0 < size < 0x80000000 or not (flags & 0xffffff) or
                   pos + (flags & 0xffffff) > length for eid, pos, size, flags in descriptors):
                if strict:
                    raise ValueError('Invalid PAK entry; native rejects entire archive: ' + str(pak))
                if diagnostics is not None:
                    diagnostics.append(dict(pak=pak.name, error='NATIVE_OPEN_ENTRY_REJECT'))
                continue
            local, first, duplicates = [], {}, set()
            for n, (eid, pos, size, flags) in enumerate(descriptors):
                entry = dict(pak=pak, entry=n, id=eid, offset=pos, size=size, flags=flags)
                local.append(entry)
                all_entries.append(entry)
                if eid in first:
                    duplicates.add(eid)
                first.setdefault(eid, entry)
            # Native Open does not require sorting. Emulate its search even on
            # an unsorted accepted index; unreachable IDs fall through to later PAKs.
            reachable = {}
            for eid in first:
                lo, hi = 0, len(local) - 1
                while lo <= hi:
                    mid = (lo + hi) // 2
                    if eid < local[mid]['id']:
                        hi = mid - 1
                    elif eid > local[mid]['id']:
                        lo = mid + 1
                    else:
                        reachable[eid] = local[mid]
                        break
            for eid, entry in reachable.items():
                winners.setdefault(eid, entry)
    return winners, all_entries


def copy_match(out, off, count):
    """Validated LZ match; bound scratch storage independently of offset."""
    while count:
        take = min(count, 65536)
        start = len(out) - off
        if take <= off:
            out.extend(out[start:start + take])
        else:
            pattern = out[start:start + off]
            out.extend((pattern * ((take + off - 1) // off))[:take])
        count -= take


def nrv2b_python(src, expected):
    pos, bits, last = 0, 0, 1
    out = bytearray()

    def byte():
        nonlocal pos
        if pos >= len(src):
            raise ValueError('Truncated NRV2B')
        b = src[pos]
        pos += 1
        return b

    def bit():
        nonlocal bits
        bits = bits * 2 if bits & 127 else byte() * 2 + 1
        return (bits >> 8) & 1

    while True:
        while bit():
            out.append(byte())
            if len(out) > expected:
                raise ValueError('NRV2B output overflow')
        off = 1
        while True:
            off = off * 2 + bit()
            if off > 0x1000002:
                raise ValueError('NRV2B invalid offset')
            if bit():
                break
        if off == 2:
            off = last
        else:
            off = ((off - 3) * 256 + byte()) & 0xffffffff
            if off == 0xffffffff:
                break
            off += 1
            last = off
        length = bit() * 2 + bit()
        if not length:
            length = 1
            while True:
                length = length * 2 + bit()
                if length > expected:
                    raise ValueError('NRV2B invalid length')
                if bit():
                    break
            length += 2
        length += off > 0xd00
        if not 0 < off <= len(out) or len(out) + length + 1 > expected:
            raise ValueError('NRV2B invalid backreference')
        # LZ77 overlapping backreferences repeat the existing offset-sized
        # suffix. Copy in C-backed byte blocks rather than Python per byte.
        # Never copy an offset-sized suffix when the match is tiny. Bound each
        # temporary allocation even for multi-megabyte overlapping matches.
        copy_match(out, off, length + 1)
    if len(out) != expected or pos != len(src):
        raise ValueError('NRV2B size/trailing data mismatch')
    return bytes(out)


_native_nrv = None


def nrv2b(src, expected):
    global _native_nrv
    if _native_nrv is None:
        dll = Path(__file__).with_name('offline_nrv2b.dll')
        _native_nrv = False
        if dll.exists():
            try:
                library = ctypes.CDLL(str(dll))
                fn = library.pt_nrv2b
                fn.argtypes = [ctypes.c_char_p, ctypes.c_uint, ctypes.c_void_p, ctypes.c_uint]
                fn.restype = ctypes.c_int
                _native_nrv = fn
            except OSError:
                pass  # Another host architecture can still use the Python path.
    if not _native_nrv:
        return nrv2b_python(src, expected)
    if not 0 <= expected <= 64*1024*1024 or len(src) > 0xffffffff:
        raise ValueError('NRV2B extraction bound')
    output = ctypes.create_string_buffer(expected)
    if _native_nrv(src, len(src), output, expected):
        raise ValueError('Invalid/truncated NRV2B native decode')
    return output.raw


def entry_payload(entry, inventory, recovery, oracle=None):
    """Decode actual selected bytes; historical caches are not version authority."""
    with entry['pak'].open('rb') as f:
        f.seek(entry['offset'])
        packed = f.read(entry['flags'] & 0xffffff)
    method = entry['flags'] >> 24
    # Current PAK bytes, not old inventory EntryIndex/SHA, are authoritative.
    if not 0 <= entry['size'] <= 64 * 1024 * 1024:
        raise ValueError('Entry exceeds extraction safety bound')
    if method in (0, 1):
        data = packed if method == 0 else nrv2b(packed, entry['size'])
        if len(data) != entry['size']:
            raise ValueError('Decoded size mismatch')
        return data, 'PAK_DECODED'
    if method == 0x11:
        return frame_sprite(packed, entry['size'])
    # Streaming/frame-compressed SPR must not be emitted as a fake full SPR.
    raise ValueError('Unsupported compression (historical cache not version proof): %02X' % method)


def sprite_rle(data, width, height, colors=None):
    """KSpriteValidation.h indexed / true-color RLE, including line bounds."""
    if not 0 < width <= 8192 or not 0 < height <= 8192:
        return False
    done = pos = line = 0
    pixels = width * height
    while done < pixels:
        if pos + 2 > len(data):
            return False
        run, alpha = data[pos:pos+2]
        pos += 2
        if not run or run > pixels - done or (colors is not None and run > width - line):
            return False
        if alpha:
            length = run if colors is not None else run * 3
            if pos + length > len(data):
                return False
            if colors is not None and max(data[pos:pos+length]) >= colors:
                return False
            pos += length
        done += run
        line = (line + run) % width
    return colors is None or line == 0


def sprite_blob_valid(blob, palette):
    if len(blob) < 32 or blob[:3] != b'SPR':
        return False
    width, height, _, _, frames, colors = struct.unpack_from('<6H', blob, 4)
    if not (0 < width <= 8192 and 0 < height <= 8192 and 0 < frames <= 4096 and 0 < colors <= 256):
        return False
    table = 32 + (colors * 3 if palette else 0)
    start = table + frames * 8
    if start > len(blob):
        return False
    for offset, length in struct.iter_unpack('<II', blob[table:start]):
        if length < 8 or start + offset + length > len(blob):
            return False
        frame = blob[start+offset:start+offset+length]
        w, h = struct.unpack_from('<HH', frame)
        if not sprite_rle(frame[8:], w, h, colors if palette else None):
            return False
    return True


def frame_sprite(packed, expected):
    """Expand XPackSprFrameInfo to standard SPROFFS, no pixel/color changes.

    Contracts: XPackFile.cpp GetSprHeader/GetSprFrame, KSprite.h and
    KSpriteValidation.h; loose KPakFile checks true-color before indexed.
    """
    if len(packed) < 32 or packed[:3] != b'SPR':
        raise ValueError('SPR_FRAME invalid header')
    width, height, _, _, frames, colors = struct.unpack_from('<6H', packed, 4)
    if not (0 < width <= 8192 and 0 < height <= 8192 and 0 < frames <= 4096 and 0 < colors <= 256):
        raise ValueError('SPR_FRAME invalid dimensions/count/palette')
    variants = []
    for palette in (True, False):
        table = 32 + (colors * 3 if palette else 0)
        prefix = table + frames * 8
        if prefix > len(packed):
            continue
        desc = list(struct.iter_unpack('<ii', packed[table:prefix]))
        if any(c <= 0 or not 8 <= abs(r) <= 64*1024*1024 for c, r in desc):
            continue
        if prefix + sum(c for c, _ in desc) != len(packed):
            continue
        # Native XPackFile validates stored boundaries and individual raw
        # frames; Index.lSize is NOT the sum of expanded frame sizes. Real PT
        # entries can count an extra global palette in that metadata field.
        # Bound the independently computed output instead of inventing bytes
        # to make it equal metadata.
        if prefix + sum(abs(r) for _, r in desc) > 64*1024*1024:
            continue
        try:
            offset = 0
            pos = prefix
            offsets, bodies = bytearray(), bytearray()
            for stored, raw in desc:
                src = packed[pos:pos+stored]
                body = src if raw < 0 else nrv2b(src, raw)
                if len(body) != abs(raw):
                    raise ValueError('SPR_FRAME raw length mismatch')
                offsets.extend(struct.pack('<II', offset, len(body)))
                bodies.extend(body)
                offset += len(body)
                pos += stored
            blob = packed[:table] + offsets + bodies
            if not sprite_blob_valid(blob, palette):
                continue
            # Loose loader tries RGB first: reject a palette blob it could
            # interpret differently, rather than silently changing its colors.
            if palette and sprite_blob_valid(blob, False):
                continue
            variants.append((bytes(blob), 'PAK_FRAME_EXPANDED_INDEXED' if palette else 'PAK_FRAME_EXPANDED_RGB'))
        except (ValueError, IndexError):
            continue
    if len(variants) != 1:
        raise ValueError('SPR_FRAME ambiguous or invalid native/loose contract')
    return variants[0]


def source_references(source):
    # C/C++ literals only. Macro concatenation / formatted paths stay explicit gaps.
    rows = []
    for file in source.rglob('*'):
        if file.suffix.lower() not in ('.cpp', '.h', '.inl'):
            continue
        text = file.read_bytes().decode('cp936', errors='replace')
        for match in re.finditer(r'"((?:\\.|[^"\\\r\n])*)"', text):
            path = match[1].replace('\\\\', '\\').replace('\\/', '/')
            if not re.match(r'^[\\/](settings|script|ui|spr|maps|font|sound)[\\/]', path, re.I):
                continue
            rows.append(dict(path=path, caller=str(file.relative_to(source)),
                line=text.count('\n', 0, match.start()) + 1,
                kind='DYNAMIC_OR_UNSAFE' if '%' in path or '\ufffd' in path else 'STATIC_LITERAL'))
    return rows


def check_output(workspace, runtime, recovery, output, allow_existing=False):
    protected = [runtime, recovery, workspace / 'PhongThanSource', workspace / 'Client_Original',
                 workspace / 'PhongThanRuntime-State']
    if output == workspace or any(output == p or output.is_relative_to(p) or p.is_relative_to(output) for p in protected):
        raise ValueError('Output overlaps protected input/runtime/source')
    if output.exists() and not allow_existing:
        raise ValueError('Output must be a NEW directory; no overwrite/delete')


def source04_reports(output, summary):
    """Proven ASCII source names -> historical unresolved IDs -> current bytes."""
    historical = {}
    historical_paks = {}
    for row in csv_read(Path(summary['input_recovery']) / '05_Reports/UNRESOLVED_IDS.csv'):
        eid = row['IdHex'].upper()
        historical.setdefault(eid, set()).add(row['SHA256'].lower())
        historical_paks.setdefault(eid, set()).add(row['PakName'])
    refs = set()
    for row in csv_read(output / 'source_references.csv'):
        if row['kind'] == 'STATIC_LITERAL' and row['path'].isascii():
            try:
                refs.add(logical(row['path']).casefold())
            except ValueError:
                pass
    for role in summary['roles']:
        rows = []
        for row in csv_read(output / (role + '_manifest.csv')):
            if row['status'] != 'RESOLVED' or row['path'].casefold() not in refs:
                continue
            # A current loose override still identifies the historical ID by
            # its proven ASCII source filename, even without an active PAK row.
            eid = row['id'].upper() or '%08X' % pak_id(row['path'].encode('ascii'))
            prior = historical.get(eid)
            if not prior:
                continue
            row = dict(row)
            row['id'] = eid
            row['historical04_packages'] = ';'.join(sorted(historical_paks[eid]))
            row['evidence'] = 'ASCII_SOURCE_LITERAL_AND_NATIVE_FILENAME_HASH'
            row['historical04_payload'] = ('SAME_SHA256' if row['sha256'] in prior else
                'DIFFERENT_VERSION_CURRENT_RUNTIME_LOOSE_USED' if row['origin']=='RUNTIME_LOOSE' else
                'DIFFERENT_VERSION_CURRENT_PAK_USED')
            rows.append(row)
        csv_write(output / (role + '_source04_mappings.csv'), rows,
                  ['path', 'origin', 'pak', 'entry', 'id', 'sha256', 'bytes', 'evidence',
                   'historical04_packages', 'historical04_payload'])
        summary['roles'][role]['source04_mapped'] = len(rows)
    return summary


def annotate_source04(args):
    output = args.output.resolve()
    summary = json.loads((output / 'summary.json').read_text('utf-8'))
    if summary.get('schema') != 1 or not summary.get('roles'):
        raise ValueError('Not an offline exporter output')
    workspace = args.workspace.resolve()
    for protected in (Path(summary['input_runtime']).resolve(), Path(summary['input_recovery']).resolve(),
                      workspace / 'PhongThanSource', workspace / 'Client_Original', workspace / 'PhongThanRuntime-State'):
        if output == protected or output.is_relative_to(protected) or protected.is_relative_to(output):
            raise ValueError('Report output overlaps protected input')
    source04_reports(output, summary)
    (output / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    print('Source04 mappings:', {r: s['source04_mapped'] for r, s in summary['roles'].items()})


def retry_frame_exports(args):
    """Only fill previously blocked frame SPR paths in a generated export."""
    annotate_source04(args)  # Validate existing export/protected input boundaries.
    output = args.output.resolve()
    summary = json.loads((output / 'summary.json').read_text('utf-8'))
    if not summary['materialized']:
        raise ValueError('Retry requires a materialized offline export')
    runtime = Path(summary['input_runtime'])
    recovery = Path(summary['input_recovery'])
    exact, aliases = {}, {}
    for r in csv_read(recovery / '05_Reports/EXACT_MAPPINGS.csv'):
        try:
            path = logical(r.get('OutputPath') or r['RuntimePath'] or r['CanonicalPath'])
            eid = int(r['IdHex'], 16)
            if eid not in ids_for(r.get('HashSpelling') or r['CanonicalPath']):
                continue
            exact[path.casefold()] = eid
            aliases[logical(r['CanonicalPath']).casefold()] = path.casefold()
        except (ValueError, KeyError):
            continue
    plans, needed = {}, 0
    for role in summary['roles']:
        chain = package_chain(runtime / role)
        if [p.name for p in chain] != summary['roles'][role]['package_order']:
            raise ValueError('Package order changed; create a new snapshot')
        winners, _ = pak_index(chain, strict=False)
        rows = list(csv_read(output / (role + '_manifest.csv')))
        tasks = []
        for row in rows:
            if row['status'] != 'BLOCKED' or not row['path'].lower().endswith('.spr'):
                continue
            path = logical(row['path'])
            eid = exact.get(path.casefold())
            if eid is None:
                matches = ids_for(path) & winners.keys()
                if len(matches) == 1:
                    eid = next(iter(matches))
            entry = winners.get(eid)
            if entry and entry['flags'] >> 24 == 0x11:
                tasks.append((row, path, entry))
                needed += entry['size']
        plans[role] = (rows, tasks)
    if shutil.disk_usage(output).free < needed + 1024*1024*1024:
        raise ValueError('Insufficient free space for all frame outputs plus 1GiB reserve')
    print('Frame retry preflight: bytes=', needed, 'paths=', sum(len(t) for _, t in plans.values()), flush=True)
    for role, (rows, tasks) in plans.items():
        errors = {r['path'].casefold(): r for r in csv_read(output / (role + '_blocked.csv'))}
        new_ids, new_paths = set(), set()
        for n, (row, path, entry) in enumerate(tasks, 1):
            try:
                if role == 'Client' and (runtime / role / Path(path.replace('\\', '/'))).exists():
                    raise ValueError('Client loose override appeared since snapshot')
                before = entry['pak'].stat()
                data, how = entry_payload(entry, {}, recovery)
                with entry['pak'].open('rb') as f:
                    f.seek(entry['offset'])
                    stored_hash = sha(f.read(entry['flags'] & 0xffffff))
                after = entry['pak'].stat()
                if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
                    raise ValueError('Input PAK changed during frame read')
                target = output / role / Path(path.replace('\\', '/'))
                if target.exists():
                    if sha(target.read_bytes()) != sha(data):
                        raise ValueError('Refusing overwrite of changed offline output')
                else:
                    target.parent.mkdir(parents=True, exist_ok=True)
                    target.write_bytes(data)
                if sha(target.read_bytes()) != sha(data):
                    raise ValueError('Frame output hash verification failed')
                row.update(origin=how, pak=entry['pak'].name, entry=entry['entry'],
                    id='%08X' % entry['id'], bytes=len(data), sha256=sha(data),
                    source_stored_sha256=stored_hash, status='RESOLVED')
                errors.pop(path.casefold(), None)
                new_ids.add(row['id'])
                new_paths.add(path.casefold())
            except (ValueError, OSError, IndexError, MemoryError) as exc:
                errors[path.casefold()] = dict(path=path, error='SPR_FRAME_RETRY: ' + type(exc).__name__ + ': ' + str(exc))
            if n % 200 == 0:
                print(role, 'frame paths tested', n, '/', len(tasks), flush=True)
        fields = list(rows[0]) if rows else ['path', 'status']
        if 'source_stored_sha256' not in fields:
            fields.append('source_stored_sha256')
        csv_write(output / (role + '_manifest.csv'), rows, fields)
        csv_write(output / (role + '_blocked.csv'), errors.values(), ['path', 'error'])
        unknown = [r for r in csv_read(output / (role + '_unknown_ids.csv')) if r['id'] not in new_ids]
        csv_write(output / (role + '_unknown_ids.csv'), unknown, ['id', 'pak', 'entry', 'bytes'])
        gaps = []
        for r in csv_read(output / (role + '_source_gaps.csv')):
            try:
                key = logical(r['path']).casefold()
                key = aliases.get(key, key)
            except ValueError:
                key = ''
            if r['kind'] != 'STATIC_LITERAL' or key not in new_paths:
                gaps.append(r)
        csv_write(output / (role + '_source_gaps.csv'), gaps, ['path', 'caller', 'line', 'kind'])
        summary['roles'][role].update(resolved=sum(r['status'] == 'RESOLVED' for r in rows),
            blocked=len(errors), bytes=sum(int(r['bytes']) for r in rows),
            unknown_winning_ids=len(unknown), source_gaps=len(gaps), frame_retry_resolved=len(new_paths))
        print(role, 'frames restored', len(new_paths), 'remaining blocked', len(errors), flush=True)
    source04_reports(output, summary)
    summary['pak_free_ready'] = False
    (output / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    return summary


def export_plan(args):
    """Materialize the exact reviewed snapshot; fail if selected bytes changed."""
    workspace = args.workspace.resolve()
    plan = args.from_plan.resolve()
    summary = json.loads((plan / 'summary.json').read_text('utf-8'))
    runtime, recovery = Path(summary['input_runtime']), Path(summary['input_recovery'])
    output = args.output.resolve()
    check_output(workspace, runtime, recovery, output)
    if output == plan or plan.is_relative_to(output) or output.is_relative_to(plan):
        raise ValueError('Output overlaps input plan')
    inventory = {}  # Historical payload inventory is not current-version authority.
    output.mkdir(parents=True)
    failures, counts = [], {}
    for role in ('Server', 'Client'):
        root = runtime / role
        chain = package_chain(root)
        if [p.name for p in chain] != summary['roles'][role]['package_order']:
            raise ValueError('Package order changed since plan')
        winners, _ = pak_index(chain, strict=False)
        count = 0
        for row in csv_read(plan / (role + '_manifest.csv')):
            if row['status'] != 'RESOLVED':
                continue
            path = logical(row['path'])
            if path.split('\\')[0].casefold() not in [r.casefold() for r in ROOTS]:
                raise ValueError('Non-content path in plan: ' + path)
            try:
                if row['origin'] == 'RUNTIME_LOOSE':
                    data = (root / Path(path.replace('\\', '/'))).read_bytes()
                else:
                    entry = winners.get(int(row['id'], 16))
                    if not entry or entry['pak'].name != row['pak']:
                        raise ValueError('Selected PAK entry changed since plan')
                    data, _ = entry_payload(entry, inventory, recovery,
                        workspace / 'Client_Original/data' / entry['pak'].name)
                if len(data) != int(row['bytes']) or sha(data) != row['sha256']:
                    raise ValueError('Snapshot bytes changed since plan')
                target = output / role / Path(path.replace('\\', '/'))
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(data)
                if sha(target.read_bytes()) != row['sha256']:
                    target.unlink()
                    raise ValueError('Export verification failed')
                count += 1
                if count % 10000 == 0:
                    print(role, count, 'files exported/verified', flush=True)
            except (ValueError, OSError, IndexError) as exc:
                failures.append(dict(role=role, path=path, error=str(exc)))
        counts[role] = count
    for name in ('source_references.csv', 'mapping_rejected.csv',
                 *(r + suffix for r in ('Client', 'Server') for suffix in
                   ('_manifest.csv', '_blocked.csv', '_unknown_ids.csv', '_source_gaps.csv', '_skipped_packages.csv'))):
        shutil.copyfile(plan / name, output / name)
    csv_write(output / 'export_failures.csv', failures, ['role', 'path', 'error'])
    summary.update(materialized=True, materialized_from=str(plan), exported_files=counts,
                   export_failures=len(failures), pak_free_ready=False)
    if (recovery / '05_Reports/UNRESOLVED_IDS.csv').exists():
        source04_reports(output, summary)
    (output / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    print('Exported', counts, 'failures=', len(failures), 'NOT_PAK_FREE', flush=True)
    return summary


def run(args):
    workspace = args.workspace.resolve()
    runtime = (args.runtime or workspace / 'PhongThanRuntime-Staging').resolve()
    recovery = (args.recovery or workspace / 'PakRecoveryWork').resolve()
    output = args.output.resolve()
    resume = getattr(args, 'resume', False)
    check_output(workspace, runtime, recovery, output, allow_existing=resume)
    if resume and not (output / 'source_references.csv').is_file():
        raise ValueError('Resume requires an exporter output')

    reports = recovery / '05_Reports'
    inventory = {}
    names, aliases, mapping_errors = {}, {}, []
    for r in csv_read(reports / 'EXACT_MAPPINGS.csv'):
        try:
            physical = logical(r.get('OutputPath') or r['RuntimePath'] or r['CanonicalPath'])
            eid = int(r['IdHex'], 16)
            if eid not in ids_for(r.get('HashSpelling') or r['CanonicalPath']):
                raise ValueError('Name/hash evidence mismatch')
            key = physical.casefold()
            if key in names and names[key][1] != eid:
                raise ValueError('Same output path maps to different IDs')
            names[key] = (physical, eid)
            aliases[logical(r['CanonicalPath']).casefold()] = key
        except (ValueError, KeyError) as exc:
            mapping_errors.append(dict(path=r['CanonicalPath'], error=str(exc)))

    refs = source_references(workspace / 'PhongThanSource' / 'Sources')
    for r in refs:
        if r['kind'] != 'STATIC_LITERAL':
            continue
        try:
            path = logical(r['path'])
            key = path.casefold()
            if key not in aliases and key not in names:
                # ASCII source paths are exact; non-ASCII unresolved encoding is not guessed.
                if path.isascii():
                    names[key] = (path, pak_id(path.encode('ascii')))
                else:
                    r['kind'] = 'NONASCII_REQUIRES_MAPPING'
        except ValueError:
            r['kind'] = 'DYNAMIC_OR_UNSAFE'

    output.mkdir(parents=True, exist_ok=resume)
    csv_write(output / 'source_references.csv', refs, ['path', 'caller', 'line', 'kind'])
    csv_write(output / 'mapping_rejected.csv', mapping_errors, ['path', 'error'])
    summary = dict(schema=1, input_runtime=str(runtime), input_recovery=str(recovery),
                   materialized=args.materialize, pak_free_ready=False,
                   scope='Conservative all-known-content superset, not exact runtime access coverage', roles={},
                   startup_blockers=['GameClient requires PAK registry and direct Ui/Setting.ini PAK probe',
                                     'CoreServer requires PAK registry open',
                                     'Dynamic paths and unknown ID names require closure/test'],
                   source_reference_count=len(refs), mapping_rejected=len(mapping_errors))
    for role in ('Server', 'Client'):
        print('Resolving ' + role, flush=True)
        root = runtime / role
        chain = package_chain(root)
        skipped = []
        winners, entries = pak_index(chain, strict=False, diagnostics=skipped)
        csv_write(output / (role + '_skipped_packages.csv'), skipped, ['pak', 'error'])
        if resume and (output / (role + '_manifest.csv')).exists():
            # Completed role is immutable on resume. Verify every selected output
            # against its recorded hash, without repeating PAK decompression.
            prior = list(csv_read(output / (role + '_manifest.csv')))
            for row in prior:
                if row['status'] == 'RESOLVED':
                    target = output / role / Path(logical(row['path']).replace('\\', '/'))
                    if sha(target.read_bytes()) != row['sha256']:
                        raise ValueError('Resume completed output changed: ' + str(target))
            summary['roles'][role] = dict(package_order=[p.name for p in chain],
                skipped_packages=skipped, package_index_entries=len(entries),
                unique_winning_ids=len(winners), resolved=sum(r['status']=='RESOLVED' for r in prior),
                blocked=len(list(csv_read(output / (role + '_blocked.csv')))),
                unknown_winning_ids=len(list(csv_read(output / (role + '_unknown_ids.csv')))),
                source_gaps=len(list(csv_read(output / (role + '_source_gaps.csv')))),
                bytes=sum(int(r['bytes']) for r in prior),
                loose_shadowed=sum(r['shadowed_loose']=='True' for r in prior))
            print(role, 'completed manifest/output hashes verified; skipped', flush=True)
            continue
        candidates = dict(names)
        loose = {}
        for folder in ROOTS:
            base = root / folder
            if not base.is_dir():
                continue
            for f in base.rglob('*'):
                if not f.is_file() or f.is_symlink():
                    continue
                try:
                    path = logical(str(f.relative_to(root)))
                    key = path.casefold()
                    candidates.setdefault(key, (path, None))
                    loose[key] = f
                except ValueError:
                    pass
        manifest, errors, used_ids = [], [], set()
        for index, (key, (path, eid)) in enumerate(sorted(candidates.items())):
            entry = winners.get(eid)
            disk = loose.get(key)
            # Existing runtime filenames without a report mapping are hashed only
            # if all viable encodings agree on the same winning entry ID.
            if eid is None:
                matches = ids_for(path) & winners.keys()
                if len(matches) == 1:
                    eid = next(iter(matches))
                    entry = winners[eid]
                elif len(matches) > 1 and role == 'Server':
                    errors.append(dict(path=path, error='AMBIGUOUS_RUNTIME_NAME_ENCODING'))
                    continue
            selected_pak = entry is not None and (role == 'Server' or disk is None)
            row = dict(path=path, origin='', pak='', entry='', id='', sha256='', bytes=0,
                       status='', shadowed_loose=bool(selected_pak and disk))
            try:
                if selected_pak:
                    data, how = entry_payload(entry, inventory, recovery,
                        workspace / 'Client_Original' / 'data' / entry['pak'].name)
                    row.update(origin=how, pak=entry['pak'].name, entry=entry['entry'], id='%08X' % eid)
                    used_ids.add(eid)
                elif disk:
                    before = disk.stat()
                    data = disk.read_bytes()
                    after = disk.stat()
                    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
                        raise ValueError('Live loose input changed during read; retry snapshot')
                    row['origin'] = 'RUNTIME_LOOSE'
                    if entry:
                        used_ids.add(eid)
                else:
                    raise ValueError('No current PAK or runtime loose input')
                row.update(sha256=sha(data), bytes=len(data), status='RESOLVED')
                if args.materialize:
                    target = output / role / Path(path.replace('\\', '/'))
                    target.parent.mkdir(parents=True, exist_ok=True)
                    if target.exists():
                        if not resume or sha(target.read_bytes()) != row['sha256']:
                            raise ValueError('Refusing changed existing offline output: ' + str(target))
                    else:
                        target.write_bytes(data)
                    if sha(target.read_bytes()) != row['sha256']:
                        raise ValueError('Output verification failed')
            except (ValueError, OSError, IndexError, MemoryError) as exc:
                row['status'] = 'BLOCKED'
                errors.append(dict(path=path, error=type(exc).__name__ + ': ' + str(exc)))
            manifest.append(row)
            if index and index % 10000 == 0:
                print(role, index, 'paths processed', flush=True)
        unknown = [dict(id='%08X' % eid, pak=e['pak'].name, entry=e['entry'], bytes=e['size'])
                   for eid, e in winners.items() if eid not in used_ids]
        # Source references absent from the selected manifest are visible gaps.
        by_key = {r['path'].casefold(): r for r in manifest}
        missing_refs = []
        for r in refs:
            try:
                key = logical(r['path']).casefold()
                key = aliases.get(key, key)
            except ValueError:
                key = ''
            if r['kind'] != 'STATIC_LITERAL' or by_key.get(key, {}).get('status') != 'RESOLVED':
                missing_refs.append(r)
        fields = ['path', 'origin', 'pak', 'entry', 'id', 'sha256', 'bytes', 'status', 'shadowed_loose']
        csv_write(output / (role + '_manifest.csv'), manifest, fields)
        csv_write(output / (role + '_blocked.csv'), errors, ['path', 'error'])
        csv_write(output / (role + '_unknown_ids.csv'), unknown, ['id', 'pak', 'entry', 'bytes'])
        csv_write(output / (role + '_source_gaps.csv'), missing_refs, ['path', 'caller', 'line', 'kind'])
        summary['roles'][role] = dict(package_order=[p.name for p in chain],
            skipped_packages=skipped,
            package_index_entries=len(entries), unique_winning_ids=len(winners),
            resolved=sum(r['status'] == 'RESOLVED' for r in manifest), blocked=len(errors),
            unknown_winning_ids=len(unknown), source_gaps=len(missing_refs),
            bytes=sum(r['bytes'] for r in manifest),
            loose_shadowed=sum(r['shadowed_loose'] for r in manifest))
        print(role, summary['roles'][role], flush=True)
    if (recovery / '05_Reports/UNRESOLVED_IDS.csv').exists():
        source04_reports(output, summary)
    (output / 'summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    print('PARTIAL / NOT_PAK_FREE: see summary.json and source gaps', flush=True)
    return summary


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--runtime', type=Path)
    parser.add_argument('--recovery', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--materialize', action='store_true', help='Write verified content, no PAKs/binaries/secrets')
    parser.add_argument('--resume', action='store_true', help='Verify/reuse existing offline outputs; never overwrite changed bytes')
    parser.add_argument('--from-plan', type=Path, help='Export an existing reviewed snapshot, verify all output hashes')
    parser.add_argument('--annotate-source04', action='store_true', help='Add source-to-historical04 evidence reports to existing export')
    parser.add_argument('--retry-frame-sprites', action='store_true', help='Fill only blocked frame SPRs in an existing offline export')
    args = parser.parse_args()
    if args.retry_frame_sprites:
        retry_frame_exports(args)
    elif args.annotate_source04:
        annotate_source04(args)
    else:
        export_plan(args) if args.from_plan else run(args)

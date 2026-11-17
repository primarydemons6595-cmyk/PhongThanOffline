"""Build only an authored Yenson population; never modify maps/PAKs or quests.

Use the existing standalone NPC-section contract, with independently selected
positions validated against explicit client obstacle cells and current server
obstacles. Empty client obstacle sections are NOT treated as walkable evidence.
"""
import argparse
import hashlib
import importlib.util
import json
import struct
from pathlib import Path


def sections(data):
    if len(data) < 52:
        raise ValueError('Short region')
    count = struct.unpack_from('<I', data)[0]
    header = 4 + count * 8
    if not 6 <= count <= 64 or header > len(data):
        raise ValueError('Invalid region header')
    result = []
    for offset, length in struct.iter_unpack('<II', data[4:header]):
        if header + offset + length > len(data):
            raise ValueError('Invalid region span')
        result.append(data[header + offset:header + offset + length])
    return result


def build(workspace):
    runtime = workspace / 'PhongThanRuntime-Staging'
    project = workspace / 'PhongThanSource'
    metadata = (runtime / 'Server/settings/WorldSet.ini').read_bytes()
    name = next(line.split(b'=', 1)[1].decode('latin1')
                for line in metadata.splitlines() if line.startswith(b'1007='))
    table_path = runtime / 'Server/settings/phongthan/Npcs.txt'
    table = table_path.read_bytes()
    lines = table.splitlines()
    headers = lines[0].split(b'\t')
    templates = {}
    for tid, expected, label, level in (
            (0, bytes.fromhex('beb8c8cb'), 'Kiem Nhan', 5),
            (7, bytes.fromhex('bbb7b9b7'), 'Hoan Cau', 12)):
        row = dict(zip(headers, lines[tid + 1].split(b'\t')))
        if row[b'Name'] != expected or row[b'Kind'] != b'0':
            raise ValueError('Template identity mismatch')
        templates[tid] = (row, label, level)
    # Reuse the existing tested binary serializer, not foreign Lua execution.
    spec = importlib.util.spec_from_file_location('region_codec',
                                                  project / 'Tools/prepare_npc_restoration.py')
    codec = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(codec)
    output = project / 'Deploy/ProjectContent/YenSonPopulation'
    plan = []
    skipped = 0
    for client in sorted((runtime / 'Client/maps' / name).rglob('*_region_c.dat')):
        ry = int(client.parent.name[2:])
        rx = int(client.name.split('_', 1)[0])
        cbytes = client.read_bytes()
        csections = sections(cbytes)
        if len(csections[0]) != 2048:
            skipped += 1
            continue
        server = runtime / 'Server/maps' / (name + '_S') / client.parent.name / client.name.replace('_c.dat', '_s.dat')
        sbytes = server.read_bytes()
        ssections = sections(sbytes)
        if codec.npc_records(ssections[2]):
            raise ValueError('Refuse replacing existing NPC population')
        if len(ssections[0]) not in (0, 2048):
            raise ValueError('Unsupported server obstacles')
        cgrid = struct.unpack('<512I', csections[0])
        sgrid = struct.unpack('<512I', ssections[0]) if ssections[0] else (0,) * 512
        # Fully clear cells only; retain an explicit clear 3x3 neighbourhood.
        cells = []
        for x in range(1, 15):
            for y in range(1, 31):
                if all((cgrid[(x + dx) * 32 + y + dy] & 15) == 0 and
                       (sgrid[(x + dx) * 32 + y + dy] & 15) == 0
                       for dx in (-1, 0, 1) for dy in (-1, 0, 1)):
                    cells.append((x, y))
        if not cells:
            skipped += 1
            continue
        # Authored sparse layout: at most one monster per proven region.
        x, y = min(cells, key=lambda p: ((p[0] - 8)**2 + (p[1] - 16)**2, p))
        tid = 0 if (rx + ry) % 5 == 0 else 7
        row, label, level = templates[tid]
        mx, my = rx * 512 + x * 32 + 16, ry * 1024 + y * 32 + 16
        record = struct.pack('<iiiB3x32shhhhBBH', tid, mx, my, 0,
                             label.encode('ascii'), level, 0, 0, 0,
                             int(row[b'Camp']), int(row[b'Series']), 0)
        payload = codec.standalone_npcs([record])
        relative = f'settings/phongthan/npc_regions/1007/v_{ry:03d}/{rx:03d}_npc_s.dat'
        plan.append(dict(relative=relative, sha256=hashlib.sha256(payload).hexdigest(),
                         template=tid, name=label, level=level, x=mx, y=my,
                         client_sha256=hashlib.sha256(cbytes).hexdigest(),
                         server_sha256=hashlib.sha256(sbytes).hexdigest(),
                         cell=[x, y], obstacle_source='EXPLICIT_CLIENT_2048_SERVER_NATIVE_CONTRACT',
                         payload=payload))
    if not plan or len(plan) > 543:
        raise ValueError('Invalid population bounds')
    # Check namespace collisions in every mounted PAK before writing loose data.
    ids = {codec.pak_id(('\\' + p['relative']).encode('ascii')) for p in plan}
    package = (runtime / 'Server/package.ini').read_text('ascii')
    for line in package.splitlines():
        if '=' not in line or not line.split('=', 1)[0].isdigit():
            continue
        pak = runtime / 'Server/data' / line.split('=', 1)[1]
        with pak.open('rb') as stream:
            signature, count, offset = struct.unpack('<4sII', stream.read(12))
            if signature != b'PACK' or offset + count * 16 > pak.stat().st_size:
                raise ValueError('Invalid PAK index')
            stream.seek(offset)
            if any(key in ids for key, _, _, _ in struct.iter_unpack('<IIII', stream.read(count * 16))):
                raise ValueError('PAK shadows authored population')
    for entry in plan:
        destination = output / entry['relative']
        if destination.exists() and destination.read_bytes() != entry['payload']:
            raise ValueError('Refuse replacing a different authored file')
    return output, plan, dict(schema=1, classification='AUTHORED_RECONSTRUCTION', map=1007,
                             original_placement=False, template_table_sha256=hashlib.sha256(table).hexdigest(),
                             population=len(plan), skipped_regions=skipped, source_resolution='LOOSE_NO_PAK_COLLISION',
                             recipe='one_per_explicit_clear_region; 3x3_clear; center_nearest; no_specials_or_drop_override',
                             rows=[{k: v for k, v in p.items() if k != 'payload'} for p in plan])


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--write', action='store_true')
    args = parser.parse_args()
    output, plan, report = build(args.workspace.resolve())
    if args.write:
        for entry in plan:
            dest = output / entry['relative']
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(entry['payload'])
        (output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print(json.dumps({k: v for k, v in report.items() if k != 'rows'}))

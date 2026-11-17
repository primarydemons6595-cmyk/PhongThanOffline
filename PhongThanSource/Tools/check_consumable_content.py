"""Read-only descriptor/source checks and targeted PAK-index collision gate."""
import hashlib
import importlib.util
import json
import struct
import argparse
from pathlib import Path

root = Path(__file__).resolve().parents[2]
project = root / 'PhongThanSource'
parser=argparse.ArgumentParser()
parser.add_argument('--data-root', type=Path, default=root/'PhongThanRuntime-Staging')
args=parser.parse_args()
canonical = project / 'Deploy/ProjectContent/settings/phongthan'
manifest = json.loads((canonical / 'ConsumableEffects.provenance.json').read_text())
payload = (canonical / 'ConsumableEffects.txt').read_bytes()
assert hashlib.sha256(payload).hexdigest() == manifest['output_sha256']
spec = importlib.util.spec_from_file_location('consumable_builder', project / 'Tools/build_consumable_effects.py')
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)
spec = importlib.util.spec_from_file_location('npc_codec', project / 'Tools/prepare_npc_restoration.py')
codec = importlib.util.module_from_spec(spec)
spec.loader.exec_module(codec)
key = codec.pak_id(b'\\settings\\phongthan\\consumableeffects.txt')
for role in ('Server', 'Client'):
    runtime = args.data_root / role
    data = (runtime / 'settings/item/001/ibitem.txt').read_bytes()
    if role==manifest.get('source_role','Server'):
        assert hashlib.sha256(data).hexdigest() == manifest['source_sha256']
    rows, _ = builder.extract(data)
    canonical_rows = {tuple(entry['descriptor']) for entry in manifest['rows']}
    actual_rows = {tuple(row) for row in rows}
    assert actual_rows <= canonical_rows, 'Runtime effects differ from native source'
    missing = canonical_rows-actual_rows
    assert not missing or (role=='Server' and missing=={(9550,4,2,0,154,275,0,0,1)}), missing
    if missing: print('BASELINE_MISSING_SERVER_TEMPLATE detail=9550 part=4; native descriptor retained')
    packages = 0
    for line in (runtime / 'package.ini').read_text('ascii').splitlines():
        number, sep, name = line.partition('=')
        if not sep or not number.isdigit():
            continue
        pak = runtime / 'data' / name.strip()
        with pak.open('rb') as stream:
            magic, count, offset = struct.unpack('<4sII', stream.read(12))
            assert magic == b'PACK' and offset + count * 16 <= pak.stat().st_size
            stream.seek(offset)
            assert all(entry[0] != key for entry in struct.iter_unpack('<IIII', stream.read(count * 16))), f'{role}: PAK shadows data'
        packages += 1
    print(f'PASS {role}: {len(rows)} descriptors match source; packages={packages}; id={key:08X}; resolution=LOOSE_NO_PAK_COLLISION')

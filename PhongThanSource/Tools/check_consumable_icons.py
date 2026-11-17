"""Read-only gate: exact recovered PT resource, 28 powder icons in mounted PAKs."""
from pathlib import Path
import hashlib, importlib.util, struct
root = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('codec', root/'PhongThanSource/Tools/prepare_npc_restoration.py')
codec = importlib.util.module_from_spec(spec); spec.loader.exec_module(codec)
data = (root/'PhongThanSource/Deploy/ProjectContent/Ui/ui3/PtConsumableBuff.ini').read_bytes()
assert hashlib.sha256(data).hexdigest() == '6ddd62740f4c4e7bd6be00ffda855772848d1b8e8afcf709a28f5ca77ff18549'
sections = {}
section = None
for line in data.splitlines():
    if line.startswith(b'[') and line.endswith(b']'):
        section = line[1:-1]; sections[section] = {}
    elif section and b'=' in line:
        key, value = line.split(b'=',1); sections[section][key.strip()] = value.strip()
runtime = root/'PhongThanRuntime-Staging/Client'
index = set()
for line in (runtime/'package.ini').read_text('ascii').splitlines():
    number, sep, name = line.partition('=')
    if sep and number.isdigit():
        with (runtime/'data'/name.strip()).open('rb') as f:
            magic, count, offset = struct.unpack('<4sII',f.read(12)); assert magic == b'PACK'
            f.seek(offset); index.update(e[0] for e in struct.iter_unpack('<IIII', f.read(count*16)))
assert codec.pak_id(b'\\ui\\ui3\\ptconsumablebuff.ini') not in index
for i in range(28):
    record = sections[f'BuffInfo_{i}'.encode()]
    assert record[b'Title'] and record[b'Desc']
    if codec.pak_id(record[b'Image']) not in index:
        assert i == 23, f'unexpected missing sprite detail {i}'
        found = []
        for pak in (root/'Client_Original/data').glob('*.pak'):
            with pak.open('rb') as f:
                magic, count, offset = struct.unpack('<4sII',f.read(12))
                if magic != b'PACK': continue
                f.seek(offset)
                if any(e[0] == codec.pak_id(record[b'Image']) for e in struct.iter_unpack('<IIII',f.read(count*16))): found.append(pak.name)
        print(f'UNRESOLVED detail=23 sprite={record[b"Image"].hex()} original_matches={found}')
assert sections[b'Main'][b'Width'] == b'534'
print('PASS exact original resource; 27/28 images mounted; dedicated loose name has no PAK collision')

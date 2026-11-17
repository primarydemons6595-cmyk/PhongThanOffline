"""Check the Loose client's PE imports against every adjacent DLL's exports."""
import json
from pathlib import Path

import pefile

ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / 'PhongThanRuntime-Loose/Client'


def main():
    files = {p.name.lower(): p for p in CLIENT.iterdir() if p.suffix.lower() in ('.dll', '.exe')}
    exports = {}
    for name, path in files.items():
        image = pefile.PE(str(path))
        symbols = getattr(getattr(image, 'DIRECTORY_ENTRY_EXPORT', None), 'symbols', [])
        exports[name] = ({s.name for s in symbols if s.name}, {s.ordinal for s in symbols})
        image.close()
    missing = []
    checked = 0
    for path in files.values():
        image = pefile.PE(str(path))
        for dependency in getattr(image, 'DIRECTORY_ENTRY_IMPORT', []):
            library = dependency.dll.decode('ascii').lower()
            if library not in exports:
                continue
            names, ordinals = exports[library]
            for symbol in dependency.imports:
                checked += 1
                if (symbol.name is not None and symbol.name not in names) or (symbol.name is None and symbol.ordinal not in ordinals):
                    missing.append(dict(importer=path.name, library=library, symbol=symbol.name.decode('ascii') if symbol.name else symbol.ordinal))
        image.close()
    result = dict(result='FAIL' if missing else 'PASS', images=len(files), local_imports=checked, missing=missing)
    print(json.dumps(result, indent=2))
    proof = ROOT / 'analysis/vtt_weight_life_20261029/client_engine_fix'
    proof.mkdir(parents=True, exist_ok=True)
    (proof / 'imports.json').write_text(json.dumps(result, indent=2) + '\n', 'utf-8')
    if missing:
        raise SystemExit(1)


if __name__ == '__main__':
    main()

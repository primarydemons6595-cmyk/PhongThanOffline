"""Restore quest 51's missing title in both Loose UI schemes and latest package."""
import hashlib
import json
import re
from pathlib import Path

from extract_client_coordinate_dictionary import encode_tcvn3

ROOT = Path(__file__).resolve().parents[2]
PACKAGE = ROOT / 'PhongThanSource/Deploy/ProjectContent/VttWeightLife20261029'
PROOF = ROOT / 'analysis/vtt_weight_life_20261029/ke_tuc_title'


def main():
    manifest_path = PACKAGE / 'manifest.json'
    manifest = json.loads(manifest_path.read_text('utf-8-sig'))
    results = []
    title = encode_tcvn3('K\u1ebf T\u1ee5c')
    for scheme in ('ui3', 'ui4'):
        relative = f'Ui/{scheme}/taskinfo.ini'
        target = ROOT / 'PhongThanRuntime-Loose/Client' / relative
        original = target.read_bytes()
        section = re.search(rb'^\[Task_51\]\r?\n(.*?)(?=^\[|\Z)', original, re.M | re.S)
        if section:
            assert re.search(rb'^Title=\$ ' + re.escape(title) + rb'\r?$', section[1], re.M), 'Unexpected existing title'
            updated = original
        else:
            updated = original.rstrip(b'\r\n') + b'\r\n\r\n[Task_51]\r\nTitle=$ ' + title + b'\r\n'
        backup = PROOF / relative
        backup.parent.mkdir(parents=True, exist_ok=True)
        if not backup.exists():
            backup.write_bytes(original)
        staged = PACKAGE / 'compiled/Client' / relative
        staged.parent.mkdir(parents=True, exist_ok=True)
        staged.write_bytes(updated)
        target.write_bytes(updated)
        assert target.read_bytes() == staged.read_bytes()
        entry = dict(role='Client', relative=relative, sha256=hashlib.sha256(updated).hexdigest(), before=hashlib.sha256(backup.read_bytes()).hexdigest())
        manifest['files'] = [e for e in manifest['files'] if not (e['role'] == 'Client' and e['relative'] == relative)]
        manifest['files'].append(entry)
        results.append(entry)
    manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', 'utf-8')
    (PROOF / 'publication.json').write_text(json.dumps(dict(result='PASS', files=results), indent=2) + '\n', 'utf-8')
    print('PASS: quest 51 title restored in ui3/ui4; progress text preserved; Loose/package hashes match.')


if __name__ == '__main__':
    main()

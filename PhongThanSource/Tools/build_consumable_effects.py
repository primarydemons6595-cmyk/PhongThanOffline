"""Generate verified PT powders, HP/mana pools and native timed entitlements."""
import argparse
import hashlib
import json
from pathlib import Path


def extract(data):
    lines = data.splitlines()
    if len(lines[0].split(b'\t')) < 70:
        raise ValueError('Expected full PT IB schema')
    result = []
    evidence = []
    for index, line in enumerate(lines[1:]):
        cols = line.split(b'\t')
        if len(cols) < 70 or cols[1] != b'8':
            continue
        detail, part = int(cols[2]), int(cols[3])
        if cols[12] != b'NONE':
            continue
        if not all((cols[i] or b'0').lstrip(b'-').isdigit() for i in (30, 32)):
            continue
        ids = [int(cols[i] or b'0') for i in (30, 32)]
        timed = detail in (*range(38, 46), *range(125, 133), 186, 192, 233, 390, 394) and part == 0
        if not (0 <= detail < 28 and part == 0) and ids[0] not in (153, 154) and not timed:
            continue
        vals = [int(cols[i] or b'0') for i in (31, 33)]
        if 0 <= detail < 28 and part == 0:
            kind = 3
            duration = int(cols[14])
            if duration <= 0 or duration > 86400 or not any(ids):
                raise ValueError('Invalid powder contract')
        elif timed:
            kind = 4
            duration = int(cols[14])
            if ids[0] not in (192, 196) or ids[1] or vals[0] <= 0 or not 0 < duration <= 2592000:
                raise ValueError('Invalid native timed entitlement')
        elif ids[0] in (153, 154) and ids[1] == 0 and vals[0] > 0:
            kind = 1 if ids[0] == 153 else 2
            duration = 0
        else:
            continue
        row = [detail, part, kind, duration, ids[0], vals[0], ids[1], vals[1], int(cols[13] or b'0') if kind == 4 else 1]
        if row[:2] in [r[:2] for r in result]:
            raise ValueError('Ambiguous item key')
        result.append(row)
        evidence.append(dict(source_row=index, key=[detail, part],
                             name_hex=cols[0].hex(), uses=int(cols[9]),
                             script='NONE', descriptor=row))
    if len([r for r in result if r[2] == 3]) != 28:
        raise ValueError('Incomplete powder set')
    if len([r for r in result if r[2] == 4]) != 21:
        raise ValueError('Incomplete native timed item set')
    return result, evidence


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--data-root', type=Path, help='Runtime root containing Server/settings/item/001/ibitem.txt')
    parser.add_argument('--source-role', choices=('Client','Server'), default='Server')
    args = parser.parse_args()
    root = args.workspace.resolve()
    data = ((args.data_root or root / 'PhongThanRuntime-Staging') / args.source_role / 'settings/item/001/ibitem.txt').read_bytes()
    rows, evidence = extract(data)
    output = root / 'PhongThanSource/Deploy/ProjectContent/settings/phongthan'
    output.mkdir(parents=True, exist_ok=True)
    payload = ('Detail\tPart\tKind\tSeconds\tEffect1\tValue1\tEffect2\tValue2\tClock\r\n' +
               ''.join('\t'.join(map(str, r)) + '\r\n' for r in rows)).encode('ascii')
    (output / 'ConsumableEffects.txt').write_bytes(payload)
    manifest = dict(classification='RECONSTRUCTED_NATIVE_FROM_PT_TABLE',
                    source_role=args.source_role,
                    source_sha256=hashlib.sha256(data).hexdigest(),
                    output_sha256=hashlib.sha256(payload).hexdigest(), rows=evidence,
                    policy='pool toggle;500ms;actual recovery charge;powder refresh;native entitlements column13 online/calendar;separate UI namespaces')
    (output / 'ConsumableEffects.provenance.json').write_text(json.dumps(manifest, indent=2)+'\n', encoding='utf8')
    print('Descriptors:', len(rows), 'powders:28', 'SHA256:', manifest['output_sha256'])

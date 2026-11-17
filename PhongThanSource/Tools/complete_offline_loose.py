"""Targeted dependency/name closure for a generated offline loose export.

Reads legacy source as bytes. Never edits C++, staging, originals or PAKs.
Historical restoration is separately labelled, never called an active version.
"""
import argparse
import json
import re
from pathlib import Path
import build_offline_loose as base

EXT = rb'(?:spr|ini|txt|lua|dat|wor|wav|mp3|fnt|tab|bin|idx|bmp|tga|png|jpg)'
ABS = re.compile(rb'(?i)[\\/](?:settings|script|ui|spr|maps|font|sound)[\\/][^\x00\r\n\t"<>|=;]*?\.' + EXT + rb'(?![a-z0-9_])')
LITERAL = re.compile(rb'"((?:\\.|[^"\\\r\n])*)"')


def c_bytes(text):
    out = bytearray()
    i = 0
    escapes = {ord(k): ord(v) for k, v in {'n':'\n', 'r':'\r', 't':'\t',
        'a':'\a', 'b':'\b', 'f':'\f', 'v':'\v', '\\':'\\', '"':'"', "'":"'", '?':'?'}.items()}
    while i < len(text):
        value = text[i]
        i += 1
        if value != 92:
            out.append(value)
            continue
        if i == len(text):
            raise ValueError('Trailing C escape')
        value = text[i]
        i += 1
        if value == ord('x'):
            match = re.match(rb'[0-9a-fA-F]+', text[i:])
            if not match or int(match[0], 16) > 255:
                raise ValueError('Invalid byte hex escape')
            out.append(int(match[0], 16))
            i += len(match[0])
        elif ord('0') <= value <= ord('7'):
            match = re.match(rb'[0-7]{0,2}', text[i:])
            octal = bytes([value]) + match[0]
            if int(octal, 8) > 255:
                raise ValueError('Invalid byte octal escape')
            out.append(int(octal, 8))
            i += len(match[0])
        elif value in escapes:
            out.append(escapes[value])
        else:
            raise ValueError('Unproven C escape')
    return bytes(out)


def raw_path(raw):
    raw = b'\\' + raw.replace(b'/', b'\\').lstrip(b'\\')
    if b'%' in raw or len(raw) > 127 or any(b < 32 for b in raw):
        raise ValueError('Dynamic/unsafe native path')
    if not re.match(rb'(?i)^\\(?:settings|script|ui|spr|maps|font|sound)\\', raw):
        raise ValueError('Not an absolute content file')
    if not re.search(rb'(?i)\.' + EXT + rb'$', raw):
        raise ValueError('Not a supported content filename')
    return raw, base.logical(raw.decode('mbcs'))


def run(args):
    output = args.output.resolve()
    summary = json.loads((output / 'summary.json').read_text('utf-8'))
    workspace = args.workspace.resolve()
    runtime = Path(summary['input_runtime'])
    recovery = Path(summary['input_recovery'])
    base.check_output(workspace, runtime, recovery, output, allow_existing=True)
    report = output / 'completion'
    report.mkdir(exist_ok=True)
    winners = {role: base.pak_index(base.package_chain(runtime / role), strict=False)[0]
               for role in summary['roles']}
    manifests = {role: list(base.csv_read(output / (role + '_manifest.csv'))) for role in winners}
    wanted = set().union(*(set(v) for v in winners.values()))
    candidates, source = {}, []

    def add(raw, evidence):
        try:
            raw, name = raw_path(raw)
        except (ValueError, UnicodeError):
            return
        eid = base.pak_id(raw)
        if eid in wanted:
            key = (eid, name.casefold())
            candidates.setdefault(key, dict(id='%08X' % eid, path=name, raw_hex=raw.hex(), evidence=evidence))

    for file in (workspace / 'PhongThanSource/Sources').rglob('*'):
        if file.suffix.lower() not in ('.cpp', '.h', '.inl'):
            continue
        data = file.read_bytes()
        for match in LITERAL.finditer(data):
            try:
                raw = c_bytes(match[1])
                raw, name = raw_path(raw)
            except (ValueError, UnicodeError):
                continue
            evidence = '%s:%d' % (file.relative_to(workspace), data.count(b'\n', 0, match.start())+1)
            source.append(dict(path=name, id='%08X' % base.pak_id(raw), raw_hex=raw.hex(), caller=evidence))
            add(raw, evidence)
    print('Byte-correct source paths', len(source), flush=True)
    for row in base.csv_read(recovery / '05_Reports/PATH_DICTIONARY.csv'):
        for spelling in {row['CanonicalPath'], row['OriginalSpelling']}:
            if not spelling.startswith(('\\', '/')):
                continue
            for encoding in ('latin1', 'cp936', 'cp1252'):
                try:
                    add(spelling.encode(encoding), 'PATH_DICTIONARY:' + row['ReferencedBy'][:180])
                except UnicodeError:
                    pass
    print('Current-index name candidates', len(candidates), flush=True)
    # Selected text content may reference dependencies absent from old dictionary.
    scanned = set()
    for role, rows in manifests.items():
        for row in rows:
            if row['status'] != 'RESOLVED' or Path(row['path']).suffix.lower() not in ('.ini','.txt','.lua','.wor'):
                continue
            if row['sha256'] in scanned:
                continue
            scanned.add(row['sha256'])
            file = output / role / Path(row['path'].replace('\\','/'))
            for match in ABS.finditer(file.read_bytes()):
                add(match[0], role + ':' + row['path'])
    base.csv_write(report / 'source_byte_paths.csv', source, ['path','id','raw_hex','caller'])
    base.csv_write(report / 'name_candidates.csv', candidates.values(), ['id','path','raw_hex','evidence'])
    exact = {}
    for row in base.csv_read(recovery / '05_Reports/EXACT_MAPPINGS.csv'):
        try:
            path = base.logical(row.get('OutputPath') or row['RuntimePath'] or row['CanonicalPath'])
            if int(row['IdHex'],16) in base.ids_for(row.get('HashSpelling') or row['CanonicalPath']):
                exact.setdefault(path.casefold(), []).append(row)
        except ValueError:
            pass
    result = dict(pak_free_ready=False, roles={}, byte_source_references=len(source))
    for role, rows in manifests.items():
        known = {r['path'].casefold():r for r in rows}
        by_id = {}
        for candidate in candidates.values():
            by_id.setdefault(candidate['id'], []).append(candidate)
        additions, errors = [], []
        for eid, names in by_id.items():
            # Filename hash collisions stay visible; never choose an arbitrary name.
            distinct_raw = {n['raw_hex'].lower() for n in names}
            if len(distinct_raw) > 1:
                # ASCII case differences have identical native spelling semantics.
                if len({bytes.fromhex(n).lower() for n in distinct_raw}) > 1:
                    errors.append(dict(path=eid, error='AMBIGUOUS_NATIVE_NAME_HASH'))
                    continue
            for name in names:
                if known.get(name['path'].casefold(),{}).get('status') == 'RESOLVED':
                    continue
                entry = winners[role].get(int(eid,16))
                if not entry:
                    continue
                additions.append(dict(name, origin='CURRENT_PAK_NAME_RECOVERED', pak=entry['pak'].name,
                    entry=entry['entry'], sha256='', bytes=entry['size'], status='PLANNED'))
        restored = []
        for row in rows:
            if row['status'] != 'BLOCKED':
                continue
            options = exact.get(row['path'].casefold(), [])
            file = recovery / '03_RecoveredLoose' / Path(row['path'].replace('\\','/'))
            if not file.is_file():
                continue
            data = file.read_bytes()
            digest = base.sha(data)
            matches = [r for r in options if r['SHA256'].lower() == digest]
            if not matches:
                errors.append(dict(path=row['path'], error='HISTORICAL_SHA_NOT_PROVEN'))
                continue
            evidence = matches[0]
            restored.append(dict(path=row['path'], id=evidence['IdHex'], origin='HISTORICAL_EXACT_SHA_FALLBACK',
                pak=evidence['PakName'], entry=evidence['EntryIndex'], sha256=digest, bytes=len(data),
                status='PLANNED', evidence=str(file)))
        if args.materialize:
            for row in additions + restored:
                try:
                    if row['origin']=='CURRENT_PAK_NAME_RECOVERED':
                        data, how = base.entry_payload(winners[role][int(row['id'],16)], {}, recovery)
                        row['codec'] = how
                    else:
                        data = Path(row['evidence']).read_bytes()
                        if base.sha(data) != row['sha256']:
                            raise ValueError('Historical input changed')
                    target = output / role / Path(base.logical(row['path']).replace('\\','/'))
                    digest = base.sha(data)
                    if target.exists() and base.sha(target.read_bytes()) != digest:
                        raise ValueError('Refusing changed existing output')
                    if not target.exists():
                        target.parent.mkdir(parents=True, exist_ok=True)
                        target.write_bytes(data)
                    if base.sha(target.read_bytes()) != digest:
                        raise ValueError('Output SHA failure')
                    row.update(status='RESOLVED', sha256=digest, bytes=len(data))
                except (ValueError, OSError, MemoryError) as exc:
                    row.update(status='BLOCKED', error=str(exc))
        fields = ['path','id','raw_hex','origin','pak','entry','sha256','bytes','status','codec','evidence','error']
        base.csv_write(report / (role + '_additional_names.csv'), additions, fields)
        base.csv_write(report / (role + '_historical_restoration.csv'), restored, fields)
        base.csv_write(report / (role + '_name_errors.csv'), errors, ['path','error'])
        planned = {r['path'].casefold():r for r in additions+restored}
        source_status = []
        for ref in source:
            row = known.get(ref['path'].casefold(), {})
            extra = planned.get(ref['path'].casefold(), {})
            status = ('EXISTING_LOOSE_RESOLVED' if row.get('status')=='RESOLVED' else
                      'SUPPLEMENT_' + extra['status'] if extra else
                      'CURRENT_ID_AVAILABLE_NAME_UNRESOLVED' if int(ref['id'],16) in winners[role] else
                      'MISSING_SOURCE_REFERENCE')
            source_status.append(dict(ref, status=status))
        base.csv_write(report / (role + '_source_contract.csv'), source_status,
            ['path','id','raw_hex','caller','status'])
        result['roles'][role] = dict(new_name_paths=len(additions), historical_known_paths=len(restored),
            materialized=sum(r['status']=='RESOLVED' for r in additions+restored),
            source_missing_current=sum(r['status']=='MISSING_SOURCE_REFERENCE' for r in source_status),
            materialize_errors=sum(r['status']=='BLOCKED' for r in additions+restored))
        print(role, result['roles'][role], flush=True)
    (report / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    return result


def retry(args):
    output=args.output.resolve()
    summary=json.loads((output/'summary.json').read_text('utf-8'))
    runtime=Path(summary['input_runtime']);recovery=Path(summary['input_recovery'])
    base.check_output(args.workspace.resolve(),runtime,recovery,output,allow_existing=True)
    report=output/'completion'
    result=json.loads((report/'summary.json').read_text('utf-8'))
    for role in summary['roles']:
        winners,_=base.pak_index(base.package_chain(runtime/role),strict=False)
        file=report/(role+'_additional_names.csv')
        rows=list(base.csv_read(file))
        fields=list(rows[0]) if rows else ['path','status']
        for row in rows:
            if row['status']!='BLOCKED':continue
            try:
                entry=winners[int(row['id'],16)]
                if entry['pak'].name!=row['pak'] or entry['entry']!=int(row['entry']):
                    raise ValueError('Selected PAK entry changed')
                data,codec=base.entry_payload(entry,{},recovery)
                target=output/role/Path(base.logical(row['path']).replace('\\','/'))
                if target.exists() and base.sha(target.read_bytes())!=base.sha(data):
                    raise ValueError('Refusing changed output')
                target.parent.mkdir(parents=True,exist_ok=True)
                if not target.exists():target.write_bytes(data)
                if base.sha(target.read_bytes())!=base.sha(data):raise ValueError('SHA verification failed')
                row.update(status='RESOLVED',sha256=base.sha(data),bytes=len(data),codec=codec,error='')
            except (ValueError,OSError,KeyError,MemoryError) as exc:row['error']=str(exc)
        base.csv_write(file,rows,fields)
        originals=list(base.csv_read(report/(role+'_historical_restoration.csv')))
        result['roles'][role]['materialized']=sum(r['status']=='RESOLVED' for r in rows+originals)
        result['roles'][role]['materialize_errors']=sum(r['status']=='BLOCKED' for r in rows+originals)
        print(role,result['roles'][role],flush=True)
    (report/'summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--materialize', action='store_true')
    parser.add_argument('--retry', action='store_true',help='Retry only blocked current-PAK additions')
    args=parser.parse_args()
    retry(args) if args.retry else run(args)

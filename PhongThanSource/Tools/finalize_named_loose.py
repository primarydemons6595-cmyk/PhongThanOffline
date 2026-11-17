"""Recover only hash-proven original filenames; never modify runtime/loaders.

Historical inventory versions must agree byte-for-byte. Ambiguous versions stay
blocked. Existing active-version manifests are retained, not relabelled.
"""
import argparse
import json
import hashlib
from pathlib import Path
import build_offline_loose as b


def run(output, workspace, materialize=False, verify=False, verify_supplements=False):
    output = output.resolve()
    summary = json.loads((output/'summary.json').read_text('utf-8'))
    runtime = Path(summary['input_runtime'])
    recovery = Path(summary['input_recovery'])
    b.check_output(workspace.resolve(), runtime, recovery, output, allow_existing=True)
    report = output/'completion'
    roles = {}
    wanted = set()
    for role in summary['roles']:
        merged = {}
        missing = {}
        for file in (output/(role+'_manifest.csv'), report/(role+'_additional_names.csv'),
                     report/(role+'_historical_restoration.csv')):
            for row in b.csv_read(file):
                key = row['path'].casefold()
                if row['status'] == 'RESOLVED':
                    previous = merged.get(key)
                    if previous and previous['sha256'] != row['sha256']:
                        raise ValueError('Conflicting resolved output: '+row['path'])
                    merged[key] = row
                elif row['status'] == 'BLOCKED':
                    missing[key] = dict(path=row['path'], id=row.get('id',''),
                        reason=row.get('error') or 'NO_CURRENT_PAYLOAD')
        for ref in b.csv_read(report/(role+'_source_contract.csv')):
            if ref['status'] == 'MISSING_SOURCE_REFERENCE':
                key = ref['path'].casefold()
                missing.setdefault(key, dict(path=ref['path'], id=ref['id'],
                    reason='MISSING_SOURCE_REFERENCE'))
                if not missing[key]['id']:
                    missing[key]['id'] = ref['id']
        missing = {k:r for k,r in missing.items() if k not in merged}
        for row in missing.values():
            # ASCII spelling is codepage-independent. Non-ASCII needs an
            # already-proven source byte ID, not a guessed codepage conversion.
            if not row['id'] and row['path'].isascii() and Path(row['path']).suffix:
                row['id'] = '%08X' % b.pak_id(row['path'].encode('ascii'))
            if row['id']:
                wanted.add(row['id'].upper())
        roles[role] = (merged, missing)
    inventory = {}
    for row in b.csv_read(recovery/'05_Reports/PAK_INVENTORY.csv'):
        if row['IdHex'].upper() in wanted:
            inventory.setdefault(row['IdHex'].upper(), []).append(row)
    result = dict(named_only=True, loader_modified=False, pak_free_ready=False, roles={})
    fields = ['path','id','status','origin','pak','entry','sha256','bytes','evidence','error']
    for role, (merged, missing) in roles.items():
        recovered, unresolved = [], []
        for key, request in sorted(missing.items()):
            row = dict(request, status='BLOCKED', origin='HISTORICAL_INVENTORY_HASH_PROVEN')
            # Current payload decoder failures must not be hidden by older data.
            if request['reason'] not in ('NO_CURRENT_PAYLOAD','MISSING_SOURCE_REFERENCE'):
                row['error'] = request['reason']
                unresolved.append(row)
                continue
            if not request['id']:
                row['error'] = 'NO_PROVEN_NATIVE_FILENAME_ID'
                unresolved.append(row)
                continue
            options = inventory.get(request['id'].upper(), [])
            hashes = {v['SHA256'].lower() for v in options if v['Status']=='OK' and v['SHA256']}
            if not hashes:
                row['error'] = 'NO_ORIGINAL_INVENTORY_PAYLOAD_FOR_EXACT_NAME_HASH'
            elif len(hashes) != 1:
                row['error'] = 'AMBIGUOUS_HISTORICAL_VERSION'
            else:
                digest = next(iter(hashes))
                payload = None
                for item in options:
                    if item['SHA256'].lower() != digest or item['Status'] != 'OK':
                        continue
                    # Inventory extraction paths must be a filename, never escape roots.
                    name = Path(item['ExtractedPath'])
                    if name.name != str(name) or not name.name:
                        continue
                    for folder in ('04_Unresolved','01_ExtractedByPak'):
                        file = recovery/folder/Path(item['PakName']).stem/name
                        if file.is_file():
                            data = file.read_bytes()
                            if b.sha(data) == digest:
                                payload = (data,file,item)
                                break
                    if payload:
                        break
                if payload is None:
                    row['error'] = 'ORIGINAL_BYTES_MISSING_OR_SHA_MISMATCH'
                else:
                    data,file,item = payload
                    target = output/role/Path(b.logical(request['path']).replace('\\','/'))
                    if target.exists() and b.sha(target.read_bytes()) != digest:
                        raise ValueError('Refusing changed output: '+str(target))
                    if materialize:
                        target.parent.mkdir(parents=True,exist_ok=True)
                        if not target.exists():
                            target.write_bytes(data)
                        if b.sha(target.read_bytes()) != digest:
                            raise ValueError('Output SHA failure: '+str(target))
                    row.update(status='RESOLVED' if materialize else 'PLANNED',sha256=digest,
                        bytes=len(data),pak=item['PakName'],entry=item['EntryIndex'],evidence=str(file))
                    recovered.append(row)
                    if materialize:
                        merged[key] = row
                    continue
            unresolved.append(row)
        b.csv_write(report/(role+'_inventory_named_recovery.csv'),recovered,fields)
        b.csv_write(report/(role+'_named_unresolved.csv'),unresolved,fields)
        b.csv_write(report/(role+'_named_effective_manifest.csv'),merged.values(),fields)
        verified = 0
        if verify or verify_supplements:
            for row in merged.values():
                if not verify and row.get('origin') not in (
                    'CURRENT_PAK_NAME_RECOVERED','HISTORICAL_EXACT_SHA_FALLBACK',
                    'HISTORICAL_INVENTORY_HASH_PROVEN'):
                    continue
                file = output/role/Path(b.logical(row['path']).replace('\\','/'))
                digest = hashlib.sha256()
                with file.open('rb') as stream:
                    for chunk in iter(lambda:stream.read(1024*1024),b''):
                        digest.update(chunk)
                if digest.hexdigest() != row['sha256'].lower():
                    raise ValueError('Named output hash mismatch: '+str(file))
                verified += 1
        result['roles'][role] = dict(named_resolved=len(merged), inventory_additions=len(recovered),
            unresolved_named_paths=len(unresolved), historical=sum(
                r.get('origin','').startswith('HISTORICAL_') for r in merged.values()),
            sha_verified_files=verified)
        result['roles'][role]['verification_scope'] = (
            'ALL_NAMED' if verify else 'SUPPLEMENTS_ONLY' if verify_supplements else 'NOT_RERUN')
        print(role,result['roles'][role],flush=True)
    (report/'named_summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    return result


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace',type=Path,default=Path(__file__).resolve().parents[2])
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--materialize',action='store_true')
    parser.add_argument('--verify',action='store_true',help='Stream-hash all resolved named outputs')
    parser.add_argument('--verify-supplements',action='store_true',help='Hash only new named supplements')
    args=parser.parse_args()
    run(args.output,args.workspace,args.materialize,args.verify,args.verify_supplements)

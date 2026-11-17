"""Preserve every active native PAK winner as an offline ID-addressed loose file.

No guessed filenames; no game deployment. A game loader adapter is required.
Unknown/invalid sprite layouts remain explicit blockers, never fake pixels.
"""
import argparse
import json
import os
import shutil
from pathlib import Path
import build_offline_loose as m


def run(args):
    output=args.output.resolve()
    summary=json.loads((output/'summary.json').read_text('utf-8'))
    runtime=Path(summary['input_runtime']);recovery=Path(summary['input_recovery'])
    m.check_output(args.workspace.resolve(),runtime,recovery,output,allow_existing=True)
    report=output/'completion';report.mkdir(exist_ok=True)
    known={}
    for role in summary['roles']:
        files=[output/(role+'_manifest.csv'),report/(role+'_additional_names.csv')]
        for csv in files:
            if not csv.exists():continue
            for row in m.csv_read(csv):
                if row['status']!='RESOLVED' or not row.get('id') or not row.get('pak'):continue
                if row.get('origin')=='RUNTIME_LOOSE':continue
                known.setdefault((row['pak'].lower(),int(row['entry']),row['id']),[]).append((role,row))
    completed={};result=dict(pak_free_ready=False,loader_required=True,roles={})
    fields=['id','pak','entry','source_offset','source_flags','path','sha256','bytes','codec','status','error']
    for role in summary['roles']:
        marker=output/role/'_loose_id/READY'
        if marker.exists():
            if marker.read_bytes()!=b'PT_LOOSE_ID_V1\n':raise ValueError('Changed READY marker')
            marker.unlink()  # Revoke generated readiness until this verification completes.
        chain=m.package_chain(runtime/role)
        snapshot={p:(p.stat().st_size,p.stat().st_mtime_ns) for p in chain}
        winners,_=m.pak_index(chain,strict=False)
        index=report/(role+'_id_store.csv')
        old={r['id']:r for r in m.csv_read(index)} if index.exists() else {}
        if shutil.disk_usage(output).free < sum(e['size'] for e in winners.values())+1024**3:
            raise ValueError('Insufficient disk space plus1GiB reserve')
        rows=[];errors=[];linked=decoded=0
        for n,(eid,entry) in enumerate(sorted(winners.items()),1):
            hid='%08X'%eid
            relative='_loose_id\\%s\\%s.bin'%(hid[:2],hid)
            target=output/role/Path(relative.replace('\\','/'))
            row=dict(id=hid,pak=entry['pak'].name,entry=entry['entry'],source_offset=entry['offset'],
                source_flags=entry['flags'],path=relative,sha256='',bytes=0,codec='',status='BLOCKED',error='')
            try:
                prior=old.get(hid,{})
                same=(prior.get('pak','').lower()==entry['pak'].name.lower() and
                    prior.get('entry')==str(entry['entry']) and prior.get('source_offset')==str(entry['offset']) and
                    prior.get('source_flags')==str(entry['flags']))
                if same and prior.get('status')=='RESOLVED' and target.exists():
                    if m.sha(target.read_bytes())!=prior['sha256']:raise ValueError('Changed existing ID output')
                    row.update(prior)
                    row['_entry']=entry
                    rows.append(row);completed[(role,hid)]=row
                    continue
                source=None;digest=None;codec=None
                # Reuse another role only after proving the actual compressed
                # entry bytes agree, not just filename/index equality.
                for other in summary['roles']:
                    if other==role:continue
                    previous=completed.get((other,hid))
                    if not previous:continue
                    other_entry=previous['_entry'] if '_entry' in previous else None
                    if not other_entry:continue
                    with entry['pak'].open('rb') as f:
                        f.seek(entry['offset']);packed=f.read(entry['flags']&0xffffff)
                    with other_entry['pak'].open('rb') as f:
                        f.seek(other_entry['offset']);packed_other=f.read(other_entry['flags']&0xffffff)
                    if entry['size']==other_entry['size'] and entry['flags']==other_entry['flags'] and packed==packed_other:
                        source=output/other/Path(previous['path'].replace('\\','/'))
                        digest=previous['sha256'];codec=previous['codec'];break
                if source is None:
                    for known_role,candidate in known.get((entry['pak'].name.lower(),entry['entry'],hid),[]):
                        # Named exports were hash-verified on creation. Never
                        # reuse a now-modified loose file as original PAK proof.
                        file=output/known_role/Path(candidate['path'].replace('\\','/'))
                        if file.exists() and m.sha(file.read_bytes())==candidate['sha256']:
                            source=file;digest=candidate['sha256'];codec=candidate.get('codec') or candidate['origin'];break
                target.parent.mkdir(parents=True,exist_ok=True)
                if source is not None:
                    if target.exists():
                        if m.sha(target.read_bytes())!=digest:raise ValueError('Changed existing ID output')
                    else:
                        try:os.link(source,target)
                        except OSError:shutil.copyfile(source,target)
                    linked+=1
                    size=target.stat().st_size
                else:
                    data,codec=m.entry_payload(entry,{},recovery)
                    digest=m.sha(data);size=len(data)
                    if target.exists():
                        if m.sha(target.read_bytes())!=digest:raise ValueError('Changed existing ID output')
                    else:target.write_bytes(data)
                    decoded+=1
                if m.sha(target.read_bytes())!=digest:raise ValueError('ID output verification failed')
                row.update(sha256=digest,bytes=size,codec=codec,status='RESOLVED')
                row['_entry']=entry
                completed[(role,hid)]=row
            except (ValueError,OSError,MemoryError) as exc:
                row['error']=str(exc);errors.append(row)
            rows.append(row)
            if n%1000==0:
                m.csv_write(index,rows,fields)
                print(role,n,'/',len(winners),'blocked',len(errors),flush=True)
        if any((p.stat().st_size,p.stat().st_mtime_ns)!=stat for p,stat in snapshot.items()):
            raise ValueError('PAK inputs changed during ID export; do not use result')
        m.csv_write(index,rows,fields)
        m.csv_write(report/(role+'_id_blocked.csv'),errors,fields)
        resolved=sum(r['status']=='RESOLVED' for r in rows)
        result['roles'][role]=dict(winning_ids=len(winners),resolved=resolved,blocked=len(errors),
            bytes=sum(int(r['bytes']) for r in rows),linked=linked,decoded=decoded)
        # This marker means data access completeness, NOT game verification.
        if not errors:
            marker=output/role/'_loose_id/READY'
            if marker.exists() and marker.read_bytes()!=b'PT_LOOSE_ID_V1\n':raise ValueError('Changed READY marker')
            marker.write_bytes(b'PT_LOOSE_ID_V1\n')
        print(role,result['roles'][role],flush=True)
    (report/'id_store_summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    return result


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--workspace',type=Path,default=Path(__file__).resolve().parents[2])
    p.add_argument('--output',type=Path,required=True)
    run(p.parse_args())

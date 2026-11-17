"""Build only ID payloads reported missing by the active Loose runtime.

This never writes guessed original names and never becomes a runtime PAK.
"""
import argparse, csv, json, shutil
from pathlib import Path
import build_offline_loose as b

def run(workspace, output):
    workspace=workspace.resolve(); output=output.resolve()
    root=workspace/'PhongThanRuntime-Loose'
    recovery=workspace/'PakRecoveryWork'
    records=json.loads((root/'completion/logged_missing_recovery.json').read_text('utf-8'))
    output.mkdir(parents=True, exist_ok=True)
    summary={"source":"logged_missing_recovery.json","roles":{},"pak_free_ready":False}
    for role in ('Client','Server'):
        ids=sorted({int(r['id'],16) for r in records if r.get('role')==role and
                    r.get('status')=='MISSING_AVAILABLE_PAYLOAD' and len(r.get('id',''))==8})
        chain=b.package_chain(workspace/'PhongThanRuntime-Staging'/role)
        winners,_=b.pak_index(chain, strict=False)
        original=workspace/'Client_Original'; listed=[]
        package=original/'package.ini'
        if package.exists():
            for line in package.read_bytes().splitlines():
                if b'=' in line and line.split(b'=',1)[0].isdigit():
                    listed.append(original/'data'/line.split(b'=',1)[1].strip().decode('ascii'))
        historical,_=b.pak_index([p for p in listed if p.is_file()], strict=False)
        role_dir=output/role/'_loose_id'; role_dir.mkdir(parents=True,exist_ok=True)
        rows=[]; blocked=[]; resolved=0
        for eid in ids:
            hid=f'{eid:08X}'; target=role_dir/hid[:2]/(hid+'.bin')
            row={'id':hid,'path':str(target.relative_to(output/role)).replace('/','\\'),
                 'sha256':'','bytes':0,'status':'BLOCKED','error':''}
            try:
                entry=winners.get(eid) or historical.get(eid)
                if entry is None: raise ValueError('no active PAK winner')
                data,codec=b.entry_payload(entry,{},recovery)
                digest=b.sha(data)
                target.parent.mkdir(parents=True,exist_ok=True)
                if target.exists() and b.sha(target.read_bytes())!=digest:
                    raise ValueError('existing output hash differs')
                if not target.exists(): target.write_bytes(data)
                if b.sha(target.read_bytes())!=digest: raise ValueError('readback hash mismatch')
                row.update(sha256=digest,bytes=len(data),status='RESOLVED',
                           pak=entry['pak'].name,entry=entry['entry'],codec=codec)
                resolved+=1
            except (ValueError,OSError,MemoryError) as exc:
                row['error']=str(exc); blocked.append(row)
            rows.append(row)
        for name,data in [('resolved.csv',rows),('blocked.csv',blocked)]:
            with (output/role/name).open('w',newline='',encoding='utf-8') as f:
                w=csv.DictWriter(f,fieldnames=sorted({k for r in data for k in r} or {'id'}));w.writeheader();w.writerows(data)
        summary['roles'][role]={'requested_ids':len(ids),'resolved':resolved,'blocked':len(blocked),'bytes':sum(int(r['bytes']) for r in rows)}
    (output/'summary.json').write_text(json.dumps(summary,indent=2),encoding='utf-8')
    print(json.dumps(summary,indent=2))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--workspace',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();run(a.workspace,a.output)

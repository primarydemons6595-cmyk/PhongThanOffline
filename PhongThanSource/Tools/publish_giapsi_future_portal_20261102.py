"""Publish tested two-NPC fix, with backups and release-content provenance."""
from pathlib import Path
import csv,datetime,hashlib,io,json,os,shutil
from prepare_npc_restoration import pak_id
from publish_original_world import parse_index
ROOT=Path(__file__).resolve().parents[2];runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server'
out=ROOT/'analysis/giapsi_future_portal_20261102';plan=json.loads((out/'plan.json').read_text('utf8'))
package=ROOT/'PhongThanSource/Deploy/ProjectContent/GiapSiFuturePortal20261102'
backup=out/('backup-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
plans={}
for row in plan:
    target=server/row['relative'];data=(out/Path(row['relative']).name).read_bytes()
    assert hashlib.sha256(target.read_bytes()).hexdigest()==row['before_sha256']
    assert hashlib.sha256(data).hexdigest()==row['sha256']
    plans[target]=data
manifest=runtime/'completion/Server_named_effective_manifest.csv'
reader=csv.DictReader(io.StringIO(manifest.read_text('utf-8-sig')));fields=reader.fieldnames;rows=list(reader)
for change in plan:
    found=[r for r in rows if r['path'].lstrip('\\').replace('\\','/').lower()==change['relative'].lower()]
    assert len(found)<=1
    ident=pak_id(('\\'+change['relative'].replace('/','\\')).encode('ascii'))
    index=parse_index((server/'loose_index.bin').read_bytes())
    assert index[ident].decode('ascii').lstrip('\\').replace('\\','/').lower()==change['relative'].lower()
    if not found:
        row={f:'' for f in fields};row.update(path=change['relative'].replace('/','\\'),id=f'{ident:08X}');rows.append(row);found=[row]
    found[0].update(sha256=change['sha256'],bytes=str(len(plans[server/change['relative']])),origin='GIAPSI_FUTURE_PORTAL_20261102',status='RESOLVED')
buf=io.StringIO(newline='');writer=csv.DictWriter(buf,fieldnames=fields,lineterminator='\n');writer.writeheader();writer.writerows(rows);plans[manifest]=buf.getvalue().encode('utf8')
for target,data in plans.items():
    saved=backup/target.relative_to(runtime);saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
    tmp=target.with_name(target.name+'.portal-next');tmp.write_bytes(data);os.replace(tmp,target)
for row in plan:
    target=package/'compiled/Server'/row['relative'];target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(plans[server/row['relative']])
receipt=dict(schema=1,scope='LOOSE_ONLY',classification='GUARDED_GIAPSI_CO_KIM_REENTRY',files=plan,backup=str(backup),utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
(package/'manifest.json').write_text(json.dumps(receipt,indent=2),'utf8');(out/'publication.json').write_text(json.dumps(receipt,indent=2),'utf8')
print('Published two tested NPC scripts and release-content hashes. Native hot reload still required.')

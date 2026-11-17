"""Restore two older doctor script bytes affected by the generic travel fix.
Their menu/shop/heal/quest behavior must remain exactly as before this batch.
"""
from pathlib import Path
import csv,hashlib,io,json,os,shutil
ROOT=Path(__file__).resolve().parents[2];out=ROOT/'analysis/thuongchu_tgpt_20261103';runtime=ROOT/'PhongThanRuntime-Loose';s=runtime/'Server';package=ROOT/'PhongThanSource/Deploy/ProjectContent/ThuongChuTgpt20261103';m=json.loads((package/'manifest.json').read_text('utf8'));published=json.loads((out/'publication.json').read_text('utf8'));backup=Path(published['backup']);restored=[]
for name in ('dong','tam'):
 rel=f'script/phongthan/giapsi/npc_doctor_{name}.lua';old=(backup/'Server'/rel).read_bytes();entry=next(r for r in m['files'] if r['relative']==rel)
 assert hashlib.sha256(old).hexdigest()==entry['before_sha256'];current=s/rel;assert hashlib.sha256(current.read_bytes()).hexdigest()==entry['sha256']
 entry.update(sha256=hashlib.sha256(old).hexdigest(),bytes=len(old));(package/'compiled/Server'/rel).write_bytes(old);tmp=current.with_suffix('.doctor-preserve-next');tmp.write_bytes(old);os.replace(tmp,current);restored.append(entry)
mf=runtime/'completion/Server_named_effective_manifest.csv';reader=csv.DictReader(io.StringIO(mf.read_text('utf-8-sig')));fields=reader.fieldnames;rows=list(reader)
for r in rows:
 entry=next((e for e in restored if e['relative'].lower()==r['path'].lstrip('\\').replace('\\','/').lower()),None)
 if entry:r.update(sha256=entry['sha256'],bytes=str(entry['bytes']),origin='EXISTING_DOCTOR_PRESERVED_20261103')
buf=io.StringIO(newline='');w=csv.DictWriter(buf,fieldnames=fields,lineterminator='\n');w.writeheader();w.writerows(rows);mf.write_text(buf.getvalue(),'utf8')
(package/'manifest.json').write_text(json.dumps(m,ensure_ascii=False,indent=2),'utf8');(out/'existing-doctors-preserved.json').write_text(json.dumps(restored,indent=2),'utf8')
print('Restored exact original bytes of the two older doctor scripts; native reload pending')

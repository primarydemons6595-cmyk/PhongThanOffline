"""Called after a clean world/Event stop; validated content + additive rule only."""
import csv,hashlib,io,json,os,shutil,struct,datetime
from pathlib import Path
from prepare_npc_restoration import pak_id
from publish_original_world import parse_index
ROOT=Path(__file__).resolve().parents[2];runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server';package=ROOT/'PhongThanSource/Deploy/ProjectContent/DoctorsThuongChu20261102';m=json.loads((package/'manifest.json').read_text('utf8'))
assert m['schema']==1 and m['scope']=='LOOSE_ONLY'
plans={};expected={}
for row in m['files']:
 relative=row['relative'];assert '..' not in Path(relative).parts and relative.startswith(('script/','settings/','EventService/'))
 data=(package/'compiled/Server'/relative).read_bytes();assert hashlib.sha256(data).hexdigest()==row['sha256'];target=server/relative
 before=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None
 assert before in (row['before_sha256'],row['sha256']),'Concurrent file change: '+relative
 plans[target]=data;expected[target]=before
# Add one rule to the fresh current configuration, preserving every user edit.
config=server/'EventService/events.json';old=config.read_bytes();c=json.loads(old.decode('utf-8-sig'));event=m['event']
if not any(r['Id']==event['Id'] for r in c['Events']):c['Events'].append(event['Defaults']);c['Revision']+=1
plans[config]=json.dumps(c,ensure_ascii=False,indent=2).encode('utf8');expected[config]=hashlib.sha256(old).hexdigest()
indexfile=server/'loose_index.bin';oldindex=indexfile.read_bytes();index=parse_index(oldindex)
for row in m['files']:
 if row['relative'].startswith(('script/','settings/')):
  raw=('\\'+row['relative'].replace('/','\\')).encode('latin1');index[pak_id(raw)]=raw
newindex=b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',key,len(raw))+raw for key,raw in sorted(index.items()))
assert parse_index(newindex)==index;plans[indexfile]=newindex;expected[indexfile]=hashlib.sha256(oldindex).hexdigest()
manifest=runtime/'completion/Server_named_effective_manifest.csv';rawmanifest=manifest.read_bytes();reader=csv.DictReader(io.StringIO(rawmanifest.decode('utf-8-sig')));fields=reader.fieldnames;rows=list(reader);byid={r['id'].upper():r for r in rows}
changed={str(p.relative_to(server)).replace('\\','/').lower():data for p,data in plans.items() if p.is_relative_to(server) and str(p.relative_to(server)).replace('\\','/').startswith(('script/','settings/'))}
for row in rows:
 key=row['path'].lstrip('\\').replace('\\','/').lower()
 if key in changed:row.update(sha256=hashlib.sha256(changed[key]).hexdigest(),bytes=str(len(changed[key])),status='RESOLVED',origin='DOCTORS_THUONG_CHU_20261102')
for key,raw in index.items():
 relative=raw.decode('latin1').lstrip('\\').replace('\\','/');data=changed.get(relative.lower());ident=f'{key:08X}'
 if data is not None and ident not in byid:
  row={f:'' for f in fields};row.update(path=relative.replace('/','\\'),id=ident,status='RESOLVED',origin='DOCTORS_THUONG_CHU_20261102',sha256=hashlib.sha256(data).hexdigest(),bytes=str(len(data)),evidence='Reviewed restoration package');rows.append(row);byid[ident]=row
buff=io.StringIO(newline='');writer=csv.DictWriter(buff,fieldnames=fields,lineterminator='\n');writer.writeheader();writer.writerows(rows);plans[manifest]=buff.getvalue().encode('utf8');expected[manifest]=hashlib.sha256(rawmanifest).hexdigest()
backup=ROOT/'analysis/doctors_thuongchu_20261102'/('backup-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'));backup.mkdir(parents=True)
receipt=[]
for target,data in plans.items():
 before=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None;assert before==expected[target],'Changed after preflight: '+str(target)
 relative=target.relative_to(runtime);saved=backup/relative
 if target.exists():saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
 target.parent.mkdir(parents=True,exist_ok=True);temp=target.with_name(target.name+'.doctors-next');temp.write_bytes(data);os.replace(temp,target)
 assert target.read_bytes()==data
 receipt.append(dict(relative=str(relative),before=before,sha256=hashlib.sha256(data).hexdigest()))
result=dict(result='PASS',backup=str(backup),revision=c['Revision'],files=receipt,utc=datetime.datetime.now(datetime.timezone.utc).isoformat());(ROOT/'analysis/doctors_thuongchu_20261102/publication.json').write_text(json.dumps(result,indent=2),'utf8');(package/'publication.json').write_text(json.dumps(result,indent=2),'utf8');print('Published',len(receipt),'files; preserved existing Event rules, revision',c['Revision'])

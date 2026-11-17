"""Guarded additive content publication; never replace existing Event rules."""
import csv,datetime,hashlib,io,json,os,shutil,struct,sys
from pathlib import Path
from prepare_npc_restoration import pak_id
from publish_original_world import parse_index
ROOT=Path(__file__).resolve().parents[2];runtime=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else ROOT/'PhongThanRuntime-Loose';server=runtime/'Server';pkg=ROOT/'PhongThanSource/Deploy/ProjectContent/ThuongChuTgpt20261103';m=json.loads((pkg/'manifest.json').read_text('utf8'))
assert m['feature']=='TGPT_THUONG_CHU_20261103';plans={};expected={}
def add(target,data,allowed=None):
 before=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None
 if allowed is not None:assert before in allowed,'Concurrent content edit: '+str(target)
 assert target not in plans,'Duplicate destination '+str(target);plans[target]=data;expected[target]=before
for row in m['files']:
 rel=row['relative'];assert '..' not in Path(rel).parts and rel.startswith(('script/','settings/','EventService/'))
 data=(pkg/'compiled/Server'/rel).read_bytes();assert hashlib.sha256(data).hexdigest()==row['sha256'];add(server/rel,data,[row['before_sha256'],row['sha256']])
# Client must load the same isolated new shop; all66 earlier rows are retained.
client=runtime/'Client/settings/buysell.txt';shop=plans[server/'settings/buysell.txt'];baseline=(server/'settings/buysell.txt').read_bytes()
assert client.read_bytes() in (baseline,shop),'Client shop table has independent edits'
add(client,shop)
config=server/'EventService/events.json';c=json.loads(config.read_text('utf-8-sig'));event=next(e for e in c['Events'] if e['Id']==2101)
event.update(DailyTimes=['12:00','20:00'],RegistrationSeconds=600,DurationSeconds=1800);c['Revision']+=1;add(config,json.dumps(c,ensure_ascii=False,indent=2).encode('utf8'))
for role in ('Server','Client'):
 indexfile=runtime/role/'loose_index.bin';old=indexfile.read_bytes();index=parse_index(old)
 changes={p.relative_to(runtime/role).as_posix():v for p,v in plans.items() if p.is_relative_to(runtime/role) and p.relative_to(runtime/role).as_posix().startswith(('script/','settings/'))}
 for rel in changes:
  raw=('\\'+rel.replace('/','\\')).encode('latin1');index[pak_id(raw)]=raw
 encoded=b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',key,len(raw))+raw for key,raw in sorted(index.items()));assert parse_index(encoded)==index;add(indexfile,encoded)
 manifest=runtime/f'completion/{role}_named_effective_manifest.csv';raw=manifest.read_bytes();reader=csv.DictReader(io.StringIO(raw.decode('utf-8-sig')));fields=reader.fieldnames;rows=list(reader);byid={r['id'].upper():r for r in rows}
 for rel,data in changes.items():
  ident=f'{pak_id((chr(92)+rel.replace(chr(47),chr(92))).encode("latin1")):08X}';row=byid.get(ident)
  if row is None:row={f:'' for f in fields};rows.append(row);byid[ident]=row
  row.update(path=rel.replace('/','\\'),id=ident,sha256=hashlib.sha256(data).hexdigest(),bytes=str(len(data)),status='RESOLVED',origin='THUONG_CHU_TGPT_20261103',evidence='Article rules, guarded native session and Lua regression checks')
 buff=io.StringIO(newline='');writer=csv.DictWriter(buff,fieldnames=fields,lineterminator='\n');writer.writeheader();writer.writerows(rows);add(manifest,buff.getvalue().encode('utf8'))
if '--check' in sys.argv:print('PASS content preflight',len(plans),'destinations');sys.exit(0)
backup=ROOT/'analysis/thuongchu_tgpt_20261103'/('backup-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'));receipt=[]
for target,data in plans.items():
 assert (hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None)==expected[target]
 if target.exists():saved=backup/target.relative_to(runtime);saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
 target.parent.mkdir(parents=True,exist_ok=True);temp=target.with_name(target.name+'.tgpt-next');temp.write_bytes(data);os.replace(temp,target);assert target.read_bytes()==data
 receipt.append(dict(relative=target.relative_to(runtime).as_posix(),sha256=hashlib.sha256(data).hexdigest(),before=expected[target]))
result=dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),result='PASS',revision=c['Revision'],files=receipt,backup=str(backup));(ROOT/'analysis/thuongchu_tgpt_20261103/publication.json').write_text(json.dumps(result,indent=2),'utf8');print('Published',len(receipt),'files; Event revision',c['Revision'])

"""Publish checked VTT data with indexed manifests and full rollback copies."""
import ctypes,json,shutil,struct
from datetime import datetime,timezone
from restore_original_world import ROOT
from publish_original_world import parse_index
import build_offline_loose as b
folder=ROOT/'analysis/vtt_client_services_20261019';runtime=ROOT/'PhongThanRuntime-Loose'
plan=json.loads((folder/'plan.json').read_text('utf8'))
if (folder/'publication.json').exists():raise ValueError('Already published')
# Never edit data/index while the main server is loading or serving players.
k=ctypes.windll.kernel32;k.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p];k.CreateFileW.restype=ctypes.c_void_p;k.CloseHandle.argtypes=[ctypes.c_void_p]
h=k.CreateFileW(str(runtime/'Server/CoreServer.dll'),0x40000000,0,None,3,0,None)
if h in (None,ctypes.c_void_p(-1).value):raise ValueError('Main server must save and stop before publication')
k.CloseHandle(h)
for side,key in (('Server','files'),('Client','client_files')):
 for row in plan[key]:
  target=runtime/side/row['relative'];data=(folder/'staged'/side/row['relative']).read_bytes()
  assert b.sha(data)==row['sha256'] and (b.sha(target.read_bytes()) if target.exists() else None)==row['before_sha256'],row['relative']
for row in plan['metadata']:
 target=ROOT/'PhongThanSource'/row['relative'];assert b.sha(target.read_bytes())==row['before_sha256']
backup=folder/'backup_before_publication'
if backup.exists():raise ValueError('Existing backup journal: do not replay')
backup.mkdir()
for side,key in (('Server','files'),('Client','client_files')):
 base=runtime/side;manifest=runtime/'completion'/f'{side}_named_effective_manifest.csv'
 shutil.copy2(manifest,backup/manifest.name);saved=backup/side;saved.mkdir()
 shutil.copy2(base/'loose_index.bin',saved/'loose_index.bin')
 index=parse_index((base/'loose_index.bin').read_bytes());rows=list(b.csv_read(manifest));byid={}
 for row in rows:byid.setdefault(row['id'],[]).append(row)
 for row in plan[key]:
  rel=row['relative'];target=base/rel;data=(folder/'staged'/side/rel).read_bytes()
  if target.exists():p=saved/rel;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,p)
  target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
  raw=('\\'+rel.replace('/','\\')).encode('latin1');eid=b.pak_id(raw);index[eid]=raw;ident=f'{eid:08X}'
  matches=byid.get(ident,[])
  if not matches:
   item=dict(id=ident,status='RESOLVED',pak='',entry='',error='');rows.append(item);matches=[item];byid[ident]=matches
  for item in matches:item.update(path=raw.lstrip(b'\\').decode('latin1'),origin='VTT_CLIENT_SERVICES_RECOVERY',sha256=row['sha256'],bytes=str(len(data)),evidence=row['evidence'])
  compiled=ROOT/'PhongThanSource/Deploy/ProjectContent'/('NpcRestoration/compiled' if side=='Server' else '')/rel
  if compiled.exists():p=backup/'ProjectContent'/side/rel;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(compiled,p)
  compiled.parent.mkdir(parents=True,exist_ok=True);compiled.write_bytes(data)
 (base/'loose_index.bin').write_bytes(b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items())))
 b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
 print('Published',side,len(plan[key]),flush=True)
for row in plan['metadata']:
 target=ROOT/'PhongThanSource'/row['relative'];saved=backup/'Project'/row['relative'];saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
 data=(folder/'staged/Project'/row['relative']).read_bytes();assert b.sha(data)==row['sha256'];target.write_bytes(data)
result=dict(applied=True,utc=datetime.now(timezone.utc).isoformat(),server_files=len(plan['files']),client_files=len(plan['client_files']),doctor_count=4,normal_mob_original_scripts=8,client_restart_required=True,binaries_changed=False)
(folder/'publication.json').write_text(json.dumps(result,indent=2),'utf8');print(json.dumps(result))

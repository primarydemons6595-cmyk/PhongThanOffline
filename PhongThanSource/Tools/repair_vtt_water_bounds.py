"""Replace the inherited wrong Water WOR with its exact original map bounds."""
import ctypes,json,re,shutil,struct
from restore_original_world import ROOT,Original
from publish_original_world import parse_index
import build_offline_loose as b
folder=ROOT/'analysis/vtt_client_services_20261019';backup=folder/'backup_water_bounds'
if (folder/'water_bounds.json').exists():raise ValueError('Water bounds already repaired')
runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server'
k=ctypes.windll.kernel32;k.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p];k.CreateFileW.restype=ctypes.c_void_p;k.CloseHandle.argtypes=[ctypes.c_void_p]
h=k.CreateFileW(str(server/'CoreServer.dll'),0x40000000,0,None,3,0,None)
if h in (None,ctypes.c_void_p(-1).value):raise ValueError('Save and stop main server first')
k.CloseHandle(h)
name=dict((int(k),v) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))[1068]
raw=b'\\maps\\'+name+b'.wor';data,proof=Original().get(raw);rel=raw.lstrip(b'\\').decode('latin1').replace('\\','/')
backup.mkdir();changes=[]
for side in ('Server','Client'):
 target=runtime/side/rel;before=target.read_bytes();saved=backup/side/rel;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
 changes.append(dict(side=side,before=before.decode('gbk','replace'),after=data.decode('gbk','replace'),sha256=b.sha(data)))
 target.write_bytes(data)
 manifest=runtime/'completion'/f'{side}_named_effective_manifest.csv';shutil.copy2(manifest,backup/manifest.name);rows=list(b.csv_read(manifest))
 matches=[row for row in rows if row['path'].lower()==raw.lstrip(b'\\').decode('latin1').lower()]
 assert matches
 for row in matches:row.update(sha256=b.sha(data),bytes=str(len(data)),origin='EXACT_ORIGINAL_VTT_WATER_WOR',evidence='Maps.pak original '+proof['id']+'; removes inherited wrong rect=85,93,114,105')
 b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
 compiled=ROOT/'PhongThanSource/Deploy/ProjectContent'/('NpcRestoration/compiled' if side=='Server' else '')/rel;compiled.parent.mkdir(parents=True,exist_ok=True);compiled.write_bytes(data)
result=dict(applied=True,original=proof,changes=changes)
(folder/'water_bounds.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8');print(json.dumps(result,ensure_ascii=False))

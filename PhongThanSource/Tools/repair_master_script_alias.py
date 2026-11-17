"""Stage the original IsMaster name against the existing authoritative API."""
from pathlib import Path
import json,hashlib,subprocess
ROOT=Path(__file__).resolve().parents[2];LIVE=ROOT/'PhongThanRuntime-Loose'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/MasterAlias20261030'
REPORT=ROOT/'analysis/master_alias_20261030';REPORT.mkdir(parents=True,exist_ok=True)
prefix=b'-- Original API name; use persistent native master status, never a stub.\r\nIsMaster=IsMantleMaster\r\n'
files=[]
for role in ('Server','Client'):
 found=subprocess.run(['rg','-l','-F','IsMaster()',str(LIVE/role/'script'),'--glob','*.lua'],capture_output=True)
 for path in found.stdout.decode('utf8').splitlines():
  p=Path(path)
  data=p.read_bytes()
  if b'IsMaster()' not in data:continue
  rel=p.relative_to(LIVE/role).as_posix();out=PACKAGE/'compiled'/role/rel
  out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(prefix+data)
  files.append(dict(role=role,relative=rel,before=hashlib.sha256(data).hexdigest(),sha256=hashlib.sha256(prefix+data).hexdigest()))
(PACKAGE/'manifest.json').write_text(json.dumps(dict(schema=1,files=files),indent=2),'utf8')
print(json.dumps(dict(files=len(files),server=sum(e['role']=='Server' for e in files))))

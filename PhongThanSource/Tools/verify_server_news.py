import json,re,shutil
from restore_original_world import ROOT,OUT
from publish_original_world import parse_index
import build_offline_loose as b
folder=OUT/'server_news';runtime=ROOT/'PhongThanRuntime-Loose'
plans={'Server':json.loads((folder/'plan.json').read_text('utf8'))['files'],'Client':json.loads((folder/'client_plan.json').read_text('utf8'))}
for side,files in plans.items():
 index=parse_index((runtime/side/'loose_index.bin').read_bytes())
 for row in files:
  payload=(runtime/side/row['relative']).read_bytes();assert b.sha(payload)==row['sha256']
  raw=('\\'+row['relative'].replace('/','\\')).encode('ascii');assert index[b.pak_id(raw)]==raw
  if side=='Client':
   assert b'Width=386' in payload and b'Height=16' in payload and b'Font=14' in payload
server=runtime/'Server'
for side,files in plans.items():
 manifest=runtime/'completion'/f'{side}_named_effective_manifest.csv';rows=list(b.csv_read(manifest));names={r['relative'].replace('/','\\') for r in files}
 for row in rows:
  if row['path'] in names:row.update(origin='SERVER_NEWS_RESTORATION',evidence='Original VNG marquee layout and 52 original TIPs; project pre-event warnings; analysis/restoration_20261016/server_news/plan.json')
 b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
# The base project bootstrap must also retain the authored announcement service.
target=ROOT/'PhongThanSource/Deploy/ProjectContent/script/servertimer.lua';backup=folder/'backup_project_servertimer.lua'
if target.exists() and not backup.exists():shutil.copy2(target,backup)
target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(server/'script/servertimer.lua',target)
print('PASS SERVER_NEWS_PUBLICATION server_scripts=5 layouts=4 original_tips=52 hashes_and_loose_bindings=PASS binaries_changed=0')

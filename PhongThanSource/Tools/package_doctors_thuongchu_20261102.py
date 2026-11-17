import json,hashlib,shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];out=ROOT/'analysis/doctors_thuongchu_20261102';stage=out/'staged/Server';server=ROOT/'PhongThanRuntime-Loose/Server';package=ROOT/'PhongThanSource/Deploy/ProjectContent/DoctorsThuongChu20261102'
doctors=json.loads((out/'doctor-services-plan.json').read_text('utf8'));placements=json.loads((out/'placement-plan.json').read_text('utf8'));battle=json.loads((out/'battle-content-plan.json').read_text('utf8'));bot=json.loads((out/'bot-service-plan.json').read_text('utf8'))
paths={r['relative'] for r in doctors['files']}|{r['relative'] for r in placements['files']}|set(battle['files'])|set(bot['files'])|{r['script'] for r in placements['entries'] if r.get('name')=='Đại Phu'}
paths={p.replace('\\','/') for p in paths}
for row in doctors['files']:
 assert hashlib.sha256((server/row['relative']).read_bytes()).hexdigest()==row['before_sha256'],'Doctor changed during work; regenerate without overwriting user edit'
files=[];syntax=[]
for n,relative in enumerate(sorted(paths)):
 data=(stage/relative).read_bytes();target=package/'compiled/Server'/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data);before=server/relative
 files.append(dict(relative=relative,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(before.read_bytes()).hexdigest() if before.exists() else None,bytes=len(data)))
 if relative.endswith('.lua'):
  path=out/'all-lua-syntax'/f'{n}.lua';path.parent.mkdir(exist_ok=True);path.write_bytes(data);syntax.append(str(path).replace('\\','/'))
manifest=dict(schema=1,scope='LOOSE_ONLY',feature='DoctorsThuongChu20261102',files=files,event=battle['event'],sources=['https://phongthan.zing.vn/cam-nang/tinh-nang/chien-truong-thuong-chu.html','https://pc.tamgioipt.vn/he-thong-nhiem-vu-phan-thuong-chien-truong-thuong-chu'],notes='Native match reconstruction follows the documented traditional marshal/general mode, not all three later TGPT battlefield variants. Original PAK battle script not recovered. Outdoor doctor cooldown/shops preserved; authored missing positions verified walkable.')
(package/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),'utf8');(out/'all-lua-list.txt').write_text('\n'.join(syntax)+'\n','ascii')
print('Packaged',len(files),'reviewed files,',len(syntax),'Lua scripts; no unrelated stage files included')

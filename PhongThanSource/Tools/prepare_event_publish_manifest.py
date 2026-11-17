import hashlib,json,shutil
from restore_original_world import ROOT
out=ROOT/'analysis/event_service_20261019';stage=out/'staged';runtime=ROOT/'PhongThanRuntime-Loose'
plan=json.loads((out/'escort_publication_plan.json').read_text('utf8'))
files={r['relative']:r for r in plan['files']}
def add(relative,data):
    target=runtime/relative;p=stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    files[relative]=dict(relative=relative,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None)
add('Server/CoreServer.dll',(ROOT/'PhongThanSource/Output/Server/CoreServer.dll').read_bytes())
for build,dest in [('service-build','Server'),('studio-build','ManagerStudio/Wpf/PhongThanStudio.App/bin/Release/net10.0-windows')]:
    for p in (out/build).iterdir():
        if p.is_file() and p.suffix in ('.exe','.dll','.json','.pdb','.cfg'):add(dest+'/'+p.name,p.read_bytes())
# Mirror the reviewed scripts/placements into the existing content build source.
# These are project artifacts, outside runtime; publication updates them too.
project=[]
for row in plan['files']:
    relative=row['relative']
    if relative.startswith('Server/') and relative!='Server/CoreServer.dll':
        target=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled'/relative[7:]
        project.append(dict(target=str(target.relative_to(ROOT)),source=relative,before_sha256=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None))
protected=[]
for pattern in ('NativeDrop*.ini','LegacyDrop*.ini','*Quarantine.txt','NpcDropPolicy.ini'):
    for p in (runtime/'Server/settings/phongthan').glob(pattern):
        if p.is_file():protected.append(dict(relative=str(p.relative_to(runtime)).replace('\\','/'),sha256=hashlib.sha256(p.read_bytes()).hexdigest()))
plan.update(files=list(files.values()),project_artifacts=project,protected_drop_config=protected)
(out/'publish_manifest.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
print('PREPARED',len(files),'runtime files;',len(project),'compiled source artifacts;',len(protected),'drop configs protected')

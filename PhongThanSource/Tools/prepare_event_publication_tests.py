import json,shutil
from restore_original_world import ROOT
out=ROOT/'analysis/event_service_20261019';fixture=out/'test_runtime';server=ROOT/'PhongThanRuntime-Loose/Server'
for sub in ('script/phongthan/van_tien','script/timertask'):
    shutil.copytree(server/sub,fixture/sub,dirs_exist_ok=True)
plan=json.loads((out/'escort_publication_plan.json').read_text('utf8'));scripts=[]
for row in plan['files']:
    if row['relative'].startswith('Server/') and row['relative'].endswith('.lua'):
        source=out/'staged'/row['relative'];target=fixture/row['relative'][7:];target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target);scripts.append(str(source).replace('\\','/'))
(out/'staged_lua_list.txt').write_text('\n'.join(scripts)+'\n','ascii')
names=['1002_f57647b5','1004_281c1583','1003_479d2a84','1001_41e3ecb2']
(out/'lifecycle_fixtures.txt').write_text(str(fixture).replace('\\','/')+'\n'+''.join('script/phongthan/npc_recovered_batch/'+n+'.lua\n' for n in names),'ascii')
print('Prepared isolated tests; staged Lua files',len(scripts))

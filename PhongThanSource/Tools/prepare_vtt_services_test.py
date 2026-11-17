import json,shutil
from restore_original_world import ROOT
folder=ROOT/'analysis/vtt_client_services_20261019';server=ROOT/'PhongThanRuntime-Loose/Server'
testroot=folder/'test_runtime';shutil.copytree(server/'script/phongthan/van_tien',testroot/'script/phongthan/van_tien',dirs_exist_ok=True)
shutil.copytree(server/'script/timertask',testroot/'script/timertask',dirs_exist_ok=True)
names=['1002_f57647b5','1004_281c1583','1003_479d2a84','1001_41e3ecb2']
for name in names:
 rel='script/phongthan/npc_recovered_batch/'+name+'.lua';target=testroot/rel;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(server/rel,target)
plan=json.loads((folder/'plan.json').read_text('utf8'))
for row in plan['files']:
 if row['relative'].endswith('.lua'):
  if (folder/'activation.json').exists() and '/van_tien/mission' in row['relative']:continue
  target=testroot/row['relative'];target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(folder/'staged/Server'/row['relative'],target)
(folder/'lifecycle_fixtures.txt').write_text('analysis/vtt_client_services_20261019/test_runtime\n'+''.join('script/phongthan/npc_recovered_batch/'+n+'.lua\n' for n in names),'ascii')
p=ROOT/'PhongThanSource/Tests/Native/VanTienLifecycleTests.cpp';data=p.read_bytes()
needle=b'{"AddGlobalCountNews",Noop}'
if b'{"GetTeam",Zero}' not in data:data=data.replace(needle,b'{"GetTeam",Zero},'+needle,1)
if b'{"ShowMissionCountdown",Noop}' not in data:
 data=data.replace(needle,b'{"ShowMissionCountdown",Noop},{"AddNews",Noop},{"MsgBox",Noop},{"Sale",Sale},'+needle,1)
p.write_bytes(data)
print('Prepared actual current runtime scripts + staged changes, not old schedule fixtures.')

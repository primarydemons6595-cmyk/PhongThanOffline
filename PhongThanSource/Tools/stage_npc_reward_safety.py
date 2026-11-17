"""Stage bounded native reward fixes and safe original book-crafting callbacks."""
import json,re,shutil
from restore_original_world import ROOT,OUT
import build_offline_loose as b
batch=OUT/'npc_reward_safety';stage=batch/'staged/Server';batch.mkdir(exist_ok=True)
ledger_path=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
ledger=json.loads(ledger_path.read_text('utf8'));server=ROOT/'PhongThanRuntime-Loose/Server'
files=[];teachers=[]
for npc in ledger['ready']:
    if npc['name']!='教师' or npc['world'] not in (1020,1021):continue
    relative=npc['script_relative'];payload=(server/relative).read_bytes()
    callbacks=sorted(set(re.findall(rb'function\s+((?:[JDY]|PT)Check\d+)\s*\(',payload)))
    for raw in callbacks:
        name=raw.decode('ascii')
        payload+=f'''
pt_book_{name}={name}
function {name}()
    if pt_runtime_world_pos()~={npc['world']} then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("Hay de trong it nhat mot o trong F4 de nhan sach hoac vat pham.",1,"Thoat/no")
        return
    end
    return pt_book_{name}()
end
'''.encode('ascii')
    p=stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(payload)
    npc['lua_sha256']=b.sha(payload);teachers.append(npc)
    files.append(dict(relative=relative,sha256=b.sha(payload),before_sha256=b.sha((server/relative).read_bytes())))
metadata_relative='Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
metadata=json.dumps(ledger,ensure_ascii=False,indent=2).encode('utf8')
p=batch/'staged/Project'/metadata_relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(metadata)
plan=dict(ready=[],blocked=[],files=files,metadata=[dict(relative=metadata_relative,sha256=b.sha(metadata),before_sha256=b.sha(ledger_path.read_bytes()))],
    binaries=[dict(relative='CoreServer.dll',sha256=b.sha((ROOT/'PhongThanSource/Output/Server/CoreServer.dll').read_bytes()),before_sha256=b.sha((server/'CoreServer.dll').read_bytes()))])
(batch/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
fixtures=(OUT/'city_service_repair/dispatch_fixtures.txt').read_text('ascii').splitlines()
for npc in teachers:
    old='analysis/restoration_20261016/city_service_repair/staged/Server/'+npc['script_relative']
    fixtures=[line.replace(old,'analysis/restoration_20261016/npc_reward_safety/staged/Server/'+npc['script_relative']) for line in fixtures]
(batch/'dispatch_fixtures.txt').write_text('\n'.join(fixtures)+'\n','ascii')
pending=batch/'pending_client';pending.mkdir(exist_ok=True)
shutil.copy2(ROOT/'PhongThanSource/Output/Client/CoreClient.dll',pending/'CoreClient.dll')
client=ROOT/'PhongThanRuntime-Loose/Client'
(pending/'plan.json').write_text(json.dumps(dict(source=str(pending/'CoreClient.dll'),target=str(client/'CoreClient.dll'),client_exe=str(client/'Game.exe'),
    before_sha256=b.sha((client/'CoreClient.dll').read_bytes()),after_sha256=b.sha((pending/'CoreClient.dll').read_bytes())),indent=2),'utf8')
print('Staged safe book crafting for two cities and native reward binaries.')

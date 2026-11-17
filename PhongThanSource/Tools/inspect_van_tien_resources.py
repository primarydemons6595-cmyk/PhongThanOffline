import json
from restore_original_world import ROOT,OUT,Original
original=Original();folder=OUT/'van_tien';folder.mkdir(exist_ok=True)
paths=['script/'+scope+'/传送门.lua' for scope in ('崇城大营','蚩尤墓','玉虚宫','封神台')]
paths+=['script/missions/mission%02d.lua'%i for i in range(1,5)]
for path in paths:
    got=original.get(path.encode('gbk'))
    if got:
        (folder/(got[1]['id']+'.lua')).write_bytes(got[0]);print('SCRIPT',path,got[1]['id'],len(got[0]));print(got[0].decode('latin1')[:3200])
    else:print('MISSING',path)
raw=original.get(b'settings/Npcs.txt')[0].splitlines();headers=raw[0].split(b'\t')
print('HEADERS',[h.decode('gbk','replace') for h in headers])
selected=[]
for i,line in enumerate(raw[1:]):
    row=dict(zip(headers,line.split(b'\t')));name=row.get(b'Name',b'').decode('gbk','replace')
    if any(word in name for word in ('青龙','白虎','玄武','朱雀','通天','万仙','传送门')) or 380<=i<=410:
        item={k.decode('gbk','replace'):v.decode('gbk','replace') for k,v in row.items()};item['template']=i
        selected.append(item);print('NPC',i,name,{k:v for k,v in item.items() if 'Script' in k or k in ('Kind','AI','Level','Camp','Life')})
(folder/'npc_templates.json').write_text(json.dumps(selected,ensure_ascii=False,indent=2),'utf8')

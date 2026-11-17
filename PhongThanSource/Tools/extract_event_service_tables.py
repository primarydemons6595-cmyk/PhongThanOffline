"""Recover original commodity and escort tables for native API restoration."""
import json
from restore_original_world import Original,OUT
names=['商品描述','商店价格配置','商店描述','镖车运行时间配置','镖车配置']
o=Original();found=[]
for name in names:
    got=o.get(('settings/'+name+'.txt').encode('gbk'))
    if not got:print(name,'MISSING');continue
    data,proof=got
    (OUT/'priority_events'/(proof['id']+'.txt')).write_bytes(data)
    found.append(proof);print(name,proof['id'],len(data));print(data[:3800].decode('gbk','replace'))
(OUT/'priority_events/service_tables.json').write_text(json.dumps(found,ensure_ascii=False,indent=2),'utf8')

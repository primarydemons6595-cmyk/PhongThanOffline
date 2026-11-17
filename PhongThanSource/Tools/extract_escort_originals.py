import json,re
from restore_original_world import ROOT,Original,path_bytes
import build_offline_loose as b

folder=ROOT/'analysis/event_service_20261019/escort_originals';folder.mkdir(exist_ok=True)
o=Original();found={};missing=[]
for row in b.csv_read(ROOT/'PakRecoveryWork/02_PathDictionary/all_paths.csv'):
    value=row['CanonicalPath']
    if not value.lower().endswith('.lua') or not any(w in value for w in ('yun_biao','运镖','运粮')):continue
    raw=path_bytes(value);path=raw.decode('gbk','replace')
    if not path.lower().endswith('.lua') or not any(w in path for w in ('yun_biao','运镖','运粮')):continue
    got=o.get(raw)
    if not got:missing.append(path);continue
    data,proof=got
    found[proof['id']]=proof
    (folder/(proof['id']+'.lua')).write_bytes(data)
    print(path,proof['id'],len(data),re.findall(rb'function\s+(\w+)',data))
(folder/'index.json').write_text(json.dumps(dict(found=list(found.values()),missing=missing),ensure_ascii=False,indent=2),'utf8')

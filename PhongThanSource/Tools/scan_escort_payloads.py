import json,re
from restore_original_world import ROOT,Original
import build_offline_loose as b
o=Original();folder=ROOT/'analysis/event_service_20261019/escort_payload_scan';folder.mkdir(exist_ok=True);found=[]
needles=[b'SendCarriage',b'GetTGuard',b'NewSiegeWeapon',b'Khao',b'khao',b'267',b'268','粮'.encode('gbk'),'金牌'.encode('gbk'),b'GetTask(64)',b'GetTask( 64)']
for eid,e in o.winners.items():
    if 'script' not in e['pak'].name.lower() or not 50<e['size']<500000:continue
    try:data,_=b.entry_payload(e,{},None)
    except (ValueError,MemoryError):continue
    if b'function' not in data:continue
    hits=[n.hex() for n in needles if n in data]
    if not hits:continue
    (folder/(f'{eid:08X}.lua')).write_bytes(data);found.append(dict(id=f'{eid:08X}',pak=e['pak'].name,entry=e['entry'],sha256=b.sha(data),bytes=len(data),hits=hits))
(folder/'index.json').write_text(json.dumps(found,indent=2),'utf8');print('ESCORT_PAYLOADS',len(found))
for d in found:print(d['id'],d['bytes'],d['hits'])

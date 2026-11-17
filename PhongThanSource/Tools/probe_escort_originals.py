import json,re
from restore_original_world import ROOT,Original
o=Original();folder=ROOT/'analysis/event_service_20261019';proofs=[]
paths=['script/孟津/风林.lua','script/孟津/风林影子.lua']
for scope in ('绝龙岭','潼关','运粮','运镖','yun_biao'):
 for name in ('张桂芳','总兵张桂芳','风林','总镖头','神秘镖师'):
  paths.append('script/'+scope+'/'+name+'.lua')
for path in paths:
 got=o.get(path.encode('gbk'))
 if got:
  data,proof=got;proofs.append(proof);(folder/(proof['id']+'.lua')).write_bytes(data)
  print(path,proof['id'],len(data),'FUNCTIONS',re.findall(rb'function\s+(\w+)\(',data));print(data.decode('latin1')[:1000])
lines=o.get(b'settings/Npcs.txt')[0].splitlines();header=lines[0].split(b'\t')
for i,line in enumerate(lines[1:]):
 row=dict(zip(header,line.split(b'\t')));name=row.get(b'Name',b'').decode('gbk','replace')
 if any(word in name for word in ('粮','镖','桂芳','风林')):
  print('TEMPLATE',i,name,{k.decode('latin1'):v.decode('latin1') for k,v in row.items() if b'Script' in k or k in (b'Kind',b'Camp')})
(folder/'escort_original_proofs.json').write_text(json.dumps(proofs,ensure_ascii=False,indent=2),'utf8')

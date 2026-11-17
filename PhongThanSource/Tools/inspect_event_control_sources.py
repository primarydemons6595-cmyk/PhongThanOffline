import json,re
from restore_original_world import ROOT,OUT,Original
folder=ROOT/'analysis/event_service_20261019';folder.mkdir(exist_ok=True);o=Original()
p=json.loads((OUT/'prepared_changes.json').read_text('utf8'))
print('PREPARED',list(p));print('BOSS',p['bosses'][:1]);print('SCHEDULE',p['boss_schedule'][:2])
ledger=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))
for row in ledger['ready']:
 if any(word in row.get('name','') for word in ('张桂芳','风林','杨任','安安','武王','太岁','赏金')):
  print('NPC',row['world'],row['name'],row['script_relative'],row['original_lua']['id'])
  data=(ROOT/'PhongThanRuntime-Loose/Server'/row['script_relative']).read_bytes()
  print('FUNCTIONS',re.findall(rb'function\s+(\w+)\(',data))
for path in ('script/运镖/总兵张桂芳.lua','script/万仙阵/宝箱.lua'):
 got=o.get(path.encode('gbk'))
 if got:
  (folder/(got[1]['id']+'.lua')).write_bytes(got[0]);print('ORIGINAL',got[1])
for ident,scope in enumerate(('崇城大营','蚩尤墓','玉虚宫','封神台'),1):
 got=o.get(('script/'+scope+'/传送门.lua').encode('gbk'));assert got
 print('PORTAL',ident,got[1]['id'],'TICKETS',re.findall(rb'HaveNormalItem\(3,(\d+)',got[0]))

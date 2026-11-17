"""Compare original service payloads and deployed catalog references."""
import json
from restore_original_world import ROOT,OUT,Original

original=Original()
for name in ('settings/goods.txt','settings/buysell.txt'):
    source=original.get(name.encode('ascii'))
    deployed=(ROOT/'PhongThanRuntime-Loose/Server'/name).read_bytes()
    print(name,'exact_original=',bool(source and source[0]==deployed),'bytes=',len(deployed))
    if source: print('PROOF',source[1])

ledger=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))
for npc in ledger['ready']:
    if npc['world'] in (1002,1020,1021):
        print('NPC',npc['world'],npc['name'],npc['script_relative'])
for name in ('script/朝歌/教师.lua','script/朝歌/铜匠.lua','script/瑶池/赤松子.lua'):
    result=original.get(name.encode('gbk'))
    if result:
        print('ORIGINAL',name,result[1]);print(result[0].decode('latin1').split('function no()')[0][:1800])

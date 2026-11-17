"""Check published NPC records, exact original Lua prefixes and loose bindings."""
import collections,json,re,struct
from restore_original_world import ROOT,OUT,Original
from publish_original_world import parse_index
from prepare_npc_restoration import npc_records,split_region
import build_offline_loose as b

server=ROOT/'PhongThanRuntime-Loose/Server';original=Original()
ledger=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))
index=parse_index((server/'loose_index.bin').read_bytes());seen=set();result=[]
for n in ledger['ready']:
    key=n['world'],n['name'];assert key not in seen;seen.add(key)
    data=(server/n['override']).read_bytes();records=npc_records(split_region(data)[2]) if n['override'].endswith('_region_s.dat') else npc_records(data)
    matching=[r for r in records if r[60:]==n['lua'].encode('ascii')]
    assert len(matching)==1,key
    template,wx,wy=struct.unpack_from('<iii',matching[0]);assert(template,wx,wy)==(n['template'],n['world_x'],n['world_y'])
    assert [wx//256,wy//512]==n['positions'][0]
    payload=(server/n['script_relative']).read_bytes();assert b.sha(payload)==n['lua_sha256']
    exact=original.get(bytes.fromhex(n['original_lua']['raw_hex']))[0];assert payload.startswith(exact)
    for relative in (n['override'],n['script_relative']):
        raw=('\\'+relative.replace('/','\\')).encode('ascii');assert index[b.pak_id(raw)]==raw
    result.append(dict(world=n['world'],name=n['name'],display_position=n['positions'][0],original_lua_id=n['original_lua']['id'],verified=True))
report=dict(status='PASS',npcs=len(result),maps=len({n['world'] for n in result}),original_lua_prefixes=True,loose_bindings=True,
    exact_display_tiles=True,full_business_live_acceptance=False,monsters_changed=0,rows=result)
(OUT/'recovered_npcs_verification.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
print(json.dumps({k:v for k,v in report.items() if k!='rows'}))

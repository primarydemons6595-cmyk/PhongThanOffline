"""Corroborate Sina field-doctor positions against original scripts/maps."""
import json,re
from restore_original_world import ROOT,OUT,Original
from recover_npc_coordinate_tables import Page,fetch,SINA
from prepare_npc_lua_library import dependencies

original=Original();server=ROOT/'PhongThanRuntime-Loose/Server'
worlds={name.decode('gbk'):int(world) for world,name in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes())}
registry={int(runtime):int(content) for content,runtime in re.findall(r'\{(\d+),\s*(\d+),\s*PHONGTHAN_MAP_', (ROOT/'PhongThanSource/Headers/PhongThanMapIdentity.h').read_text('ascii'))}
url=SINA+'213000.shtml';page=Page(fetch(url).decode('gb18030','replace'));facts=[]
for cells in page.rows:
    if len(cells)!=3 or cells[0] not in worlds or not cells[1].isdigit() or not cells[2].isdigit():continue
    scope,x,y=cells[0],int(cells[1]),int(cells[2]);world=worlds[scope]
    got=original.get(('script/龙套/野外医生-'+scope+'.lua').encode('gbk'))
    if not got:got=original.get(('script/'+scope+'/医生.lua').encode('gbk'))
    facts.append(dict(world=world,content_map=registry.get(world),name='医生',positions=[[x,y]],translated_names=['Đại Phu'],
        source_lines=[],coordinate_sources=[dict(url=url,date='2005-03-15',map_name=scope,classification='SINA_HISTORICAL_CORROBORATE_WITH_CLIENT')],
        templates=[149],map_scripts=[got[1]] if got else [],scripts=[got[1]] if got else [],world_registered=True,
        appearance_evidence='Original populated region doctor resource/template 149, passerby001',existing=[]))
report=json.loads((OUT/'missing_npc_batch.json').read_text('utf8'))
existing=json.loads((ROOT/'PhongThanSource/Docs/NPC_RESTORATION_DEPLOYMENT.json').read_text('utf8'))['rows']
ledger=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))['ready']
populated=json.loads((OUT/'placement_payloads.json').read_text('utf8'))['populated']
for row in facts:
    # Existing doctor identity wins even when old China coordinates differ.
    row['existing']=[dict(name=n.get('name'),status='PUBLISHED') for n in existing+ledger if n['world']==row['world'] and (n.get('name')=='医生' or n.get('template') in (149,766) or n.get('archetype')=='doctor')]
    for p in populated:
        for n in p['records']:
            if n['kind']==3 and n['template'] in (149,766) and n['script'].endswith('野外医生-'+next(k for k,v in worlds.items() if v==row['world'])+'.lua'):
                row['existing'].append(dict(name=n['name'],status='ORIGINAL_PLACEMENT'))
    if any(n['world']==row['world'] and n['name']=='医生' for n in report['rows']):continue
    report['rows'].append(row)
(OUT/'wild_doctor_coordinates.json').write_text(json.dumps(facts,ensure_ascii=False,indent=2),'utf8')
(OUT/'wild_doctor_batch.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
print(json.dumps(dict(facts=len(facts),new=sum(not n['existing'] for n in facts),original_map_lua=sum(bool(n['map_scripts']) and not n['existing'] for n in facts))))
for n in facts:print(n['world'],n['positions'],'EXISTING',bool(n['existing']),'SCRIPT',bool(n['map_scripts']))

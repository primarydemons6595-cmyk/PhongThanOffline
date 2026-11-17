"""Export exact client NPC MPS separately from website display-tile evidence."""
import csv,json
from restore_original_world import ROOT,OUT

imports=json.loads((ROOT/'PhongThanSource/Docs/VNG_NPC_REGION_IMPORT.json').read_text('utf-8-sig'))['Npcs']
worlds={(n['Template'],n['X'],n['Y']):n['MapIds'] for n in imports}
payloads=json.loads((OUT/'placement_payloads.json').read_text('utf8'))['populated']
rows=[]
for p in payloads:
    if not p['selected_by_available_chain']:continue
    for n in p['records']:
        if n['kind']!=3:continue
        rows.append(dict(maps=worlds.get((n['template'],n['x'],n['y']),[]),name=n['name'],template=n['template'],
            mps_x=n['x'],mps_y=n['y'],display_x=n['x']//256,display_y=n['y']//512,script=n['script'],
            pak=p['pak'],entry=p['entry'],payload_id=p['id'],payload_sha256=p['sha256'],
            status='EXACT_CLIENT_RECORD; KHAl_MINH_DIEN_2026_VERSION_NOT_PROVEN'))
assert len(rows)==38
(OUT/'original_exact_npc_coordinates.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2),'utf8')
with (OUT/'original_exact_npc_coordinates.csv').open('w',encoding='utf-8-sig',newline='') as stream:
    writer=csv.DictWriter(stream,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)
facts=json.loads((OUT/'vng_coordinate_evidence.json').read_text('utf8'))['facts']
with (OUT/'vng_all_coordinate_contexts.csv').open('w',encoding='utf-8-sig',newline='') as stream:
    writer=csv.DictWriter(stream,fieldnames=['x','y','source','context','classification']);writer.writeheader();writer.writerows(facts)
print('EXPORTED exact_client_npcs=38 vng_contexts='+str(len(facts)))

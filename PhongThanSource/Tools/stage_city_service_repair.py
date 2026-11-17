"""Recover missing city services using published coordinates and PAK Lua."""
import json
from restore_original_world import ROOT,OUT,Original
import stage_missing_npcs as staging

original=Original()
facts=json.loads((OUT/'historical_npc_coordinates.json').read_text('utf8'))['facts']
ledger=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json').read_text('utf8'))
names={'教师':(154,'Võ Sư'),'铜匠':(777,'Thợ Đồng'),'仓库管理员':(768,'Thủ Khố'),'杂货商':(778,'Tạp Hóa')}
rows=[]
for world,scope in ((1020,'西岐'),(1021,'朝歌')):
    for name,(template,vn) in names.items():
        if any(n['world']==world and n['name']==name for n in ledger['ready']):continue
        positions=[f for f in facts if f['world']==world and f['original_name']==name]
        if not positions:continue
        # Keep the first published service coordinate; retain conflicting facts.
        fact=positions[0]
        got=original.get(('script/'+scope+'/'+name+'.lua').encode('gbk'))
        if not got:continue
        rows.append(dict(world=world,world_registered=True,content_map=world-1000,name=name,existing=[],positions=[[fact['x'],fact['y']]],
            templates=[template],map_scripts=[got[1]],translated_names=[vn],coordinate_evidence=positions,
            appearance_status='ORIGINAL_NAMED_SERVICE_RESOURCE',scripts=[got[1]],source_lines=[]))
input_path=OUT/'city_service_repair_input.json'
input_path.write_text(json.dumps(dict(rows=rows),ensure_ascii=False,indent=2),'utf8')
staging.BATCH=OUT/'city_service_repair';staging.STAGE=staging.BATCH/'staged/Server'
staging.stage(input_path.name)

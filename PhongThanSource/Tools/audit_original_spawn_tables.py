"""Inspect original named spawn/load tables rather than inferring region data."""
import json,re
from restore_original_world import Original,ROOT,OUT,path_bytes
import build_offline_loose as b
o=Original();found=[];missing=[]
for row in b.csv_read(ROOT/'PakRecoveryWork/02_PathDictionary/all_paths.csv'):
    try:raw=path_bytes(row['CanonicalPath']);path=raw.decode('gbk')
    except UnicodeError:continue
    name=path.rsplit('\\',1)[-1].lower()
    if not path.lower().startswith('\\settings\\') or not name.endswith(('.txt','.ini','.tab')):continue
    if not re.search(r'npc.*(?:load|pos)|spawn|刷怪|npc坐标|怪物坐标|任务npc|npc加载|npc位置',name,re.I):continue
    got=o.get(raw)
    if not got:missing.append(path);continue
    data,proof=got;found.append(proof)
    folder=OUT/'spawn_tables';folder.mkdir(exist_ok=True);(folder/(proof['id']+'.txt')).write_bytes(data)
    print(path,len(data),repr(data[:500].decode('gbk','replace')))
(OUT/'spawn_table_audit.json').write_text(json.dumps(dict(found=found,missing=missing),ensure_ascii=False,indent=2),'utf8')
print('FOUND',len(found),'MISSING',len(missing))

from pathlib import Path
import importlib.util,re,json,hashlib
root=Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('pak',root/'PhongThanSource/Tools/build_offline_loose.py');pak=importlib.util.module_from_spec(spec);spec.loader.exec_module(pak)
client=root/'Client_Original';out=root/'analysis/skill_progression_20261018';out.mkdir(exist_ok=True)
chain=[client/'data'/n.strip() for n in re.findall(r'^\d+=([^\r\n]+)',(client/'package.ini').read_text('ascii'),re.M) if (client/'data'/n.strip()).exists()]
index,_=pak.pak_index(chain,strict=False)
paths=[b'\\settings\\skills.ini',b'\\settings\\item\\001\\skillbook.txt']
receipt=[]
for path in paths:
 entry=index[pak.pak_id(path)];data,backend=pak.entry_payload(entry,None,None)
 target=out/('original_'+path.split(b'\\')[-1].decode());target.write_bytes(data)
 receipt.append({'path':path.decode(),'pak':str(entry['pak']),'offset':entry['offset'],'sha256':hashlib.sha256(data).hexdigest(),'backend':backend,'output':str(target)})
(out/'original_provenance.json').write_text(json.dumps(receipt,indent=2),encoding='utf8')
ini=(out/'original_skills.ini').read_text('latin1');sections={};section=''
for line in ini.splitlines():
 if line.startswith('['):section=line.strip('[]');sections[section]={}
 elif '=' in line and section:
  k,v=line.split('=',1);sections[section][k]=v
parents={k:v for k,v in sections.items() if k.isdigit() and 3<=int(k)<=51 or k.isdigit() and 450<=int(k)<=458}
(out/'skill_parent_rules.json').write_text(json.dumps(parents,indent=2),encoding='utf8')
print(json.dumps(receipt,indent=2));print('Nonempty parents:',[(k,v.get('SkillParent')) for k,v in parents.items() if v.get('SkillParent') not in ('0:',None)])

import json,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];out=ROOT/'analysis/doctors_thuongchu_20261102';server=ROOT/'PhongThanRuntime-Loose/Server';stage=out/'staged/Server'
doctors=json.loads((out/'doctor-services-plan.json').read_text('utf8'))['files'];audit=json.loads((out/'full-audit.json').read_text('utf8'))
aliases={}
for row in doctors:
 key=('\\'+row['relative'].replace('/','\\')).encode('latin1').lower();aliases[key]=row['shop']-1
 for npc in audit['doctors']:
  if npc['world']==row['world']:aliases[bytes.fromhex(npc['script_hex']).lower()]=row['shop']-1
placements=json.loads((out/'placement-plan.json').read_text('utf8'))['entries']
for row in placements:
 if row.get('name')=='Đại Phu':aliases[('\\'+row['script'].replace('/','\\')).encode('ascii')]=22
old=(server/'settings/phongthan/BotShopRegistry.tsv').read_bytes();rows=[]
for line in old.splitlines():
 c=line.split(b'\t')
 if len(c)==3 and bytes.fromhex(c[0].decode()).lower() in aliases:continue
 rows.append(line)
for key,shop in sorted(aliases.items()):rows.append(key.hex().encode()+b'\tpt_doctor_shop\t'+str(shop).encode())
data=b'\n'.join(rows)+b'\n';relative='settings/phongthan/BotShopRegistry.tsv';target=stage/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
(out/'bot-service-plan.json').write_text(json.dumps(dict(files=[relative],before_sha256=hashlib.sha256(old).hexdigest(),aliases=len(aliases)),indent=2),'utf8')
print('Doctor merchant bindings',len(aliases),'all callbacks must still be issued by live NPC UI')

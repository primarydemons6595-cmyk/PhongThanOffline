"""Inspect the existing identities before binding TGPT battlefield rewards."""
import json,sys
from pathlib import Path
from extract_client_coordinate_dictionary import decode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
s=ROOT/'PhongThanRuntime-Loose/Server'
out=ROOT/'analysis/thuongchu_tgpt_20261103';out.mkdir(exist_ok=True)
items=[]
for p in (s/'settings/item/001').glob('*.txt'):
 rows=p.read_bytes().splitlines()
 for i,line in enumerate(rows[1:],1):
  c=line.split(b'\t');name=decode_tcvn3(c[0])
  if any(w in name.lower() for w in ['tướng ấn','thương chu','chiến trường']):
   items.append(dict(file=p.name,row=i,name=name,columns=[x.decode('latin1') for x in c],header=[x.decode('latin1') for x in rows[0].split(b'\t')]))
a=json.loads((ROOT/'analysis/doctors_thuongchu_20261102/full-audit.json').read_text('utf8'))
npcs=[r for r in a['npcs'] if r['world'] in (1064,1071)]
(out/'identities.json').write_text(json.dumps(dict(items=items,npcs=npcs),ensure_ascii=False,indent=2),'utf8')
for r in items:print(r['file'],r['row'],r['name'],r['columns'][1:5])
for r in npcs:print('NPC',r['world'],r['template'],r['display_x'],r['display_y'],bytes.fromhex(r['name_hex']).decode('gbk','replace'),bytes.fromhex(r['script_hex']).decode('latin1'))

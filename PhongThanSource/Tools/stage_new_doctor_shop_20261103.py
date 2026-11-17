"""Only the24 newly added doctors get buttons and an isolated expanded shop."""
import json,hashlib
from pathlib import Path
from extract_client_coordinate_dictionary import decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];s=ROOT/'PhongThanRuntime-Loose/Server';out=ROOT/'analysis/thuongchu_tgpt_20261103';package=ROOT/'PhongThanSource/Deploy/ProjectContent/ThuongChuTgpt20261103';m=json.loads((package/'manifest.json').read_text('utf8'));stage=package/'compiled/Server'
def put(path,data):
 p=stage/path;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data);before=s/path
 m['files'].append(dict(relative=path,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(before.read_bytes()).hexdigest() if before.exists() else None,bytes=len(data)))
placements=json.loads((ROOT/'analysis/doctors_thuongchu_20261102/placement-plan.json').read_text('utf8'))
paths=sorted({r['script'] for r in placements['entries'] if r.get('script','').startswith('script/phongthan/services/doctor_')})
assert len(paths)==24
goods=(s/'settings/goods.txt').read_bytes().splitlines();potion=(s/'settings/item/001/potion.txt').read_bytes().splitlines()
names=['Tiểu Hồng Đơn','Tiểu Hoàn Đơn','Trung Hồng Đơn','Trung Hoàn Đơn'];found={}
for line in potion[1:]:
 c=line.split(b'\t');name=decode_tcvn3(c[0]).lstrip('#')
 match=next((wanted for wanted in names if wanted.casefold()==name.casefold()),None)
 if match:found[match]=c
assert len(found)==4,found.keys()
shop=(s/'settings/buysell.txt').read_bytes().splitlines();rows=len(shop)-1
# Shop23 is shared by the older doctors: clone it and leave its row intact.
stock=[int(x) for x in shop[23].split(b'\t') if x and int(x)>0];added=[]
for name in names:
 c=found[name];identity=tuple(c[i] for i in [1,2,3,11]);matches=[]
 for i,line in enumerate(goods[1:],1):
  g=line.split(b'\t')
  if len(g)>=16 and tuple(g[j] for j in [0,1,2,4])==identity and not any(v not in (b'',b'0') for v in g[5:15]):matches.append(i)
 assert matches,(name,identity,matches)
 # Goods repeats the same potion for multiple shop categories. Use the first
 # silver-only row, whose generated identity is exactly the verified potion.
 matches=[min(matches)]
 if matches[0] not in stock:stock.append(matches[0]);added.append(dict(name=name,goods=matches[0],tuple=[int(x) for x in identity]))
width=max(len(x.split(b'\t')) for x in shop);assert len(stock)<=width
newShop=rows+1;assert newShop-1<95 # no collision with cash-shop category indices
newRow=b'\t'.join([str(i).encode() for i in stock]+[b'-1']*(width-len(stock)))
put('settings/buysell.txt',b'\r\n'.join(shop+[newRow])+b'\r\n')
for path in paths:
 data=(s/path).read_bytes();assert data.count(b'Say(')==1 and data.count(b'Sale(23)')==1
 put(path,data.replace(b'Say(',b'SayButtons(').replace(b'Sale(23)',f'Sale({newShop})'.encode()))
registry=(s/'settings/phongthan/BotShopRegistry.tsv').read_bytes().splitlines();updated=[];changes=0
for row in registry:
 c=row.split(b'\t')
 if len(c)==3:
  path=bytes.fromhex(c[0].decode()).decode('latin1').lstrip('\\').replace('\\','/')
  if path in paths:assert c[1]==b'pt_doctor_shop';c[2]=str(newShop-1).encode();row=b'\t'.join(c);changes+=1
 updated.append(row)
assert changes==24,changes
put('settings/phongthan/BotShopRegistry.tsv',b'\n'.join(updated)+b'\n')
(package/'manifest.json').write_text(json.dumps(m,ensure_ascii=False,indent=2),'utf8')
(out/'new-doctors.json').write_text(json.dumps(dict(paths=paths,shop=newShop,added=added,preserved_shared_shop_sha256=hashlib.sha256(shop[23]).hexdigest()),ensure_ascii=False,indent=2),'utf8')
print('Staged only',len(paths),'new doctor button menus, isolated shop',newShop,'and four verified potion identities')

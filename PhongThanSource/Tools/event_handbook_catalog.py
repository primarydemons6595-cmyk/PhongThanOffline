from pathlib import Path
import sys,re,json,hashlib,collections
sys.path.insert(0,'PhongThanSource/Tools')
from extract_client_coordinate_dictionary import decode_tcvn3,encode_tcvn3
sys.stdout.reconfigure(encoding='utf-8')
root=Path('PhongThanRuntime-Loose/Server/settings/item/001');out=Path('analysis/event_handbook_20261026');stage=out/'compiled';entries=[];tables={};icons={};proofs={}
def rows(path):return [r.split(b'\t') for r in path.read_bytes().splitlines() if r]
def num(x,default=0):
 try:return int(x)
 except ValueError:return default
def clean(raw):
 raw=re.sub(rb'<(?:c|color|bclr|bordercolor|font|pic)(?:=[^>]*)?>',b'',raw).lstrip(b'#$')
 raw=re.sub(rb'<([^>]*)>',rb'(\1)',raw) # Keep authored Quai Phu direction names.
 return raw.replace(b'/',b'-').replace(b'|',b'-').replace(b'\t',b' ').replace(b'\n',b' ')
def load(name):
 if name not in tables:
  p=root/(name+'.txt')
  if name in ('instrument','jadependant'):p=out/('probe_'+name+'.txt')
  tables[name]=rows(p);proofs[name]=hashlib.sha256(p.read_bytes()).hexdigest()
 return tables[name]
def add(table,row,category,quality=0,recipe=0,name=None):
 r=load(table)[row];equip=table in ('helm','armor','belt','boot','pendant','meleeweapon','rangeweapon','horse','amulet','ring','cuff','signet','instrument','jadependant')
 genre=num(r[1]);detail=num(r[2]);part=num(r[3]) if equip or table in ('magicscript','ibitem') else 0;level=num(r[11]) if equip or table in ('magicscript','ibitem') else 0
 offset=53 if table=='instrument' else 44
 req={num(r[c]):num(r[c+1]) for c in range(offset,offset+12,2) if r[c]} if equip else {}
 profession=req.get(37,-1);tier=req.get(219,req.get(36,0));realm=1 if 219 in req else 0
 icon=r[4] if equip or table in ('magicscript','ibitem') else r[3];icon=b'\\'+icon.replace(b'/',b'\\').lstrip(b'\\');key=hashlib.sha256(icon).hexdigest()[:12];alias=b'\\spr\\pt_test\\'+key.encode()+b'.spr';icons[key]=icon
 if icon==b'\\':
  icons.pop(key,None);return None # state-only pseudo-items have no physical item icon
 name=clean(name or r[0])
 state=''
 if table=='material':
  if detail in set(range(253,281))|{374,383,392,401,5231}|set(range(5225,5228))|set(range(5524,5538)):state='Chưa mài'
  elif detail in set(range(281,302))|set(range(375,383))|set(range(384,392))|set(range(393,401))|set(range(402,410)):state='Đã mài'
  elif detail in set(range(302,309))|set(range(5228,5231))|set(range(5232,5240)):state='Mẫu chưa hỗ trợ thuộc tính'
 if table=='material' and (detail in set(range(274,281))|set(range(5225,5228))|{5231}|set(range(5524,5538))):state='Chưa mài (chưa hỗ trợ thuộc tính)'
 e=dict(id=len(entries)+1,table=table,row=row,genre=genre,detail=detail,part=part,level=level,profession=profession,tier=tier,realm=realm,slot=detail if equip else -1,quality=quality,recipe=recipe,category=category,name=decode_tcvn3(name),name_hex=name.hex(),polish_state=state,image=alias.decode(),original_icon_hex=icon.hex());entries.append(e);return e
def tuple_rows(genre,detail,part=0):
 table={3:'material',4:'questkey',6:'magicscript',8:'ibitem'}.get(genre)
 if not table:return []
 result=[]
 for n,r in enumerate(load(table)[1:],1):
  if num(r[1])==genre and num(r[2])==detail and (table not in ('magicscript','ibitem') or num(r[3])==part):result.append((table,n))
 return result
# Equipment class comes from the original requireseries attribute, NOT the
# source Series field (which also encodes green/set attribute groups).
for table in ['helm','armor','belt','pendant','boot']:
 for n,r in enumerate(load(table)[1:],1):
  if len(r)<70:continue
  req={num(r[c]):num(r[c+1]) for c in range(44,56,2) if r[c]}
  if req.get(37,-1) not in range(4):continue
  realm=219 in req;setid=num(r[69]);additional=any(num(r[c]) for c in range(29,44,3))
  if realm:add(table,n,'realm',3)
  elif setid>0:add(table,n,'green',1)
  elif not additional and num(r[9])==0:add(table,n,'white',0)
recipe_rows=load('zhuang_bei_he_cheng_gui_ze_biao');attributes=load('zhuang_bei_dian_lan_fu_jia_shu_xing_biao');attr_byid={num(r[0]):r for r in attributes[1:]}
upgrade=set();orange=set()
def recipe_tuple(raw):
 m=re.match(rb'^(\d+)\.(\d+)\.(\d+)\.(\d+)',raw)
 return tuple(map(int,m.groups())) if m else None
for r in recipe_rows[1:]:
 if len(r)!=19:continue
 group,kind=num(r[14]),num(r[15]);count=num(r[2]);ingredients=[recipe_tuple(v) for v in r[3:3+count]]
 target=orange if kind==2 and 2<=group<=6 else upgrade if group==1 and kind in (1,7) else None
 if target is not None:
  for t in ingredients:
   if t and t[0] in (3,4,6,8):target.add(t[:3])
 if kind==2 and 2<=group<=6 and num(r[16])==100 and ingredients and ingredients[0] and ingredients[0][0]==0:
  t=ingredients[0];table={7:'helm',2:'armor',6:'belt',9:'pendant',5:'boot'}.get(t[1]);a=attr_byid.get(num(r[1]))
  if not table or not a:continue
  prefix=clean(a[1]).rstrip(b'* ')
  mask=re.search(rb'\(([^)]+)\)',r[3]);allowed={int(x) for x in mask[1].split(b',')} if mask else None
  for n,base in enumerate(load(table)[1:],1):
   if num(base[1])==0 and num(base[2])==t[1] and num(base[3])==t[2] and (allowed is None or num(base[11]) in allowed):add(table,n,'orange',2,num(r[0]),prefix+b' '+clean(base[0]))
# Socket gem/catalyst identities are recovered directly from the rule tables.
socket=Path('PhongThanRuntime-Loose/Server/settings/item/da_kong_xiang_qian')
for p,col in [('da_kong_xu_qiu_biao_sc.txt',49),('bao_shi_kong_xiang_qian_xu_qiu_biao_sc.txt',2)]:
 for r in rows(socket/p)[1:]:
  for raw in r[col:col+6]:
   m=re.fullmatch(rb'(\d+)\.(\d+)\.(\d+)',raw)
   if m:qty,g,d=map(int,m.groups());upgrade.add((g,d,0))
for r in rows(socket/'bao_shi_shu_xing_biao_sc.txt')[1:]:upgrade.add((num(r[1]),num(r[2]),num(r[3])))
for cat,items in [('upgrade_material',upgrade),('orange_material',orange)]:
 for g,d,p in sorted(items):
  for table,n in tuple_rows(g,d,p):add(table,n,cat)
for table in ['meleeweapon']:
 for n,r in enumerate(load(table)[1:],1):
  req={num(r[c]):num(r[c+1]) for c in range(44,56,2) if r[c]}
  if req.get(37,-1) not in range(4):continue
  if num(r[9])==1000:add(table,n,'weapon_gold',4)
  elif num(r[9])==0 and not any(num(r[c]) for c in range(29,44,3)):add(table,n,'weapon_white',0)
for table in ['amulet']:
 if not (root/(table+'.txt')).exists():continue
 for n,r in enumerate(load(table)[1:],1):add(table,n,'fabao')
for table,category in [('instrument','phapkhi'),('jadependant','ngocboi')]:
 for n,r in enumerate(load(table)[1:],1):add(table,n,category)
for n,r in enumerate(load('horse')[1:],1):
 req={num(r[c]):num(r[c+1]) for c in range(44,56,2) if r[c]}
 if req.get(37,-1) in range(4):add('horse',n,'mount')
pet_words=['linh nguyện','linh thú','thú lương','thú tiên','thú đan','linh đơn','hồn tinh','thú hồn','thức ăn','linh ngọc','ngự thú','linh quả']
for table in ['material','ibitem','magicscript','questkey']:
 for n,r in enumerate(load(table)[1:],1):
  name=decode_tcvn3(r[0]).casefold()
  if any(w in name for w in pet_words):add(table,n,'pet_material')
# Explicit original materials needed for jade testing; appended to preserve old IDs.
for genre,detail,part in [(6,739,1),(8,1081,2)]+[(3,n,0) for n in list(range(1045,1050))+list(range(253,309))+list(range(374,410))+list(range(5225,5240))]:
 if genre==6:
  candidates=[('magicscript',n) for n,r in enumerate(load('magicscript')[1:],1) if r[1:4]==[b'6',b'1',b'739']]
 else:candidates=tuple_rows(genre,detail,part)
 for table,row in candidates:add(table,row,'jade_material')

# Keep old choice IDs stable, but never show orange templates that cannot
# satisfy the original recipe output [native level] identity.
recipe_by_id={num(r[0]):r for r in recipe_rows[1:]}
for e in entries:
 if e['category']!='orange':continue
 r=recipe_by_id[e['recipe']]
 m=re.fullmatch(rb'(\d+)\.(\d+)\.(\d+)\.1\[(\d+)\]',r[13])
 if not m or tuple(map(int,m.groups()))!=(e['genre'],e['detail'],e['part'],e['level']):
  e['disabled_reason']='ORANGE_RECIPE_TEMPLATE_LEVEL_MISMATCH'

# Original Tinh Quan rows are their own authored templates, not fabricated
# orange prefixes. They were previously omitted by the white/green filter.
tinh_quan_classes={}
for r in load('armor')[1:]:
 if len(r)<70 or 'tinh quân' not in decode_tcvn3(r[0]).casefold():continue
 req={num(r[c]):num(r[c+1]) for c in range(44,56,2) if r[c]}
 if req.get(37,-1) in range(4):tinh_quan_classes[(num(r[3]),num(r[11]))]=req[37]
for table in ['helm','armor','belt','pendant','boot','meleeweapon']:
 for n,r in enumerate(load(table)[1:],1):
  if len(r)<70 or 'tinh quân' not in decode_tcvn3(r[0]).casefold():continue
  req={num(r[c]):num(r[c+1]) for c in range(44,56,2) if r[c]}
  profession=req.get(37,-1)
  if profession not in range(4) and table in ('belt','boot'):
   profession=tinh_quan_classes.get((num(r[3]),num(r[11])),-1)
  if profession not in range(4):continue
  entry=add(table,n,'tinh_quan_weapon' if table=='meleeweapon' else 'tinh_quan',4)
  if entry:entry['profession']=profession

# Reversion catalyst, exact original IB identity. Append to preserve prior IDs.
for table,n in tuple_rows(8,509,2):add(table,n,'jade_material')

target=stage/'Server/settings/phongthan/EventHandbookCatalog.txt';target.parent.mkdir(parents=True,exist_ok=True)
columns=['Id','Category','Profession','Tier','Realm','Slot','Quality','Table','Row','Genre','Detail','Part','Level','Recipe','Name','Image']
payload=['\t'.join(columns).encode()]
for e in entries:
 values=[str(e[k]).encode('ascii') for k in ['id','category','profession','tier','realm','slot','quality','table','row','genre','detail','part','level','recipe']]
 payload.append(b'\t'.join(values+[bytes.fromhex(e['name_hex']),e['image'].encode('ascii')]))
target.write_bytes(b'\r\n'.join(payload)+b'\r\n')
(out/'catalog.json').write_text(json.dumps(dict(entries=entries,icons={k:v.hex() for k,v in icons.items()},table_hashes=proofs),ensure_ascii=False,indent=2),encoding='utf-8')
print('Entries',len(entries),'icons',len(icons),'categories',dict(collections.Counter(e['category'] for e in entries)))

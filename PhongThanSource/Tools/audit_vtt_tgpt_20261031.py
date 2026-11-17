"""Read-only comparison of Loose VTT with the supplied TGPT article."""
from pathlib import Path
import configparser,hashlib,json,re,struct,unicodedata
from prepare_npc_restoration import split_region,npc_records
from extract_client_coordinate_dictionary import decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];SERVER=ROOT/'PhongThanRuntime-Loose/Server'
OUT=ROOT/'analysis/vtt_tgpt_20261031';OUT.mkdir(exist_ok=True)
article=json.loads((OUT/'article.json').read_text('utf-8-sig'))['data']['post']
config=json.loads((SERVER/'EventService/events.json').read_text('utf8'))
placements=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/VttPopulation20261031/manifest.json').read_text('utf8'))['placements']
rows=[r.split(b'\t') for r in (SERVER/'settings/phongthan/Npcs.txt').read_bytes().splitlines()];header=rows[0]
audit=[]
portal_files=['1002_f57647b5.lua','1004_281c1583.lua','1003_479d2a84.lua','1001_41e3ecb2.lua']
portal_maps=[1002,1004,1003,1001]
for ident in range(1,5):
 rule=next(r for r in config['Events'] if r['Id']==ident)
 mission=(SERVER/('script/phongthan/van_tien/mission%02d.lua'%ident)).read_bytes()
 mobs=re.findall(rb'\{(\d+),(\d+),(\d+)\}',re.search(rb'PT_MOBS=([^\r\n]+)',mission)[1])
 p=placements[ident-1];records=npc_records(split_region((SERVER/p['override']).read_bytes())[2])
 services=[dict(template=struct.unpack_from('<i',r)[0],x=struct.unpack_from('<i',r,4)[0],y=struct.unpack_from('<i',r,8)[0],script=r[60:].decode('ascii')) for r in records]
 slots={str(tid):{k.decode():v.decode('latin1') for k,v in zip(header,rows[tid+1]) if b'DropRateFile' in k} for tid in [128+ident*5,[410,413,414,415][ident-1]]}
 drops=[]
 for typ in ['tongtian','baoxiang']:
  path=SERVER/('settings/item/droprate/%s%d.ini'%(typ,ident));ini=configparser.ConfigParser(strict=False);ini.read_string(path.read_bytes().decode('latin1'))
  entries=[dict(section=s,**dict(ini[s])) for s in ini.sections() if s!='Main']
  drops.append(dict(path=str(path.relative_to(SERVER)),sha256=hashlib.sha256(path.read_bytes()).hexdigest(),main=dict(ini['Main']),rows=entries))
 portals=[];target=('\\script\\phongthan\\npc_recovered_batch\\'+portal_files[ident-1]).encode()
 for path in (SERVER/('settings/phongthan/npc_regions/%d'%portal_maps[ident-1])).rglob('*.dat'):
  raw=path.read_bytes();rs=npc_records(split_region(raw)[2]) if '_region_s' in path.name else npc_records(raw)
  for r in rs:
   if r[60:].rstrip(b'\0').lower()==target.lower():
    tid,x,y=struct.unpack_from('<iii',r);portals.append(dict(template=tid,display=[x//256,y//512]))
 audit.append(dict(id=ident,map=1066+ident,config=rule,actual_level_range=[ident*20+11,ident*20+30],normal_count=len(mobs),normal_templates=sorted({int(m[0]) for m in mobs}),guard_templates=list(range(129+(ident-1)*5,133+(ident-1)*5)),boss_template=128+ident*5,services=services,portals=portals,drop_slots=slots,drop_tables=drops))
def norm(text):return ''.join(c for c in unicodedata.normalize('NFD',text.lower()) if unicodedata.category(c)!='Mn' and c.isalnum())
pearl_rows=[]
for path in (SERVER/'settings/item/001').glob('*.txt'):
 for r in path.read_bytes().splitlines()[1:]:
  cols=r.split(b'\t');name=decode_tcvn3(cols[0])
  if 'thongthienchau' in norm(name):pearl_rows.append(dict(file=path.name,name=name,identity=[v.decode('latin1') for v in cols[1:4]]))
result=dict(scope='READ_ONLY_LOOSE_AUDIT',source_url='https://tamgioipt.vn/van-tien-tran-tgpt',article_id=article['id'],article_public_time=article['public_time'],article_sha256=hashlib.sha256((OUT/'article.json').read_bytes()).hexdigest(),event_revision=config['Revision'],arenas=audit,pearl_catalog_matches=pearl_rows,world_live_verified=False,gameplay_changed=False)
(OUT/'audit.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
print(json.dumps(dict(arenas=len(audit),levels=[a['actual_level_range'] for a in audit],portals=[a['portals'] for a in audit],pearl_catalog_matches=pearl_rows,gameplay_changed=False),ensure_ascii=True))

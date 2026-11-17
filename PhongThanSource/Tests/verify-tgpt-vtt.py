"""Verify the staged/runtime TGPT derivative without starting a world."""
from pathlib import Path
import hashlib,json,re,struct,sys,configparser
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from prepare_npc_restoration import split_region,npc_records
from build_authored_npcs import barrier_at,blocked
SERVER=ROOT/'PhongThanRuntime-Loose/Server'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/TgptVtt20261031'
manifest=json.loads((PACKAGE/'manifest.json').read_text('utf8'))
base=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else PACKAGE/'compiled'
def staged(role,rel):
 p=base/role/rel
 return p.read_bytes() if p.exists() else (ROOT/'PhongThanRuntime-Loose'/role/rel).read_bytes()
for f in manifest['files']:assert hashlib.sha256((base/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
config=json.loads(staged('Server','EventService/events.json'))
catalog=json.loads(staged('Server','EventService/catalog.json'))
for i in range(1,5):
 rule=next(r for r in config['Events'] if r['Id']==i);default=next(r['Defaults'] for r in catalog if r['Id']==i)
 expected=['%02d:%02d'%(h,(i-1)*5) for h in [10,13,18,21,23]]
 assert rule['DailyTimes']==default['DailyTimes']==expected and rule['RegistrationSeconds']+rule['DurationSeconds']==2100
 assert rule['TicketDetail']==61+i and rule['MinimumLevel']==[30,51,71,91][i-1]
for role in ['Server','Client']:
 for file in ['material.txt','magicscript.txt']:
  rel='settings/item/001/'+file;before=(PACKAGE/'source'/role/rel).read_bytes().splitlines();after=staged(role,rel).splitlines()
  if file=='material.txt':
   assert after[:len(before)]==before and len(after)==len(before)+4
   for ident,line in enumerate(after[-4:]):
    r=line.split(b'\t');assert r[1:3]==[b'3',str(manifest['pearls'][ident]).encode()] and r[9]==b'10000'
    assert all(r[k]==b'1' for k in [14,15,17,20]) and r[18]==b'0'
  else:
   assert len(after)==len(before)
   for old,new in zip(before,after):
    if old==new:continue
    a,b=old.split(b'\t'),new.split(b'\t');assert a[1:4]==[b'6',b'1',b'12'] and all(x==y for n,(x,y) in enumerate(zip(a,b)) if n not in [0,8])
worlds=dict((int(k),v.decode('latin1')) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(SERVER/'settings/WorldSet.ini').read_bytes()))
def effective(world,rx,ry,changed):
 rel='settings/phongthan/npc_regions/%d/v_%03d/%03d_region_s.dat'%(world,ry,rx)
 if changed and (base/'Server'/rel).exists():return npc_records(split_region((base/'Server'/rel).read_bytes())[2])
 original=PACKAGE/'source/Server'/rel
 if original.exists() and original.stat().st_size:return npc_records(split_region(original.read_bytes())[2])
 if not original.exists() and (SERVER/rel).exists():return npc_records(split_region((SERVER/rel).read_bytes())[2])
 for suffix in ['_S','']:
  p=SERVER/'maps'/(worlds[world]+suffix)/('v_%03d/%03d_region_s.dat'%(ry,rx))
  if p.exists():
   records=npc_records(split_region(p.read_bytes())[2]);break
 else:records=[]
 if records:return records
 rel=rel.replace('_region_s.dat','_npc_s.dat')
 if changed and (base/'Server'/rel).exists():return npc_records((base/'Server'/rel).read_bytes())
 p=PACKAGE/'source/Server'/rel
 if p.exists():return npc_records(p.read_bytes()) if p.stat().st_size else []
 return npc_records((SERVER/rel).read_bytes()) if (SERVER/rel).exists() else []
regions=set()
for f in manifest['files']:
 m=re.search(r'npc_regions/(\d+)/v_(\d+)/(\d+)_(?:region|npc)_s.dat',f['path'])
 if m:regions.add((int(m[1]),int(m[3]),int(m[2])))
before={};after={}
for w,rx,ry in regions:
 before.setdefault(w,[]).extend(effective(w,rx,ry,False));after.setdefault(w,[]).extend(effective(w,rx,ry,True))
for w,rs in before.items():
 for r in rs:
  tid=struct.unpack_from('<i',r)[0]
  moving=(w==1067 and tid in [155,766]) or (w in [1003,1004] and tid==247) or (w==1052 and tid==206)
  assert moving or r in after[w],('Unrelated NPC lost',w,tid)
def locate(w,script):
 expected=('\\'+script.replace('/','\\')).encode()
 return [struct.unpack_from('<iii',r) for r in after[w] if r[60:].rstrip(b'\0').lower()==expected.lower()]
targets=[(1067,'script/phongthan/van_tien/doctor.lua',(185,205)),(1067,'script/phongthan/van_tien/thien_hung.lua',(186,205)),(1003,'script/phongthan/npc_recovered_batch/1003_479d2a84.lua',(213,193)),(1004,'script/phongthan/npc_recovered_batch/1004_281c1583.lua',(194,201)),(1052,'script/phongthan/npc_services/xich_tung_tu.lua',(195,195)),(1021,'script/phongthan/van_tien/ung_tiem_thuong.lua',(207,191))]
for w,script,expected in targets:
 records=locate(w,script);assert len(records)==1,(w,script,records)
 tid,x,y=records[0];assert (x//256,y//512)==expected
 rel='settings/phongthan/npc_regions/%d/v_%03d/%03d_region_s.dat'%(w,y//1024,x//512)
 assert not blocked(barrier_at(staged('Server',rel),x,y),x,y)
catalog_items=json.loads((ROOT/'analysis/event_handbook_20261026/catalog.json').read_text('utf8'))['entries']
pbtiers={e['part']:e['tier'] for e in catalog_items if e['category']=='fabao' and e['realm']==0}
for i in range(1,5):
 for kind in ['tongtian','baoxiang']:
  rel='settings/item/droprate/%s%d.ini'%(kind,i);a=configparser.RawConfigParser();b=configparser.RawConfigParser();a.optionxform=b.optionxform=str
  original=(PACKAGE/'source/Server'/rel).read_bytes();assert (SERVER/rel).read_bytes()==original
  a.read_string(original.decode('latin1'));b.read_string(staged('Server','settings/item/droprate/pt_tgpt_%s%d.ini'%(kind,i)).decode('latin1'))
  expected=[dict(a[r]) for r in a.sections() if r!='Main' and int(a[r]['RandRate'])>0]
  if kind=='tongtian':expected=[r for r in expected if not (r['Genre']=='0' and r['Detail']=='4' and pbtiers.get(int(r['Particular']),999)>[30,50,70,70][i-1])]
  actual=[dict(b[r]) for r in b.sections() if r!='Main'];assert actual==expected
  assert b['Main']['Count']==str(len(actual))
  for key in a['Main']:
   if key not in ['Count','MinItemLevel']:assert a['Main'][key]==b['Main'][key]
  assert b['Main']['MinItemLevel']==(str(5 if i==2 else 6) if kind=='tongtian' and i in [2,3] else a['Main']['MinItemLevel'])
rel='settings/phongthan/Npcs.txt';old=[r.split(b'\t') for r in (PACKAGE/'source/Server'/rel).read_bytes().splitlines()];new=[r.split(b'\t') for r in staged('Server',rel).splitlines()];assert len(old)==len(new)
changed={128+i*5:('tongtian',i) for i in range(1,5)}
changed.update({tid:('baoxiang',i) for i,tid in enumerate([410,413,414,415],1)})
for n,(a,b) in enumerate(zip(old,new)):
 if n-1 not in changed:assert a==b;continue
 kind,i=changed[n-1];cols=[old[0].index(b'DropRateFile1'),old[0].index(b'DropRateFile3')]
 assert all(x==y for k,(x,y) in enumerate(zip(a,b)) if k not in cols)
 assert all(b[col]==('\\settings\\item\\droprate\\pt_tgpt_%s%d.ini'%(kind,i)).encode() for col in cols)
rel='settings/phongthan/NativeDropQuarantine.txt';a=(PACKAGE/'source/Server'/rel).read_bytes().splitlines();b=staged('Server',rel).splitlines();assert b==[r for r in a if r!=b'N 414']
rel='settings/systemtimetask.txt';old=(PACKAGE/'source/Server'/rel).read_bytes().splitlines();new=staged('Server',rel).splitlines()
prefix='\\script\\万仙阵\\'.encode('gbk');removed={prefix+(label+suffix).encode('gbk') for label in ['土','水','火','风'] for suffix in ['on.lua','review.lua']}
kept=[r for r in old if len(r.split(b'\t'))<3 or r.split(b'\t')[2].lower() not in removed]
assert new==kept and len(old)-len(new)==48
proof=json.loads((PACKAGE/'chest-position-proof.json').read_text('utf8'));assert len(proof)==4 and all(len(r['positions'])==5 and r['all_inside_closed_gate_boss_component'] for r in proof)
out=ROOT/'analysis/vtt_tgpt_20261031/content-verification.json'
out.write_text(json.dumps(dict(status='PASS',root=str(base),files=len(manifest['files']),schedule_openings=20,pearls=manifest['pearls'],npc_targets=targets,chests_per_arena=5,chest_quantity=[1,5],original_drop_weights_preserved=True,unrelated_npcs_preserved=True),indent=2),'utf8')
print('PASS TGPT content: %d hashes, schedules/tickets, server+client item identities/flags, six exact NPC targets, unrelated NPCs preserved, five in-castle chest points per arena, original drop weights'%len(manifest['files']))

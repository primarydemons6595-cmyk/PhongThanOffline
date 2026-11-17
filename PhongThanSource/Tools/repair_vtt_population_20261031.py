"""Stage the four arena timer repair and nearby original Thien Hung NPCs."""
from pathlib import Path
import hashlib,json,re,struct
from prepare_npc_restoration import split_region,npc_records,merge_region
from build_authored_npcs import barrier_at,blocked
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
SERVER=ROOT/'PhongThanRuntime-Loose/Server'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/VttPopulation20261031'
files=[]
def stage(rel,data):
 target=SERVER/rel;snapshot=PACKAGE/'source'/rel
 if not snapshot.exists():
  snapshot.parent.mkdir(parents=True,exist_ok=True);snapshot.write_bytes(target.read_bytes() if target.exists() else b'')
 out=PACKAGE/'compiled/Server'/rel;out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(data)
 files.append(dict(path='Server/'+rel,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None))
def baseline(rel):
 p=PACKAGE/'source'/rel
 return (p if p.exists() else SERVER/rel).read_bytes()
layout=(ROOT/'PhongThanSource/Headers/PhongThanVttLayout.h').read_text('ascii')
scriptrel='script/phongthan/van_tien/thien_hung.lua'
source=(SERVER/'script/phongthan/npc_recovered_batch/1021_b5deef29.lua').read_bytes()
needle=b'if w ~= 1021 then CloseDialog(); return end'
assert source.count(needle)==1
stage(scriptrel,source.replace(needle,b'if w < 1067 or w > 1070 then CloseDialog(); return end'))
placements=[]
for ident in range(1,5):
 world=1066+ident;rel='script/phongthan/van_tien/mission%02d.lua'%ident
 data=baseline(rel);register=2999+ident*2;combat=3000+ident*2
 data=re.sub(rb'ShowMissionCountdown\(PT_ID,\d+\)',lambda m:('ShowMissionCountdown(PT_ID,%d)'%(0 if m[0].endswith(b',0)') else combat)).encode(),data)
 data=re.sub(rb'StartMissionTimer\(PT_ID,\d+,18\*seconds\)',('StartMissionTimer(PT_ID,%d,18*seconds)'%register).encode(),data)
 data=re.sub(rb'StopMissionTimer\(PT_ID,\d+\);StopMissionTimer\(PT_ID,\d+\);StopMissionTimer\(PT_ID,3009\)',('StopMissionTimer(PT_ID,%d);StopMissionTimer(PT_ID,%d);StopMissionTimer(PT_ID,3009)'%(register,combat)).encode(),data)
 assert ('StartMissionTimer(PT_ID,%d,18*seconds)'%register).encode() in data
 stage(rel,data)
 folder=SERVER/('settings/phongthan/npc_regions/%d'%world)
 doctors=[]
 for path in folder.rglob('*_region_s.dat'):
  for record in npc_records(split_region(path.read_bytes())[2]):
   if struct.unpack_from('<i',record)[0]==766:doctors.append((path,record))
 assert len(doctors)==1,(world,len(doctors))
 path,doctor=doctors[0];relative=path.relative_to(SERVER).as_posix();base=baseline(relative)
 _,dx,dy=struct.unpack_from('<iii',doctor)
 fence=set()
 for label in ['FENCE','DOOR']:
  points=re.search(r'PT_%s_%d\[\]=\{([^;]+)'%(label,ident),layout)[1]
  fence.update((int(x)//32,int(y)//32) for x,y in re.findall(r'\{(\d+),(\d+)\}',points))
 current=npc_records(split_region(base)[2]);occupied={struct.unpack_from('<iii',r)[1:] for r in current}
 for ox,oy in [(128,0),(-128,0),(0,128),(0,-128),(128,64),(-128,64)]:
  x,y=dx+ox,dy+oy
  if x//512!=dx//512 or y//1024!=dy//1024 or (x,y) in occupied:continue
  points=[(dx+(x-dx)*n//16,dy+(y-dy)*n//16) for n in range(17)]
  if all((px//32,py//32) not in fence and not blocked(barrier_at(base,px,py),px,py) for px,py in points):break
 else:raise ValueError(('No walkable nearby Thien Hung point',world))
 script=('\\'+scriptrel.replace('/','\\')).encode('ascii')
 record=struct.pack('<iiiB3x32shhhhBBH',155,x,y,0,encode_tcvn3('Thi\u00ean H\u00f9ng'),1,0,0,3,0,0,len(script))+script
 result=merge_region(base,[record]);assert len(npc_records(split_region(result)[2]))==len(current)+1
 stage(relative,result)
 placements.append(dict(world=world,template=155,x=x,y=y,display=[x//256,y//512],doctor=[dx,dy],override=relative,coordinate_provenance='authored walkable point adjacent to existing doctor; original Thien Hung template and callbacks'))
manifest=dict(scope='LOOSE_ONLY',files=files,placements=placements,registration_timers=[3001,3003,3005,3007],combat_timers=[3002,3004,3006,3008],npc_script_source='script/phongthan/npc_recovered_batch/1021_b5deef29.lua')
(PACKAGE/'manifest.json').write_text(json.dumps(manifest,indent=2),'utf8')
print(json.dumps(dict(staged=len(files),placements=placements)))

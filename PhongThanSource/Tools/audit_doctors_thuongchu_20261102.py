"""Read-only audit of effective doctor placements and original battle references."""
import json,re,struct,sys
from pathlib import Path
from prepare_npc_restoration import npc_records,split_region
from publish_original_world import parse_index
from extract_client_coordinate_dictionary import decode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
server=ROOT/'PhongThanRuntime-Loose/Server';out=ROOT/'analysis/doctors_thuongchu_20261102';out.mkdir(exist_ok=True)
index=parse_index((server/'loose_index.bin').read_bytes())
from prepare_npc_restoration import pak_id
worlds={int(a):b for a,b in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes())}
names={}
for row in (server/'settings/phongthan/NpcDisplayNames.txt').read_bytes().splitlines()[1:]:
 c=row.split(b'\t')
 if len(c)>=2:names[c[0]]=c[1]
doctors=[];battle=[];maprecords={}
def resolve(raw):
 target=index.get(pak_id(raw))
 return server/target.decode('mbcs').lstrip('\\').replace('\\','/') if target else None
for world,folder in worlds.items():
 records=[]
 # World map region payloads have per-ID overrides in loose_index.bin.
 root=server/'maps'/(folder.decode('latin1')+'_S')
 originals=list(root.rglob('*')) if '--full' in sys.argv and root.exists() else []
 override=server/'settings/phongthan/npc_regions'/str(world)
 if override.exists():originals+=list(override.rglob('*'))
 seen=set()
 for original in originals:
  if not original.is_file() or not original.name.lower().endswith(('_region_s.dat','_npc_s.dat')):continue
  raw=('\\'+str(original.relative_to(server)).replace('/','\\')).encode('latin1')
  effective=resolve(raw) or original
  if original.is_relative_to(root):
   parts=original.relative_to(root).parts
   replacement=override.joinpath(*parts)
   if replacement.exists():effective=replacement
  if effective in seen:continue
  seen.add(effective)
  if not effective.exists():continue
  try:data=effective.read_bytes();rows=npc_records(split_region(data)[2]) if original.name.lower().endswith('_region_s.dat') else npc_records(data)
  except ValueError:continue
  for r in rows:
   tid,x,y=struct.unpack_from('<iii',r);kind=struct.unpack_from('<h',r,54)[0];name=r[16:48].split(b'\0')[0];script=r[60:].rstrip(b'\0')
   row=dict(world=world,template=tid,x=x,y=y,display_x=x//256,display_y=y//512,kind=kind,name_hex=name.hex(),display_hex=names.get(name,b'').hex(),script_hex=script.hex(),effective=str(effective.relative_to(server)),record_hex=r.hex())
   records.append(row)
   visible=decode_tcvn3(names.get(name,name)).strip('#$')
   if kind==3 and (name.decode('gbk','ignore')=='医生' or visible.startswith('Đại Phu') or b'Doctor' in script or b'doctor' in script):
    bound=resolve(script);row['script_relative']=str(bound.relative_to(server)).replace('\\','/') if bound else '';doctors.append(row)
   if world==1071:battle.append(row)
 maprecords[world]=records
result=dict(doctors=doctors,battle=battle,maps_with_doctors=sorted({r['world'] for r in doctors}),loaded_map_records={str(k):len(v) for k,v in maprecords.items()},npcs=[r for rows in maprecords.values() for r in rows])
(out/('full-audit.json' if '--full' in sys.argv else 'audit.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf-8')
print('Doctors:',len(doctors),'Maps:',result['maps_with_doctors'],'Battle NPCs:',len(battle))
for r in doctors:print(r['world'],r['template'],r['display_x'],r['display_y'],r['script_relative'])
if '--original' in sys.argv:
 from restore_original_world import Original
 original=Original();found=[]
 candidates=[]
 # Decode authored source-evidence ledgers, then ask original package by path ID.
 for p in server.joinpath('script').rglob('*.lua'):
  if any(w in str(p).lower() for w in ('battle','shangzhou','zhanchang','songjin')):candidates.append(str(p.relative_to(server)).encode('latin1'))
 for scope in ('商周','商周战场','战场','战场商周','商周戰場'):
  for stem in ('main','mission','on','off','start','end','review','战场','商周','元帅','元帥','商朝将领','周朝将领','商朝接引','周朝接引','比干','武王','碑神'):
   candidates.append(('script/'+scope+'/'+stem+'.lua').encode('gbk'))
 for path in set(candidates):
  got=original.get(path)
  if got:
   data,proof=got;(out/(proof['id']+'.original.lua')).write_bytes(data);found.append(proof)
 (out/'original_battle_candidates.json').write_text(json.dumps(found,ensure_ascii=False,indent=2),'utf-8');print('Original battle candidates found:',len(found))

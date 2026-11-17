"""Restore shadowed floor-3 story bosses into the effective NPC section only."""
from pathlib import Path
import hashlib,json,struct
from prepare_npc_restoration import split_region,npc_records,merge_region
from build_authored_npcs import barrier_at,blocked

ROOT=Path(__file__).resolve().parents[2]
RUNTIME=ROOT/'PhongThanRuntime-Loose'
CONTENT=ROOT/'PhongThanSource/Deploy/ProjectContent/KhonTienBoss20261030'
def sha(b):return hashlib.sha256(b).hexdigest()
def build():
 files=[];placements=[]
 for ry,rx,dx,dy in [(102,103,206,204),(94,105,210,188)]:
  rel=f'Server/settings/phongthan/npc_regions/1049/v_{ry:03d}/{rx:03d}_region_s.dat'
  path=RUNTIME/rel;before=path.read_bytes()
  standalone=path.with_name(path.name.replace('_region_s','_npc_s'))
  additions=[r for r in npc_records(standalone.read_bytes()) if struct.unpack_from('<i',r)[0]==118]
  assert len(additions)==1
  rec=additions[0];tid,x,y=struct.unpack_from('<iii',rec)
  assert (x//256,y//512)==(dx,dy) and struct.unpack_from('<h',rec,54)[0]==0
  assert not blocked(barrier_at(before,x,y),x,y)
  oldparts=split_region(before);old=npc_records(oldparts[2]);key=lambda r:struct.unpack_from('<iii',r)
  existing=[r for r in old if key(r)==key(rec)]
  assert len(existing)<=1
  after=before if existing else merge_region(before,[rec])
  newparts=split_region(after)
  assert all(a==b for i,(a,b) in enumerate(zip(oldparts[1],newparts[1])) if i!=2)
  new=npc_records(newparts[2]);assert all(r in new for r in old)
  assert sum(key(r)==key(rec) for r in new)==1
  dest=CONTENT/'compiled'/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(after)
  files.append(dict(path=rel,sha256=sha(after),before_sha256=sha(before)))
  placements.append(dict(map=1049,template=118,display_x=dx,display_y=dy,level=75))
 result=dict(scope='LOOSE_ONLY',reason='Standalone NPC section was shadowed by a combined region file',files=files,placements=placements)
 CONTENT.mkdir(parents=True,exist_ok=True);(CONTENT/'manifest.json').write_text(json.dumps(result,indent=2))
 print('STAGED floor-3 Hoa Ma Nu:',len(files),'NPC sections; geometry and existing actors preserved')
if __name__=='__main__':build()

"""Read-only terrain verification for article coordinates and portal arrivals."""
from pathlib import Path
import re,json
from build_authored_npcs import barrier_at,blocked
from prepare_npc_restoration import pak_id
from publish_original_world import parse_index
ROOT=Path(__file__).resolve().parents[2];s=ROOT/'PhongThanRuntime-Loose/Server';index=parse_index((s/'loose_index.bin').read_bytes());worlds={int(a):b for a,b in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(s/'settings/WorldSet.ini').read_bytes())}
points={1064:[(176,195),(216,195),(192,195),(185,201),(191,207),(204,194),(207,201),(205,208),(191,201),(202,203)],1063:[(198,200)],1062:[(203,198)],1001:[(189,191),(212,205)]}
result=[]
for world,positions in points.items():
 for x,y in positions:
  px=x*256;py=y*512
  if world in (1062,1063):px+=128;py+=256
  path=b'\\maps\\'+worlds[world]+f'_S\\v_{py//1024:03d}\\{px//512:03d}_region_s.dat'.encode()
  bound=index.get(pak_id(path));p=s/(bound.decode('latin1').lstrip('\\').replace('\\','/') if bound else path.decode('latin1').lstrip('\\').replace('\\','/'))
  data=p.read_bytes() if p.exists() else None
  row=dict(world=world,x=x,y=y,region_exists=data is not None,blocked=bool(blocked(barrier_at(data,px,py),px,py)) if data is not None else None);result.append(row)
(ROOT/'analysis/thuongchu_tgpt_20261103/terrain-audit.json').write_text(json.dumps(result,indent=2),'utf8')
print(json.dumps(result,indent=2))

from pathlib import Path
import sys,re,json,struct,hashlib
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from prepare_npc_restoration import split_region,npc_records
package=ROOT/'PhongThanSource/Deploy/ProjectContent/VttPopulation20261031'
manifest=json.loads((package/'manifest.json').read_text('utf8'))
base=Path(sys.argv[1]) if len(sys.argv)>1 else package/'compiled/Server'
server=ROOT/'PhongThanRuntime-Loose/Server'
for entry in manifest['files']:
 assert hashlib.sha256((base/entry['path'][7:]).read_bytes()).hexdigest()==entry['sha256']
proof=[]
for p in manifest['placements']:
 world=p['world'];ident=world-1066
 original=(package/'source'/p['override']).read_bytes();updated=(base/p['override']).read_bytes()
 old=npc_records(split_region(original)[2]);new=npc_records(split_region(updated)[2])
 assert all(r in new for r in old) and len(new)==len(old)+1
 assert sum(struct.unpack_from('<i',r)[0]==155 for r in new)==1
 r=next(r for r in new if struct.unpack_from('<i',r)[0]==155)
 assert struct.unpack_from('<iii',r)==(155,p['x'],p['y'])
 assert r[60:]==b'\\script\\phongthan\\van_tien\\thien_hung.lua'
 oldhead,oldsections,_=split_region(original);head,sections,_=split_region(updated)
 assert oldhead==head and all(a==b for i,(a,b) in enumerate(zip(oldsections,sections)) if i!=2)
 assert updated[head:len(original)]==original[head:]
 data=(base/('script/phongthan/van_tien/mission%02d.lua'%ident)).read_bytes()
 mobs=[tuple(map(int,r)) for r in re.findall(rb'\{(\d+),(\d+),(\d+)\}',re.search(rb'PT_MOBS=([^\r\n]+)',data)[1])]
 assert len(mobs)==250
 domain=(server/('settings/phongthan/spawn_domains/%d.bin'%world)).read_bytes()
 mid,left,top,width,height=struct.unpack_from('<5i',domain,8)
 assert mid==world and domain[:8]==b'PTSPAWN1' and len(domain)==28+width*height
 for tid,x,y in mobs:
  gx,gy=x//32-left,y//32-top
  assert tid in [51+ident*2,52+ident*2] and 0<=gx<width and 0<=gy<height and domain[28+gy*width+gx]==1
 proof.append(dict(world=world,mobs=len(mobs),npc_display=p['display'],region_preserved=True,all_mobs_in_spawn_domain=True))
out=ROOT/'analysis/vtt_repair_20261031/verification.json'
out.write_text(json.dumps(dict(root=str(base),arenas=proof),indent=2),'utf8')
print('PASS 9 content hashes, 4 original NPC sections preserved, 4 unique Thien Hung, all 1000 monster points inside active spawn domains')

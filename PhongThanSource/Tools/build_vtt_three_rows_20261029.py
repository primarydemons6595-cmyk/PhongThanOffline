from pathlib import Path
import json,re,struct,sys
ROOT=Path(__file__).resolve().parents[2];B=ROOT/'analysis/vtt_weight_life_20261029';OUT=B/'staged';T=ROOT/'PhongThanRuntime-Loose'
sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from prepare_npc_restoration import split_region
layouts=json.loads((ROOT/'analysis/vtt_collision_20261024/layouts.json').read_text('utf8'))
worlds={int(k):v.decode('latin1') for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(T/'Server/settings/WorldSet.ini').read_bytes())}
def stage(side,rel,data):
 p=OUT/side/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
header='''#ifndef PT_VTT_LAYOUT_H
#define PT_VTT_LAYOUT_H
struct PT_VTT_POINT {int x,y;};
struct PT_VTT_WALL {int x,y,tid;};
struct PT_VTT_LAYOUT {int map,gateX,gateY,bossX,bossY,count;const PT_VTT_POINT* fence;int visualCount;const PT_VTT_WALL* visual;int gateCount;const PT_VTT_POINT* door;const int* groups;int entranceCount;const PT_VTT_POINT* entrances;};
'''
audit=[]
for q in layouts:
 id=q['Id'];scope=worlds[q['Map']];free=set()
 for p in (T/'Client/maps'/scope).rglob('*_region_c.dat'):
  rx=int(p.name.split('_')[0]);ry=int(p.parent.name.split('_')[1]);d=p.read_bytes();head,sections,_=split_region(d);off,size=sections[0]
  for x in range(16):
   for y in range(32):
    if not size or not struct.unpack_from('<I',d,head+off+(x*32+y)*4)[0]&255:free.add((rx*16+x,ry*32+y))
 safe={tuple(v) for v in q['SafeCells']};original={tuple(v) for v in q['Fence']}
 # Seal every walkable boundary opening; solid original terrain needs no NPC.
 seam={n for x,y in safe for n in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)] if n in free and n not in safe}
 center=original|seam
 blocked={(x+dx,y+dy) for x,y in center for dx in [-1,0,1] for dy in [-1,0,1]}
 visuals=[]
 for layer in [-1,0,1]:
  for v in q['Visuals']:visuals.append([v['X'],v['Y']+layer*32,v['Template']])
 assert len(visuals)<=64
 cells=sorted(blocked);groups=[min(range(len(visuals)),key=lambda i:(visuals[i][0]-(x*32+16))**2+(visuals[i][1]-(y*32+16))**2) for x,y in cells]
 # Walk outside the fence must only become possible by destroying segments.
 oldentry=tuple(q['Entry']);start=oldentry
 if start in blocked:
  start=min(safe&free-blocked,key=lambda p:(p[0]-oldentry[0])**2+(p[1]-oldentry[1])**2)
  q['Entry']=list(start)
 seen={start};pending=[start]
 for x,y in pending:
  for n in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)]:
   if n in free and n not in blocked and n not in seen:seen.add(n);pending.append(n)
 assert not (seen-safe),('Safe area still leaks',id,len(seen-safe))
 assert start not in blocked,('Entry blocked',id)
 header+='static const PT_VTT_POINT PT_FENCE_%d[]={%s};\n'%(id,','.join('{%d,%d}'%(x*32+16,y*32+16) for x,y in cells))
 header+='static const PT_VTT_WALL PT_WALL_%d[]={%s};\n'%(id,','.join('{%d,%d,%d}'%tuple(v) for v in visuals))
 header+='static const int PT_GROUP_%d[]={%s};\n'%(id,','.join(map(str,groups)))
 entrances=q['Gates'];doors=[tuple(p) for p in q['GateCells']];newgates=[]
 for entrance in entrances:
  own=[p for p in doors if min(range(len(entrances)),key=lambda i:(entrances[i][0]-p[0]*32)**2+(entrances[i][1]-p[1]*32)**2)==entrances.index(entrance)]
  newgates.append([sum(p[0]*32+16 for p in own)//len(own),sum(p[1]*32+16 for p in own)//len(own)])
 q['Gate']=newgates[0];q['Gates']=newgates
 q['GateCells']=sorted({(x+dx,y+dy) for x,y in doors for dx in [-1,0,1] for dy in [-1,0,1]})
 for label,points,scale in [('DOOR',q['GateCells'],32),('ENTRANCES',q['Gates'],1)]:
  header+='static const PT_VTT_POINT PT_%s_%d[]={%s};\n'%(label,id,','.join('{%d,%d}'%(x*scale+(16 if scale==32 else 0),y*scale+(16 if scale==32 else 0)) for x,y in points))
 q['NewCells']=cells;q['NewVisuals']=visuals
 rel='settings/phongthan/spawn_domains/%d.bin'%q['Map'];raw=bytearray((T/'Server'/rel).read_bytes());mid,left,top,w,h=struct.unpack_from('<5i',raw,8)
 for x,y in blocked:
  if left<=x<left+w and top<=y<top+h:raw[28+(y-top)*w+x-left]=0
 allowed={(left+x,top+y) for y in range(h) for x in range(w) if raw[28+y*w+x]};stage('Server',rel,raw)
 mission=(T/'Server/script/phongthan/van_tien'/('mission%02d.lua'%id)).read_bytes();m=re.search(rb'PT_MOBS=\{[^\r\n]+',mission)[0];mobs=[[int(v) for v in row] for row in re.findall(rb'\{(\d+),(\d+),(\d+)\}',m)];changed=0
 for row in mobs:
  cell=(row[1]//32,row[2]//32)
  if cell not in allowed:
   x,y=min(allowed,key=lambda p:(p[0]-cell[0])**2+(p[1]-cell[1])**2);row[1:]=[x*32+16,y*32+16];changed+=1
 replacement=('PT_MOBS={'+','.join('{%d,%d,%d}'%tuple(row) for row in mobs)+'}').encode()
 for name in ['mission%02d.lua'%id,'guard%d.lua'%id,'boss%d.lua'%id]:
  rel='script/phongthan/van_tien/'+name;data=(T/'Server'/rel).read_bytes();data=re.sub(rb'PT_MOBS=\{[^\r\n]+',lambda _:replacement,data)
  data=re.sub(rb'PT_ENTRY_X=\d+',('PT_ENTRY_X=%d'%start[0]).encode(),data);data=re.sub(rb'PT_ENTRY_Y=\d+',('PT_ENTRY_Y=%d'%start[1]).encode(),data);stage('Server',rel,data)
 if start!=oldentry:
  gate=['1002_f57647b5','1004_281c1583','1003_479d2a84','1001_41e3ecb2'][id-1];rel='script/phongthan/npc_recovered_batch/'+gate+'.lua';data=(T/'Server'/rel).read_bytes();data=re.sub(rb'PT_ENTRY_X=\d+',('PT_ENTRY_X=%d'%start[0]).encode(),data);data=re.sub(rb'PT_ENTRY_Y=\d+',('PT_ENTRY_Y=%d'%start[1]).encode(),data);stage('Server',rel,data)
 audit.append(dict(map=q['Map'],rows=3,segments=len(visuals),blockedCells=len(cells),seamCells=len(seam-original),safeReachable=len(seen),leaks=0,adjustedMobs=changed))
header+='static const PT_VTT_LAYOUT PT_VTT_LAYOUTS[4]={\n'
for q in layouts:
 id=q['Id'];header+='{%d,%d,%d,%d,%d,%d,PT_FENCE_%d,%d,PT_WALL_%d,%d,PT_DOOR_%d,PT_GROUP_%d,%d,PT_ENTRANCES_%d},\n'%(q['Map'],*q['Gate'],*q['Boss'],len(q['NewCells']),id,len(q['NewVisuals']),id,len(q['GateCells']),id,id,len(q['Gates']),id)
header+='};\n#endif\n'
source=ROOT/'PhongThanSource/Headers/PhongThanVttLayout.h';saved=B/'before/PhongThanVttLayout.h';saved.parent.mkdir(exist_ok=True);saved.write_bytes(source.read_bytes());source.write_text(header,'ascii')
for name in ['fence_down.spr','fence_up.spr']:
 raw=bytearray((T/'Client/spr/phongthan/vtt'/name).read_bytes());struct.pack_into('<HH',raw,8,100,108);stage('Client','spr/phongthan/vtt/'+name,raw)
# The original gate frame is a 541px wall section. Fit its native indexed
# pixels to the door opening rather than letting it cover adjacent city walls.
raw=(T/'Client/spr/phongthan/vtt/gate_closed.spr').read_bytes();colors=struct.unpack_from('<H',raw,14)[0];table=32+colors*3;off,size=struct.unpack_from('<II',raw,table);start=table+8+off
w,h,fx,fy=struct.unpack_from('<4H',raw,start);pixels=[];at=start+8
while len(pixels)<w*h:
 count,alpha=raw[at:at+2];at+=2
 if not count:continue
 if alpha:
  pixels.extend((alpha,index) for index in raw[at:at+count]);at+=count
 else:pixels.extend([(0,0)]*count)
nw,nh=w*2//3,h*2//3;body=bytearray(struct.pack('<4H',nw,nh,fx*2//3,fy*2//3))
for y in range(nh):
 row=[pixels[(y*3//2)*w+x*3//2] for x in range(nw)];x=0
 while x<nw:
  end=x+1
  while end<nw and end-x<255 and row[end][0]==row[x][0]:end+=1
  alpha=row[x][0];body.extend([end-x,alpha])
  if alpha:body.extend(value[1] for value in row[x:end])
  x=end
head=bytearray(raw[:table]);struct.pack_into('<4H',head,4,408,394,202,279)
stage('Client','spr/phongthan/vtt/gate_closed.spr',head+struct.pack('<II',0,len(body))+body)
(B/'three_rows_audit.json').write_text(json.dumps(audit,indent=2),'utf8');print(json.dumps(audit))

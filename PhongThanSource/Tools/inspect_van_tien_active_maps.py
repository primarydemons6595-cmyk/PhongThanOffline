import re,json
from restore_original_world import ROOT,OUT,Original
o=Original();server=ROOT/'PhongThanRuntime-Loose/Server'
worlds=dict((int(a),v) for a,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
for world in (1067,1068,1069,1070):
 name=worlds[world];raw=b'maps/'+name+b'.wor';got=o.get(raw)
 print('WORLD',world,'WOR',got[1] if got else None)
 for enc in ('gbk','latin1'):
  path=server/'maps'/(name.decode(enc)+'.wor')
  print(enc,'WOR_DISK',path.exists())
  if path.exists():print(path.read_bytes()[:900].decode('gbk','replace'))
  directory=server/'maps'/(name.decode(enc)+'_S')
  print('REGION_DIR',directory.exists(),'FILES',sum(1 for p in directory.rglob('*') if p.is_file()) if directory.exists() else 0)
for file in ['NpcDropPolicy.ini']:
 print('policy unchanged',file)
for world in (1067,1069,1070):
 name=worlds[world];data=o.get(b'maps/'+name+b'.wor')[0];bounds=re.search(rb'rect=(\d+),(\d+),(\d+),(\d+)',data).groups();left,top,right,bottom=map(int,bounds)
 counts={'server':0,'client':0}
 for y in range(top,bottom+1):
  for x in range(left,right+1):
   for typ in ('server','client'):
    suffix=b'_S' if typ=='server' else b'';ext=b'_region_s.dat' if typ=='server' else b'_region_c.dat'
    if o.get(b'maps/'+name+suffix+f'/v_{y:03d}/{x:03d}'.encode()+ext):counts[typ]+=1
 print('CELLS',world,counts)

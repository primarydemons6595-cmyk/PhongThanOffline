"""Read-only audit of actual VTT resources, entry coordinates and original doctor."""
import json,re,struct
from restore_original_world import ROOT,Original
from publish_original_world import parse_index
import build_offline_loose as b
o=Original(); runtime=ROOT/'PhongThanRuntime-Loose'; client=runtime/'Client'
folder=ROOT/'analysis/vtt_client_services_20261019';folder.mkdir(exist_ok=True)
index=parse_index((client/'loose_index.bin').read_bytes())
worlds=dict((int(k),v) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(runtime/'Server/settings/WorldSet.ini').read_bytes()))
def audit(raw):
 raw=b'\\'+raw.replace(b'/',b'\\').lstrip(b'\\');eid=b.pak_id(raw)
 path=client/raw.lstrip(b'\\').decode('latin1').replace('\\','/')
 got=o.get(raw)
 return dict(path=raw.decode('gbk','replace'),id=f'{eid:08X}',exists=path.exists(),indexed=eid in index,original=got[1] if got else None,equal=bool(got and path.exists() and path.read_bytes()==got[0]))
result=[]
for world,name in worlds.items():
 if world not in (1067,1068,1069,1070):continue
 wor=o.get(b'maps/'+name+b'.wor')[0]
 files=[audit(b'maps/'+name+b'24.jpg'),audit(b'settings/phongthan/minimap/'+name+b'.wor')]
 for x,y in ((103,106),(97,96),(92,102)):
  files.append(audit(b'maps/'+name+f'/v_{y:03d}/{x:03d}_region_c.dat'.encode()))
 result.append(dict(world=world,wor=wor.decode('gbk','replace'),files=files))
for raw in ('script/万仙阵/医生.lua','script/朝歌/赏金猎人.lua'):
 got=o.get(raw.encode('gbk'))
 if got:
  (folder/(got[1]['id']+'.lua')).write_bytes(got[0]); print('SCRIPT',got[1],got[0].decode('latin1')[:8000])
(folder/'audit.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
for row in result:print(json.dumps(row,ensure_ascii=False))

"""Audit original scene sprite references without changing PAK or client files."""
import collections,json,re,struct
from restore_original_world import ROOT,Original
from prepare_npc_restoration import split_region
from publish_original_world import parse_index
import build_offline_loose as b
runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server';client=runtime/'Client';o=Original();index=parse_index((client/'loose_index.bin').read_bytes())
names=dict((int(k),v) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
result=[]
for world in range(1067,1071):
 name=names[world];root=client/'maps'/name.decode('latin1');refs=collections.Counter();sections=collections.Counter();regions=0;buildin_count=0;bad_buildin=[]
 for path in root.rglob('*_region_c.dat'):
  data=path.read_bytes();head,spans,_=split_region(data);regions+=1
  for n,(off,size) in enumerate(spans):
   sections[n]+=size
   if n==5 and size:
    count,tree,line,point,above,order,lights=struct.unpack_from('<I6H',data,head+off)
    buildin_count+=count
    if size!=16+count*228+lights*28:bad_buildin.append(dict(file=str(path.relative_to(client)),size=size,count=count,lights=lights))
  for m in re.finditer(rb'(?i)\\[^\x00\r\n]{1,230}?\.spr',data):refs[m[0]]+=1
 missing=[];assets=[]
 for raw,count in refs.items():
  eid=b.pak_id(raw);bound=index.get(eid);target=client/(bound if bound else raw).lstrip(b'\\').decode('latin1').replace('\\','/')
  exists=target.exists()
  row=dict(path=raw.decode('gbk','replace'),id=f'{eid:08X}',references=count,indexed=bound is not None,exists=exists)
  assets.append(row)
  if not exists:missing.append(row)
 result.append(dict(map=world,regions=regions,section_bytes=dict(sections),native_buildin_objects=buildin_count,native_buildin_layout_mismatches=bad_buildin,unique_scene_sprites=len(refs),missing=missing,assets=assets))
 print(json.dumps({k:v for k,v in result[-1].items() if k!='assets'},ensure_ascii=False))
(ROOT/'analysis/vtt_client_services_20261019/scene_assets.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')

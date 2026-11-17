"""Stage Phong Lam's VNG/client-corroborated 191/212 display coordinate."""
import json,re,struct
from restore_original_world import ROOT,OUT,Original
from prepare_npc_restoration import split_region,npc_records,standalone_npcs,merge_region
from build_authored_npcs import place_in_tile
import build_offline_loose as b

server=ROOT/'PhongThanRuntime-Loose/Server';batch=OUT/'phong_lam_coordinate';stage=batch/'staged/Server';o=Original()
project=ROOT/'PhongThanSource';doc='Docs/NPC_RESTORATION_DEPLOYMENT.json'
deployment=json.loads((project/doc).read_text('utf8'));n=next(n for n in deployment['rows'] if n['world']==1015 and n['name']=='Phong Lam')
assert (n['x'],n['y'])==(192,212)
rel=n['override'];old=(server/rel).read_bytes();h,sections,payload=split_region(old);records=npc_records(payload)
matched=[r for r in records if r[60:]==n['lua']['logical'].encode('ascii')];assert len(matched)==1
record=matched[0];assert struct.unpack_from('<h',record,54)[0]==3
remaining=[r for r in records if r!=record]
changed=bytearray(old);newpayload=standalone_npcs(remaining);struct.pack_into('<II',changed,20,len(old)-h,len(newpayload));changed.extend(newpayload)
x,y=191,212;wx,wy=x*256+128,y*512+256;rx,ry=wx//512,wy//1024
scope=dict(re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))[b'1015']
stem=f'settings/phongthan/npc_regions/1015/v_{ry:03d}/{rx:03d}'
base=None;destination=None;geometry=None
for suffix in ('_region_s.dat','_npc_s.dat'):
    if (server/(stem+suffix)).exists():destination=stem+suffix;base=(server/destination).read_bytes();break
for suffix in (b'_S',b''):
    got=o.get(b'\\maps\\'+scope+suffix+f'\\v_{ry:03d}\\{rx:03d}_region_s.dat'.encode())
    if got:geometry=got[0];break
if base is None:
    destination=stem+('_region_s.dat' if geometry else '_npc_s.dat');base=geometry if geometry else standalone_npcs([])
if destination.endswith('_region_s.dat'):geometry=base
if geometry is None:
    got=o.get(b'\\maps\\'+scope+f'\\v_{ry:03d}\\{rx:03d}_region_c.dat'.encode());assert got;geometry=got[0]
peers=npc_records(split_region(base)[2]) if destination.endswith('_region_s.dat') else npc_records(base)
wx,wy=place_in_tile(geometry,wx,wy,[struct.unpack_from('<ii',r,4) for r in peers]);moved=bytearray(record);struct.pack_into('<ii',moved,4,wx,wy)
after=merge_region(base,[bytes(moved)]) if destination.endswith('_region_s.dat') else standalone_npcs(peers+[bytes(moved)])
changes={rel:bytes(changed),destination:after};files=[]
for relative,data in changes.items():
    p=stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    target=server/relative;files.append(dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha(target.read_bytes()) if target.exists() else None))
n.update(x=x,y=y,world_x=wx,world_y=wy,region_x=rx,region_y=ry,local_x=wx%512,local_y=wy%1024,override=destination)
n['coordinate_evidence']=dict(vng='https://phongthan.zing.vn/tin-tuc/can-biet/huong-dan-chi-tiet-nhiem-vu-van-luong.html',client='cn_dict.txt: (风林-191,212)',precision='DISPLAY_TILE')
n.pop('subcell_adjustment',None)
metadata={doc:deployment}
catalog_name='Deploy/ProjectContent/NpcRestoration/coordinates.json';catalog=json.loads((project/catalog_name).read_text('utf8'))
matches=[r for r in catalog['maps']['1015'] if r[0]=='Phong Lam'];assert len(matches)==1;matches[0][2:4]=[x,y];matches[0][4]='VNG Van Luong + original client dictionary: 191/212'
metadata[catalog_name]=catalog;metafiles=[]
for relative,data in metadata.items():
    raw=json.dumps(data,ensure_ascii=False,indent=2).encode('utf8');p=batch/'staged/Project'/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(raw)
    metafiles.append(dict(relative=relative,sha256=b.sha(raw),before_sha256=b.sha((project/relative).read_bytes())))
plan=dict(ready=[],blocked=[],files=files,metadata=metafiles,relocations=[dict(lua=n['lua']['logical'],before_record_hex=record.hex(),after_record_hex=bytes(moved).hex(),world=1015,before=[192,212],after=[191,212])],monsters_changed=0)
(batch/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8');print(json.dumps(dict(files=len(files),coordinate=[x,y],world=1015)))

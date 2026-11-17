"""Prepare (never install) reviewed Loose habitat and production-window data.

Fishing evidence is a native water-wave art object AND solid water cell with a
walkable bank in casting range. Collision alone is never classified as water.
"""
from pathlib import Path
from functools import lru_cache
import json,re,struct,hashlib,sys
from prepare_npc_restoration import split_region
from build_authored_npcs import barrier_at,blocked
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
SOURCE=ROOT/'PhongThanSource';RUNTIME=ROOT/'PhongThanRuntime-Loose'
OUT=ROOT/'analysis/studio_life_20261105/content'
def sha(b):return hashlib.sha256(b).hexdigest()
def cbytes(b):return '"'+''.join('\\x%02x'%x for x in b)+'"'
def cstr(s):return cbytes(encode_tcvn3(s))
meta=dict((int(k),v.decode('latin1')) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(RUNTIME/'Server/settings/WorldSet.ini').read_bytes()))
@lru_cache(maxsize=2048)
def region(side,mid,rx,ry):
    for n in ([meta[mid]+'_S',meta[mid]] if side=='Server' else [meta[mid]]):
        p=RUNTIME/side/'maps'/n/f'v_{ry:03d}/{rx:03d}_region_{"s" if side=="Server" else "c"}.dat'
        if p.exists():return p.read_bytes()
    return None
@lru_cache(maxsize=200000)
def free(mid,x,y):
    for side in ['Client','Server']:
        b=region(side,mid,x//512,y//1024)
        if b is None or blocked(barrier_at(b,x,y),x,y):return False
    return True
def bank(mid,x,y):
    for radius in range(96,449,32):
        for dx,dy in [(radius,0),(-radius,0),(0,radius),(0,-radius),(radius//2,radius//2),(-radius//2,radius//2),(radius//2,-radius//2),(-radius//2,-radius//2)]:
            if free(mid,x+dx,y+dy):return (x+dx,y+dy)
    return None
def water_sites(mid):
    result=[]
    for p in sorted((RUNTIME/'Client/maps'/meta[mid]).rglob('*_region_c.dat')):
        b=p.read_bytes();head,sections,_=split_region(b)
        # Island maps use explicit sea-water ground tiles without animated
        # waves. Only full sea-water tiles qualify; mixed shore tiles do not.
        off,length=sections[4];ground=b[head+off:head+off+length]
        if len(ground)>=12:
            count=struct.unpack_from('<i',ground)[0];cursor=12
            rx=int(p.name.split('_')[0]);ry=int(p.parent.name.split('_')[1])
            for tile in range(count):
                th,tv,frame,z=struct.unpack_from('<HHHH',ground,cursor);raw=ground[cursor+8:cursor+8+z].rstrip(b'\0');cursor+=8+z
                name=raw.decode('gbk',errors='replace')
                if not name.endswith('\\海水.spr'):continue
                x,y=rx*512+th*32+16,ry*1024+tv*64+32
                if any((x-r['x'])**2+(y-r['y'])**2<768**2 for r in result):continue
                cb=region('Client',mid,rx,ry);sb=region('Server',mid,rx,ry)
                if cb is None or sb is None or not blocked(barrier_at(cb,x,y),x,y) or not blocked(barrier_at(sb,x,y),x,y):continue
                shore=bank(mid,x,y)
                if shore:result.append(dict(x=x,y=y,bank=list(shore),water_art=name,source=str(p.relative_to(RUNTIME)),source_sha256=sha(b)))
        for section in [4,5]:
            off,length=sections[section];g=b[head+off:head+off+length]
            if len(g)<16:continue
            if section==4:count,start=struct.unpack_from('<ii',g,4);size,nameoff,xy=146,8,0
            else:count=struct.unpack_from('<i',g)[0];start,size,nameoff,xy=16,228,56,4
            for at in range(count):
                rec=g[start+at*size:start+(at+1)*size]
                name=rec[nameoff:nameoff+128].split(b'\0')[0].decode('gbk',errors='replace')
                if '\\地图动画\\水波\\' not in name:continue
                ox,oy=struct.unpack_from('<ii',rec,xy)
                if any((ox-r['x'])**2+(oy-r['y'])**2<768**2 for r in result):continue
                for dx,dy in [(0,0),(0,-32),(32,0),(-32,0),(0,32)]:
                    x,y=ox+dx,oy+dy
                    if x<0 or y<0:continue
                    cb=region('Client',mid,x//512,y//1024);sb=region('Server',mid,x//512,y//1024)
                    if cb is None or sb is None:continue
                    if not blocked(barrier_at(cb,x,y),x,y) or not blocked(barrier_at(sb,x,y),x,y):continue
                    shore=bank(mid,x,y)
                    if shore is None or any((x-r['x'])**2+(y-r['y'])**2<768**2 for r in result):continue
                    result.append(dict(x=x,y=y,bank=list(shore),water_art=name,source=str(p.relative_to(RUNTIME)),source_sha256=sha(b)))
                    break
    return result
def place(mid,occupied):
    wor=(RUNTIME/'Client/maps'/(meta[mid]+'.wor')).read_bytes()
    rect=list(map(int,re.search(rb'(?i)rect\s*=\s*(\d+),(\d+),(\d+),(\d+)',wor).groups()))
    cx=(rect[0]+rect[2]+1)*256;cy=(rect[1]+rect[3]+1)*512
    for radius in range(64,18000,128):
        for dx,dy in [(radius,0),(-radius,0),(0,radius),(0,-radius),(radius,radius),(-radius,-radius),(radius,-radius),(-radius,radius)]:
            x,y=cx+dx,cy+dy
            if any((x-a)**2+(y-b)**2<192**2 for a,b in occupied):continue
            if free(mid,x,y):occupied.append((x,y));return x,y
    raise ValueError(('No verified ground point',mid))
def build():
    OUT.mkdir(parents=True,exist_ok=True)
    header=SOURCE/'Headers/PhongThanLifeData.h';old=header.read_text('utf8')
    nodes=[]
    for m in re.finditer(r'\{(\d+),(\d+),(\d+),(\d+),(\d+),(\d+),(\d+),"((?:\\x[0-9a-fA-F]{2})+)"\}',old):
        n=dict(zip(['map','object','detail','skill','rank','x','y'],map(int,m.groups()[:7])));n['name']=decode_tcvn3(bytes(int(x,16) for x in re.findall(r'\\x(..)',m[8])));nodes.append(n)
    assert nodes and any(n['object']==383 for n in nodes)
    original_nodes=[dict(n) for n in nodes]
    # Exact material/map associations stated in Content_0 and Content_1 of the
    # original living-skill INI; retain higher materials not specified there.
    habitats={910:[1005,1009,1012],911:[1005,1009,1012],912:[1005,1009,1012],913:[1018],914:[1018,1065],915:[1065],931:[1019,1047],932:[1019,1047,1049,1051],933:[1049,1051,1055],934:[1055]}
    occupied={};replacement=[]
    for n in nodes:
        if n['skill']==4:continue
        for mid in habitats.get(n['detail'],[n['map']]):
            copy=dict(n)
            if mid!=n['map'] or n['detail'] in habitats:copy['x'],copy['y']=place(mid,occupied.setdefault(mid,[]))
            copy['map']=mid;replacement.append(copy)
    active=[int(x) for x in re.findall(rb'(?m)^World\d+=(\d+)',(RUNTIME/'Server/maps/WorldSet.ini').read_bytes())]
    fish={n['detail']:n for n in nodes if n['skill']==4};water={};missing=[]
    for mid in active:
        points=water_sites(mid)
        print('WATER_AUDIT',mid,len(points),flush=True)
        if not points:continue
        # Spread resources across separated, evidenced stretches of bank. Avoid
        # spawning an object on every decorative wave (native object pool).
        points=points[::max(1,len(points)//16)][:20]
        species=[944,945,946,947] if mid==1021 else [949,950,952] if mid==1018 else [955] if mid==1055 else [953,954] if mid==1052 else [944,946,948,951]
        water[mid]=points
        for at,p in enumerate(points):
            n=dict(fish[species[at%len(species)]]);n.update(map=mid,x=p['x'],y=p['y']);replacement.append(n)
    for mid in [1021,1018,1055,1052]:
        if mid not in water:missing.append(mid)
    assert not missing,('Required map has no proven fishable water',missing)
    assert all(any(n['detail']==d and n['map']==mid for n in replacement) for mid,ds in [(1021,[944,945,946,947]),(1018,[949,950,952]),(1055,[955])] for d in ds)
    node_text='static const PT_LIFE_NODE PT_LIFE_NODES[]={\n'+',\n'.join('{'+','.join(str(n[k]) for k in ['map','object','detail','skill','rank','x','y'])+','+cstr(n['name'])+'}' for n in replacement)+'};'
    new=re.sub(r'static const PT_LIFE_NODE PT_LIFE_NODES\[\]=\{.*?\};',lambda _:node_text,old,flags=re.S)
    (OUT/'PhongThanLifeData.h').write_text(new,'utf8')
    def lua(v):
        if isinstance(v,(int,float)):return str(v)
        if isinstance(v,str):return json.dumps(v,ensure_ascii=False)
        if isinstance(v,list):return '{'+','.join(lua(x) for x in v)+'}'
        return '{'+','.join('['+lua(k)+']='+lua(x) for k,x in v.items())+'}'
    # Add a recipe-ID-only entry point; all rank, book, inventory and resource
    # validation remains in the existing server transaction.
    gui='\n-- PT_LIFE_GUI_20261105\nfunction pt_life_gui_make(id)\n local r=PT_LIFE_RECIPES[id]\n if not r or not pt_life_ok() or GetLiveSkillLevel(r.skill)<r.rank or not pt_life_known(r) then return end\n pt_ls(5145,r.skill);pt_ls(5144,id);return pt_life_make(id)\nend\n'
    names={1005:'Sùng Thành',1009:'Tây Côn Lôn',1012:'Miêu Cương',1018:'Mục Dã',1019:'Tuyệt Long Lĩnh',1021:'Triều Ca',1047:'Khổn Tiên tầng 1',1049:'Khổn Tiên tầng 3',1051:'Khổn Tiên tầng 5',1052:'Diêu Trì',1055:'Đông Doanh Đảo',1057:'Thanh Đồng Sơn',1065:'Trần Đường'}
    files=[]
    for p in (RUNTIME/'Server/script').rglob('*.lua'):
        if not ('phongthan' in p.parts and ('life' in p.parts or 'npc_restore' in p.parts)):continue
        b=p.read_bytes()
        if b'PT_LIFE_LOCATIONS=' not in b:continue
        n=re.sub(rb'PT_LIFE_LOCATIONS=[^\r\n]*',lambda _:encode_tcvn3('PT_LIFE_LOCATIONS='+lua(replacement)),b)
        n=n.split(b'\n-- PT_LIFE_GUI_20261105')[0]+encode_tcvn3(gui+'\n'.join('PT_LIFE_MAP_NAMES['+str(mid)+']='+lua(name) for mid,name in names.items())+'\n')
        # All additional water maps have an unambiguous numeric label even when
        # no Vietnamese map title was recovered. Never encode Chinese as TCVN3.
        n+=encode_tcvn3('\n'.join('PT_LIFE_MAP_NAMES['+str(mid)+']=PT_LIFE_MAP_NAMES['+str(mid)+'] or "Bản đồ '+str(mid)+'"' for mid in water)+'\n')
        rel=p.relative_to(RUNTIME);target=OUT/rel;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(n)
        files.append(dict(path=str(rel),before_sha256=sha(b),sha256=sha(n)))
    rel=Path('Server/script/phongthan/life/harvest.lua');before=(RUNTIME/rel).read_bytes();payload=encode_tcvn3((SOURCE/'Deploy/ProjectContent/LifeGiaoHuan20261030/harvest.lua').read_text('utf8'));target=OUT/rel;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(payload);files.append(dict(path=str(rel),before_sha256=sha(before),sha256=sha(payload)))
    report=dict(nodes=replacement,previous_nodes=original_nodes,water=water,files=files,policy='WAVE_ART_AND_BLOCKED_WATER_CELL_AND_WALKABLE_BANK; 512 MPS casting; authored sparse sites, not original VNG rates')
    (OUT/'habitats.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
    print('PREPARED',len(replacement),'sites;',len(water),'water maps;',len(files),'scripts; no runtime writes')
if __name__=='__main__':build()

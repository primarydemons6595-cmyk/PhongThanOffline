"""Compile original nine-profession recipe data and explicit authored resource sites."""
from pathlib import Path
import csv,hashlib,json,re,struct,sys
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
from build_authored_npcs import barrier_at,blocked,place_in_tile
from prepare_npc_restoration import merge_region,npc_records,split_region,standalone_npcs
ROOT=Path(__file__).resolve().parents[2];PROJECT=ROOT/'PhongThanSource'
RUNTIME=ROOT/'PhongThanRuntime-Loose';CONTENT=PROJECT/'Deploy/ProjectContent/LifeGiaoHuan20261030'
def sha(b):return hashlib.sha256(b).hexdigest()
def lua(v):
    if isinstance(v,(int,float)):return str(v)
    if isinstance(v,str):return json.dumps(v,ensure_ascii=False)
    if isinstance(v,list):return '{'+','.join(lua(x) for x in v)+'}'
    return '{'+','.join('['+lua(k)+']='+lua(x) for k,x in v.items())+'}'
def table(path):return list(csv.reader(path.read_bytes().decode('latin1').splitlines(),delimiter='\t'))
def tuple7(text,count):
    v=[int(x) for x in text.split('.')];assert len(v) in [3,4]
    assert v[0] in [3,6,8] and (v[0]!=3 or v[2]==0)
    if len(v)==3:v.append(0)
    return v+[0,0,int(count)]
def build():
    s=RUNTIME/'Server';folder=s/'settings/liveskill'
    rp=folder/bytes.fromhex('c9fabbeebcbcc4dcbacfb3c9b9e6d4f2b1ed2e747874').decode('latin1')
    recipes={}
    for r in table(rp)[1:]:
        if not r[0]:continue
        id=int(r[0]);assert id not in recipes
        needs=[tuple7(r[i],r[i+1]) for i in range(2,14,2) if r[i]]
        out=[dict(item=tuple7(r[i],1),chance=int(r[i+1])) for i in [15,17] if r[i]]
        assert sum(x['chance'] for x in out)==100
        recipes[id]=dict(id=id,name=decode_tcvn3(r[1].lstrip('#').encode('latin1')),needs=needs,outputs=out,
            cost=int(r[14] or 0),skill=int(r[19]),rank=int(r[20]),strength=int(r[21] or 0),
            energy=int(r[22] or 0),chance=int(r[23]),bonus=int(r[24]),uses=int(r[25] or 0),default=int(r[26] or 0))
    assert len(recipes)==128 and {r['skill'] for r in recipes.values()}=={5,6,7,8,9,10}
    gp=folder/'熟练度成长表.txt'.encode('gbk').decode('latin1')
    growth={int(r[0]):[int(v or 0) for v in r[1:11]] for r in table(gp)[1:] if r[0]}
    assert all(id in growth for id in recipes)
    limits=table(folder/'jing_li_ti_li_shang_xian_dui_zhao_biao.txt')[1:]
    caps={int(r[0]):[int(r[1]),int(r[2]),int(r[9]),int(r[10])] for r in limits if r[0]}
    caprows=[caps[max(min(caps),min(max(caps),level))] for level in range(201)]
    material=table(s/'settings/item/001/material.txt')
    names={int(r[2]):decode_tcvn3(r[0].lstrip('#').strip().encode('latin1')) for r in material[1:] if len(r)>2 and r[2].isdigit()}
    meta=dict(re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(s/'settings/WorldSet.ini').read_bytes()))
    centers={int(m):[int(x),int(y)] for m,x,y in re.findall(r'\{\d+,(\d+),(\d+),(\d+),[01]\}',(PROJECT/'Headers/PhongThanTravelDestinations.h').read_text())}
    for mid,x,y in re.findall(rb'\{(\d+),(\d+),(\d+),"',(s/'script/item/ibitem/bigwildsuper.lua').read_bytes()):
        centers.setdefault(int(mid),[int(x),int(y)])
    raw_specs=[]
    for pit in range(4):raw_specs.append([1057,284,3,3,1])
    for i in range(16):
        rank=min(10,max(1,i-1));mid=[1008,1008,1008,1009,1009,1010,1010,1016,1016,1017,1017,1056,1056,1056,1056,1056][i]
        raw_specs.append([mid,355+i,910+i,1,rank])
    for i in range(10):
        raw_specs.append([1057 if i<3 else 1065 if i<7 else 1056,283+i if i<3 else 371+i-3,928+i,3,i+1])
    for i in range(6):raw_specs.append([1057 if i<3 else 1065,283,938+i,3,i+1])
    for i in range(12):raw_specs.append([1006 if i<3 else 1014 if i<6 else 1012 if i<9 else 1052,378,944+i,4,min(10,i+1)])
    nodes=[];occupied={}
    for mid,tid,detail,skill,rank in raw_specs:
        name=meta[str(mid).encode()].decode('latin1')
        if mid not in centers:
            wor=(RUNTIME/'Client/maps'/(name+'.wor')).read_bytes()
            left,top,right,bottom=map(int,re.search(rb'rect\s*=\s*(\d+),(\d+),(\d+),(\d+)',wor)[0].split(b'=')[1].split(b','))
            centers[mid]=[(left+right+1)*8,(top+bottom+1)*16]
        cx,cy=centers[mid]
        occ=occupied.setdefault(mid,[]);best=None
        # Explicit server/client collision evidence at the exact native cell.
        for radius in range(8,101,8):
            for dx,dy in [(radius,0),(0,radius),(-radius,0),(0,-radius),(radius,radius),(-radius,radius),(radius,-radius),(-radius,-radius)]:
                x,y=(cx+dx)*32+16,(cy+dy)*32+16
                if any((x-a)**2+(y-b)**2<160**2 for a,b in occ):continue
                rx,ry=x//512,y//1024
                sp=s/'maps'/(name+'_S')/f'v_{ry:03d}/{rx:03d}_region_s.dat'
                cp=RUNTIME/'Client/maps'/name/f'v_{ry:03d}/{rx:03d}_region_c.dat'
                if not sp.exists() or not cp.exists():continue
                if blocked(barrier_at(sp.read_bytes(),x,y),x,y) or blocked(barrier_at(cp.read_bytes(),x,y),x,y):continue
                best=(x,y);break
            if best:break
        assert best,(mid,detail);x,y=best;occ.append(best)
        nodes.append(dict(map=mid,object=tid,detail=detail,skill=skill,rank=rank,x=x,y=y,name='Thanh Đồng Khoáng' if detail==3 else names[detail]))
    header='#ifndef PT_LIFE_DATA_H\n#define PT_LIFE_DATA_H\n'
    header+='static const int PT_LIFE_LIMITS[201][4]={\n'+',\n'.join('{'+','.join(map(str,r))+'}' for r in caprows)+'};\n'
    header+='struct PT_LIFE_NODE {int map,object,detail,skill,rank,x,y;const char* name;};\n'
    def cstr(text):return '"'+''.join('\\x%02x'%x for x in encode_tcvn3(text))+'"'
    header+='static const PT_LIFE_NODE PT_LIFE_NODES[]={\n'+',\n'.join('{'+','.join(str(n[k]) for k in ['map','object','detail','skill','rank','x','y'])+','+cstr(n['name'])+'}' for n in nodes)+'};\n'
    header+='#define PT_LIFE_NODE_COUNT (sizeof(PT_LIFE_NODES)/sizeof(PT_LIFE_NODES[0]))\n#endif\n'
    (PROJECT/'Headers/PhongThanLifeData.h').write_text(header)
    skills={1:'Thu Thập',3:'Khai Khoáng',4:'Câu Cá',5:'Luyện Đơn',6:'Nấu Nướng',7:'Chế Luyện',8:'Chế Biến Thức Ăn',9:'Gia Công Khoáng Vật',10:'Gia Công Thuốc'}
    itemnames={}
    ib=table(s/'settings/item/001/ibitem.txt')
    ibnames={int(r[2]):decode_tcvn3(r[0].lstrip('#').strip().encode('latin1')) for r in ib[1:] if len(r)>2 and r[2].isdigit()}
    magic=table(s/'settings/item/001/magicscript.txt')
    magicnames={int(r[3]):decode_tcvn3(r[0].lstrip('#').strip().encode('latin1')) for r in magic[1:] if len(r)>3 and r[3].isdigit()}
    for r in recipes.values():
        for t in r['needs']+[x['item'] for x in r['outputs']]:
            itemnames[f'{t[0]}:{t[1]}']=(names[t[1]] if t[0]==3 else ibnames[t[1]] if t[0]==8 else magicnames[t[2]])
    mapnames={1006:'Bắc Hải',1008:'Tây Côn Lôn',1009:'Mục Dã',1010:'Trần Đường',1012:'Miêu Cương',1014:'Đồng Quan',1016:'Tam Sơn',1017:'Giới Bài Quan',1052:'Diêu Trì',1056:'Phương Trượng',1057:'Thanh Đồng Sơn',1065:'Hàn Băng Trận'}
    data='PT_LIFE_NAMES='+lua(skills)+'\nPT_LIFE_RECIPES='+lua(recipes)+'\nPT_LIFE_GROWTH='+lua(growth)+'\nPT_LIFE_LOCATIONS='+lua(nodes)+'\nPT_LIFE_ITEM_NAMES='+lua(itemnames)+'\nPT_LIFE_MAP_NAMES='+lua(mapnames)+'\n'
    runtime=(CONTENT/'life_skills.lua').read_text('utf8')
    # Recipe callbacks carry no client-authored item tuple or price.
    for id in recipes:
        runtime+=f'\nfunction pt_life_recipe{id}() return pt_life_recipe({id}) end\nfunction pt_life_make{id}() return pt_life_make({id}) end\n'
    for page in range(43):runtime+=f'\nfunction pt_life_page{page}() return pt_life_page({page}) end\n'
    runtime+='\nfunction main() return pt_life_main() end\n'
    files={'Server/script/phongthan/life/skills.lua':encode_tcvn3(data+runtime)}
    for mid,slot in [(1002,18),(1003,15),(1004,15)]:
        files[f'Server/script/phongthan/npc_restore/{mid}_{slot}.lua']=encode_tcvn3(data+runtime+'\nfunction main() return pt_life_main() end\nfunction GetPlayerTaskState() return 0,0 end\n')
    teacher_sites=[]
    npcrows=table(s/'settings/phongthan/Npcs.txt')
    for mid,tid,dx,dy in [(1020,1362,187,191),(1021,1363,217,194)]:
        path=f'script/phongthan/life/teacher_{mid}.lua'
        files['Server/'+path]=encode_tcvn3(data+runtime+'\nfunction main() return pt_life_main() end\nfunction GetPlayerTaskState() return 0,0 end\n')
        x,y=dx*256+128,dy*512+256;rx,ry=x//512,y//1024
        rel=f'Server/settings/phongthan/npc_regions/{mid}/v_{ry:03d}/{rx:03d}_region_s.dat'
        physical=meta[str(mid).encode()].decode('latin1')
        basepath=(RUNTIME/rel) if (RUNTIME/rel).exists() else (s/'maps'/(physical+'_S')/f'v_{ry:03d}/{rx:03d}_region_s.dat')
        base=basepath.read_bytes() if basepath.exists() else None
        client=RUNTIME/'Client/maps'/physical/f'v_{ry:03d}/{rx:03d}_region_c.dat'
        if base is None:
            rel=rel.replace('_region_s.dat','_npc_s.dat')
            old=npc_records((RUNTIME/rel).read_bytes()) if (RUNTIME/rel).exists() else []
        else:old=npc_records(split_region(base)[2])
        occupied=[struct.unpack_from('<ii',rec,4) for rec in old]
        x,y=place_in_tile(base if base is not None else client.read_bytes(),x,y,occupied)
        assert not blocked(barrier_at(client.read_bytes(),x,y),x,y)
        script=('\\'+path.replace('/','\\')).encode();name=npcrows[tid+1][0].encode('latin1')
        record=struct.pack('<iiiB3x32shhhhBBH',tid,x,y,0,name,1,0,0,3,6,0,len(script))+script
        assert all(struct.unpack_from('<i',rec)[0]!=tid for rec in old)
        files[rel]=merge_region(base,[record]) if base is not None else standalone_npcs(old+[record]);teacher_sites.append(dict(map=mid,template=tid,x=x,y=y,display_x=dx,display_y=dy,path=path))
    for role in ['Server','Client']:
        rel=f'{role}/settings/phongthan/NpcDisplayNames.txt';lines=(RUNTIME/rel).read_bytes().splitlines()
        for tid in [1362,1363]:
            name=npcrows[tid+1][0].encode('latin1');entry=name+b'\t'+encode_tcvn3('Sinh Hoạt Sư')
            matching=[i for i,line in enumerate(lines) if line.split(b'\t',1)[0]==name]
            if matching:lines[matching[0]]=entry
            else:lines.append(entry)
        files[rel]=b'\r\n'.join(lines)+b'\r\n'
    # The original Dieu Tri smith is the only entry point used here.
    smith='script/'+('\u7476\u6c60'.encode('gbk').decode('latin1'))+'/'+('\u94dc\u5320.lua'.encode('gbk').decode('latin1'))
    old=(s/smith).read_bytes().split(b'\n-- PT_BRONZE_GATE_20261030\n')[0]
    files['Server/'+smith]=old+b'\n-- PT_BRONZE_GATE_20261030\n'+encode_tcvn3((CONTENT/'bronze_gate.lua').read_text('utf8'))
    files['Server/script/phongthan/life/bronze_return.lua']=encode_tcvn3((CONTENT/'bronze_return.lua').read_text('utf8'))
    files['Server/settings/phongthan/LifeMiningLoot.ini']=b'; Explicit Loose policy; original extra-metal odds were not recovered.\r\n[Mining]\r\nExtraChancePer10000=1000\r\n'
    mid=1057;x,y=1608*32+16,3094*32+16;rx,ry=x//512,y//1024
    physical=meta[b'1057'].decode('latin1');rel=f'Server/settings/phongthan/npc_regions/{mid}/v_{ry:03d}/{rx:03d}_region_s.dat'
    bp=(RUNTIME/rel) if (RUNTIME/rel).exists() else s/'maps'/(physical+'_S')/f'v_{ry:03d}/{rx:03d}_region_s.dat'
    base=bp.read_bytes();x,y=place_in_tile(base,x,y,[])
    script=b'\\script\\phongthan\\life\\bronze_return.lua';label=encode_tcvn3('Điểm chuyển tiếp')
    rec=struct.pack('<iiiB3x32shhhhBBH',998,x,y,0,label,1,0,0,3,6,0,len(script))+script
    files[rel]=merge_region(base,[rec])
    files['Server/script/phongthan/life/harvest.lua']=encode_tcvn3((CONTENT/'harvest.lua').read_text('utf8'))
    manifest=[]
    for rel,b in files.items():
        p=CONTENT/'compiled'/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(b)
        manifest.append(dict(path=rel,sha256=sha(b),before_sha256=sha((RUNTIME/rel).read_bytes()) if (RUNTIME/rel).exists() else None))
    result=dict(scope='LOOSE_ONLY',files=manifest,recipes=recipes,nodes=nodes,teacher_sites=teacher_sites,
        source_tables=[dict(path=str(p.relative_to(s)),sha256=sha(p.read_bytes())) for p in [rp,gp,folder/'jing_li_ti_li_shang_xian_dui_zhao_biao.txt']],
        policy='ORIGINAL_RECIPE_ROWS; AUTHORED_TRAINING_FOR_NINE_PROFESSIONS_AND_HARVEST_SITES; not a claim of complete VNG server recovery')
    (CONTENT/'life-manifest.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf8')
    return result
if __name__=='__main__':
    sys.stdout.reconfigure(encoding='utf8');m=build();print('STAGED',len(m['recipes']),'recipes',len(m['nodes']),'harvest sites')

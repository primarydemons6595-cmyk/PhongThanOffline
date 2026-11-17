"""Stage Loose-only NPC sections; preserve geometry, original boss stats and drops."""
from pathlib import Path
import collections,csv,hashlib,json,re,struct,sys
from prepare_npc_restoration import split_region,npc_records,merge_region
from build_authored_npcs import barrier_at,blocked
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
from restore_original_world import Original

ROOT=Path(__file__).resolve().parents[2]
CONTENT=ROOT/'PhongThanSource/Deploy/ProjectContent/VienCoTruLung20261030'
RUNTIME=ROOT/'PhongThanRuntime-Loose'
def sha(b):return hashlib.sha256(b).hexdigest()
def section_records(p):
    b=p.read_bytes()
    return npc_records(split_region(b)[2]) if '_region_s.dat' in p.name else npc_records(b)
def build():
    server=RUNTIME/'Server'
    metadata=dict(re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
    lines=(server/'settings/phongthan/Npcs.txt').read_bytes().splitlines()
    templates=[dict(zip(lines[0].split(b'\t'),r.split(b'\t'))) for r in lines[1:]]
    # One static native NPC per boss; native VOID-region revival reuses the
    # same entity after 64800 frames. No periodic AddNpc job can duplicate it.
    specs=[(1064,998,1,1584,3262,'\u0110i\u1ec3m chuy\u1ec3n ti\u1ebfp'),
           (1066,126,40,1556,3160,'Ph\u1ee7 \u0110\u1ea7u \u0110\u1ea1i L\u00e3o'),
           (1066,127,40,1652,3216,'Tr\u01b0 Lung Th\u1ea7n'),
           (1066,128,40,1738,3312,'H\u1ecfa T\u00e0 Th\u1ea7n')]
    files={};placements=[]
    for mid,tid,level,tx,ty,label in specs:
        row=templates[tid];name=metadata[str(mid).encode()].decode('latin1')
        folder=server/'settings/phongthan/npc_regions'/str(mid)
        existing=[r for p in folder.rglob('*.dat') for r in section_records(p)
                  if struct.unpack_from('<i',r)[0]==tid]
        if existing:raise ValueError(f'Review existing template {tid} in {mid}: {len(existing)}')
        x,y=tx*32+16,ty*32+16
        if mid==1066:
            domain=(server/f'settings/phongthan/spawn_domains/{mid}.bin').read_bytes()
            assert domain[:8]==b'PTSPAWN1'
            m,left,top,width,height=struct.unpack_from('<5i',domain,8)
            assert m==mid and 0<=tx-left<width and 0<=ty-top<height
            if domain[28+(ty-top)*width+tx-left]!=1:
                # Select the closest explicitly authored monster-domain cell.
                points=[(left+i%width,top+i//width) for i,v in enumerate(domain[28:]) if v]
                tx,ty=min(points,key=lambda p:(p[0]-tx)**2+(p[1]-ty)**2)
                x,y=tx*32+16,ty*32+16
            assert row[b'ReviveFrame']==b'64800' and row[b'Kind']==b'0'
            assert row[b'DeathScript']==b'\\script\\npcdeath\\normal.lua'
            assert row[b'DropRateFile1']==b'\\settings\\item\\droprate\\zhulong.ini'
        rx,ry=x//512,y//1024
        relative=f'settings/phongthan/npc_regions/{mid}/v_{ry:03d}/{rx:03d}_region_s.dat'
        basepath=server/relative
        if not basepath.exists():basepath=server/'maps'/(name+'_S')/f'v_{ry:03d}/{rx:03d}_region_s.dat'
        base=files.get('Server/'+relative,basepath.read_bytes())
        assert not blocked(barrier_at(base,x,y),x,y),(mid,x,y,'blocked server cell')
        client=RUNTIME/'Client/maps'/name/f'v_{ry:03d}/{rx:03d}_region_c.dat'
        assert client.exists() and not blocked(barrier_at(client.read_bytes(),x,y),x,y)
        script=b'\\script\\phongthan\\gate\\supertrap.lua' if mid==1064 else b''
        recordname=encode_tcvn3(label) if mid==1064 else row[b'Name']
        assert len(recordname)<=31
        record=struct.pack('<iiiB3x32shhhhBBH',tid,x,y,0,recordname,level,0,0,
                           int(row[b'Kind']),int(row[b'Camp']),int(row[b'Series']),len(script))+script
        payload=merge_region(base,[record])
        assert merge_region(payload,[record])==payload or npc_records(split_region(merge_region(payload,[record]))[2])==npc_records(split_region(payload)[2])
        files['Server/'+relative]=payload
        placements.append(dict(map=mid,template=tid,level=level,name=label,x=x,y=y,
            display_x=x//256,display_y=y//512,overlay=relative,base_sha256=sha(base),
            record_hex=record.hex(),origin='AUTHORED_PLACEMENT_ON_EXISTING_WALKABLE_GEOMETRY'))
    # Translate existing native boss identities only; stats/drop columns stay untouched.
    for role in ['Server','Client']:
        rel=f'{role}/settings/phongthan/NpcDisplayNames.txt'
        raw=(RUNTIME/rel).read_bytes();existing=raw.splitlines()
        for mid,tid,level,tx,ty,label in specs[1:]:
            key=templates[tid][b'Name'];value=encode_tcvn3(label)
            matches=[i for i,line in enumerate(existing) if line.split(b'\t',1)[0]==key]
            assert len(matches)<=1
            if matches:existing[matches[0]]=key+b'\t'+value
            else:existing.append(key+b'\t'+value)
        files[rel]=b'\r\n'.join(existing)+b'\r\n'
    original=Original();drops=[]
    for filename in ['zhulong.ini','npcdroprate.ini']:
        data,proof=original.get(('\\settings\\item\\droprate\\'+filename).encode())
        current=(server/'settings/item/droprate'/filename).read_bytes()
        def ini(b):
            import configparser
            c=configparser.ConfigParser();c.read_string(b.decode('latin1'));return c
        a,b=ini(data),ini(current);changes=[]
        assert a.sections()==b.sections()
        for s in a.sections():
            for key in a[s]:
                if a[s][key]!=b[s][key]:changes.append([s,key,a[s][key],b[s][key]])
        assert len(changes)==3 and {p[1] for p in changes}=={'genre','detail','particular'}
        assert {p[2] for p in changes}=={'6','1','2020'}
        drops.append(dict(file=filename,original=proof,runtime_sha256=sha(current),
            differences=changes,action='PRESERVE_EXISTING_REVIEWED_LOOSE_TABLE; NO_DROP_EDITS'))
    portal=RUNTIME/'Client/spr/npcres/passerby/NpcS_passerby099.spr'
    assert portal.exists() and portal.read_bytes()[:3]==b'SPR'
    for rel,data in files.items():
        p=CONTENT/'compiled'/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    manifest=dict(scope='LOOSE_ONLY',placements=placements,drop_audit=drops,
        portal_script='\\script\\phongthan\\gate\\supertrap.lua',
        portal_sprite=str(portal.relative_to(RUNTIME)),portal_sprite_sha256=sha(portal.read_bytes()),
        respawn='native same-entity DoRevive/OnRevive; 64800 frames at GAME_FPS=18',
        files=[dict(path=rel,sha256=sha(data),before_sha256=sha((RUNTIME/rel).read_bytes()) if (RUNTIME/rel).exists() else None)
               for rel,data in files.items()])
    (CONTENT/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),encoding='utf8')
    return manifest
if __name__=='__main__':
    sys.stdout.reconfigure(encoding='utf8');m=build()
    print(json.dumps(dict(placements=m['placements'],drop_audit=m['drop_audit']),ensure_ascii=False,indent=2))

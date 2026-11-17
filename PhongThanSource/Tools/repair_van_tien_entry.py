"""Recover three missing active arenas and make every entry rejection visible.

Client Region_C is never relabeled as Region_S: only its verified 2048-byte
obstacle grid is copied into a new six-section server container, with no traps,
NPCs, or objects. This is authored server geometry, not recovered Region_S.
"""
import argparse,json,re,struct,shutil
from restore_original_world import ROOT,OUT,Original
from prepare_npc_restoration import split_region
from publish_original_world import parse_index
import build_offline_loose as b

parser=argparse.ArgumentParser();parser.add_argument('--apply',action='store_true');args=parser.parse_args()
folder=OUT/'van_tien_entry_fix';folder.mkdir(exist_ok=True)
if (folder/'publication.json').exists():raise ValueError('Already published; use test_van_tien_entry_maps.py to verify without replacing the journal')
original=Original();runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server'
worlds=dict((int(a),v) for a,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
changes={'Server':{},'Client':{}};evidence=[]
def add(side,raw,data,why):
    raw=b'\\'+raw.replace(b'/',b'\\').lstrip(b'\\')
    # The runtime ANSI loader stores byte-preserving Latin-1 filenames.
    relative=raw.lstrip(b'\\').decode('latin1').replace('\\','/')
    target=runtime/side/relative
    if target.exists() and target.read_bytes()==data:return
    changes[side][relative]=(raw,data,why,b.sha(target.read_bytes()) if target.exists() else None)
for side in changes:
    p=runtime/side/'maps/WorldSet.ini';data=p.read_bytes()
    ids=[int(x) for x in re.findall(rb'(?m)^World\d+=(\d+)',data)]
    assert len(ids)==len(set(ids))
    for world in (1067,1069,1070):
        if world not in ids:ids.append(world)
    # Append, do not renumber existing world slots.
    result=b'[Init]\r\nCount='+str(len(ids)).encode()+b'\r\n[World]\r\n'+b''.join(f'World{i:03d}={world}\r\n'.encode() for i,world in enumerate(ids))
    add(side,b'maps/WorldSet.ini',result,'Activate missing VTT worlds; existing 102 slots retained')
for world in (1067,1069,1070):
    name=worlds[world];wor,proof=original.get(b'maps/'+name+b'.wor')
    for side in changes:add(side,b'maps/'+name+b'.wor',wor,'Exact original WOR '+proof['id'])
    left,top,right,bottom=map(int,re.search(rb'rect=(\d+),(\d+),(\d+),(\d+)',wor).groups())
    cells=0
    for y in range(top,bottom+1):
        for x in range(left,right+1):
            stem=b'maps/'+name+f'/v_{y:03d}/{x:03d}'.encode()
            got=original.get(stem+b'_region_c.dat')
            if not got:continue
            data,p=got;head,sections,_=split_region(data)
            off,size=sections[0];assert size in (0,2048),(world,x,y,size)
            obstacle=data[head+off:head+off+size]
            # Native server format: six sections, only original obstacle bytes.
            spans=[(0,size)]+[(size,0)]*5
            geometry=struct.pack('<I',6)+b''.join(struct.pack('<II',a,z) for a,z in spans)+obstacle
            gh,gs,_=split_region(geometry);assert geometry[gh:]==obstacle and all(z==0 for a,z in gs[1:])
            add('Server',b'maps/'+name+b'_S'+f'/v_{y:03d}/{x:03d}_region_s.dat'.encode(),geometry,'Authored obstacle-only server container; exact source client obstacle '+p['id'])
            add('Client',stem+b'_region_c.dat',data,'Exact original client region '+p['id'])
            cells+=1
    assert cells>0;evidence.append(dict(world=world,client_regions=cells,server_obstacle_regions=cells,source_wor=proof,server_geometry='EXACT_CLIENT_OBSTACLES_ONLY_AUTHORED_SERVER_CONTAINER_NO_TRAPS'))
ledger_path=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
ledger=json.loads(ledger_path.read_text('utf8'))
for npc in ledger['ready']:
    if npc['world'] not in (1001,1002,1003,1004) or '万仙阵入口' not in npc['name']:continue
    relative=npc['script_relative'];data=(server/relative).read_bytes()
    data=data.replace(b'if SubWorld<0 then CloseDialog();return end',b'if SubWorld<0 then Say("Ban do Van Tien Tran chua san sang. Bao loi cho quan tri vien.",1,"Thoat/no");return end')
    # Explicitly retain each rejection instead of closing silently.
    data=data.replace(b'Say("Chua den gio mo tran.",1,"Thoat/no")',b'Say("Chua den gio mo tran. Dang ky 02:20, 10:20, 13:20, 18:20, 21:20, 23:20 (gio Viet Nam), trong 5 phut.",1,"Thoat/no")')
    add('Server',relative.encode('ascii'),data,'Visible unavailable-map and outside-registration feedback')
    npc['lua_sha256']=b.sha(data)
plan=dict(changes={side:[dict(relative=r,raw_hex=raw.hex(),sha256=b.sha(data),before_sha256=before,bytes=len(data),evidence=why) for r,(raw,data,why,before) in files.items()] for side,files in changes.items()},maps=evidence,active_map_count=105)
for side,files in changes.items():
    for relative,(raw,data,why,before) in files.items():
        p=folder/'staged'/side/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
(folder/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
if args.apply:
    # Require a cleanly stopped game; never overwrite a mapped server binary.
    import ctypes
    kernel=ctypes.windll.kernel32;kernel.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p];kernel.CreateFileW.restype=ctypes.c_void_p;kernel.CloseHandle.argtypes=[ctypes.c_void_p]
    handle=kernel.CreateFileW(str(server/'CoreServer.dll'),0x40000000,0,None,3,0,None)
    if handle in (None,ctypes.c_void_p(-1).value):raise ValueError('Stop GameServer through save-and-stop before applying')
    kernel.CloseHandle(handle)
    backup=folder/'backup_before_publication'
    if backup.exists():raise ValueError('Already published: inspect journal before repeating')
    backup.mkdir()
    for side,files in changes.items():
        base=runtime/side;indexpath=base/'loose_index.bin';index=parse_index(indexpath.read_bytes())
        manifest=runtime/'completion'/f'{side}_named_effective_manifest.csv';rows=list(b.csv_read(manifest))
        for relative in list(files)+['loose_index.bin']:
            src=base/relative
            if src.exists():p=backup/side/relative;p.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,p)
        shutil.copy2(manifest,backup/manifest.name)
        for relative,(raw,data,why,before) in files.items():
            target=base/relative;assert (b.sha(target.read_bytes()) if target.exists() else None)==before
            target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data);index[b.pak_id(raw)]=raw
            logical=raw.lstrip(b'\\').decode('latin1')
            entryid=f'{b.pak_id(raw):08X}'
            found=[r for r in rows if r['id']==entryid]
            if not found:found=[dict(path=logical,id=f'{b.pak_id(raw):08X}',status='RESOLVED',pak='',entry='',error='')];rows+=found
            for row in found:row.update(path=logical,origin='VTT_ENTRY_MAP_RECOVERY',sha256=b.sha(data),bytes=str(len(data)),evidence=why)
            if side=='Server':
                compiled=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled'/relative
                compiled.parent.mkdir(parents=True,exist_ok=True);compiled.write_bytes(data)
        indexpath.write_bytes(b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items())))
        b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
    shutil.copy2(ledger_path,backup/'recovered_client_npcs.json');ledger_path.write_text(json.dumps(ledger,ensure_ascii=False,indent=2),'utf8')
    manifest_path=ROOT/'PhongThanSource/Deploy/RUNTIME_CONTENT_MANIFEST.json';shutil.copy2(manifest_path,backup/manifest_path.name)
    content=json.loads(manifest_path.read_text('utf8'));mapping=content['DataRegistries']['Gameplay']['Map'];mapping['ExpectedActiveMapCount']=105
    mapping['ExpectedActiveMapIds']=sorted(set(mapping['ExpectedActiveMapIds'])|{1067,1069,1070})
    manifest_path.write_text(json.dumps(content,ensure_ascii=False,indent=2),'utf8')
    (folder/'publication.json').write_text(json.dumps(dict(applied=True,active_maps=105,files={s:len(v) for s,v in changes.items()}),indent=2),'utf8')
print(json.dumps(dict(applied=args.apply,files={s:len(v) for s,v in changes.items()},maps=evidence),ensure_ascii=False))

"""Publish the checked coordinate batch after a clean GameServer save-and-stop."""
import argparse,ctypes,json,shutil,struct
from restore_original_world import ROOT,OUT
from publish_original_world import parse_index
from prepare_npc_restoration import npc_records,split_region
import build_offline_loose as b

def run(apply=False,batch_name='missing_npcs'):
    batch=OUT/batch_name;stage=batch/'staged/Server';server=ROOT/'PhongThanRuntime-Loose/Server'
    plan=json.loads((batch/'plan.json').read_text('utf8'));changes=[]
    ledger=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
    previous=json.loads(ledger.read_text('utf8')) if ledger.exists() else dict(ready=[],batches=[])
    assert not any((a['world'],a['name'])==(c['world'],c['name']) for a in previous['ready'] for c in plan['ready']),'NPC already published'
    index=parse_index((server/'loose_index.bin').read_bytes())
    binaries=[]
    for item in plan.get('binaries',[]):
        assert item['relative']=='CoreServer.dll'
        target=server/item['relative'];data=(ROOT/'PhongThanSource/Output/Server'/item['relative']).read_bytes()
        assert b.sha(data)==item['sha256'] and b.sha(target.read_bytes())==item['before_sha256']
        binaries.append((item['relative'],data))
    removed_allowed={bytes.fromhex(n['before_record_hex']) for n in plan.get('relocations',[])}
    identities=plan.get('identity_changes',[])
    for change in identities:
        before=bytes.fromhex(change['before_record_hex']);after=bytes.fromhex(change['after_record_hex'])
        assert len(before)==len(after) and before[4:]==after[4:],'Identity update may only replace the template'
        assert struct.unpack_from('<h',before,54)[0]==3,'Identity update must be a service NPC'
        assert struct.unpack_from('<i',before)[0]==change['before_template']
        assert struct.unpack_from('<i',after)[0]==change['after_template']
        assert 0<=change['before_template']<2703 and 0<=change['after_template']<2703
        removed_allowed.add(before)
    metadata=[]
    for item in plan.get('metadata',[]):
        relative=item['relative'];target=ROOT/'PhongThanSource'/relative;data=(batch/'staged/Project'/relative).read_bytes()
        assert b.sha(data)==item['sha256'] and b.sha(target.read_bytes())==item['before_sha256']
        metadata.append((relative,data))
    for item in plan['files']:
        relative=item['relative'];target=server/relative;data=(stage/relative).read_bytes()
        assert b.sha(data)==item['sha256'],relative
        assert (b.sha(target.read_bytes()) if target.exists() else None)==item['before_sha256'],relative
        if relative.startswith('settings/phongthan/npc_regions/'):
            get=lambda raw:npc_records(split_region(raw)[2]) if relative.endswith('_region_s.dat') else npc_records(raw)
            before=get(target.read_bytes()) if target.exists() else []
            after=get(data)
            assert all(record in after or record in removed_allowed for record in before),'Lost existing NPC/monster: '+relative
            assert all(struct.unpack_from('<h',r,54)[0]==3 for r in after if r not in before),'Non-NPC addition'
            if target.exists() and relative.endswith('_region_s.dat'):
                old=target.read_bytes();h,sections,_=split_region(old);nh,ns,_=split_region(data)
                assert h==nh and all(a==c for i,(a,c) in enumerate(zip(sections,ns)) if i!=2)
                if any(change['relative']==relative for change in identities):
                    for i,((offset,length),(new_offset,new_length)) in enumerate(zip(sections,ns)):
                        if i!=2:assert old[h+offset:h+offset+length]==data[nh+new_offset:nh+new_offset+new_length]
                else:assert old[h:]==data[h:len(old)]
        raw=('\\'+relative.replace('/','\\')).encode('ascii');index[b.pak_id(raw)]=raw
        changes.append((relative,data))
    for move in plan.get('relocations',[]):
        before=bytes.fromhex(move['before_record_hex']);after=bytes.fromhex(move['after_record_hex'])
        assert before[:4]==after[:4] and before[12:]==after[12:] and struct.unpack_from('<h',before,54)[0]==3
        assert list(v//scale for v,scale in zip(struct.unpack_from('<ii',after,4),(256,512)))==move['after']
        observed=0
        for relative,data in changes:
            if relative.startswith('settings/phongthan/npc_regions/'):
                observed+=(npc_records(split_region(data)[2]) if relative.endswith('_region_s.dat') else npc_records(data)).count(after)
        assert observed==1,'Relocated NPC must appear exactly once'
    for change in identities:
        after=bytes.fromhex(change['after_record_hex'])
        relative,data=next((relative,data) for relative,data in changes if relative==change['relative'])
        records=npc_records(split_region(data)[2]) if relative.endswith('_region_s.dat') else npc_records(data)
        assert records.count(after)==1,'Corrected NPC identity must appear exactly once'
    result=dict(applied=apply,npcs_added=len(plan['ready']),npcs_relocated=len(plan.get('relocations',[])),npc_identities_corrected=len(identities),files=len(changes),existing_records_preserved=True,monsters_changed=0)
    if apply:
        kernel=ctypes.windll.kernel32;kernel.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p];kernel.CreateFileW.restype=ctypes.c_void_p
        handle=kernel.CreateFileW(str(server/'CoreServer.dll'),0x40000000,0,None,3,0,None)
        if handle in (None,ctypes.c_void_p(-1).value):raise ValueError('GameServer must stop and save first')
        kernel.CloseHandle.argtypes=[ctypes.c_void_p];kernel.CloseHandle(handle)
        backup=batch/'backup_before_publication'
        if backup.exists():raise ValueError('Existing publication backup: inspect journal before repeating')
        backup.mkdir()
        manifest=ROOT/'PhongThanRuntime-Loose/completion/Server_named_effective_manifest.csv';rows=list(b.csv_read(manifest))
        for relative in [r for r,_ in changes]+[r for r,_ in binaries]+['loose_index.bin']:
            target=server/relative
            if target.exists():
                saved=backup/'Server'/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
        shutil.copy2(manifest,backup/manifest.name)
        for relative,data in metadata:
            target=ROOT/'PhongThanSource'/relative;saved=backup/'Project'/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
            target.write_bytes(data)
            if target==ledger:previous=json.loads(data.decode('utf8'))
        for relative,data in binaries:(server/relative).write_bytes(data)
        for relative,data in changes:
            target=server/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
            compiled=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled'/relative
            if compiled.exists():
                saved=backup/'Compiled'/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(compiled,saved)
            compiled.parent.mkdir(parents=True,exist_ok=True);compiled.write_bytes(data)
            logical=relative.replace('/','\\');raw=('\\'+logical).encode('ascii')
            matching=[r for r in rows if r['path'].lower()==logical.lower()]
            if not matching:
                matching=[dict(path=logical,id=f'{b.pak_id(raw):08X}',status='RESOLVED',origin='ORIGINAL_CLIENT_COORDINATE_BATCH',pak='',entry='',error='')];rows.extend(matching)
            for row in matching:row.update(sha256=b.sha(data),bytes=str(len(data)),evidence='Original client hyperlink/help coordinate + map-specific original Lua; analysis/restoration_20261016/missing_npcs/plan.json')
        encoded=b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items()))
        (server/'loose_index.bin').write_bytes(encoded)
        b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
        for relative,data in changes:assert (server/relative).read_bytes()==data
        for relative,data in binaries:assert (server/relative).read_bytes()==data
        # A separate source ledger preserves these additions without relabeling
        # the earlier authored reconstruction's evidence.
        if ledger.exists():shutil.copy2(ledger,backup/'recovered_client_npcs.json')
        previous['ready']+=plan['ready'];previous.setdefault('batches',[]).append(dict(name=batch_name,npcs=len(plan['ready']),files=plan['files']))
        ledger.write_text(json.dumps(previous,ensure_ascii=False,indent=2),'utf8')
        result['index_sha256']=b.sha(encoded)
        (batch/'publication.json').write_text(json.dumps(result,indent=2),'utf8')
    print(json.dumps(result))

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--apply',action='store_true');parser.add_argument('--batch',default='missing_npcs');args=parser.parse_args();run(args.apply,args.batch)

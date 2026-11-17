"""Publish the prepared original-script repair to Loose after its server stops.

Keep original PAKs, static spawn sections and character stores unchanged.
Back up every changed file and preserve the original ID -> named-file index.
"""
import argparse, csv, json, shutil, struct, ctypes
from pathlib import Path
import build_offline_loose as b
from restore_original_world import ROOT, OUT


def parse_index(data):
    if data[:8]!=b'PTLOOSE1': raise ValueError('Not a loose index')
    count=struct.unpack_from('<I',data,8)[0];pos=12;result={}
    for _ in range(count):
        eid,length=struct.unpack_from('<IH',data,pos);pos+=6
        raw=data[pos:pos+length];pos+=length
        if eid in result: raise ValueError('Duplicate loose ID')
        result[eid]=raw
    if pos!=len(data): raise ValueError('Trailing loose index bytes')
    return result


def run(apply):
    runtime=ROOT/'PhongThanRuntime-Loose';server=runtime/'Server'
    prepared=json.loads((OUT/'prepared_changes.json').read_text('utf8'))
    recovery=json.loads((OUT/'script_recovery.json').read_text('utf8'))
    manifest=runtime/'completion/Server_named_effective_manifest.csv'
    rows=list(b.csv_read(manifest))
    original_index=(server/'loose_index.bin').read_bytes()
    index=parse_index(original_index)
    changes=[]
    proof_by_id={e['id']:e for e in recovery['recovered']}
    # Publish named exact bytes only when absent/different in the current binding.
    # Existing adapted payloads are retained unless this repair needs the proven
    # original boss-spawn or boss-death script specifically.
    required={row['id'] for row in prepared['bosses']} | {row['death']['id'] for row in prepared['bosses']}
    for proof in recovery['recovered']:
        eid=int(proof['id'],16)
        old=index.get(eid)
        if old:
            old_file=server/old.decode('mbcs').lstrip('\\').replace('\\','/')
            if old_file.exists() and b.sha(old_file.read_bytes())==proof['sha256']: continue
            if proof['id'] not in required: continue
        relative=proof['operational_path']
        source=OUT/'staged/Server'/relative
        if b.sha(source.read_bytes())!=proof['sha256']: raise ValueError('Staged original changed')
        changes.append(dict(relative=relative,data=source.read_bytes(),id=eid,proof=proof))
        raw=('\\'+relative.replace('/','\\')).encode('ascii')
        index[eid]=raw;index[b.pak_id(raw)]=raw
    for change in prepared['npc_changes']:
        relative=change['relative'];target=server/relative
        if b.sha(target.read_bytes())!=change['before_sha256']: raise ValueError('NPC changed after preparation: '+relative)
        data=(OUT/'staged/Server'/relative).read_bytes()
        if b.sha(data)!=change['sha256']: raise ValueError('Staged NPC changed')
        changes.append(dict(relative=relative,data=data,proof=change))
    for name in ('OriginalWorldSchedule.ini','OriginalWorldSchedule.txt','OriginalBossState.txt'):
        relative='settings/phongthan/'+name
        changes.append(dict(relative=relative,data=(OUT/'staged/Server'/relative).read_bytes()))
        raw=('\\'+relative.replace('/','\\')).encode('ascii');index[b.pak_id(raw)]=raw
    coordinate_plan=OUT/'coordinate_changes.json'
    coordinate_changes=json.loads(coordinate_plan.read_text('utf8')) if coordinate_plan.exists() else []
    for change in coordinate_changes:
        relative=change['relative'];target=server/relative
        observed=b.sha(target.read_bytes()) if target.exists() else None
        if observed!=change['before_sha256']: raise ValueError('Coordinate file changed: '+relative)
        data=(OUT/'staged/Server'/relative).read_bytes()
        if b.sha(data)!=change['sha256']: raise ValueError('Coordinate stage changed')
        changes.append(dict(relative=relative,data=data,proof=change))
        raw=('\\'+relative.replace('/','\\')).encode('ascii');index[b.pak_id(raw)]=raw
    encoded=b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items()))
    result=dict(applied=apply,npc_direct_original=len(prepared['npc_changes']),bosses=len(prepared['bosses']),
        boss_schedule_rows=len(prepared['boss_schedule']),files=len(changes),index_before=len(parse_index(original_index)),index_after=len(index),
        changes=[{k:v for k,v in c.items() if k!='data'}|{'sha256':b.sha(c['data'])} for c in changes],
        static_spawn_files_changed=len(coordinate_changes),schedule_enabled=prepared['schedule_enabled'],full_restoration=False)
    if apply:
        # A mapped CoreServer DLL cannot be opened exclusively for writing.
        # Verify this BEFORE modifying scripts, not halfway through publication.
        kernel=ctypes.windll.kernel32
        kernel.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p]
        kernel.CreateFileW.restype=ctypes.c_void_p
        kernel.CloseHandle.argtypes=[ctypes.c_void_p]
        handle=kernel.CreateFileW(str(server/'CoreServer.dll'),0x40000000,0,None,3,0,None)
        if handle in (None,ctypes.c_void_p(-1).value): raise ValueError('Stop the Loose GameServer through save-and-stop first')
        kernel.CloseHandle(ctypes.c_void_p(handle))
        backup=OUT/'backup_before_publication'
        if backup.exists(): raise ValueError('Publication backup already exists; review journal before repeating')
        backup.mkdir(parents=True)
        for relative in [c['relative'] for c in changes]+['loose_index.bin','CoreServer.dll']:
            target=server/relative
            if target.is_file():
                saved=backup/'Server'/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
        shutil.copy2(manifest,backup/'Server_named_effective_manifest.csv')
        for c in changes:
            target=server/c['relative'];target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(c['data'])
            logical=c['relative'].replace('/','\\');proof=c.get('proof',{})
            matching=[r for r in rows if r['path'].lower()==logical.lower()]
            if not matching:
                matching=[dict(path=logical,id=f"{c.get('id',b.pak_id(('\\'+logical).encode('ascii'))):08X}",status='RESOLVED',
                    origin='ORIGINAL_WORLD_REPAIR',pak=proof.get('pak',''),entry=str(proof.get('entry','')),error='')]
                rows.extend(matching)
            for r in matching:
                r.update(sha256=b.sha(c['data']),bytes=str(len(c['data'])),
                    evidence='20261016 original world repair; exact source or preserved transactional adaptation; see analysis/restoration_20261016')
            if c['relative'].startswith(('script/phongthan/npc_restore/','settings/phongthan/npc_regions/')):
                compiled=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled'/c['relative']
                if compiled.is_file():
                    saved=backup/'Compiled'/c['relative'];saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(compiled,saved)
                compiled.parent.mkdir(parents=True,exist_ok=True);compiled.write_bytes(c['data'])
        # Persist reviewed authoring metadata together with runtime publication.
        for relative in ('Deploy/ProjectContent/NpcRestoration/coordinates.json','Docs/NPC_RESTORATION_DEPLOYMENT.json'):
            staged_metadata=OUT/'staged/Project'/relative
            if staged_metadata.exists():
                target=ROOT/'PhongThanSource'/relative
                saved=backup/'Project'/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
                shutil.copy2(staged_metadata,target)
        b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
        (server/'loose_index.bin').write_bytes(encoded)
        shutil.copy2(ROOT/'PhongThanSource/Output/Server/CoreServer.dll',server/'CoreServer.dll')
        # Hash every index binding changed by this repair against its named file.
        for c in changes:
            assert (server/c['relative']).read_bytes()==c['data']
        result['core_sha256']=b.sha((server/'CoreServer.dll').read_bytes())
        result['index_sha256']=b.sha(encoded)
        (OUT/'publication.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
    (OUT/'publication_plan.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
    print(json.dumps({k:v for k,v in result.items() if k!='changes'},ensure_ascii=True))


if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--apply',action='store_true');run(p.parse_args().apply)

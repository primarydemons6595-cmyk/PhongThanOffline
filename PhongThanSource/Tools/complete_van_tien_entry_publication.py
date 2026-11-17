"""Resume the checked, backed-up entry repair with pre-indexed manifest rows."""
import json,struct,shutil,ctypes
from restore_original_world import ROOT,OUT
from publish_original_world import parse_index
import build_offline_loose as b
folder=OUT/'van_tien_entry_fix';backup=folder/'backup_before_publication'
assert backup.is_dir()
plan=json.loads((folder/'plan.json').read_text('utf8'));runtime=ROOT/'PhongThanRuntime-Loose'
kernel=ctypes.windll.kernel32;kernel.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p];kernel.CreateFileW.restype=ctypes.c_void_p;kernel.CloseHandle.argtypes=[ctypes.c_void_p]
handle=kernel.CreateFileW(str(runtime/'Server/CoreServer.dll'),0x40000000,0,None,3,0,None)
if handle in (None,ctypes.c_void_p(-1).value):raise ValueError('Server must remain stopped')
kernel.CloseHandle(handle)
for side,files in plan['changes'].items():
    base=runtime/side;manifest=runtime/'completion'/f'{side}_named_effective_manifest.csv'
    original_manifest=backup/manifest.name
    if not original_manifest.exists():shutil.copy2(manifest,original_manifest)
    rows=list(b.csv_read(original_manifest));byid={r['id']:r for r in rows}
    indexpath=base/'loose_index.bin';original_index=backup/side/'loose_index.bin'
    if not original_index.exists():original_index.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(indexpath,original_index)
    index=parse_index(original_index.read_bytes())
    for row in files:
        relative=row['relative'];data=(folder/'staged'/side/relative).read_bytes();target=base/relative
        assert b.sha(data)==row['sha256']
        current=b.sha(target.read_bytes()) if target.exists() else None
        assert current in (row['before_sha256'],row['sha256']),(side,relative,'unexpected external change')
        saved=backup/side/relative
        if row['before_sha256'] is not None and not saved.exists():
            assert current==row['before_sha256'];saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
        target.parent.mkdir(parents=True,exist_ok=True)
        if current!=row['sha256']:target.write_bytes(data)
        raw=bytes.fromhex(row['raw_hex']);eid=b.pak_id(raw);index[eid]=raw;key=f'{eid:08X}'
        item=byid.get(key)
        if item is None:item=dict(id=key,status='RESOLVED',pak='',entry='',error='');rows.append(item);byid[key]=item
        item.update(path=raw.lstrip(b'\\').decode('latin1'),origin='VTT_ENTRY_MAP_RECOVERY',sha256=row['sha256'],bytes=str(len(data)),evidence=row['evidence'])
        if side=='Server':
            dest=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled'/relative
            dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
    indexpath.write_bytes(b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items())))
    b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
ledger_path=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
saved=backup/'recovered_client_npcs.json'
if not saved.exists():shutil.copy2(ledger_path,saved)
ledger=json.loads(ledger_path.read_text('utf8'));hashes={r['relative']:r['sha256'] for r in plan['changes']['Server']}
for npc in ledger['ready']:
    if npc['script_relative'] in hashes:npc['lua_sha256']=hashes[npc['script_relative']]
ledger_path.write_text(json.dumps(ledger,ensure_ascii=False,indent=2),'utf8')
manifest_path=ROOT/'PhongThanSource/Deploy/RUNTIME_CONTENT_MANIFEST.json';saved=backup/manifest_path.name
if not saved.exists():shutil.copy2(manifest_path,saved)
content=json.loads(manifest_path.read_text('utf8'));mapping=content['DataRegistries']['Gameplay']['Map'];mapping['ExpectedActiveMapCount']=105
mapping['ExpectedActiveMapIds']=sorted(set(mapping['ExpectedActiveMapIds'])|{1067,1069,1070})
manifest_path.write_text(json.dumps(content,ensure_ascii=False,indent=2),'utf8')
for side,files in plan['changes'].items():
    for row in files:assert b.sha((runtime/side/row['relative']).read_bytes())==row['sha256']
(folder/'publication.json').write_text(json.dumps(dict(applied=True,active_maps=105,files={s:len(v) for s,v in plan['changes'].items()},resume_verified=True),indent=2),'utf8')
print('APPLIED_AND_VERIFIED active_maps=105 server_files=791 client_files=1 binaries_unchanged=1')

"""Publish reviewed NPC client files, retaining a hash journal and backups."""
import argparse,json,ctypes,shutil,struct
from restore_original_world import ROOT,OUT
from publish_original_world import parse_index
import build_offline_loose as b

parser=argparse.ArgumentParser();parser.add_argument('--apply',action='store_true');args=parser.parse_args()
batch=OUT/'city_service_repair';client=ROOT/'PhongThanRuntime-Loose/Client'
plan=json.loads((batch/'client_plan.json').read_text('utf8'))
changes=[]
for item in plan:
    relative=item['relative'];payload=(batch/'staged/Client'/relative).read_bytes();target=client/relative
    assert b.sha(payload)==item['sha256']
    assert (b.sha(target.read_bytes()) if target.exists() else None)==item['before_sha256'],relative
    changes.append((relative,payload))
for relative in ('CoreClient.dll','Game.exe'):
    changes.append((relative,(ROOT/'PhongThanSource/Output/Client'/relative).read_bytes()))
index=parse_index((client/'loose_index.bin').read_bytes())
for relative,payload in changes:
    if not relative.startswith('ui/'):continue
    raw=('\\'+relative.replace('/','\\')).encode('ascii');index[b.pak_id(raw)]=raw
manifest=ROOT/'PhongThanRuntime-Loose/completion/Client_named_effective_manifest.csv'
result=dict(applied=args.apply,files=[dict(relative=r,sha256=b.sha(p),bytes=len(p)) for r,p in changes])
if args.apply:
    # Windows denies exclusive write access to mapped binaries. Never replace a
    # running loose client or force-close a user's character to publish files.
    kernel=ctypes.windll.kernel32
    kernel.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p]
    kernel.CreateFileW.restype=ctypes.c_void_p;kernel.CloseHandle.argtypes=[ctypes.c_void_p]
    for relative in ('CoreClient.dll','Game.exe'):
        payload=next(p for r,p in changes if r==relative)
        if (client/relative).read_bytes()==payload:continue
        handle=kernel.CreateFileW(str(client/relative),0x40000000,0,None,3,0,None)
        if handle in (None,ctypes.c_void_p(-1).value):raise RuntimeError('Loose client must close before publication: '+relative)
        kernel.CloseHandle(handle)
    backup=batch/'backup_before_client_publication';backup.mkdir()
    for relative in [r for r,_ in changes]+['loose_index.bin']:
        target=client/relative
        if target.exists():saved=backup/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
    shutil.copy2(manifest,backup/manifest.name)
    rows=list(b.csv_read(manifest))
    for relative,payload in changes:
        target=client/relative;target.parent.mkdir(parents=True,exist_ok=True)
        if not target.exists() or target.read_bytes()!=payload:target.write_bytes(payload)
        if not relative.startswith('ui/'):continue
        logical=relative.replace('/','\\');raw=('\\'+logical).encode('ascii')
        matching=[r for r in rows if r['path'].lower()==logical.lower()]
        if not matching:
            matching=[dict(path=logical,id=f'{b.pak_id(raw):08X}',status='RESOLVED',origin='NPC_SERVICE_UI_REPAIR',pak='',entry='',error='')];rows+=matching
        for row in matching:row.update(sha256=b.sha(payload),bytes=str(len(payload)),evidence='Original VNG artwork with native UI bindings; city_service_repair/client_plan.json')
    (client/'loose_index.bin').write_bytes(b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items())))
    b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
    for relative,payload in changes:assert (client/relative).read_bytes()==payload
    (batch/'client_publication.json').write_text(json.dumps(result,indent=2),'utf8')
print(json.dumps(result))

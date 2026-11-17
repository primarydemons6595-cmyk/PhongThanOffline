"""Offline, Loose-only companion collection. Never delete a referenced snapshot."""
from pathlib import Path
import argparse,hashlib,json,os,re,shutil,struct,subprocess,time

WORKSPACE=Path(__file__).resolve().parents[2]
SERVER=WORKSPACE/'PhongThanRuntime-Loose/Server'
REPORT=WORKSPACE/'analysis/magic_retention_20261031'
HASH=re.compile(r'^[0-9a-f]{64}\.(magic-v1|required)$')

def fnv(data):
    h=2166136261
    for b in data:h=((h^b)*16777619)&0xffffffff
    return h

def state(path):
    raw=path.read_bytes()
    assert len(raw)>=424 and raw[:4]==b'PHTC', 'not a character state'
    magic,version,header_size,size,checksum=struct.unpack_from('<IHHII',raw)
    data=raw[16:]
    assert version==1 and header_size in [0,16] and size==len(data)<=65536 and fnv(data)==checksum,'file checksum/size'
    schema,state_size,revision=struct.unpack_from('<III',data)
    fight,skills,tasks,items=struct.unpack_from('<IIII',data,392)
    assert schema==1 and state_size==len(data)==408+12*(fight+skills)+20*tasks+268*items,'state schema'
    assert fight<=512 and skills<=512 and tasks<=255 and items<=512,'state limits'
    role=data[16:48].split(b'\0')[0];account=data[48:80].split(b'\0')[0]
    assert role and account and len(role)<32 and len(account)<32,'identity'
    return dict(path=str(path),key=hashlib.sha256(data).hexdigest(),revision=revision,items=items,
                account=account.decode('latin1'),role=role.decode('latin1'),role_key=hashlib.sha256(role.lower()+b'\0'+account.lower()).hexdigest(),file_sha256=hashlib.sha256(raw).hexdigest())

def account_component(account):
    data=account.encode('latin1').lower();text=data.decode('latin1')
    plain=re.fullmatch('[a-z0-9_-]+',text) and text not in ['con','prn','aux','nul','consumablebuffs'] and not re.fullmatch(r'(com|lpt)[1-9]',text)
    return text if plain else '~'+data.hex()

def pair(root,reference):
    key=reference['key'];p=root/(key+'.magic-v1');q=root/(key+'.required')
    if not p.exists() and not q.exists():return False
    assert not p.is_symlink() and not q.is_symlink(),'linked companion'
    raw=p.read_bytes();required=q.read_bytes()
    assert len(raw)>=144 and required==hashlib.sha256(raw).hexdigest().encode(),'companion checksum'
    magic,version,entry,count=struct.unpack_from('<IIII',raw)
    assert magic==0x474d3050 and version in [1,2] and entry==748 and count==reference['items'] and len(raw)==144+count*748,'companion schema'
    assert raw[16:80]==key.encode() and raw[80:144]==hashlib.sha256(raw[144:]).hexdigest().encode(),'companion state binding'
    ids=set()
    for i in range(count):
        ordinal,serial=struct.unpack_from('<II',raw,144+i*748)
        assert ordinal==i and 0<serial<0xfffffffe and serial not in ids,'serial/ordinal'
        ids.add(serial)
    return True

def stopped():
    cmd="@(Get-CimInstance Win32_Process | Where-Object {$_.Name -in 'GameServer.exe','Goddess.exe','PhongThanCharacterEditor.exe','PhongThanCharacterEditorV4.exe'} | Select-Object Name,ProcessId,ExecutablePath)|ConvertTo-Json -Compress"
    raw=subprocess.check_output(['powershell','-NoProfile','-Command',cmd],text=True).strip()
    processes=json.loads(raw) if raw else []
    if isinstance(processes,dict):processes=[processes]
    for p in processes:
        if not p.get('ExecutablePath') or str(SERVER).lower() in p['ExecutablePath'].lower() or 'charactereditor' in p['Name'].lower():
            raise RuntimeError('Stop the Loose writer/editor before compacting: '+str(p))

def references():
    result=[];warnings=[]
    for base in [SERVER/'CharacterStore',SERVER/'BotAIgent/characters',SERVER/'CharacterAdmin']:
        if not base.exists():continue
        for directory,dirs,files in os.walk(base,followlinks=False):
            dirs[:]=[d for d in dirs if d!='NativeDropPersistenceV1' and not (Path(directory)/d).is_symlink() and not (Path(directory)/d).is_junction()]
            for name in files:
                if '.pthc' not in name.lower():continue
                path=Path(directory)/name
                with path.open('rb') as f:magic=f.read(4)
                if magic!=b'PHTC':
                    if name.lower().endswith('.pthc'):raise RuntimeError('Invalid canonical character: '+str(path))
                    continue
                r=state(path);r['canonical']=name.lower().endswith('.pthc') and 'CharacterAdmin' not in path.parts
                root=SERVER/'CharacterStore'/account_component(r['account'])/'NativeDropPersistenceV1'
                roots=[root,SERVER/'NativeDropPersistenceV1'];resolved=None
                for candidate in roots:
                    if pair(candidate,r):resolved=candidate;break
                if resolved is None:
                    marker=root/('role-'+r['role_key']+'.managed-v1')
                    if not marker.exists():marker=SERVER/'NativeDropPersistenceV1'/marker.name
                    if marker.exists():
                        first=struct.unpack('<I',marker.read_bytes())[0]
                        if r['revision']>=first:
                            if r['canonical']:raise RuntimeError('Current character missing required companion: '+str(path))
                            warnings.append(dict(path=str(path),reason='historical/admin state without live companion'));continue
                    r['legacy_without_companion']=True
                else:r['root']=str(resolved)
                result.append(r)
    return result,warnings

def journal(role,refs):
    # Exact packed PtMagicRetention::Journal: 3 history slots, 8 pending slots.
    ordered=sorted({r['key']:r for r in refs if r.get('canonical') and r.get('root')}.values(),key=lambda r:r['revision'],reverse=True)[:3]
    if not ordered:return None
    raw=struct.pack('<IIIII',0x3152474d,1,max(r['revision'] for r in ordered),len(ordered),0)+role.encode()
    for i in range(11):
        raw+=(ordered[i]['key'].encode()+struct.pack('<I',ordered[i]['revision'])) if i<len(ordered) else b'\0'*68
    return raw+struct.pack('<I',fnv(raw))

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--apply',action='store_true');args=parser.parse_args()
    assert SERVER.resolve()==WORKSPACE/'PhongThanRuntime-Loose/Server' and (SERVER/'loose_only.flag').exists()
    stopped();REPORT.mkdir(parents=True,exist_ok=True)
    free_before=shutil.disk_usage(SERVER).free
    cutoff=time.time_ns()
    refs,warnings=references();assert refs,'No validated character states'
    keep={};groups={}
    for r in refs:
        if 'root' in r:keep.setdefault(r['root'],set()).add(r['key']);groups.setdefault((r['root'],r['role_key']),[]).append(r)
    roots=[SERVER/'NativeDropPersistenceV1']+[p/'NativeDropPersistenceV1' for p in (SERVER/'CharacterStore').iterdir() if p.is_dir() and not p.is_symlink() and not p.is_junction() and (p/'NativeDropPersistenceV1').is_dir()]
    backup=REPORT/('protected_backup_'+time.strftime('%Y%m%d_%H%M%S'));stats=[]
    if args.apply:
        backup.mkdir()
        # Small backup of all valid states and only the companions they reference.
        for r in refs:
            src=Path(r['path']);dst=backup/src.relative_to(SERVER);dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
        for root,keys in keep.items():
            for key in keys:
                for suffix in ['.magic-v1','.required']:
                    src=Path(root)/(key+suffix);dst=backup/src.relative_to(SERVER);dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
        for root in roots:
            for src in root.glob('role-*.managed-v1'):
                dst=backup/src.relative_to(SERVER);dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,dst)
        (backup/'references.json').write_text(json.dumps(refs,ensure_ascii=True,indent=2),'utf8')
        # Seed bounded journals before removing unreachable legacy generations.
        for (root,role),rs in groups.items():
            raw=journal(role,rs)
            if raw:
                dest=Path(root)/('role-'+role+'.retention-v1');temp=dest.with_suffix('.retention-v1.tmp.compact')
                if dest.exists():
                    dst=backup/dest.relative_to(SERVER);dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(dest,dst)
                with temp.open('wb') as f:f.write(raw);f.flush();os.fsync(f.fileno())
                os.replace(temp,dest)
        stopped()
    for root in roots:
        assert not root.is_symlink() and not root.is_junction() and root.resolve().is_relative_to(SERVER.resolve()),root
        s=dict(root=str(root.relative_to(SERVER)),bytes_before=0,files_before=0,bytes_removed=0,files_removed=0)
        with os.scandir(root) as entries:
            for e in entries:
                if not e.is_file(follow_symlinks=False):continue
                info=e.stat(follow_symlinks=False);size=info.st_size;s['bytes_before']+=size;s['files_before']+=1
                match=HASH.fullmatch(e.name)
                if not match or e.name[:64] in keep.get(str(root),set()) or info.st_mtime_ns>=cutoff:continue
                if args.apply:
                    # Literal native path only; no shell interpolation or recursive deletion.
                    Path(e.path).unlink()
                s['bytes_removed']+=size;s['files_removed']+=1
                if args.apply and s['files_removed']%10000==0:print(json.dumps(dict(progress=str(root.relative_to(SERVER)),files_removed=s['files_removed'],bytes_removed=s['bytes_removed'])),flush=True)
        s['bytes_after']=s['bytes_before']-s['bytes_removed'];s['files_after']=s['files_before']-s['files_removed'];stats.append(s)
        print(json.dumps(s),flush=True)
    # Prove every referenced .pthc and required companion is byte-identical afterwards.
    if args.apply:
        for r in refs:
            assert hashlib.sha256(Path(r['path']).read_bytes()).hexdigest()==r['file_sha256']
            if 'root' in r:assert pair(Path(r['root']),r)
    report=dict(disk_free_before=free_before,disk_free_after=shutil.disk_usage(SERVER).free,applied=args.apply,scope='LOOSE_ONLY',backup=str(backup) if args.apply else None,characters=len(refs),protected_pairs=sum(map(len,keep.values())),warnings=warnings,roots=stats,
                bytes_removed=sum(s['bytes_removed'] for s in stats),files_removed=sum(s['files_removed'] for s in stats),bytes_after=sum(s['bytes_after'] for s in stats),files_after=sum(s['files_after'] for s in stats))
    (REPORT/('compaction-result.json' if args.apply else 'compaction-preview.json')).write_text(json.dumps(report,indent=2),'utf8')
    print(json.dumps({k:v for k,v in report.items() if k not in ['roots','warnings']}),flush=True)

if __name__=='__main__':main()

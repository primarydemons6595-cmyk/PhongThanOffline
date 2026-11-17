"""PTHC v1/v2 reader, offline Loose migration and portable character export.

No item property is regenerated. Migration keeps canonical state bytes unchanged.
All replaced/deleted files are first captured in one verified ZIP outside runtime.
"""
from pathlib import Path
import argparse, configparser, hashlib, json, os, re, struct, subprocess, zipfile
from datetime import datetime, timezone

def sha(b): return hashlib.sha256(b).hexdigest()
def fnv(b):
    h=2166136261
    for v in b:h=((h^v)*16777619)&0xffffffff
    return h
def magic_valid(c,state=None):
    assert len(c)>=144,'short attributes'
    m,v,e,n=struct.unpack_from('<4I',c)
    assert m==0x474d3050 and v in (1,2) and e==748 and n<=512 and len(c)==144+748*n,'attribute bounds'
    assert re.fullmatch(b'[0-9a-f]{64}',c[16:80]) and c[80:144]==sha(c[144:]).encode(),'attribute checksum'
    if state is not None:assert n==struct.unpack_from('<I',state,404)[0] and c[16:80]==sha(state).encode(),'attribute/state binding'
    serials=set()
    for i in range(n):
        ordinal,serial=struct.unpack_from('<II',c,144+748*i)
        assert ordinal==i and 0<serial<0xfffffffe and serial not in serials,'attribute item identity'
        serials.add(serial)
    return c
def parse(raw):
    assert 424<=len(raw)<=4*1024*1024,'character file bounds'
    m,v,h,n,checksum=struct.unpack_from('<IHHII',raw)
    assert m==0x43544850 and v in (1,2) and (h==16 or (v==1 and h==0)) and 408<=n<=65536 and 16+n<=len(raw),'character header'
    state=raw[16:16+n]
    schema,size=struct.unpack_from('<II',state);a,b,t,items=struct.unpack_from('<4I',state,392)
    assert schema==1 and size==n and a<=512 and b<=512 and t<=255 and items<=512 and n==408+12*(a+b)+20*t+268*items and fnv(state)==checksum,'character state/checksum'
    assert b'\0' in state[16:48] and b'\0' in state[48:80],'character names'
    result=dict(state=state,magic=b'',pending=[],utc=0,ip=0,version=v)
    if v==1:assert len(raw)==16+n;return result
    tail=bytearray(raw[16+n:]);assert len(tail)>=32,'short item trailer'
    m,tv,sz,count,utc,ip,checksum,reserved=struct.unpack_from('<8I',tail);struct.pack_into('<I',tail,24,0)
    assert m==0x32425450 and tv==1 and reserved==0 and count<=8 and checksum==fnv(tail),'item trailer checksum'
    at=32;assert sz<=len(tail)-at
    if sz:result['magic']=magic_valid(bytes(tail[at:at+sz]),state)
    at+=sz;keys=set();revision=struct.unpack_from('<I',state,8)[0]
    for _ in range(count):
        assert len(tail)-at>=8
        rev,sz=struct.unpack_from('<II',tail,at);at+=8
        assert rev>revision and sz<=len(tail)-at
        c=magic_valid(bytes(tail[at:at+sz]));at+=sz
        assert c[16:80] not in keys;keys.add(c[16:80]);result['pending'].append((rev,c))
    assert at==len(tail),'unexpected trailing bytes'
    result.update(utc=utc,ip=ip);return result
def encode(b):
    state,c=b['state'],b['magic']
    if c:magic_valid(c,state)
    tail=bytearray(struct.pack('<8I',0x32425450,1,len(c),len(b['pending']),b['utc'],b['ip'],0,0))+c
    for rev,c in b['pending']:tail+=struct.pack('<II',rev,len(c))+c
    struct.pack_into('<I',tail,24,fnv(tail))
    raw=struct.pack('<IHHII',0x43544850,2,16,len(state),fnv(state))+state+tail
    parse(raw);return raw
def names(state):return tuple(state[a:a+32].split(b'\0',1)[0] for a in (16,48))
def component(account):
    s=account.lower().decode('latin1')
    return s if re.fullmatch('[a-z0-9_-]+',s) and s not in ('con','prn','aux','nul','consumablebuffs') and not re.fullmatch('(com|lpt)[0-9]',s) else '~'+account.lower().hex()
def legacy(server,state):
    role,account=names(state);key=sha(state)
    roots=[server/'CharacterStore'/component(account)/'NativeDropPersistenceV1',server/'NativeDropPersistenceV1']
    for root in roots:
        p=root/(key+'.magic-v1');q=root/(key+'.required')
        if p.exists() or q.exists():
            c=p.read_bytes();assert q.read_bytes()==sha(c).encode(),str(p)
            return magic_valid(c,state)
    rolekey=sha(role.lower()+b'\0'+account.lower())
    for root in roots:
        marker=root/('role-'+rolekey+'.managed-v1')
        if marker.exists():
            floor=struct.unpack('<I',marker.read_bytes())[0]
            assert struct.unpack_from('<I',state,8)[0]<floor,'missing required attributes: '+role.decode('latin1')
    return b''
def atomic(path,raw):
    temp=path.with_name(path.name+'.tmp.bundle-migration')
    assert not temp.exists(),str(temp)
    try:
        with temp.open('xb') as f:f.write(raw);f.flush();os.fsync(f.fileno())
        os.replace(temp,path)
    finally:
        if temp.exists():temp.unlink()
def stopped(server):
    cmd="Get-CimInstance Win32_Process | Where-Object {$_.Name -in @('GameServer.exe','Goddess.exe','PhongThanCharacterEditor.exe','PhongThanStudio.exe')} | Select-Object -ExpandProperty ExecutablePath | ConvertTo-Json -Compress"
    out=subprocess.check_output(['powershell','-NoProfile','-Command',cmd],text=True).strip()
    paths=json.loads(out) if out else [];paths=[paths] if isinstance(paths,str) else paths
    runtime=server.parent.resolve()
    assert not any(p and Path(p).is_relative_to(runtime) for p in paths),'Stop Loose world/database/editor before migration'
def migrate(runtime,backup,apply=False):
    runtime=runtime.resolve();assert runtime.name=='PhongThanRuntime-Loose','Loose only'
    server=runtime/'Server';backup=backup.resolve();assert not backup.is_relative_to(runtime),'backup must be outside runtime'
    if apply:stopped(server)
    paths=[]
    for folder in ('CharacterStore','BotAIgent/characters','CharacterAdmin'):
        for p in (server/folder).rglob('*'):
            if p.is_file() and '.pthc' in p.name.lower() and not p.name.endswith('.login.ini'):
                if p.open('rb').read(4)==b'PHTC':paths.append(p)
    converted={};original={};roles=[];archived={}
    for p in sorted(paths):
        raw=p.read_bytes();b=parse(raw)
        if b['version']==1:b['magic']=legacy(server,b['state'])
        login=Path(str(p)+'.login.ini')
        if login.exists():
            ini=configparser.ConfigParser();ini.read(login,encoding='utf-8-sig')
            b['utc']=ini.getint('Login','Utc',fallback=0);b['ip']=ini.getint('Login','IP',fallback=0)
        # Offline migration discards unacknowledged captures, keeping canonical only.
        b['pending']=[];next_raw=encode(b);assert parse(next_raw)['state']==b['state'] and parse(next_raw)['magic']==b['magic']
        live=p.suffix.lower()=='.pthc' and 'CharacterAdmin' not in p.relative_to(server).parts
        if live:
            if next_raw!=raw:converted[p]=next_raw;original[p]=raw
        else:
            if p.name.endswith('.before-starter-v1'):
                assert Path(str(p).removesuffix('.before-starter-v1')+'.starter-live-v1.done').exists(),'unfinished starter migration marker'
            archived[p]=next_raw;original[p]=raw
        roles.append(dict(path=str(p.relative_to(server)),items=struct.unpack_from('<I',b['state'],404)[0],bytes=len(next_raw),exact_attributes=bool(b['magic'])))
    assert roles,'No character files found'
    cleanup=list(archived)
    queue=server/'CharacterAdmin'
    for done in queue.glob('*.done'):
        job_id=done.stem
        assert re.fullmatch('[0-9a-f]{32}',job_id)
        if not done.with_suffix('.result').exists() or done.with_suffix('.ready').exists():continue
        cleanup.extend([done,done.with_suffix('.result')])
        job=queue/job_id
        if job.exists():cleanup.extend(p for p in job.rglob('*') if p.is_file())
    for root in [server/'NativeDropPersistenceV1',*(server/'CharacterStore').glob('*/NativeDropPersistenceV1')]:
        if not root.exists():continue
        for p in root.iterdir():
            if p.is_file() and (re.fullmatch(r'[0-9a-f]{64}\.(magic-v1|required)',p.name) or re.fullmatch(r'role-[0-9a-f]{64}\.(managed-v1|retention-v1)',p.name)):
                cleanup.append(p)
    cleanup+= [Path(str(p)+'.login.ini') for p in paths if Path(str(p)+'.login.ini').exists()]
    cleanup=list(dict.fromkeys(cleanup))
    result=dict(characters=len(roles)-len(archived),archived_character_backups=len(archived),converted=len(converted),remove_files=len(cleanup),character_bytes=sum(len(raw) for raw in converted.values()),roles=roles,applied=apply)
    if not apply:return result
    backup.parent.mkdir(parents=True,exist_ok=True)
    for p in cleanup:original.setdefault(p,p.read_bytes())
    if not backup.exists():
        with zipfile.ZipFile(backup,'x',zipfile.ZIP_DEFLATED) as z:
            for p,raw in original.items():z.writestr(p.relative_to(server).as_posix(),raw)
            for p,raw in archived.items():z.writestr('unified-backups/'+p.relative_to(server).as_posix(),raw)
            z.writestr('migration-manifest.json',json.dumps(result,indent=2))
    with zipfile.ZipFile(backup) as z:
        assert z.testzip() is None
        for p,raw in original.items():assert z.read(p.relative_to(server).as_posix())==raw
        for p,raw in archived.items():assert z.read('unified-backups/'+p.relative_to(server).as_posix())==raw
    stopped(server)
    for p,raw in converted.items():assert p.read_bytes()==original[p],str(p)
    for p,raw in converted.items():atomic(p,raw)
    for p in paths:
        if p in archived:continue
        b=parse(p.read_bytes());assert b['version']==2 and not b['pending']
    for p in cleanup:assert p.read_bytes()==original[p],str(p)
    for p in cleanup:p.unlink()
    for p in sorted(queue.rglob('*'),key=lambda p:len(p.parts),reverse=True):
        if p.is_dir() and not any(p.iterdir()):p.rmdir()
    for root in [server/'NativeDropPersistenceV1',*(server/'CharacterStore').glob('*/NativeDropPersistenceV1')]:
        if root.exists() and not any(root.iterdir()):root.rmdir()
    result['backup']=str(backup);return result
def export(source,destination,account=None,role=None):
    b=parse(source.read_bytes());assert b['version']==2,'Migrate this character first';s=bytearray(b['state'])
    for offset,value in ((16,role),(48,account)):
        if value is None:continue
        data=value.encode('latin1');assert 0<len(data)<32 and b'\0' not in data
        if offset==48:component(data)
        s[offset:offset+32]=data+b'\0'*(32-len(data))
    b['state']=bytes(s);b['pending']=[]
    if b['magic']:
        c=bytearray(b['magic']);c[16:80]=sha(b['state']).encode();b['magic']=bytes(c)
    raw=encode(b);assert not destination.exists(),'Never overwrite a destination character';destination.parent.mkdir(parents=True,exist_ok=True);atomic(destination,raw)
    return dict(destination=str(destination),bytes=len(raw),items=struct.unpack_from('<I',s,404)[0],exact_attributes=bool(b['magic']))
def main():
    p=argparse.ArgumentParser();sub=p.add_subparsers(dest='command',required=True)
    m=sub.add_parser('migrate');m.add_argument('--runtime',type=Path,required=True);m.add_argument('--backup',type=Path,required=True);m.add_argument('--apply',action='store_true');m.add_argument('--report',type=Path)
    e=sub.add_parser('export');e.add_argument('source',type=Path);e.add_argument('destination',type=Path);e.add_argument('--account');e.add_argument('--role')
    a=p.parse_args();result=migrate(a.runtime,a.backup,a.apply) if a.command=='migrate' else export(a.source,a.destination,a.account,a.role)
    if getattr(a,'report',None):a.report.write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({k:v for k,v in result.items() if k!='roles'},ensure_ascii=False))
if __name__=='__main__':main()

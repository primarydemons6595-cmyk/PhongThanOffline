"""Client-only visual overlay with hash-checked backups and rollback."""
from pathlib import Path
import json,struct,shutil,sys,subprocess
import build_offline_loose as b
root=Path(__file__).resolve().parents[2];client=root/'PhongThanRuntime-Loose/Client'
out=root/'analysis/spr_visual_compat_20261018';receipt=out/'npc_icon_applied.json';backup=out/'extra_backup'
index=client/'loose_index.bin'
def replace(path,data):
    path.parent.mkdir(parents=True,exist_ok=True);tmp=path.with_suffix('.visual.tmp');tmp.write_bytes(data);tmp.replace(path)
if '--rollback' in sys.argv:
    processes=subprocess.check_output(['powershell','-NoProfile','-Command','Get-Process Game -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Path'],text=True,errors='replace')
    if str(client/'Game.exe').lower() in processes.lower():raise SystemExit('Save/logout and close Main client first')
    r=json.loads(receipt.read_text());assert b.sha(index.read_bytes())==r['index_after_sha256']
    for f in r['files']:assert b.sha((client/f['path']).read_bytes())==f['after_sha256'],f['path']
    for f in r['files']:
        p=client/f['path']
        if f['before_sha256']:replace(p,(backup/f['path']).read_bytes())
        else:p.unlink()
    replace(index,(backup/'loose_index.bin').read_bytes());print('NPC/icon overlay rolled back; no player data touched');sys.exit()
if receipt.exists():raise SystemExit('Already applied')
plan=json.loads((out/'npc_icon_plan.json').read_text());files={};normal=plan['normal_table'];np=client/normal['path']
assert b.sha(np.read_bytes())==normal['before_sha256']
files[normal['path']]=(out/'extra_prepared'/normal['path']).read_bytes()
assert b.sha(files[normal['path']])==normal['after_sha256']
# Match normal-NPC frame metadata to the selected asset, preserving other cells.
kind=client/'settings/npcres'/bytes.fromhex('c8cbceefc0e0d0cd2e747874').decode('latin1')
kinds={r.split(b'\t')[0]:r.split(b'\t') for r in kind.read_bytes().splitlines()[1:]}
meta=np.with_name(bytes.fromhex('c6d5cda86e7063d7cad4b4d0c5cfa22e747874').decode('latin1'))
rows=[r.split(b'\t') for r in meta.read_bytes().splitlines()];byname={r[0]:r for r in rows[1:]}
normalrows={r.split(b'\t')[0]:r.split(b'\t') for r in files[normal['path']].splitlines()[1:]}
for change in plan['npc_changes']:
    name=change['npc_resource'].encode('latin1');col=change['column']-1
    data=(client/(kinds[name][2]+b'\\'+normalrows[name][col]).decode('latin1').lstrip('\\').replace('\\','/')).read_bytes()
    h=struct.unpack_from('<8H',data,4);assert h[4]>0 and h[6]>0
    row=byname[name]
    while len(row)<=col:row.append(b'')
    row[col]=('%d,%d,%d'%(h[4],h[6],h[7])).encode('ascii')
files[str(meta.relative_to(client))]=b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n'
blob=index.read_bytes();assert blob[:8]==b'PTLOOSE1';pos=12;entries={}
for _ in range(struct.unpack_from('<I',blob,8)[0]):
    eid,n=struct.unpack_from('<IH',blob,pos);pos+=6;entries[eid]=blob[pos:pos+n];pos+=n
for alias in plan['icon_aliases']:
    raw=bytes.fromhex(alias['path_hex']);eid=int(alias['id'],16)
    assert eid==b.pak_id(raw)
    if eid in entries:assert entries[eid].lower()==raw.lower(),'ID collision'
    dest=client/alias['path'];assert not dest.exists()
    source=client/alias['source_path'].lstrip('\\');data=source.read_bytes()
    assert b.sha(data)==alias['source_sha256'] and (b.sprite_blob_valid(data,True) or b.sprite_blob_valid(data,False))
    files[alias['path']]=data;entries[eid]=raw
for alias in plan.get('index_aliases',[]):
    requested=bytes.fromhex(alias['request_hex']);eid=b.pak_id(requested)
    source=alias['source_path'].encode('latin1')
    assert (client/alias['source_path'].lstrip('\\')).exists()
    assert eid not in entries,'Existing ID must not be overwritten'
    entries[eid]=source
payload=b'PTLOOSE1'+struct.pack('<I',len(entries))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(entries.items()))
backup.mkdir(exist_ok=True);shutil.copy2(index,backup/'loose_index.bin');records=[]
for rel,data in files.items():
    p=(client/rel).resolve();assert p.is_relative_to(client.resolve())
    before=b.sha(p.read_bytes()) if p.exists() else None
    if p.exists():save=backup/rel;save.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,save)
    records.append(dict(path=rel,before_sha256=before,after_sha256=b.sha(data)))
for rel,data in files.items():replace(client/rel,data)
replace(index,payload)
r=dict(files=records,index_before_sha256=b.sha(blob),index_after_sha256=b.sha(payload),summary=plan['summary'],client_restart_required=True,server_modified=False,player_data_modified=False)
receipt.write_text(json.dumps(r,ensure_ascii=True,indent=2));print(json.dumps(r['summary']))

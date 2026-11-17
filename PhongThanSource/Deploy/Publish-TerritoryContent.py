"""Publish reviewed territory content to a stopped main Loose world/client.

No nation/role data edits. Existing edits conflict instead of being overwritten.
"""
from pathlib import Path
import json,sys,hashlib,struct,csv,io,shutil,datetime,subprocess
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from build_offline_loose import pak_id
def sha(data):return hashlib.sha256(data).hexdigest()
def publish(role,backup,package_name='Territory20261106'):
 if role not in ['Client','Server']:raise ValueError('Unknown role')
 if package_name not in ['Territory20261106','DaoStarter20261106','PlayerServices20261106','AvatarInfo20261106','OfflineNative20261106']:raise ValueError('Package is not reviewed for this publisher')
 root=ROOT/'PhongThanRuntime-Loose'/role;package=Path(__file__).resolve().parent/'ProjectContent'/package_name;manifest=json.loads((package/'manifest.json').read_text('utf8'));plans=[];indexed=[]
 processes=json.loads(subprocess.check_output(['powershell','-NoProfile','-Command',"@(Get-CimInstance Win32_Process | Where-Object {$_.Name -in @('Game.exe','GameServer.exe')} | Select-Object Name,ExecutablePath) | ConvertTo-Json -Compress"],text=True) or '[]')
 if isinstance(processes,dict):processes=[processes]
 consumer='Game.exe' if role=='Client' else 'GameServer.exe'
 if any(p['Name'].lower()==consumer.lower() and str(p['ExecutablePath']).lower().startswith(str(root).lower()+'\\') for p in processes):raise RuntimeError('Close the target '+consumer+' before publishing')
 for entry in manifest['files']:
  if not entry['path'].startswith(role+'/'):continue
  rel=entry['path'][len(role)+1:];target=root/rel
  if not target.resolve().is_relative_to(root.resolve()) or '..' in Path(rel).parts:raise ValueError('Path escaped role root')
  data=(package/'compiled'/entry['path']).read_bytes()
  if sha(data)!=entry['sha256']:raise ValueError('Content source hash mismatch: '+rel)
  indexed.append(rel)
  current=target.read_bytes() if target.exists() else None
  if current is not None and sha(current)==sha(data):continue
  if (sha(current) if current is not None else None)!=entry['before_sha256']:raise ValueError('Existing edit preserved; content conflict: '+rel)
  plans.append((rel,data,entry))
 index=root/'loose_index.bin';raw=index.read_bytes();assert raw[:8]==b'PTLOOSE1';count,=struct.unpack_from('<I',raw,8);at=12;bindings={}
 for _ in range(count):
  key,length=struct.unpack_from('<IH',raw,at);at+=6;assert key not in bindings and at+length<=len(raw);bindings[key]=raw[at:at+length];at+=length
 assert at==len(raw)
 for rel in indexed:
  name=b'\\'+rel.replace('/','\\').encode('latin1');key=pak_id(name)
  if key in bindings and bindings[key].lower()!=name.lower():raise ValueError('Loose path hash collision')
  bindings[key]=name
 next_index=b'PTLOOSE1'+struct.pack('<I',len(bindings))+b''.join(struct.pack('<IH',key,len(value))+value for key,value in sorted(bindings.items()))
 manifest_path=ROOT/'PhongThanRuntime-Loose/completion'/f'{role}_named_effective_manifest.csv';old_manifest=manifest_path.read_bytes();reader=csv.DictReader(io.StringIO(old_manifest.decode('utf-8-sig')));fields=reader.fieldnames;rows=list(reader);lookup={row['path'].lstrip('\\').replace('/','\\').lower():row for row in rows}
 for rel,data,entry in plans:
  name=rel.replace('/','\\');row=lookup.get(name.lower())
  if row is None:row={key:'' for key in fields};rows.append(row);row['path']=name
  row.update(sha256=entry['sha256'],bytes=str(len(data)),status='RESOLVED',origin=package_name,id=f'{pak_id(bytes([92])+name.encode("latin1")):08X}')
 text=io.StringIO(newline='');writer=csv.DictWriter(text,fieldnames=fields,lineterminator='\r\n');writer.writeheader();writer.writerows(rows);next_manifest=text.getvalue().encode('utf-8-sig')
 writes=[(root/rel,data) for rel,data,_ in plans]+[(index,next_index),(manifest_path,next_manifest)];saved=[]
 try:
  for target,data in writes:
   rel=target.relative_to(ROOT/'PhongThanRuntime-Loose');prior=target.read_bytes() if target.exists() else None;saved.append((target,prior))
   if prior is not None:p=backup/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(prior)
   target.parent.mkdir(parents=True,exist_ok=True);temp=target.with_name(target.name+'.territory-new');temp.write_bytes(data);temp.replace(target)
   assert target.read_bytes()==data
 except BaseException:
  for target,prior in reversed(saved):
   if prior is None:target.unlink(missing_ok=True)
   else:target.write_bytes(prior)
  raise
 return dict(role=role,package=package_name,files=len(plans),index_entries=len(bindings),sha256_index=sha(next_index),nation_data_modified=False)
if __name__=='__main__':
 if len(sys.argv) not in [3,4]:raise SystemExit('Usage: Publish-TerritoryContent.py Client|Server backup-directory [reviewed-package]')
 backup=Path(sys.argv[2]).resolve();assert backup.is_relative_to(ROOT);backup.mkdir(parents=True,exist_ok=True);print(json.dumps(publish(sys.argv[1],backup,*sys.argv[3:]),indent=2))

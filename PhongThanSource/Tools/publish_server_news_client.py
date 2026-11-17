import json,shutil,struct
from restore_original_world import ROOT,OUT
from publish_original_world import parse_index
import build_offline_loose as b
folder=OUT/'server_news';client=ROOT/'PhongThanRuntime-Loose/Client'
plan=json.loads((folder/'client_plan.json').read_text('utf8'));backup=folder/'backup_before_client_publication'
if backup.exists():raise ValueError('Already applied: preserve journal')
backup.mkdir();manifest=ROOT/'PhongThanRuntime-Loose/completion/Client_named_effective_manifest.csv'
shutil.copy2(manifest,backup/manifest.name);shutil.copy2(client/'loose_index.bin',backup/'loose_index.bin')
rows=list(b.csv_read(manifest));byid={r['id']:r for r in rows};index=parse_index((client/'loose_index.bin').read_bytes())
for item in plan:
 relative=item['relative'];data=(folder/'staged/Client'/relative).read_bytes();target=client/relative
 assert b.sha(data)==item['sha256'] and (b.sha(target.read_bytes()) if target.exists() else None)==item['before_sha256']
 if target.exists():saved=backup/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(target,saved)
 target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
 raw=('\\'+relative.replace('/','\\')).encode('ascii');eid=b.pak_id(raw);index[eid]=raw;key=f'{eid:08X}'
 row=byid.get(key)
 if row is None:row=dict(id=key,status='RESOLVED',pak='',entry='',error='');rows.append(row)
 row.update(path=relative.replace('/','\\'),origin='ORIGINAL_MARQUEE_LAYOUT_BINDING',sha256=b.sha(data),bytes=str(len(data)),evidence='Original VNG news layout '+item['source']['id']+'; ASCII binding; channel 2 at y72')
 source=ROOT/'PhongThanSource/Deploy/ProjectContent'/relative
 if source.exists():saved=backup/'ProjectContent'/relative;saved.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,saved)
 source.parent.mkdir(parents=True,exist_ok=True);source.write_bytes(data)
(client/'loose_index.bin').write_bytes(b'PTLOOSE1'+struct.pack('<I',len(index))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(index.items())))
b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
(folder/'client_publication.json').write_text(json.dumps(dict(applied=True,files=4,restart_client_required=True,binaries_changed=False),indent=2),'utf8')
print('APPLIED news_layouts=4 original_width=386 height=16 font=14 client_restart_required=1 binaries_changed=0')

"""Recover only exact original-table dependencies missing from main Loose."""
from pathlib import Path
from collections import Counter
from datetime import datetime, timezone
import json
import re
import shutil
import struct
import build_offline_loose as b

root=Path(__file__).resolve().parents[2]
original=root/'Client_Original'
client=root/'PhongThanRuntime-Loose/Client'
out=root/'analysis/spr_recovery_20261018/original_dependency_recovery'
out.mkdir(exist_ok=True)
if (out/'applied.json').exists():raise SystemExit('Already applied; preserve initial journal')
listed=[original/'data'/n for n in re.findall(r'^\d+=([^\r\n]+)',(original/'package.ini').read_text('ascii'),re.M)]
winners,_=b.pak_index([p for p in listed if p.is_file()],strict=False)
trace=json.loads((out.parent/'original_name_trace/trace.json').read_text('utf8'))
refs={bytes.fromhex(r['path_hex']):dict(r) for r in trace['records']}
table_records=[]

def scan_table(raw,base=None):
    eid=b.pak_id(raw);entry=winners.get(eid)
    if not entry:return
    try:data,method=b.entry_payload(entry,None,None)
    except ValueError:return
    table_records.append(dict(path_hex=raw.hex(),id='%08X'%eid,pak=entry['pak'].name,
                              entry=entry['entry'],sha256=b.sha(data)))
    for row,line in enumerate(data.splitlines(),1):
        for col,cell in enumerate(line.split(b'\t'),1):
            cell=cell.strip()
            candidates=[]
            if base and cell.lower().endswith(b'.spr'):
                candidates.append(b'\\'+base.strip(b'\\')+b'\\'+cell)
            candidates+=re.findall(rb'\\spr\\[^\t\r\n"=]*?\.spr',cell,re.I)
            for name in candidates:
                rec=refs.setdefault(name,dict(path=name.decode('latin1'),path_hex=name.hex(),
                                              id='%08X'%b.pak_id(name),evidence=[]))
                rec['evidence'].append(dict(table_hex=raw.hex(),row=row,column=col,cell_hex=cell.hex()))

# Equipment effect sidecars follow the exact _effect construction in KNpcResNode.
for tab in trace['tables']:
    raw=bytes.fromhex(tab['path_hex'])
    if raw.lower().endswith(b'.txt'):
        scan_table(raw[:-4]+b'_effect.txt',b'spr\\npcres\\human')
# Use available names only as candidate table identifiers. Accept reference
# contents only after locating and decoding the corresponding original PAK entry.
for folder in ['settings/item/000','settings/item/001','settings/obj']:
    for p in (client/folder).rglob('*'):
        if p.is_file() and p.suffix.lower() in ('.txt','.ini'):
            try:raw=('\\'+str(p.relative_to(client)).replace('/','\\')).encode('latin1')
            except UnicodeEncodeError:continue
            scan_table(raw)

blob=(client/'loose_index.bin').read_bytes();assert blob[:8]==b'PTLOOSE1'
entries={};pos=12
for _ in range(struct.unpack_from('<I',blob,8)[0]):
    eid,n=struct.unpack_from('<IH',blob,pos);pos+=6;entries[eid]=blob[pos:pos+n];pos+=n
assert pos==len(blob)
records=[];staged=[];added=[]
for raw,original_rec in refs.items():
    rec=dict(original_rec);eid=b.pak_id(raw)
    try:rel=Path(b.logical(raw.decode('latin1')).replace('\\','/'))
    except ValueError:continue
    target=client/rel
    if target.is_file():rec['status']='PRESENT';records.append(rec);continue
    entry=winners.get(eid)
    if not entry:rec['status']='NO_PAYLOAD_IN_AVAILABLE_CHAIN';records.append(rec);continue
    try:
        data,method=b.entry_payload(entry,None,None)
        if not (b.sprite_blob_valid(data,True) or b.sprite_blob_valid(data,False)):
            raise ValueError('SPR frame/RLE validation failed')
        if eid in entries:
            old=client/entries[eid].decode('latin1').lstrip('\\').replace('\\','/')
            if entries[eid].lower()!=raw.lower() and (not old.is_file() or old.read_bytes()!=data):
                raise ValueError('Index collision with '+entries[eid].decode('latin1'))
        else:entries[eid]=raw;added.append('%08X'%eid)
        stage=out/'prepared'/rel;stage.parent.mkdir(parents=True,exist_ok=True);stage.write_bytes(data)
        rec.update(status='RECOVERED',sha256=b.sha(data),bytes=len(data),pak=entry['pak'].name,
                   entry=entry['entry'],method=method)
        staged.append((rel,stage,rec))
    except Exception as exc:rec.update(status='BLOCKED',error=str(exc))
    records.append(rec)
    if len(staged)%100==0 and staged:print('prepared',len(staged),flush=True)

backup=out/'loose_index.before.bin';shutil.copy2(client/'loose_index.bin',backup)
for rel,stage,rec in staged:
    target=client/rel;target.parent.mkdir(parents=True,exist_ok=True)
    with target.open('xb') as f:f.write(stage.read_bytes())
    assert b.sha(target.read_bytes())==rec['sha256']
payload=b'PTLOOSE1'+struct.pack('<I',len(entries))+b''.join(struct.pack('<IH',eid,len(raw))+raw for eid,raw in sorted(entries.items()))
if added:
    temp=client/'loose_index.original-recovery.tmp';temp.write_bytes(payload);temp.replace(client/'loose_index.bin')
summary=dict(utc=datetime.now(timezone.utc).isoformat(),additional_original_tables=len(table_records),
             references=len(records),counts=dict(Counter(r['status'] for r in records)),
             index_additions=len(added),index_before_sha256=b.sha(blob),index_after_sha256=b.sha(payload),
             main_client_restart_required=bool(added),server_modified=False)
(out/'applied.json').write_text(json.dumps(dict(summary=summary,tables=table_records,records=records),
                                          ensure_ascii=True,indent=2),encoding='utf8')
print(json.dumps(summary),flush=True)

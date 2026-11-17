"""Stage original missing glow resources and matching Dev Core DLLs for Loose."""
import hashlib
import json
import struct
from pathlib import Path

from restore_original_world import Original
from build_offline_loose import pak_id

ROOT=Path(__file__).resolve().parents[2]
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/EquipmentGlow20261029'
RUNTIME=ROOT/'PhongThanRuntime-Loose'


def sha(data):return hashlib.sha256(data).hexdigest()


def main():
    original=Original();table,table_proof=original.get(b'\\settings\\npcres\\zhuang_tai_tu_xing_dui_zhao_biao.txt')
    rows=table.splitlines();ids=[44,45,62,63,64,72,73];entries={};proofs=[]
    raw_index=(RUNTIME/'Client/loose_index.bin').read_bytes();at=12;index={}
    for _ in range(struct.unpack_from('<I',raw_index,8)[0]):
        key,size=struct.unpack_from('<IH',raw_index,at);at+=6
        name=raw_index[at:at+size];at+=size;index[key]=name
    assert at==len(raw_index)
    added=0
    for state in ids:
        row=rows[state].split(b'\t');logical=row[1];data,proof=original.get(logical)
        key=pak_id(logical);name=index.get(key,logical.lstrip(b'\\'))
        if key not in index:index[key]=name;added+=1
        relative=name.lstrip(b'\\').replace(b'\\',b'/').decode('cp1252')
        entries[('Client',relative)]=data
        proofs.append(dict(state=state,logical=logical.decode('gbk'),relative=relative,source=proof))
    for role in ('Client','Server'):
        relative='settings/npcres/zhuang_tai_tu_xing_dui_zhao_biao.txt'
        current=(RUNTIME/role/relative).read_bytes().splitlines()
        for state in ids:
            assert len(current)>state and current[state].split(b'\t')[0]==rows[state].split(b'\t')[0]
            current[state]=rows[state]
        entries[(role,relative)]=b'\r\n'.join(current)+b'\r\n'
    if added:
        # Keep every previous mapping and use the same bounded native format.
        header=bytearray(raw_index[:12]);struct.pack_into('<I',header,8,len(index))
        entries[('Client','loose_index.bin')]=bytes(header)+b''.join(struct.pack('<IH',key,len(name))+name for key,name in sorted(index.items()))
    for role,name in [('Client','CoreClient.dll'),('Server','CoreServer.dll')]:
        entries[(role,name)]=(ROOT/'PhongThanSource/Output'/role/name).read_bytes()
    files=[]
    for (role,relative),data in sorted(entries.items()):
        path=PACKAGE/'compiled'/role/relative;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(data)
        old=RUNTIME/role/relative
        files.append(dict(role=role,relative=relative,sha256=sha(data),before=sha(old.read_bytes()) if old.exists() else None))
    (PACKAGE/'manifest.json').write_text(json.dumps(dict(schema=1,files=files),ensure_ascii=False,indent=2)+'\n','utf-8')
    (PACKAGE/'provenance.json').write_text(json.dumps(dict(table=table_proof,effects=proofs,index_entries_added=added),ensure_ascii=False,indent=2)+'\n','utf-8')
    print('Staged',len(files),'files; original glow resources:',len(proofs),'new index entries:',added)


if __name__=='__main__':main()

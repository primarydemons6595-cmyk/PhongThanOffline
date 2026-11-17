"""Stage one mouth point per original crater and restore its scene decoration.

Merge only the removed original crater records. Preserve all other map edits.
No files under the running Loose runtime are written by this preparer.
"""
from pathlib import Path
import struct,re,json,sys,hashlib
from prepare_npc_restoration import split_region
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];RUNTIME=ROOT/'PhongThanRuntime-Loose';SOURCE=ROOT/'PhongThanSource'
OUT=ROOT/'analysis/studio_life_20261105/mining-content'
def sha(b):return hashlib.sha256(b).hexdigest()
def build():
    OUT.mkdir(parents=True,exist_ok=True);old=ROOT/'analysis/tet_event_shop_20261103'
    report=json.loads((old/'mining-craters.json').read_text('utf8'));files=[]
    backups=sorted(old.glob('bank-f2-mining-content-before-*'));assert len(backups)==1
    byfile={}
    for entry in report:
        for ref in entry['references']:byfile.setdefault(ref['region'],[]).append((entry,ref))
    for file,refs in byfile.items():
        p=ROOT/file;relative=p.relative_to(RUNTIME);before=p.read_bytes();original=(backups[0]/relative).read_bytes()
        head,sections,_=split_region(before);offset,length=sections[4];g=before[head+offset:head+offset+length];count,objects,start=struct.unpack_from('<iii',g)
        records=[g[start+i*146:start+(i+1)*146] for i in range(objects)]
        for entry,ref in refs:
            record=original[ref['offset']:ref['offset']+146]
            assert struct.unpack_from('<ii',record)==(entry['x'],entry['y']) and struct.unpack_from('<H',record,140)[0]==ref['frame']
            assert record[8:136].split(b'\0')[0].hex()==entry['image_hex']
            if record not in records:records.append(record)
        new=bytearray(g[:start]);struct.pack_into('<i',new,4,len(records));new.extend(b''.join(records))
        parts=[before[head+o:head+o+n] for o,n in sections];parts[4]=bytes(new);cursor=0;directory=[]
        for part in parts:directory.append((cursor,len(part)));cursor+=len(part)
        payload=struct.pack('<i',len(parts))+b''.join(struct.pack('<ii',*v) for v in directory)+b''.join(parts)
        target=OUT/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(payload);files.append(dict(path=str(relative),before_sha256=sha(before),sha256=sha(payload)))
    for role in ['Client','Server']:
        relative=Path(role)/'settings/obj/ObjData.txt';before=(RUNTIME/relative).read_bytes();lines=before.splitlines();h=lines[0].split(b'\t');at=next(i for i,line in enumerate(lines[1:],1) if dict(zip(h,line.split(b'\t'))).get(b'DataID')==b'383');row=lines[at].split(b'\t')
        # Sprite remains available for a small unlit lava highlight, not a
        # duplicate mountain. Original default appearance is owned by map art.
        for key,value in {b'LightRadius':b'280',b'LightRed':b'255',b'LightGreen':b'190',b'LightBlue':b'40',b'Height':b'16',b'LifeTime':b'0'}.items():
            if key in h:row[h.index(key)]=value
        lines[at]=b'\t'.join(row);payload=b'\r\n'.join(lines)+b'\r\n';target=OUT/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(payload);files.append(dict(path=str(relative),before_sha256=sha(before),sha256=sha(payload)))
    materials={int(r[2]):decode_tcvn3(r[0]).lstrip('#') for row in (RUNTIME/'Server/settings/item/001/material.txt').read_bytes().splitlines()[1:] if len(r:=row.split(b'\t'))>2 and r[2].isdigit()}
    # Explicit project defaults; the original server distribution was not found
    # in the recovered settings. They are fully editable in Studio.
    rows=['; Custom Loose mining policy. Default rates are authored, not VNG.','[Main]','Version=1','Enabled=1','ChancePer10000=2000','LuckBonusPerPoint=10','SiteCount=34','LootCount=4','','[Messages]']
    for k,v in [('PrefixHex','Bạn đào được '),('BetweenHex',' '),('SuffixHex','.')]:rows.append(k+'='+encode_tcvn3(v).hex().upper())
    points=[e for e in report if e['references'][0]['frame']==0];assert len(points)==34
    for i,p in enumerate(points,1):
        values=dict(Id=i,NameHex=encode_tcvn3('Mỏ Đồng '+str(i)).hex().upper(),Map=1057,X=p['x']+240,Y=p['y']+160,Capacity=100,RespawnSeconds=600,Reach=512,Table=0,Enabled=1)
        rows+=['','[Mine'+str(i)+']']+[k+'='+str(v) for k,v in values.items()]
    for i,(detail,weight,maximum) in enumerate([(3,8000,2),(928,1000,2),(929,700,2),(930,300,1)],1):
        assert detail in materials
        values=dict(Table=0,Genre=3,Detail=detail,Particular=0,Level=0,Series=0,Weight=weight,Minimum=1,Maximum=maximum)
        rows+=['','[Loot'+str(i)+']']+[k+'='+str(v) for k,v in values.items()]
    relative=Path('Server/settings/phongthan/MiningPolicy.ini');payload=('\r\n'.join(rows)+'\r\n').encode('ascii');target=OUT/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(payload)
    files.append(dict(path=str(relative),before_sha256=sha((RUNTIME/relative).read_bytes()) if (RUNTIME/relative).exists() else None,sha256=sha(payload)))
    result=dict(files=files,restored_scene_records=len(report),sites=34,default_loot=[dict(detail=d,name=materials[d]) for d in [3,928,929,930]],policy='AUTHORED_PER_CAST_RANDOM; one mouth per crater; existing native skill animation; no progress bar; persistent shared reserves')
    (OUT/'mining-manifest.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8');print('STAGED',len(report),'original scene records, 34 mouth points,',len(files),'files; runtime unchanged')
if __name__=='__main__':build()

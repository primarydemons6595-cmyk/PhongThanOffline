"""Try documented NPC names across actual known original script directories."""
import collections,json,re,hashlib
from restore_original_world import Original,OUT,ROOT,path_bytes
from extract_client_coordinate_dictionary import decode_tcvn3
import build_offline_loose as b

o=Original();scopes={'script/运粮','script/运镖','script/押镖','script/瑶池','script/孟津','script/绝龙岭','script/封神台'}
for row in b.csv_read(ROOT/'PakRecoveryWork/02_PathDictionary/all_paths.csv'):
    try:p=path_bytes(row['CanonicalPath']).decode('gbk').replace('\\','/').lstrip('/')
    except UnicodeError:continue
    if p.startswith('script/') and p.endswith(('.lua','.luax')):scopes.add(p.rsplit('/',1)[0])
names={'风林','风林影子','张桂芳','总兵张桂芳','安安仙子','安安','武王','太岁师','星相师','传送门幻'}
localized={line.split(b'\t',1)[0].decode('ascii','replace'):decode_tcvn3(line.split(b'\t',1)[1]) for line in (OUT/'localize_vietnamese.expanded.txt').read_bytes().splitlines() if b'\t' in line}
for line in (ROOT/'Client_Original/cn_dict.txt').read_bytes().splitlines():
    parts=line.split(b':',1)
    if len(parts)!=2:continue
    raw=parts[1].strip();vn=localized.get(hashlib.shake_128(raw).hexdigest(8).upper(),'')
    if len(vn)<60 and any(t in vn.lower() for t in ('an an','quế phương','chiêm tinh','phong lâm')):
        cn=raw.decode('gbk','replace');print('NAME',cn,repr(vn))
        if len(cn)<30 and not any(c in cn for c in '/<>\\'):names.add(cn)
found=[]
for directory in sorted(scopes):
    for name in sorted(names):
        for suffix in ('.lua','.luax'):
            path=directory+'/'+name+suffix
            for encoding in ('gbk','utf8'):
                got=o.get(path.encode(encoding))
                if got:
                    data,proof=got
                    if proof['id'] not in {p['id'] for p in found}:
                        found.append(proof|dict(candidate_path=path,encoding=encoding))
                        (OUT/'priority_events'/(proof['id']+'.lua')).write_bytes(data)
                        print('FOUND',path,encoding,proof['id'],len(data),'ENTRYPOINTS',re.findall(rb'function\s+(\w+)',data)[:7])
(OUT/'priority_events/path_probe.json').write_text(json.dumps(dict(scopes=len(scopes),names=sorted(names),found=found),ensure_ascii=False,indent=2),'utf8')

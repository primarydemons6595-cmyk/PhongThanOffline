"""Build ID -> real named loose path metadata, without ID payloads or PAK reads."""
import argparse
import json
import struct
from pathlib import Path
import build_offline_loose as b


def run(root):
    root=root.resolve()
    if root.name!='PhongThanRuntime-Loose':
        raise ValueError('Only the separate PhongThanRuntime-Loose root is allowed')
    result={}
    for role in ('Client','Server'):
        folder=root/role
        manifest=root/'completion'/(role+'_named_effective_manifest.csv')
        candidates={};skipped=[]
        for row in b.csv_read(manifest):
            if row['status']!='RESOLVED':continue
            name=b.logical(row['path'])
            file=folder/Path(name.replace('\\','/'))
            if not file.is_file():raise ValueError('Manifest file absent: '+str(file))
            try:
                raw=('\\'+name).encode('mbcs',errors='strict')
                if ('\\'+name)!=raw.decode('mbcs'):
                    raise ValueError('Lossy ANSI path')
                if len(raw)+len(str(folder).encode('mbcs'))>=260:
                    raise ValueError('Native full path exceeds MAX_PATH')
            except (UnicodeError,ValueError) as exc:
                skipped.append(dict(path=name,error=str(exc)));continue
            ids={b.pak_id(raw)}
            if row.get('id'):ids.add(int(row['id'],16))
            for eid in ids:
                candidates.setdefault(eid,[]).append((raw,row['sha256'].lower()))
        entries=[]
        # Explicit Studio edits supersede recovery provenance for their bound IDs.
        # Never silently drop new named scripts after a later offline index rebuild.
        overlay_file=root/'completion'/'studio_effective_overrides.json'
        if overlay_file.is_file():
            for change in json.loads(overlay_file.read_text(encoding='utf-8-sig')):
                if change['Side']!=role:continue
                name=b.logical(change['Path'])
                relative=Path(name.replace('\\','/'))
                if relative.is_absolute() or '..' in relative.parts:
                    raise ValueError('Unsafe Studio overlay path')
                file=folder/relative
                if not file.is_file() or b.sha(file.read_bytes()).lower()!=change['Sha256'].lower():
                    raise ValueError('Studio edited file absent/changed outside Studio: '+str(file))
                raw=('\\'+name).encode('mbcs',errors='strict')
                if len(raw)+len(str(folder).encode('mbcs'))>=260:
                    raise ValueError('Studio native path exceeds MAX_PATH')
                for eid in {*change['Ids'], b.pak_id(raw)}:
                    candidates[eid]=[(raw,change['Sha256'].lower())]
        for eid,choices in sorted(candidates.items()):
            if eid==0 or len({sha for _,sha in choices})!=1:
                skipped.append(dict(path='%08X'%eid,error='AMBIGUOUS_ID_DIFFERENT_PAYLOAD'));continue
            raw=min(path for path,_ in choices)
            entries.append(struct.pack('<IH',eid,len(raw))+raw)
        data=b'PTLOOSE1'+struct.pack('<I',len(entries))+b''.join(entries)
        (folder/'loose_index.bin').write_bytes(data)
        (folder/'loose_only.flag').write_bytes(b'PT_LOOSE_ONLY_V1\r\n')
        b.csv_write(root/'completion'/(role+'_loose_index_skipped.csv'),skipped,['path','error'])
        result[role]=dict(ids=len(entries),skipped=len(skipped),sha256=b.sha(data),
            payload_storage='ORIGINAL_NAMED_FILES_ONLY',pak_fallback=False)
        print(role,result[role],flush=True)
    (root/'completion'/'loose_index_summary.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    return result


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime',type=Path,required=True)
    run(parser.parse_args().runtime)

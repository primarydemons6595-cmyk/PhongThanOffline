"""Find coordinate evidence in ALL available original text payloads, named or not."""
import collections,json,re,sys
from pathlib import Path
import build_offline_loose as b
from restore_original_world import ROOT,OUT,Original,path_bytes


def main():
    original=Original()
    catalog=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/coordinates.json').read_text('utf8'))
    names={path_bytes(row[1]):row[0] for items in catalog['maps'].values() for row in items if row[1]}
    names={raw:name for raw,name in names.items() if len(raw)>2}
    hints=[];counts=collections.Counter()
    chinese=[s.encode('gbk') for s in ('\u5750\u6807','\u5ea7\u6807','\u5bfb\u8def','\u4f4d\u7f6e')]
    # Path identities are hints only; original winner/decoded bytes are evidence.
    path_ids=collections.defaultdict(set)
    for f,column in [(ROOT/'PakRecoveryWork/02_PathDictionary/all_paths.csv','CanonicalPath'),
                      (ROOT/'PhongThanRuntime-Loose/completion/Server_named_effective_manifest.csv','path')]:
        for row in b.csv_read(f):
            try: raw=path_bytes(row[column]);path_ids[b.pak_id(raw)].add(row[column])
            except (UnicodeError,KeyError):pass
    for number,e in enumerate(original.winners.values()):
        if e['flags']>>24 not in (0,1) or not 16<e['size']<8*1024*1024:continue
        try:data,_=b.entry_payload(e,{},None)
        except (ValueError,MemoryError):counts['decode_failure']+=1;continue
        if b'\0' in data[:4096] or data[:3]==b'SPR':continue
        if sum(c<9 or 13<c<32 for c in data[:4096])>max(4,len(data[:4096])//100):continue
        counts['text']+=1
        matched=[(raw,name) for raw,name in names.items() if raw in data]
        spawn=b'AddNpc' in data
        header=data[:1000].lower()
        coordinate_header=bool(re.search(rb'(?:position[xy]|npcpos|npc_pos|npcmap|npc_map|posx|posy|coord[xy]|mapid)',header))
        positional_words=any(s in data for s in chinese)
        links=bool(re.search(rb'<(?:npc|pos|map|location|find|goto)[= :]',data,re.I))
        pairs=bool(re.search(rb'\b\d{2,5}\s*[,;/]\s*\d{2,5}\b',data))
        if not (coordinate_header or spawn or links or (matched and (pairs or positional_words))):continue
        eid=f"{e['id']:08X}";relative=f'coordinate_evidence/original_{eid}.txt'
        target=OUT/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
        lines=data.splitlines()
        interesting=[]
        for i,line in enumerate(lines):
            if (any(raw in line for raw,name in matched) or b'AddNpc' in line or
                re.search(rb'(?:position[xy]|npcpos|npc_pos|mapid|posx|posy|<npc[= :])',line,re.I)):
                for j in range(max(0,i-1),min(len(lines),i+2)):
                    interesting.append(dict(line=j+1,text=lines[j].decode('gbk','replace')))
        hints.append(dict(id=eid,pak=e['pak'].name,entry=e['entry'],sha256=b.sha(data),bytes=len(data),
            possible_paths=sorted(path_ids[e['id']]),names=[name for raw,name in matched],
            spawn=spawn,coordinate_header=coordinate_header,links=links,local=relative,
            lines=list({x['line']:x for x in interesting}.values())))
        if len(hints)%100==0:print('coordinate_sources',len(hints),'text',counts['text'],flush=True)
    report=dict(scope='ALL_AVAILABLE_ORIGINAL_WINNERS_TEXT_SCAN',counts=dict(counts),sources=hints,
        missing_packages=original.missing)
    (OUT/'coordinate_sources.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
    print('COORDINATE_SCAN',json.dumps(dict(counts),ensure_ascii=True),'sources',len(hints),flush=True)


if __name__=='__main__':main()

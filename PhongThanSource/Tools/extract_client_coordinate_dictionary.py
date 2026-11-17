"""Read original loose dictionaries; decode bounded NRV2B, no client execution."""
import json,re,struct
from pathlib import Path
from restore_original_world import ROOT,OUT
from build_offline_loose import nrv2b


_TCVN3_MAP=None
def decode_tcvn3(raw):
    global _TCVN3_MAP
    if _TCVN3_MAP is None:
        source=(ROOT/'PhongThanSource/Tools/Export-StarterBagLookup.ps1').read_text('utf-8-sig')
        left=re.search(r'\$from=@\(([^)]+)',source)[1].split(',')
        right=re.search(r'\$to=@\(([^)]+)',source)[1].split(',')
        _TCVN3_MAP={int(a,16):chr(int(b,16)) for a,b in zip(left,right)}
    return ''.join(_TCVN3_MAP.get(c,chr(c)) for c in raw)


def encode_tcvn3(text):
    """Encode authored Vietnamese using canonical font slots, never fallbacks.

    Reversing decode_tcvn3 over all 256 bytes is ambiguous: an unmapped byte
    such as E0 decodes as Latin-1 'à', overriding the real TCVN3 B5 glyph.
    """
    decode_tcvn3(b'')
    reverse={character:byte for byte,character in _TCVN3_MAP.items()}
    reverse.update({chr(i):i for i in range(128)})
    return bytes(reverse[character] for character in text)


def lzss(data):
    expected=struct.unpack_from('<I',data)[0]
    if not 0<expected<64*1024*1024:raise ValueError('Invalid expanded length')
    ring=bytearray(b' '*4096);cursor=4096-18;pos=4;flags=0;out=bytearray()
    while len(out)<expected:
        flags>>=1
        if flags&256==0:
            if pos>=len(data):raise ValueError('Short flags')
            flags=data[pos]|0xff00;pos+=1
        if flags&1:
            c=data[pos];pos+=1;out.append(c);ring[cursor]=c;cursor=(cursor+1)&4095
        else:
            i,j=data[pos:pos+2];pos+=2;i|=(j&240)<<4
            for k in range((j&15)+3):
                c=ring[(i+k)&4095];out.append(c);ring[cursor]=c;cursor=(cursor+1)&4095
        if len(out)>expected:
            raise ValueError('Expanded length overflow: consumed=%d/%d head=%r tail=%r'%(pos,len(data),bytes(out[:180]),bytes(out[-100:])))
    if pos!=len(data):raise ValueError('Trailing compressed bytes')
    return bytes(out)


def main():
    texts={'cn_dict.txt':(ROOT/'Client_Original/cn_dict.txt').read_bytes().decode('gbk','replace')}
    try:
        packed=(ROOT/'Client_Original/localize_vietnamese.lz').read_bytes()
        decoded=nrv2b(packed[4:],struct.unpack_from('<I',packed)[0])
        (OUT/'localize_vietnamese.expanded.txt').write_bytes(decoded)
        # Vietnamese legacy payloads use Windows-1258; record raw bytes too.
        texts['localize_vietnamese.lz']=decode_tcvn3(decoded)
        print('LOCALIZE_EXPANDED',len(decoded),'HEAD',repr(decoded[:180]))
    except (ValueError,IndexError) as exc:print('LOCALIZE_DECODE_FAILED',str(exc))
    records=[]
    for source,text in texts.items():
        for number,line in enumerate(text.splitlines(),1):
            pairs=re.findall(r'(?<!\d)([1-3]\d{2})\s*[,，/:：]\s*([1-3]\d{2})(?!\d)',line)
            if pairs:records.append(dict(source=source,line=number,text=line,coordinates=[[int(x),int(y)] for x,y in pairs]))
    (OUT/'client_dictionary_coordinates.json').write_text(json.dumps(records,ensure_ascii=False,indent=2),'utf8')
    print('DICTIONARY_COORDINATE_LINES',len(records),'FACTS',sum(len(r['coordinates']) for r in records))
    links=[]
    for source,text in texts.items():
        for number,line in enumerate(text.splitlines(),1):
            for match in re.finditer(r'HyperLinkWorldPos\s*=\s*[^>]*?>',line,re.I):
                tag=match[0]
                for p in re.finditer(r'\[\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\]',tag):
                    name=re.sub(r'^HyperLinkWorldPos\s*=\s*','',tag[:p.start()],flags=re.I).strip(' "“”\'')
                    world,x,y=map(int,p.groups())
                    links.append(dict(source=source,line=number,name=name,content_map=world,x=x,y=y,tag=tag))
    # The client's own NPC help table declares map groups by adjacent IDs.
    # Preserve this as older help evidence; quest hyperlinks take precedence.
    help_rows=[]
    groups=[(11137,11143,2),(11144,11148,3),(11149,11154,4),(11155,11165,20),
        (11166,11176,21),(11177,11180,1),(11181,11188,52)]
    for number,line in enumerate(texts['cn_dict.txt'].splitlines(),1):
        match=re.match(r'(\d+):(.*)',line)
        if not match:continue
        key=int(match[1]);world=next((w for low,high,w in groups if low<=key<=high),None)
        if world is None:continue
        for p in re.finditer(r'([\u3400-\u9fff\s]{2,20})\s*"?\s*(\d{3})\s*[,，]\s*(\d{3})',match[2]):
            help_rows.append(dict(source='cn_dict.txt',line=number,text_id=key,name=re.sub(r'\s+','',p[1]),content_map=world,
                x=int(p[2]),y=int(p[3]),classification='CLIENT_LEGACY_HELP_TABLE'))
    (OUT/'client_help_npc_coordinates.json').write_text(json.dumps(help_rows,ensure_ascii=False,indent=2),'utf8')
    (OUT/'client_hyperlink_coordinates.json').write_text(json.dumps(links,ensure_ascii=False,indent=2),'utf8')
    print('HYPERLINKS',len(links),'UNIQUE',len({(r['source'],r['name'],r['content_map'],r['x'],r['y']) for r in links}))
    print(json.dumps([r for r in links if r['source']=='cn_dict.txt'][:28],ensure_ascii=False))


if __name__=='__main__':main()

"""Audit the effective Loose F2 catalogue, descriptions and use entry points."""
from pathlib import Path
import json,re,csv,collections,argparse,struct
ROOT=Path(__file__).resolve().parents[2]

def run():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,default=ROOT/'analysis/f2_functions_20261023');args=parser.parse_args()
    folder=ROOT/'PhongThanRuntime-Loose/Server';args.output.mkdir(parents=True,exist_ok=True)
    mapping=(ROOT/'PhongThanSource/Tools/Export-StarterBagLookup.ps1').read_text(encoding='utf-8-sig')
    a=re.search(r'\$from=@\(([^)]+)',mapping)[1];b=re.search(r'\$to=@\(([^)]+)',mapping)[1]
    decode={int(x,16):chr(int(y,16)) for x,y in zip(a.split(','),b.split(','))}
    def text(raw):return ''.join(decode.get(c,chr(c)) for c in raw).lstrip('#')
    def lines(p):return [line.split(b'\t') for line in p.read_bytes().splitlines()[1:] if line.strip(b'\t\r\n\0 ')]
    tables={}
    for name in ('ibitem.txt','magicscript.txt','material.txt','questkey.txt','skillbook.txt','potion.txt'):
        p=folder/'settings/item/001'/name
        if p.exists():tables[name]=lines(p)
    effects={tuple(map(int,line[:2])) for line in lines(folder/'settings/phongthan/ConsumableEffects.txt')}
    categories=collections.defaultdict(list)
    for path in (folder/'settings/ibshop').glob('*goods.txt'):
        if path.name=='ibshopgoods.txt':continue
        for row in lines(path):
            if row[0].isdigit():categories[int(row[0])].append(path.stem)
    goods=lines(folder/'settings/ibshop/ibshopgoods.txt')
    if not any(len(c)>3 and c[1:4]==[b'8',b'35',b'2'] for c in goods):
        goods.append([b'Native appended travel row',b'8',b'35',b'2',b'0',b'0',b'1'])
    records=[]
    for n,c in enumerate(goods,1):
        try:
            genre,detail,part=map(int,c[1:4]);level=int(c[4] or 0)
            if genre>=9:genre,detail,part,level=8,genre,detail,part
        except (ValueError,IndexError):continue
        name={8:'ibitem.txt',6:'magicscript.txt',3:'material.txt',4:'questkey.txt',7:'skillbook.txt',1:'potion.txt'}.get(genre)
        matches=[r for r in tables.get(name,[]) if len(r)>8 and all(v.isdigit() for v in r[1:4]) and tuple(map(int,r[1:4]))==(genre,detail,part)]
        if genre in (3,4):matches=[r for r in tables.get(name,[]) if len(r)>7 and r[1]==str(genre).encode() and r[2]==str(detail).encode()]
        if genre==7:matches=[r for r in matches if len(r)>12 and int(r[12] or 0)==level] or matches
        row=matches[0] if matches else None
        script=b''
        if row:script=row[12] if genre==8 else row[14] if genre==6 else b''
        script=script.strip(b'\0 ')
        status='UNCLASSIFIED';payload=b'';path=''
        if not row:status='TEMPLATE_NOT_MATCHED'
        elif script and script!=b'NONE' and script.lower().endswith((b'.lua',b'.luax')):
            path=script.decode('cp1252',errors='strict').lstrip('\\').replace('\\','/')
            p=folder/path
            if p.is_file():
                payload=p.read_bytes()
                if b'BLOCKED_SPEC' in payload or b'generated safety adapter' in payload:status='BLOCKED_SPEC_ADAPTER'
                elif re.search(rb'function\s+main\s*\([^)]*\)\s*(?:--[^\n]*\n\s*)?end\b',payload):status='EMPTY_MAIN_REVIEW'
                else:status='LUA_PRESENT_NOT_GAME_VERIFIED'
            else:status='LUA_PATH_ABSENT_CHECK_INDEX'
        elif genre==8 and (detail,part) in effects:status='NATIVE_CONSUMABLE'
        elif genre==8 and detail in (269,270):status='BAG_UNLOCK_MATERIAL'
        elif genre==3 or genre==4:status='MATERIAL_OR_CURRENCY'
        elif genre==7:status='NATIVE_SKILLBOOK'
        else:status='NATIVE_USE_REVIEW'
        records.append(dict(goods_row=n,genre=genre,detail=detail,particular=part,level=level,
            categories=','.join(categories[n]),name=text(row[0]) if row else text(c[0]),
            description=text(row[7 if genre in (3,4) else 8]) if row else '',table=name or '',script=path,status=status,
            lua_bytes=len(payload),lua_preview=payload[:500].decode('cp1252',errors='replace')))
    (args.output/'catalog.json').write_text(json.dumps(records,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    with (args.output/'catalog.csv').open('w',encoding='utf-8-sig',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=list(records[0]));writer.writeheader();writer.writerows(records)
    counts=collections.Counter(r['status'] for r in records)
    print(json.dumps(dict(rows=len(records),coverage=counts),ensure_ascii=True))
    review=[{k:r[k] for k in ('goods_row','name','description','script','status')} for r in records if r['status'] in ('LUA_PATH_ABSENT_CHECK_INDEX','BLOCKED_SPEC_ADAPTER','EMPTY_MAIN_REVIEW','NATIVE_USE_REVIEW')]
    (args.output/'review.json').write_text(json.dumps(review,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(json.dumps(review[:35],ensure_ascii=True,indent=2))
if __name__=='__main__':run()

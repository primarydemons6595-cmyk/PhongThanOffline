"""Read-only inventory of native/scripted item-use coverage; never consumes items."""
from pathlib import Path
import argparse,csv,json,re,unicodedata

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--runtime',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    project=Path(__file__).resolve().parents[1]
    mapping=(project/'Tools/Export-StarterBagLookup.ps1').read_text(encoding='utf-8-sig')
    a=re.search(r'\$from=@\(([^)]+)',mapping)[1];b=re.search(r'\$to=@\(([^)]+)',mapping)[1]
    dec={int(x,16):chr(int(y,16)) for x,y in zip(a.split(','),b.split(','))}
    def text(raw):return ''.join(dec.get(c,chr(c)) for c in raw)
    descriptors={tuple(map(int,r.split(b'\t')[:2])) for r in (args.runtime/'Server/settings/phongthan/ConsumableEffects.txt').read_bytes().splitlines()[1:] if r}
    report=[]
    for filename in ('ibitem.txt','magicscript.txt','event.txt','townportal.txt','skillbook.txt','medicine.txt','material.txt'):
        path=args.runtime/'Server/settings/item/001'/filename
        if not path.exists():continue
        for stt,line in enumerate(path.read_bytes().splitlines()[1:]):
            c=line.split(b'\t')
            if len(c)<4 or not all(x.isdigit() for x in c[1:4]):continue
            genre,detail,part=map(int,c[1:4])
            scripts=[x.strip() for x in c if x.strip().lower().endswith(b'.lua')]
            script=scripts[0] if scripts else b''
            if filename=='ibitem.txt' and (detail,part) in descriptors:state='NATIVE_HANDLER_SUPPORTED'
            elif filename=='ibitem.txt' and (detail,part)==(226,2):state='OFFLINE_ENTRUST_ITEM_NOT_COMBAT_BUFF'
            elif filename=='skillbook.txt' and detail<58:state='NATIVE_BOOK_HANDLER'
            elif filename=='medicine.txt':state='NATIVE_MEDICINE_CHECK_EFFECTS'
            elif filename=='material.txt':state='MATERIAL_NOT_TIMED_BUFF'
            elif script:
                relative=script.lstrip(b'\\/').decode('latin1').replace('\\','/')
                state='LUA_PRESENT_RUNTIME_UNVERIFIED' if (args.runtime/'Server'/relative).is_file() else 'LUA_MISSING'
            else:state='NATIVE_HANDLER_REQUIRED'
            report.append(dict(Table=filename,STT=stt,Genre=genre,Detail=detail,Part=part,Name=text(c[0]),Script=script.decode('latin1'),Status=state))
    args.output.parent.mkdir(parents=True,exist_ok=True)
    with args.output.open('w',encoding='utf-8-sig',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=list(report[0]));writer.writeheader();writer.writerows(report)
    counts={state:sum(r['Status']==state for r in report) for state in sorted({r['Status'] for r in report})}
    print(json.dumps({'rows':len(report),'coverage':counts,'output':str(args.output)},ensure_ascii=False))
if __name__=='__main__':main()

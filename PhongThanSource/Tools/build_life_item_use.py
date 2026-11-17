"""Compile original crafted IB descriptors; never rewrite original item rows.

These IDs belong to the IB table, NOT Skills.txt. Internal buff identifiers
are separate from all combat skills and fit the existing saved 14-bit field.
"""
from pathlib import Path
import json,hashlib,sys
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];SOURCE=ROOT/'PhongThanSource';SERVER=ROOT/'PhongThanRuntime-Loose/Server'
def cs(b):return '"'+''.join('\\x%02x'%c for c in b)+'"'
def build():
    manifest=json.loads((SOURCE/'Deploy/ProjectContent/LifeGiaoHuan20261030/life-manifest.json').read_text('utf8'))
    ids={o['item'][1] for r in manifest['recipes'].values() for o in r['outputs'] if o['item'][0]==8}
    source=SERVER/'settings/item/001/ibitem.txt';rows=[l.split(b'\t') for l in source.read_bytes().splitlines()];byid={int(r[2]):r for r in rows[1:] if len(r)>2 and r[2].isdigit()}
    result=[];records=[]
    for id in sorted(ids):
        r=byid[id];num=lambda c:int(r[c] or b'0') if c<len(r) else 0
        attrs=[(num(c),num(c+1)) for c in range(30,54,2) if num(c)]
        req=[(num(c),num(c+1)) for c in range(54,66,2) if num(c)]
        assert all(a==36 for a,v in req),(id,req)
        level=max([v for a,v in req]+[0]);seconds=num(14)
        kind=3 if id==987 else 4 if id==988 else 1 if seconds else 2
        assert (kind in [3,4] and not attrs) or (attrs and kind in [1,2]),id
        assert kind!=1 or r[12]==b'NONE',id
        assert num(13)==0 and num(15)==0 and num(17)==0,(id,'unreviewed stacking/clock policy')
        padded=attrs+[(0,0)]*(12-len(attrs))
        record='{'+','.join(map(str,[id,num(3),12000+id,kind,seconds,level,num(19)]))+',{'+','.join('{%d,%d}'%v for v in padded)+'},'+','.join(cs(v.lstrip(b'#$') if i<2 else v) for i,v in enumerate([r[0],r[8],r[4]]))+'}'
        records.append(record);result.append(dict(detail=id,buff=12000+id,kind=kind,seconds=seconds,level=level,attributes=attrs,name=decode_tcvn3(r[0])))
    external=byid[1217]
    external_record='{1217,0,1217,1,600,0,0,{{88,-50}},'+','.join(cs(v.lstrip(b'#$') if i<2 else v) for i,v in enumerate([external[0],external[8],external[4]]))+'}'
    h='''#ifndef PHONGTHAN_LIFE_ITEM_USE_DATA_H
#define PHONGTHAN_LIFE_ITEM_USE_DATA_H
struct PTLifeItemUse {int detail,part,buff,kind,seconds,level,cancelable;int attrs[12][2];const char* name;const char* description;const char* image;};
static const PTLifeItemUse ptLifeItemUses[]={
'''+',\n'.join(records)+'''};
static const PTLifeItemUse ptLifeHacSa='''+external_record+''';
inline const PTLifeItemUse* PtLifeItem(int detail){for(int i=0;i<sizeof(ptLifeItemUses)/sizeof(ptLifeItemUses[0]);++i)if(ptLifeItemUses[i].detail==detail)return &ptLifeItemUses[i];return 0;}
inline const PTLifeItemUse* PtLifeBuff(int id){if(id==1217)return &ptLifeHacSa;const PTLifeItemUse* d=id>=12000?PtLifeItem(id-12000):0;return d&&d->kind==1?d:0;}
#endif
'''
    (SOURCE/'Headers/PhongThanLifeItemUseData.h').write_text(h,'ascii')
    out=ROOT/'analysis/studio_life_20261105';out.mkdir(exist_ok=True)
    (out/'life-item-use-data.json').write_text(json.dumps(dict(source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),items=result),ensure_ascii=False,indent=2),'utf8')
    print('Prepared',len(result),'original IB descriptors; no runtime item table writes')
if __name__=='__main__':build()

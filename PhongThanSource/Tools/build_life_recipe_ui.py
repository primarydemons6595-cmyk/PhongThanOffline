"""Compile the existing recipe catalog for native UI presentation only."""
from pathlib import Path
import json,sys,hashlib
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];SOURCE=ROOT/'PhongThanSource';SERVER=ROOT/'PhongThanRuntime-Loose/Server'
def cb(b):return '"'+''.join('\\x%02x'%x for x in b)+'"'
def cs(s):return cb(encode_tcvn3(s))
def build():
    manifest=json.loads((SOURCE/'Deploy/ProjectContent/LifeGiaoHuan20261030/life-manifest.json').read_text('utf8'))
    for s in manifest['source_tables']:
        assert hashlib.sha256((SERVER/s['path']).read_bytes()).hexdigest()==s['sha256'],('Recipe source changed; recompile server data first',s['path'])
    tables={}
    for genre,file in [(3,'material'),(6,'magicscript'),(8,'ibitem')]:
        rows=[r.split(b'\t') for r in (SERVER/f'settings/item/001/{file}.txt').read_bytes().splitlines()]
        headers=[h.decode('gbk') for h in rows[0]];image=headers.index('动画文件名');index=3 if genre==6 else 2
        tables[genre]={int(r[index]):(decode_tcvn3(r[0]).lstrip('#'),r[image]) for r in rows[1:] if len(r)>image and r[index].isdigit()}
    def item(t):return tables[t[0]][t[2] if t[0]==6 else t[1]]
    def icons(ts,n):return '{'+','.join(cb(item(t)[1]) for t in ts)+(',' if ts and len(ts)<n else '')+','.join('""' for _ in range(n-len(ts)))+'}'
    def tips(ts,n):return '{'+','.join(cs(item(t)[0]+' x'+str(t[6])) for t in ts)+(',' if ts and len(ts)<n else '')+','.join('""' for _ in range(n-len(ts)))+'}'
    h='#ifndef PT_LIFE_RECIPE_UI_H\n#define PT_LIFE_RECIPE_UI_H\nstruct PT_LIFE_RECIPE_UI {int id,skill,rank,energy,strength,cost,chance,bonus,known;const char* name;const char* icons[4];const char* needs[4];const char* outputIcons[2];const char* outputs[2];int outputChance[2];};\nstatic const PT_LIFE_RECIPE_UI PT_LIFE_RECIPE_CATALOG[]={\n'
    records=[]
    for r in manifest['recipes'].values():
        needs=r['needs'];outputs=[x['item'] for x in r['outputs']]
        assert len(needs)<=4 and len(outputs)<=2
        chances=[o['chance'] for o in r['outputs']]+[0]*(2-len(outputs))
        records.append('{'+','.join(str(r[k]) for k in ['id','skill','rank','energy','strength','cost','chance','bonus','default'])+','+cs(r['name'])+','+icons(needs,4)+','+tips(needs,4)+','+icons(outputs,2)+','+tips(outputs,2)+',{'+','.join(map(str,chances))+'}}')
    h+=',\n'.join(records)+'};\n#endif\n';(SOURCE/'Headers/PhongThanLifeRecipeUi.h').write_text(h,'ascii')
    print('PREPARED UI catalog:',len(records),'recipes; original item icons; server recipe source hashes verified')
if __name__=='__main__':build()

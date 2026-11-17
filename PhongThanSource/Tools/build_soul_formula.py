"""Compile byte-exact original formula identities recovered from VNG PAKs."""
from pathlib import Path
import json,hashlib
ROOT=Path(__file__).resolve().parents[2]
out=ROOT/'analysis/studio_life_20261105/huanhon'
p=out/'formula.txt'
rows=[l.split(b'\t') for l in p.read_bytes().splitlines()][1:]
values=[]
material_rows=[r.split(b'\t') for r in (ROOT/'PhongThanRuntime-Loose/Server/settings/item/001/material.txt').read_bytes().splitlines()[1:]]
names={int(r[2]):r[0].lstrip(b'#') for r in material_rows if len(r)>2 and r[2].isdigit()}
def cs(raw):return '"'+''.join('\\x%02x'%v for v in raw)+'"'
for row in rows:
    if not row or not row[0]:continue
    assert len(row)==11,row
    v=[int(x) for x in row]
    assert v[0]>0 and v[1]==3 and v[4]>0 and 0<v[5]<100 and 0<v[6]<=1000000 and 0<v[7]<=v[6]
    values.append(v)
assert len({v[0] for v in values})==len(values)
h='''#ifndef PHONGTHAN_SOUL_FORMULA_DATA_H
#define PHONGTHAN_SOUL_FORMULA_DATA_H
struct PTSoulFormula {int id,genre,detail,part,skill,level,capacity,cost,colored,requiredLevel,profession;const char* name;};
static const PTSoulFormula ptSoulFormulas[]={
'''+',\n'.join('{'+','.join(map(str,v))+','+cs(names.get(v[2],b'Hoa\x6e Hon'))+'}' for v in values)+'''};
inline const PTSoulFormula* PtSoulFormula(int id){for(int i=0;i<sizeof(ptSoulFormulas)/sizeof(ptSoulFormulas[0]);++i)if(ptSoulFormulas[i].id==id)return &ptSoulFormulas[i];return 0;}
inline const PTSoulFormula* PtSoulFormulaItem(int genre,int detail,int part){for(int i=0;i<sizeof(ptSoulFormulas)/sizeof(ptSoulFormulas[0]);++i){const PTSoulFormula& f=ptSoulFormulas[i];if(f.genre==genre&&f.detail==detail&&f.part==part)return &f;}return 0;}
#endif
'''
(ROOT/'PhongThanSource/Headers/PhongThanSoulFormulaData.h').write_text(h,'ascii')
magic=[[int(v) for v in r.split(b'\t')] for r in (out/'formulamagic.txt').read_bytes().splitlines()[1:] if r]
assert all(len(r)==5 and r[2]<=r[3] and r[4]>0 for r in magic)
assert sum(r[4] for r in magic)==100
mh='''#ifndef PHONGTHAN_SOUL_MAGIC_DATA_H
#define PHONGTHAN_SOUL_MAGIC_DATA_H
struct PTSoulMagic {int id,type,minimum,maximum,weight;};
static const PTSoulMagic ptSoulMagic[]={
'''+',\n'.join('{'+','.join(map(str,r))+'}' for r in magic)+'''};
#endif
'''
(ROOT/'PhongThanSource/Headers/PhongThanSoulMagicData.h').write_text(mh,'ascii')
requirements=next(p for p in (ROOT/'PhongThanRuntime-Loose/Server/settings/item').iterdir() if p.name.encode('latin1','ignore')==bytes.fromhex('bbbdbbead0e8c7f3b1ed5f73632e747874'))
recipes=[r.split(b'\t') for r in requirements.read_bytes().splitlines()[1:] if r]
assert all(len(r)==11 and r[10]==b'100' for r in recipes)
rules='''#ifndef PHONGTHAN_SOUL_RECIPE_DATA_H
#define PHONGTHAN_SOUL_RECIPE_DATA_H
struct PTSoulRecipe {const char* equipment;const char* materials[8];int money,chance;};
static const PTSoulRecipe ptSoulRecipes[]={
'''+',\n'.join('{"'+r[0].decode()+'",{'+','.join('"'+v.decode()+'"' for v in r[1:9])+'},'+r[9].decode()+','+r[10].decode()+'}' for r in recipes)+'''};
#endif
'''
(ROOT/'PhongThanSource/Headers/PhongThanSoulRecipeData.h').write_text(rules,'ascii')
(out/'requirements-provenance.json').write_text(json.dumps(dict(path=str(requirements),sha256=hashlib.sha256(requirements.read_bytes()).hexdigest(),count=len(recipes)),indent=2),'utf8')
(out/'formula-catalog.json').write_text(json.dumps(dict(sha256=hashlib.sha256(p.read_bytes()).hexdigest(),count=len(values),formulas=values),indent=2),'utf8')
print('PASS original soul formulas:',len(values))

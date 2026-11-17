from pathlib import Path
import json,sys
root=Path(__file__).resolve().parents[1];content=root/'Deploy/ProjectContent/LifeGiaoHuan20261030'
manifest=json.loads((content/'life-manifest.json').read_text('utf8'));work=root.parent/'analysis/life_giaohuan_20261030';work.mkdir(exist_ok=True)
raw=(content/'compiled/Server/script/phongthan/life/skills.lua').read_bytes()
fixture=(root/'Tests/fixtures/life-skills.lua').read_bytes()
(work/'life-test.lua').write_bytes(fixture+b'\n'+raw+b'\nTestLifeSkills()\n')
tuples=set()
for r in manifest['recipes'].values():
    for t in r['needs']+[o['item'] for o in r['outputs']]:tuples.add(tuple(t[:6]))
for n in manifest['nodes']:tuples.add((3,n['detail'],0,0,0,0))
(root/'Output/Tools/LifeRecipeTuples.inl').write_text('static const int lifeTuples[][6]={\n'+',\n'.join('{'+','.join(map(str,t))+'}' for t in sorted(tuples))+'};\n')
cases=[]
for r in manifest['recipes'].values():
    needs=r['needs']+[[0]*7]*(6-len(r['needs']))
    outputs=[o['item'] for o in r['outputs']]+[[0]*7]*(2-len(r['outputs']))
    cases.append('{%d,%d,%d,{%s},{%s}}'%(r['id'],len(r['needs']),len(r['outputs']),','.join('{'+','.join(map(str,t))+'}' for t in needs),','.join('{'+','.join(map(str,t))+'}' for t in outputs)))
(root/'Output/Tools/LifeRecipeCases.inl').write_text('struct LifeRecipeCase {int id,needCount,outCount,needs[6][7],outputs[2][7];};\nstatic const LifeRecipeCase lifeCases[]={\n'+',\n'.join(cases)+'};\n')
native=(root/'Tests/Native/QuestExchangeTests.cpp').read_bytes()
native=native.replace(b'MAX_TASK=5000',b'MAX_TASK=5400').replace(b'int main(int argc,char** argv)',b'int OriginalQuestRegression(int argc,char** argv)')
(root/'Output/Tools/LifeQuestFixture.inl').write_bytes(native)
print('Prepared actual runtime Lua4 fixture and',len(tuples),'native item identities')
bootstrap=(root/'Tests/fixtures/bronze-gate.lua').read_bytes()
smith=next(p for p in (content/'compiled/Server/script').rglob('*.lua') if b'PT_BRONZE_OLD_MAIN' in p.read_bytes())
(work/'bronze-entry-test.lua').write_bytes(bootstrap+b'\n'+smith.read_bytes()+b'\nTestBronzeGate()\n')
(work/'bronze-return-test.lua').write_bytes(bootstrap+b'\n'+(content/'compiled/Server/script/phongthan/life/bronze_return.lua').read_bytes()+b'\nTestBronzeReturn()\n')

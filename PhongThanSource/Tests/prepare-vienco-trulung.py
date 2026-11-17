from pathlib import Path
import sys,json,struct,collections,hashlib
root=Path(__file__).resolve().parents[1];sys.path.insert(0,str(root/'Tools'))
from prepare_npc_restoration import split_region,npc_records
content=root/'Deploy/ProjectContent/VienCoTruLung20261030';m=json.loads((content/'manifest.json').read_text('utf8'))
counts=collections.Counter()
for f in m['files']:
    p=content/'compiled'/f['path'];b=p.read_bytes();assert hashlib.sha256(b).hexdigest()==f['sha256']
    if '_region_s.dat' not in p.name:continue
    for record in npc_records(split_region(b)[2]):
        tid,x,y=struct.unpack_from('<iii',record)
        if tid in [126,127,128,998]:counts[tid]+=1
assert counts=={126:1,127:1,128:1,998:1},counts
src=(root/'Sources/Core/Src/KNpc.cpp').read_bytes().decode('latin1')
def body(signature):
    start=src.index(signature);end=src.index('{',start)+1;depth=1
    while depth:depth+=(src[end]=='{')-(src[end]=='}');end+=1
    return src[start:end]+'\n'
out=root/'Output/Tools';out.mkdir(parents=True,exist_ok=True)
(out/'TruLungReviveMethods.inl').write_text(''.join(body(s) for s in ['void KNpc::DoRevive()', 'void KNpc::OnRevive()', 'BOOL KNpc::WaitForFrame()']))
# Dead NPCs in VOID_REGION keep ticking even when all players leave the map.
world=(root/'Sources/Core/Src/KSubWorld.cpp').read_bytes()
assert b'm_NoneRegionNpcList.GetHead()' in world and b'Npc[pNode->m_nIndex].Activate();' in world
print('PASS staged NPC counts/hashes and native VOID-region timer routing')

"""Fix quest/portal item initialization and extract the actual methods for tests."""
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=root/'Sources/Core/Src/KItem.cpp';data=p.read_bytes()
for kind in ('Require','Magic'):
    data=data.replace(f'ZeroMemory(m_ary{kind}Attrib, sizeof(m_aryBaseAttrib))'.encode(),f'ZeroMemory(m_ary{kind}Attrib, sizeof(m_ary{kind}Attrib))'.encode())
p.write_bytes(data)
methods=[]
for typename in ('QUEST','TOWNPORTAL'):
    start=data.index(('void KItem::operator = (const KBASICPROP_'+typename+'& sData)').encode())
    body=data.index(b'{',start);depth=1;end=body+1
    while depth:
        if data[end:end+1]==b'{':depth+=1
        elif data[end:end+1]==b'}':depth-=1
        end+=1
    methods.append(data[start:end])
out=root/'Output/Tools/NpcCatalogItemOperators.inl';out.parent.mkdir(parents=True,exist_ok=True);out.write_bytes(b'\n'.join(methods))
print('Quest/portal attribute clearing uses actual array bounds; production methods extracted for regression tests.')

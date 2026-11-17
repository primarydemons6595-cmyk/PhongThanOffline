"""Avoid constructing the large KMission as a lookup key in nested Lua calls."""
import re
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=root/'Sources/Core/Src/KMissionArray.h';data=p.read_bytes()
if b'FindByMissionId(' not in data:
 marker=b'public:\r\n'
 helper=b'''public:
    T* FindByMissionId(unsigned long id)
    {
        unsigned long slot=0;
        while ((slot=this->m_UseIdx.GetNext(slot))!=0)
            if (this->m_Data[slot].GetMissionId()==id) return &this->m_Data[slot];
        return NULL;
    }
'''.replace(b'\n',b'\r\n')
 if marker not in data:marker=b'public:\n';helper=helper.replace(b'\r\n',b'\n')
 assert marker in data;data=data.replace(marker,helper,1);p.write_bytes(data)
p=root/'Sources/Core/Src/ScriptFuns.cpp';data=p.read_bytes()
pattern=rb'KMission\s+(\w+);\s*\1.SetMissionId\((\w+)\);\s*KMission\s*\*\s*(\w+)\s*=\s*SubWorld\[(\w+)\]\.m_MissionArray.GetData\(&\1\);'
data,count=re.subn(pattern,lambda m:b'KMission* '+m[3]+b'=SubWorld['+m[4]+b'].m_MissionArray.FindByMissionId('+m[2]+b');',data)
p.write_bytes(data);print('Removed large stack lookup keys:',count)
p=root/'Sources/Core/Src/PhongThanEventLuaCompat.inl';data=p.read_bytes()
data=re.sub(rb'KMission lookup;lookup.SetMissionId\(id\);\s*KMission \*mission=SubWorld\[world\].m_MissionArray.GetData\(&lookup\);',b'KMission *mission=SubWorld[world].m_MissionArray.FindByMissionId(id);',data)
p.write_bytes(data)
# Test hosts use the same scalar lookup API, with their own small adapter.
p=root/'Tests/Native/EventLuaCompatTests.cpp';data=p.read_bytes()
if b'FindByMissionId' not in data:
 print('Review EventLuaCompatTests adapter before running legacy event tests.')
assert not re.search(rb'KMission\s+\w+;', (root/'Sources/Core/Src/ScriptFuns.cpp').read_bytes())

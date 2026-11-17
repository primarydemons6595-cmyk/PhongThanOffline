from restore_original_world import ROOT
p=ROOT/'PhongThanSource/Sources/Core/Src/ScriptFuns.cpp'
data=p.read_bytes()
anchor=b'#include "PhongThanXichTungTuService.inl"'
if b'#include "PhongThanEscort.inl"' not in data:
    assert anchor in data;data=data.replace(anchor,anchor+b'\r\n#include "PhongThanEscort.inl"',1)
if b'{"EventEscortAction",' not in data:
    anchor=b'{"EventEnterVanTien",'
    at=data.index(anchor);line=data.rfind(b'\n',0,at)+1
    data=data[:line]+b'\t{"EventEscortAction", LuaEventEscortAction},\r\n\t{"EventEscortInfo", LuaEventEscortInfo},\r\n'+data[line:]
p.write_bytes(data)
p=ROOT/'PhongThanSource/Sources/Core/Src/CoreServerShell.cpp';data=p.read_bytes()
if b'#include "PhongThanEscort.h"' not in data:data=data.replace(b'#include "PhongThanEventControl.inl"',b'#include "PhongThanEventControl.inl"\r\n#include "PhongThanEscort.h"',1)
if b'\tPhongThanEscortTick();' not in data:data=data.replace(b'\tPhongThanEventControlTick();',b'\tPhongThanEventControlTick();\r\n\tPhongThanEscortTick();',1)
p.write_bytes(data)
print('Escort APIs and world-thread tick wired; no runtime files modified')

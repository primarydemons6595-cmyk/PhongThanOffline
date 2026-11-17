"""Byte-preserving native wiring; keeps the restored drop algorithm unchanged."""
from restore_original_world import ROOT
core=ROOT/'PhongThanSource/Sources/Core/Src'
p=core/'CoreServerShell.cpp';data=p.read_bytes()
inc=b'#include "PhongThanEventControl.inl"'
if inc not in data:data=data.replace(b'#include "PhongThanOriginalSchedule.inl"',inc+b'\r\n#include "PhongThanOriginalSchedule.inl"',1)
if b'\tPhongThanEventControlTick();' not in data:data=data.replace(b'\tPhongThanOriginalScheduleTick();',b'\tPhongThanEventControlTick();\r\n\tPhongThanOriginalScheduleTick();',1)
p.write_bytes(data)
p=core/'PhongThanOriginalSchedule.inl';data=p.read_bytes()
if b'PhongThanEventOwnsSchedule(row.Path)' not in data:data=data.replace(b'        if (row.Hour != hour || row.Minute != minute) continue;',b'        if (row.Hour != hour || row.Minute != minute) continue;\r\n        if (PhongThanEventOwnsSchedule(row.Path)) continue;',1)
p.write_bytes(data)
p=core/'ScriptFuns.cpp';data=p.read_bytes()
inc=b'#include "PhongThanEventControl.h"'
if inc not in data:data=data.replace(b'#include "KCore.h"',b'#include "KCore.h"\r\n'+inc,1)
if b'int LuaEventServiceOwns(' not in data:
 code=b'''int LuaEventServiceOwns(Lua_State* L){
#ifdef _SERVER
    Lua_PushNumber(L,PhongThanEventOwns((int)Lua_ValueToNumber(L,1))?1:0);
#else
    Lua_PushNumber(L,0);
#endif
    return 1;
}
int LuaEventServiceAccepting(Lua_State* L){
#ifdef _SERVER
    Lua_PushNumber(L,PhongThanEventAccepting((int)Lua_ValueToNumber(L,1))?1:0);
#else
    Lua_PushNumber(L,0);
#endif
    return 1;
}
int LuaEventServiceValue(Lua_State* L){
#ifdef _SERVER
    Lua_PushNumber(L,PhongThanEventValue((int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2)));
#else
    Lua_PushNumber(L,-1);
#endif
    return 1;
}

'''.replace(b'\n',b'\r\n')
 data=data.replace(b'int LuaShowMissionCountdown(',code+b'int LuaShowMissionCountdown(',1)
 data=data.replace(b'{"ShowMissionCountdown",',b'{"EventServiceOwns",LuaEventServiceOwns},\r\n\t{"EventServiceAccepting",LuaEventServiceAccepting},\r\n\t{"EventServiceValue",LuaEventServiceValue},\r\n\t{"ShowMissionCountdown",',1)
p.write_bytes(data)
p=core/'PhongThanWorldScriptCompat.inl';data=p.read_bytes()
if b'PhongThanEventBindNpc(result)' not in data:
 data=data.replace(b'            result = NpcSet.Add(MAKELONG(level, id), world, x, y, barrier);',b'            result = NpcSet.Add(MAKELONG(level, id), world, x, y, barrier);\r\n#ifdef _SERVER\r\n            if(result>0) PhongThanEventBindNpc(result);\r\n#endif',1)
p.write_bytes(data)
p=core/'LegacyDropRuntime.cpp';data=p.read_bytes()
if b'#include "PhongThanEventControl.h"' not in data:data=data.replace(b'#include "LegacyDropRuntime.h"',b'#include "LegacyDropRuntime.h"\r\n#include "PhongThanEventControl.h"',1)
if b'PhongThanEventApplyDropProfile(index' not in data:
 needle=b'    if(c.profile==PROFILE_UNBOUND)return Error(death,"PROFILE_UNBOUND");'
 assert needle in data
 data=data.replace(needle,b'''    // Original NPC/table quarantine has already passed above. Event profiles
    // replace only the immutable per-entity slot filenames, never RNG or rules.
    if(PhongThanEventApplyDropProfile(index,npc.m_dwID,snap.slots)){
        Fields event;for(int es=0;es<4;++es)event[std::string("slot_")+Number(es)]=snap.slots[es];
        if(!logger.Write("EVENT_PINNED_DROP_PROFILE",death,event))return false;
    }
'''.replace(b'\n',b'\r\n')+needle,1)
 needle=b'if(!Resolve(snap,slot,path)||!loader.Load(path,bytes,error))'
 assert needle in data
 data=data.replace(needle,b'bool eventHandled=false;bool loaded=Resolve(snap,slot,path);if(loaded){loaded=PhongThanEventLoadTable(path,bytes,error,eventHandled);if(!eventHandled)loaded=loader.Load(path,bytes,error);}\r\n        if(!loaded)',1)
p.write_bytes(data)
p=core/'ScriptFuns.cpp';data=p.read_bytes()
if b'#include "PhongThanEventTicket.inl"' not in data:
 data=data.replace(b'int LuaEventServiceOwns(',b'#include "PhongThanEventTicket.inl"\r\nint LuaEventServiceOwns(',1)
 data=data.replace(b'{"EventServiceOwns",',b'{"EventEnterVanTien",LuaEventEnterVanTien},\r\n\t{"EventServiceOwns",',1)
p.write_bytes(data)
print('Native event world-thread bridge wired; baseline drop parser/RNG/generation unchanged.')

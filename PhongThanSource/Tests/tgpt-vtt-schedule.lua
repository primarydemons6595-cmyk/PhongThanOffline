clock=20000*86400-7*3600
SubWorld=99
opens={0,0,0,0}
managed=0
function SystemTime() return clock end
function EventServiceOwns(id) return managed end
function AddNews(...) end
function AddGlobalCountNews(...) end
function GetGlobalValue(...) return 0 end
function SubWorldID2Idx(id) return id-66 end
function IsMission(id) return 0 end
function OpenMission(id)
 assert(SubWorld==id)
 local minute=mod(floor(clock/60)+420,1440)
 local hour=floor(minute/60)
 assert((hour==10 or hour==13 or hour==18 or hour==21 or hour==23) and mod(minute,60)==(id-1)*5)
 opens[id]=opens[id]+1
end
Include("script/servertimer.lua")
for minute=0,1439 do clock=20000*86400-7*3600+minute*60;main();main();assert(SubWorld==99) end
for id=1,4 do assert(opens[id]==5) end
managed=1
for minute=0,1439 do clock=20001*86400-7*3600+minute*60;main() end
for id=1,4 do assert(opens[id]==5) end
print("PASS Lua 4: UTC+7 full-day schedule, 20 staggered openings, no 02h event, per-minute dedupe, managed-service ownership and SubWorld restoration")

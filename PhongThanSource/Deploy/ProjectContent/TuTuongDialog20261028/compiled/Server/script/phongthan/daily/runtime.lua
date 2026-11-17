Include("\\script\\phongthan\\daily\\reward.lua")
Include("\\script\\phongthan\\daily\\data.lua")
PT_DAILY_OWNED={}
PT_DAILY_SEEN={}
function PTDailyBase(kind) return 4380+(kind-1)*8 end
function PTDailyGet(kind,field) return GetTask(PTDailyBase(kind)+field) end
function PTDailySet(kind,field,value) SetTask(PTDailyBase(kind)+field,value) end
function PTDailyDay() return floor((SystemTime()+25200)/86400) end
function PTDailyNear(map,x,y)
 local m,px,py=DailyWorldPos()
 if m~=map then return nil end
 px=px*32;py=py*32
 return (px-x)*(px-x)+(py-y)*(py-y)<=400*400
end
function PTDailyCanStart(kind,level,limit)
 if GetLevel()<level or PTDailyGet(kind,0)~=0 then return nil end
 local count=PTDailyGet(kind,1)==PTDailyDay() and PTDailyGet(kind,2) or 0
 if count>=limit then return nil end
 return 1
end
function PTDailyStart(kind,needs,rewards,fee)
 local base=PTDailyBase(kind)
 if DailyExchange(base,0,1,needs,rewards,0,fee,0)~=1 then return nil end
 local day=PTDailyDay()
 if PTDailyGet(kind,1)~=day then PTDailySet(kind,1,day);PTDailySet(kind,2,0) end
 PTDailySet(kind,2,PTDailyGet(kind,2)+1)
 PTDailySet(kind,3,SystemTime()+1800);PTDailySet(kind,4,0);PTDailySet(kind,5,0)
 return 1
end
function PTDailyChooseTarget(level)
 local best={}
 for i=1,getn(PT_DAILY_DATA.targets) do
  local p=PT_DAILY_DATA.targets[i]
  if p.level>=level-20 and p.level<=level+5 then tinsert(best,i) end
 end
 if getn(best)==0 then for i=1,getn(PT_DAILY_DATA.targets) do tinsert(best,i) end end
 return best[random(1,getn(best))]
end
function PTDailyCleanup(kind)
 local owner=GetName();local list=PT_DAILY_OWNED[owner] or {};local keep={}
 for i=1,getn(list) do local n=list[i]
  if n.kind==kind then if GetNpcID(n.index)==n.serial and GetNpcLife(n.index)>0 then DelNpc(n.index) end else tinsert(keep,n) end
 end
 PT_DAILY_OWNED[owner]=keep
end
function PTDailySpawn(kind,tid,map,x,y,name,life)
 local n,id=DailySpawn(tid,map,x,y,GetLevel(),life,name)
 if n>0 then
  local owner=GetName();if not PT_DAILY_OWNED[owner] then PT_DAILY_OWNED[owner]={} end
  tinsert(PT_DAILY_OWNED[owner],{kind=kind,index=n,serial=id})
  SetNpcTimeout(n,1800*18)
 end
 return n,id
end
function PTDailyOwned(kind,n,id)
 local list=PT_DAILY_OWNED[GetName()] or {}
 for i=1,getn(list) do if list[i].kind==kind and list[i].index==n and list[i].serial==id then return 1 end end
 return nil
end
function PTDailyFinish(kind,rewards,xp,money)
 if PTDailyGet(kind,0)~=2 or PTDailyGet(kind,3)<SystemTime() then return nil end
 if DailyExchange(PTDailyBase(kind),2,0,{},rewards,xp,0,money)~=1 then return nil end
 PTDailyCleanup(kind);DailyJournal(kind,0,"§· hoµn thµnh nhiÖm vô.")
 PTDailyHope(kind,PTDailyNextReceipt(kind));return 1
end
function PTDailyCancel(kind)
 if PTDailyGet(kind,0)~=0 then PTDailySet(kind,0,0);PTDailyCleanup(kind);DailyJournal(kind,0,"§· hñy nhiÖm vô.") end
end
function PTDailyJournal(kind)
 local phase=PTDailyGet(kind,0)
 if kind==2 then
  local p=PT_DAILY_DATA.targets[PTDailyGet(2,6)]
  if p then DailyJournal(2,phase,"Dïng <color=yellow>Chiªu Hån Ph­ín<color> t¹i "..p.mapname.." ("..floor(p.x/256)..","..floor(p.y/512).."). Dô <color=red>"..p.name.."<color> ®Õn gÇn <color=green>Chiªu Hån trËn<color> råi h¹ qu¸i: "..PTDailyGet(2,4).."/50. TrËn tån t¹i 300 gi©y. §ñ sè l­îng vÒ <color=green>¢n Hång<color>, Phong ThÇn §µi (190,199).") end
 elseif kind==5 then
  local family=PTDailyGet(5,6);local stage=PTDailyGet(5,5)+1
  if family>=1 and family<=4 then
   if stage<=5 then local p=PT_DAILY_DATA.floors[family][stage]
    DailyJournal(5,phase,"Tø T­îng Linh Tª: v­ît ¶i "..stage.."/5 t¹i "..p.mapname.." ("..floor(p.x/256)..","..floor(p.y/512).."). §¸nh <color=red>"..p.name.."<color>: "..PTDailyGet(5,4).."/20.")
   else DailyJournal(5,phase,"§· gi¶i cøu Tø Tinh. VÒ <color=green>ThÇy T­íng Sè<color> t¹i TriÒu Ca hoÆc T©y Kú phôc mÖnh.") end
  end
 elseif kind==10 then
  local p=PT_DAILY_DATA.targets[PTDailyGet(10,6)]
  if phase==1 and p then DailyJournal(10,1,"Thiªn C­¬ng Hån: ®¸nh <color=red>"..p.name.."<color> t¹i "..p.mapname.." ("..floor(p.x/256)..","..floor(p.y/512)..") ®Ó dô Thiªn C­¬ng ¶nh. H¹ ®ñ 5 ¶nh råi h¹ <color=red>Thiªn C­¬ng Tinh<color>.")
  elseif phase==2 then DailyJournal(10,2,"Mang <color=yellow>Thiªn C­¬ng Hån<color> vÒ <color=green>¢n Giao<color>, Phong ThÇn §µi (184,193), nhËn th­ëng.") end
 end
end
function pt_daily_tick()
 DailyHopeFlush()
 for _,kind in {2,5,10} do
  local phase=PTDailyGet(kind,0)
  if phase>0 and PTDailyGet(kind,3)<SystemTime() then PTDailyCancel(kind) end
  if PTDailyGet(kind,0)==0 then PTDailyCleanup(kind) end
 end
 if PTDailyGet(5,0)==2 and PTDailyGet(5,5)==5 then
  local family=PTDailyGet(5,6);local m=DailyWorldPos()
  if family>=1 and family<=4 and m==PT_DAILY_DATA.floors[family][5].map then
   if PTDailyGet(5,7)==1020 then NewWorld(20,175*8,188*16) else NewWorld(21,214*8,195*16) end
  end
 end
 if PTDailyGet(10,0)==1 and PTDailyGet(10,5)==2 then
  local target=PT_DAILY_DATA.targets[PTDailyGet(10,6)];local m,x,y=DailyWorldPos()
  if target and m==target.map then local n=PTDailySpawn(10,572,m,x*32+96,y*32+96,"Thi?n C??ng Tinh",GetLevel()*5000);if n>0 then PTDailySet(10,5,3) end end
 end
 if PTDailyGet(5,0)==1 then
  local family=PTDailyGet(5,6);local stage=PTDailyGet(5,5)+1
  if family>=1 and family<=4 and stage<=5 then
   local p=PT_DAILY_DATA.floors[family][stage];local m=DailyWorldPos()
   if stage>1 and m==PT_DAILY_DATA.floors[family][stage-1].map then
    NewWorld(p.map-1000,floor(p.x/32),floor(p.y/32))
   end
   if m==p.map then
    local owner=GetName();local list=PT_DAILY_OWNED[owner] or {};local alive=0
    for i=1,getn(list) do local n=list[i];if n.kind==5 and GetNpcID(n.index)==n.serial and GetNpcLife(n.index)>0 then alive=alive+1 end end
    local need=20-PTDailyGet(5,4)-alive
    for i=1,need do local at=p.points[mod(i+alive-1,getn(p.points))+1];PTDailySpawn(5,p.tid,p.map,at[1],at[2],p.name,GetLevel()*500) end
   end
  end
 end
end
function pt_daily_kill(n,id,tid,map,inSoulCircle)
 if PTDailyGet(2,0)~=1 and PTDailyGet(5,0)~=1 and PTDailyGet(10,0)~=1 then return end
 if PT_DAILY_SEEN[n]==id then return end
 PT_DAILY_SEEN[n]=id
 if PTDailyGet(2,0)==1 and PTDailyGet(2,3)>=SystemTime() and inSoulCircle==1 then
  local p=PT_DAILY_DATA.targets[PTDailyGet(2,6)]
  if p and p.tid==tid and p.map==map then
   PTDailySet(2,4,PTDailyGet(2,4)+1);if PTDailyGet(2,4)>=50 then PTDailySet(2,0,2) end
   PTDailyJournal(2)
  end
 end
 if PTDailyGet(5,0)==1 and PTDailyGet(5,3)>=SystemTime() then
  local family=PTDailyGet(5,6);local stage=PTDailyGet(5,5)+1
  if family>=1 and family<=4 and stage<=5 then local p=PT_DAILY_DATA.floors[family][stage]
   if p.tid==tid and p.map==map then
    PTDailySet(5,4,PTDailyGet(5,4)+1)
    if PTDailyGet(5,4)>=20 then
     PTDailyCleanup(5);PTDailySet(5,4,0);PTDailySet(5,5,stage)
     AddIBBuff(300+family*5+stage,600)
     if stage<5 then AddOwnExp(GetLevel()*stage*1000) end
     if stage==5 then PTDailySet(5,0,2) end
    end
    PTDailyJournal(5)
   end
  end
 end
 if PTDailyGet(10,0)==1 and PTDailyGet(10,3)>=SystemTime() then
  local p=PT_DAILY_DATA.targets[PTDailyGet(10,6)]
  if not p or map~=p.map then return end
  local stage=PTDailyGet(10,5)
  if stage==0 and tid==p.tid then
   local m,x,y=DailyWorldPos();local spawned=0
   for i=1,5 do local a,b=PTDailySpawn(10,571,map,x*32+(i-3)*64,y*32+96,"Thiªn C­¬ng ¶nh",GetLevel()*1200);if a>0 then spawned=spawned+1 end end
   if spawned==5 then PTDailySet(10,5,1);PTDailySet(10,4,0) else PTDailyCleanup(10) end
  elseif stage==1 and tid==571 and PTDailyOwned(10,n,id) then
   PTDailySet(10,4,PTDailyGet(10,4)+1)
   if PTDailyGet(10,4)>=5 then PTDailySet(10,5,2) end
  elseif stage==3 and tid==572 and PTDailyOwned(10,n,id) then
   if DailyExchange(PTDailyBase(10),1,2,{},{{3,133,0,0,0,0,1}},0,0,0)==1 then PTDailyCleanup(10);PTDailyJournal(10) end
  end
  if PTDailyGet(10,5)==2 then local m,x,y=DailyWorldPos();local a,b=PTDailySpawn(10,572,map,x*32+96,y*32+96,"Thiªn C­¬ng Tinh",GetLevel()*5000);if a>0 then PTDailySet(10,5,3) end end
 end
end

Include("\\script\\phongthan\\restoration\\tutuong.lua")
PTTT_OLD_KILL=pt_daily_kill
function pt_daily_kill(npc,entity,template,map,near)
 PTTTKill(npc,entity,template,map)
 if PTTT_OLD_KILL then PTTT_OLD_KILL(npc,entity,template,map,near) end
end

Include("\\script\\phongthan\\restoration\\trial.lua")
PTTR_OLD_KILL=pt_daily_kill
function pt_daily_kill(npc,entity,template,map,near)
 PTTrialKill(npc,entity,template,map)
 if PTTR_OLD_KILL then PTTR_OLD_KILL(npc,entity,template,map,near) end
end

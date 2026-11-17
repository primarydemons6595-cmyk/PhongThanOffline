Include("\\script\\phongthan\\daily\\runtime.lua")
function pt_daily_tu_linh()
 if GetLevel()<65 then Talk(1,"no",11940);return end
 local phase=PTDailyGet(5,0)
 if phase>0 and PTDailyGet(5,3)<SystemTime() then PTDailyCancel(5);phase=0 end
 if phase==0 then SayTask(11940,{{"Më Linh Tª m«n","pt_daily_tu_take";show=1}})
 elseif phase==2 then SayTask(11941,{{"NhËn th­ëng","pt_daily_tu_claim";show=1}})
 else PTDailyJournal(5);SayTask(11943,{{"§Õn kh¶o nghiÖm","pt_daily_tu_go";show=1},{"Hñy bá","pt_daily_tu_cancel";show=1}}) end
end
function PTDailySeerNear() return PTDailyNear(1021,54912,100096) or PTDailyNear(1020,44672,96512) end
function pt_daily_tu_take()
 if not PTDailyAcceptReady(5,65,PTDailySeerNear()) then return end
 local fee=50000+floor((GetLevel()-65)/10)*50000
 if HaveNormalItem(3,82,0,0)<1 then PTDailyNotice("Më Linh Tª m«n cÇn 1 S¬n th¹ch trong hµnh trang. H·y chuÈn bÞ råi quay l¹i.");return end
 if GetCash()<fee then PTDailyNotice("Më Linh Tª m«n cÇn "..fee.." l­îng. B¹n ch­a ®ñ tiÒn.");return end
 if PTDailyStart(5,{{3,82,0,0,0,0,1}},{},50000+floor((GetLevel()-65)/10)*50000) then
  local origin=DailyWorldPos();PTDailySet(5,7,origin);PTDailySet(5,6,random(1,4));PTDailySet(5,3,SystemTime()+1800);PTDailyJournal(5);pt_daily_tu_go()
 else PTDailyNotice("Ch­a thÓ nhËn Tø T­îng Linh Tª. KiÓm tra S¬n th¹ch trong tói chÝnh, tiÒn vµ khãa vËt phÈm; ch­a trõ l­ît nhËn.")
 end
end
function pt_daily_tu_go()
 if not PTDailySeerNear() or PTDailyGet(5,0)~=1 then return end
 local family=PTDailyGet(5,6);local stage=PTDailyGet(5,5)+1
 if family>=1 and family<=4 and stage<=5 then local p=PT_DAILY_DATA.floors[family][stage];NewWorld(p.map-1000,floor(p.x/32),floor(p.y/32));CloseDialog() end
end
function pt_daily_tu_claim()
 if PTDailySeerNear() and PTDailyGet(5,5)==5 and PTDailyFinish(5,{},GetLevel()*6000,0) then Talk(1,"no",11941) end
end
function pt_daily_tu_cancel() if PTDailySeerNear() then PTDailyCancel(5);CloseDialog() end end

Include("\\script\\phongthan\\daily\\runtime.lua")
function PTDailySeerNear() return PTDailyNear(1021,54912,100096) or PTDailyNear(1020,44672,96512) end
function pt_daily_tu_linh()
 if GetLevel()<65 then PTDailyNotice("CÇn cÊp 65 ®Ó nhËn Tø T­îng Linh Tª.");return end
 local phase=PTDailyGet(5,0)
 if phase==0 then
  local runs=0
  if PTDailyGet(5,1)==PTDailyDay() then runs=PTDailyGet(5,2) end
  local fee=50000+floor((GetLevel()-65)/10)*50000
  local text="§· nhËn h«m nay: <color=red>"..runs.."/6 l­ît<color>. Mçi l­ît cÇn <color=green>1 Tha S¬n Th¹ch<color> vµ <color=yellow>"..fee.." l­îng<color>.<enter>L­ît <color=red>®Çu tiªn<color> kh«ng cÇn vĞ. <color=red>5 l­ît tiÕp theo<color>: tù dïng <color=green>1 Ch×a Khãa Linh Tª<color> trong tói F4; kh«ng cã ch×a khãa dïng ®­îc sÏ trõ <color=yellow>4 tiÒn ®ång<color>.<enter>Thêi gian: 30 phót. Hoµn thµnh 5 tÇng cã 10% nhËn thªm <color=green>L©m Tiªn Lé<color>."
  SayButtons(text,1,"NhËn nhiÖm vô/pt_daily_tu_take")
 else
  local options={}
  if PTDailyGet(5,5)>0 then tinsert(options,{"NhËn th­ëng c¸c tÇng ®· v­ît","pt_daily_tu_claim";show=1}) end
  if phase==1 and PTDailyGet(5,3)>=SystemTime() then tinsert(options,{"TiÕp tôc kh¶o nghiÖm","pt_daily_tu_go";show=1}) end
  tinsert(options,{"Hñy l­ît hiÖn t¹i","pt_daily_tu_cancel";show=1})
  SayTask(11943,options)
 end
end
function pt_daily_tu_take()
 local result=TuTuongService(1,0)
 if result==1 then PTDailyJournal(5);pt_daily_tu_go()
 else PTDailyNotice("Ch­a thÓ nhËn: kiÓm tra cÊp 65, giíi h¹n 6 l­ît, nhiÖm vô cò, Tha S¬n Th¹ch, tiÒn l­îng vµ vĞ hoÆc 4 tiÒn ®ång trong tói. Kh«ng trõ l­ît khi nhËn thÊt b¹i.") end
end
function pt_daily_tu_first() pt_daily_tu_take() end
function pt_daily_tu_key() pt_daily_tu_take() end
function pt_daily_tu_coin() pt_daily_tu_take() end
function pt_daily_tu_go()
 if not PTDailySeerNear() or PTDailyGet(5,0)~=1 or PTDailyGet(5,3)<SystemTime() then return end
 local family=PTDailyGet(5,6);local stage=PTDailyGet(5,5)+1
 if family>=1 and family<=4 and stage<=5 then local p=PT_DAILY_DATA.floors[family][stage];NewWorld(p.map-1000,floor(p.x/32),floor(p.y/32));CloseDialog() end
end
function pt_daily_tu_claim()
 local full=PTDailyGet(5,5)==5
 local result=TuTuongService(2,0)
 if result>0 then
  PTDailyCleanup(5);DailyJournal(5,0,"§· nhËn th­ëng Tø T­îng Linh Tª.")
  if full then PTDailyHope(5,PTDailyNextReceipt(5)) end
  PTDailyNotice("§· nhËn "..result.." kinh nghiÖm cho c¸c tÇng ®· v­ît. Thø T­ nh©n ®«i kinh nghiÖm.")
 else PTDailyNotice("Ch­a nhËn ®­îc th­ëng. H·y ®Õn gÇn ThÇy T­íng Sè, më khãa tói vµ dµnh « trèng cho phÇn th­ëng.") end
end
function pt_daily_tu_cancel() if PTDailySeerNear() then PTDailyCancel(5);CloseDialog() end end

Include("\\script\\phongthan\\daily\\runtime.lua")
function PTDailySeerNear() return PTDailyNear(1021,54912,100096) or PTDailyNear(1020,44672,96512) end
function pt_daily_tu_linh()
 if GetLevel()<65 then PTDailyNotice("CÇn cÊp 65 ®Ó nhËn Tø T­îng Linh Tª.");return end
 local phase=PTDailyGet(5,0)
 if phase==0 then
  SayButtons("Mçi ngµy 6 l­ît. Mçi l­ît cÇn 1 Tha S¬n Th¹ch vµ tiÒn theo cÊp. L­ît ®Çu kh«ng cÇn vĞ; 5 l­ît sau dïng 1 Ch×a Khãa Linh Tª hoÆc 4 tiÒn ®ång. Thêi gian hiÖn t¹i: 30 phót. V­ît ®ñ 5 tÇng cã 10% nhËn thªm L©m Tiªn Lé.",3,"NhËn l­ît ®Çu/pt_daily_tu_first","Dïng ch×a khãa/pt_daily_tu_key","Dïng 4 tiÒn ®ång/pt_daily_tu_coin")
 else
  local options={}
  if PTDailyGet(5,5)>0 then tinsert(options,{"NhËn th­ëng c¸c tÇng ®· v­ît","pt_daily_tu_claim";show=1}) end
  if phase==1 and PTDailyGet(5,3)>=SystemTime() then tinsert(options,{"TiÕp tôc kh¶o nghiÖm","pt_daily_tu_go";show=1}) end
  tinsert(options,{"Hñy l­ît hiÖn t¹i","pt_daily_tu_cancel";show=1})
  SayTask(11943,options)
 end
end
function pt_daily_tu_take(method)
 local result=TuTuongService(1,method)
 if result==1 then PTDailyJournal(5);pt_daily_tu_go()
 else PTDailyNotice("Ch­a thÓ nhËn: kiÓm tra cÊp 65, giíi h¹n 6 l­ît, nhiÖm vô cò, Tha S¬n Th¹ch, tiÒn l­îng vµ vĞ hoÆc 4 tiÒn ®ång trong tói. Kh«ng trõ l­ît khi nhËn thÊt b¹i.") end
end
function pt_daily_tu_first() pt_daily_tu_take(0) end
function pt_daily_tu_key() pt_daily_tu_take(1) end
function pt_daily_tu_coin() pt_daily_tu_take(2) end
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

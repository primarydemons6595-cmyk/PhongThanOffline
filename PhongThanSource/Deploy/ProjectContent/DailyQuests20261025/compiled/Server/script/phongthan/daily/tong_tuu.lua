Include("\\script\\phongthan\\daily\\runtime.lua")
function pt_daily_tong_tuu()
 if GetLevel()<95 then Talk(1,"no",14656);return end
 local phase=DailyWineAction(0)
 if phase==0 then SayTask(14654,{{"Tèng Töu","pt_daily_wine_take";show=1}})
 elseif phase==1 then SayTask(14654,{{"Giao r­îu","pt_daily_wine_deliver";show=1},{"Hñy bá","pt_daily_wine_cancel";show=1}})
 else SayTask(14668,{{"Kinh nghiÖm","pt_daily_wine_xp";show=1},{"M¶nh Phong Ma BÝch","pt_daily_wine_item";show=1}}) end
end
function pt_daily_wine_take()
 if PTDailyNear(1020,185*256,192*512) and DailyWineAction(0)==0 then NewWorld(44,236*8,192*16);CloseDialog();return end
 if DailyWineAction(1)==1 then DailyJournal(9,1,"Tèng Töu: nhÊp xe ®Ó vµo hoÆc ra. §­a xe tíi BÝch Du Cung tÇng 5 (237,187). Ra khái xe ®¸nh qu¸i lÊy thªm r­îu, sau ®ã giao cho <color=green>BÝch Du Nh©n<color>. Thêi h¹n 20 phót.");CloseDialog() else Talk(1,"no",14667) end
end
function pt_daily_wine_deliver() if DailyWineAction(2)==1 then pt_daily_tong_tuu() else Talk(1,"no",14659) end end
function pt_daily_wine_cancel() if DailyWineAction(5)==1 then DailyJournal(9,0,"§· hñy Tèng Töu.");Talk(1,"no",14665) end end
function pt_daily_wine_xp() local before=GetIBBuffTimes(418);if DailyWineAction(3)==1 then DailyJournal(9,0,"§· hoµn thµnh Tèng Töu.");if GetIBBuffTimes(418)>before then TopMessage(14423) end;CloseDialog() end end
function pt_daily_wine_item() local before=GetIBBuffTimes(418);if DailyWineAction(4)==1 then DailyJournal(9,0,"§· hoµn thµnh Tèng Töu.");if GetIBBuffTimes(418)>before then TopMessage(14423) end;CloseDialog() end end

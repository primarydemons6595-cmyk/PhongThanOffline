Include("\\script\\phongthan\\daily\\runtime.lua")
function pt_daily_sieu_do()
 if GetLevel()<40 then Talk(1,"no",12764);return end
 local phase=PTDailyGet(2,0)
 if phase==0 then SayTask(12764,{{"Siªu §é","pt_daily_sieu_take";show=1},{"Mua ph­ín","pt_daily_sieu_buy";show=1}})
 elseif phase==2 then SayTask(12764,{{"Kinh nghiÖm","pt_daily_sieu_xp";show=1},{"Ng©n l­îng","pt_daily_sieu_money";show=1}})
 else PTDailyJournal(2);SayTask(12764,{{"TiÕn ®é","pt_daily_sieu_progress";show=1},{"Mua ph­ín","pt_daily_sieu_buy";show=1},{"Hñy bá","pt_daily_sieu_cancel";show=1}}) end
end
function pt_daily_sieu_take()
 if not PTDailyNear(1001,190*256,199*512) or GetLevel()<40 or PTDailyGet(2,0)~=0 then return end
 local needs={}
 if PTDailyGet(2,1)==PTDailyDay() and PTDailyGet(2,2)>=5 then needs={{8,257,2,0,0,0,1}} end
 if PTDailyStart(2,needs,{{6,1,187,0,0,0,1}},0) then PTDailySet(2,6,PTDailyChooseTarget(GetLevel()));PTDailySet(2,7,0);PTDailyJournal(2);Talk(1,"no",12767) end
end
function pt_daily_sieu_buy()
 if not PTDailyNear(1001,190*256,199*512) then return end
 local phase=PTDailyGet(2,0)
 if phase==0 then return end
 if DailyExchange(PTDailyBase(2),phase,phase,{},{{6,1,187,0,0,0,1}},0,10000,0)==1 then Talk(1,"no",12766) else Talk(1,"no",12768) end
end
function pt_daily_sieu_progress() PTDailyJournal(2);pt_daily_sieu_do() end
function pt_daily_sieu_cancel() PTDailyCancel(2);CloseDialog() end
function pt_daily_sieu_xp() if PTDailyNear(1001,190*256,199*512) and PTDailyFinish(2,{},GetLevel()*10000,0) then CloseDialog() end end
function pt_daily_sieu_money() if PTDailyNear(1001,190*256,199*512) and PTDailyFinish(2,{},0,GetLevel()*1000) then CloseDialog() end end
function no() CloseDialog() end

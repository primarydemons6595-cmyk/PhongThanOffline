Include("\\script\\phongthan\\daily\\runtime.lua")
function pt_daily_thien_cuong()
 if GetLevel()<75 then Talk(1,"no",12703);return end
 local phase=PTDailyGet(10,0)
 if phase==0 then SayTask(12703,{{"Thiªn C­¬ng Hån","pt_daily_cuong_take";show=1}})
 elseif phase==2 then SayTask(12708,{{"NhËn th­ëng","pt_daily_cuong_claim";show=1}})
 else PTDailyJournal(10);SayTask(12704,{{"TiÕn ®é","pt_daily_cuong_progress";show=1},{"Hñy bá","pt_daily_cuong_cancel";show=1}}) end
end
function PTDailyCuongNear() return PTDailyNear(1001,184*256,193*512) end
function pt_daily_cuong_take()
 if not PTDailyCuongNear() or not PTDailyCanStart(10,75,4) then return end
 if PTDailyStart(10,{{3,114,0,0,0,0,1},{3,115,0,0,0,0,1}},{},GetLevel()*1000) then
  local choices={}
  for i=1,getn(PT_DAILY_DATA.targets) do local p=PT_DAILY_DATA.targets[i];if p.map>=1042 and p.map<=1051 then tinsert(choices,i) end end
  if getn(choices)>0 then PTDailySet(10,6,choices[random(1,getn(choices))]);PTDailyJournal(10) else PTDailyCancel(10) end
 end
end
function pt_daily_cuong_claim()
 if not PTDailyCuongNear() or PTDailyGet(10,0)~=2 or PTDailyGet(10,3)<SystemTime() then return end
 if DailyExchange(PTDailyBase(10),2,0,{{3,133,0,0,0,0,1}},{},GetLevel()*15000,0,0)==1 then
  PTDailyHope(10,PTDailyNextReceipt(10));DailyJournal(10,0,"§· giao Thiªn C­¬ng Hån.");CloseDialog()
 end
end
function pt_daily_cuong_exchange() Talk(1,"no",12708) end
function pt_daily_cuong_progress() PTDailyJournal(10);pt_daily_thien_cuong() end
function pt_daily_cuong_cancel() if PTDailyCuongNear() then PTDailyCancel(10);Talk(1,"no",12707) end end

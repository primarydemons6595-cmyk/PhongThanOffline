Include("\\script\\phongthan\\daily\\thien_cuong.lua")
function main() return pt_daily_thien_cuong() end
function no() CloseDialog() end

function GetPlayerTaskState() if GetLevel()<75 then return 0,75 end;local phase=PTDailyGet(10,0);return phase==0 and 1 or (phase==2 and 3 or 2),75 end

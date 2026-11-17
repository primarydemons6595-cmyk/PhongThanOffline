Include("\\script\\phongthan\\daily\\sieu_do.lua")
function main() return pt_daily_sieu_do() end
function no() CloseDialog() end

function GetPlayerTaskState() if GetLevel()<40 then return 0,40 end;local phase=PTDailyGet(2,0);return phase==0 and 1 or (phase==2 and 3 or 2),40 end

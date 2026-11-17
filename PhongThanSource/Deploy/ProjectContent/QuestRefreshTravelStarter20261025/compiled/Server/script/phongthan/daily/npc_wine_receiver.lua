Include("\\script\\phongthan\\daily\\tong_tuu.lua")
function main() return pt_daily_tong_tuu() end
function no() CloseDialog() end

function GetPlayerTaskState()
 if GetLevel()<95 or GetTask(4460)==0 or GetTask(4463)<SystemTime() then return 0,95 end
 return (GetTask(4460)==2 or GetTask(4478)==1) and 3 or 2,95
end

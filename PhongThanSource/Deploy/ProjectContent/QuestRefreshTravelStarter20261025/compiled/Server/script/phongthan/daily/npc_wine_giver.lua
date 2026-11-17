Include("\\script\\phongthan\\daily\\tong_tuu.lua")
function main() return pt_daily_tong_tuu() end
function no() CloseDialog() end

function GetPlayerTaskState()
 if GetLevel()<95 then return 0,95 end
 local phase=GetTask(4460);local day=floor((SystemTime()+25200)/86400)
 local runs=GetTask(4461)==day and GetTask(4462) or 0
 if phase==2 then return 0,95 end
 return phase==1 and 2 or (runs<9 and 1 or 0),95
end

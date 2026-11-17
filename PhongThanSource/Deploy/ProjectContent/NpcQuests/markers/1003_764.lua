
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(10)==1 or GetTask(10)==3 or GetTask(10)==5 or GetTask(10)==7 then return 3,1 end
    if GetTask(50)==7 or GetTask(50)==4 or (GetTask(50)==5 and HaveEventItem(39)==1 and HaveEventItem(40)==1 and GetItemCount(41)>=2) then return 3,7 end
    return 0,0
end

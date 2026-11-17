
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(21)==7 or GetTask(21)==1 or (GetTask(21)==3 and HaveNormalItem(3,10,0,0)>=10) then return 3,3 end
    if GetTask(24)==1 then return 3,12 end
    if GetTask(25)==2 and HaveEventItem(21)>=1 and HaveEventItem(22)>=1 and HaveEventItem(23)>=1 then return 3,7 end
    if GetTask(21)==3 then return 2,3 end
    return 0,0
end

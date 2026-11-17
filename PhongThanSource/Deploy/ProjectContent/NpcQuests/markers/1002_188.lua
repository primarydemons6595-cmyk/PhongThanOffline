
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(20)==18 and HaveEventItem(26)>=1 then return 3,1 end
    if GetTask(20)==0 then return 1,1 end
    if GetTask(20)>0 and GetTask(20)<19 then return 2,1 end
    return 0,0
end

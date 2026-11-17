
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(3)==2 and HaveEventItem(11)==1 then return 3,25 end
    if GetTask(21)==4 then return 3,3 end
    if GetPlayerType()==0 and GetLevel()>=25 and GetTask(3)==0 then return 1,25 end
    if GetTask(3)==2 then return 2,25 end
    return 0,0
end

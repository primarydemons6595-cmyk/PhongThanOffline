
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(11)==1 then return 3,3 end
    if GetPlayerType()==1 and GetLevel()>=25 and GetTask(1)==0 then return 1,25 end
    return 0,0
end

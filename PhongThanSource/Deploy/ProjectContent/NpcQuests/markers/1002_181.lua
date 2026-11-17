
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(21)==6 then return 3,3 end
    if GetPlayerType()==0 and GetLevel()>=35 and GetTask(3)==10 then return 1,35 end
    return 0,0
end

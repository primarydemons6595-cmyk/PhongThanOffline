
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetPlayerType()==0 and GetTask(3)==1 then return 3,25 end
    return 0,0
end


-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(21)==2 then return 3,3 end
    return 0,0
end

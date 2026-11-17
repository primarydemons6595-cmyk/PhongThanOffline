
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(20)==10 or GetTask(20)==11 or GetTask(20)==14 or GetTask(20)==15 then return 3,1 end
    return 0,0
end

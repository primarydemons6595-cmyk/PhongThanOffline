
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(35)==2 then return 3,7 end
    if GetTask(31)==4 or GetTask(31)==2 then return 3,3 end
    if GetTask(51)==2 then return 3,7 end
    return 0,0
end

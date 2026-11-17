
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(10)==1 or GetTask(10)==5 or GetTask(10)==9 or GetTask(10)==13 then return 3,1 end
    if GetTask(15)==1 then return 3,7 end
    return 0,0
end

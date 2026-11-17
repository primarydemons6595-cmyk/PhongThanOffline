
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(10)==15 then return 3,1 end
    if GetTask(10)==0 then return 1,1 end
    if GetTask(10)>0 and GetTask(10)<16 then return 2,1 end
    return 0,0
end


-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(21)==8 then return 3,3 end
    if GetTask(21)==5 then return 3,3 end
    if GetTask(21)==0 and GetLevel()>=3 then return 1,3 end
    if GetTask(21)>0 and GetTask(21)<9 then return 2,3 end
    return 0,0
end

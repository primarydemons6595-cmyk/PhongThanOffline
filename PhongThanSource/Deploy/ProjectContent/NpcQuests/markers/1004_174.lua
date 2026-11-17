
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(31)==5 and HaveNormalItem(3,12,0,0)>=10 then return 3,3 end
    if GetTask(31)==0 and GetLevel()>=3 then return 1,3 end
    if GetTask(31)>0 and GetTask(31)<6 then return 2,3 end
    return 0,0
end

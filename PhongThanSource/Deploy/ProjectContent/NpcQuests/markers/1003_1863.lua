
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(11)==3 then return 3,3 end
    if GetTask(14)==1 and HaveNormalItem(3,9,0,0)>=10 then return 3,12 end
    if GetTask(14)==0 and GetPlayerType()==1 and GetLevel()>=12 then return 1,12 end
    if GetTask(14)==1 then return 2,12 end
    return 0,0
end

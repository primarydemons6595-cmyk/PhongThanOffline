
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetPlayerType()==1 and (GetTask(15)==6 or GetTask(15)==2) then return 3,7 end
    if GetTask(15)==0 and GetPlayerType()==1 and GetLevel()>=7 then return 1,7 end
    if GetPlayerType()==1 and GetTask(15)>0 and GetTask(15)<7 then return 2,7 end
    return 0,0
end

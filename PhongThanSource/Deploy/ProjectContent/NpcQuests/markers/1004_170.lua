
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(30)==1 or GetTask(30)==5 or GetTask(30)==9 or GetTask(30)==13 then return 3,1 end
    if GetTask(34)==6 then return 3,12 end
    if GetTask(34)==0 and GetPlayerType()==2 and GetLevel()>=12 then return 1,12 end
    if GetTask(34)>0 and GetTask(34)<7 then return 2,12 end
    return 0,0
end

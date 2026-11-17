
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(20)==10 or GetTask(20)==12 or GetTask(20)==14 or GetTask(20)==16 then return 3,1 end
    if GetTask(24)==3 and HaveEventItem(25)>=1 then return 3,12 end
    if GetTask(24)==0 and GetPlayerType()==0 and GetLevel()>=12 then return 1,12 end
    if GetTask(24)>0 and GetTask(24)<4 then return 2,12 end
    return 0,0
end

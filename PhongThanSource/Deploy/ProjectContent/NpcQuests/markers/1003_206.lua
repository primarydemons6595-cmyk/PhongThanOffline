
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(10)==1 or GetTask(10)==3 or GetTask(10)==9 or GetTask(10)==11 then return 3,1 end
    if (GetTask(11)==4 and HaveEventItem(20)>=1) or (GetTask(11)==2 and HaveNormalItem(3,13,0,0)>=10) then return 3,3 end
    if GetTask(11)==0 and GetLevel()>=3 then return 1,3 end
    if GetTask(11)>0 and GetTask(11)<5 then return 2,3 end
    return 0,0
end

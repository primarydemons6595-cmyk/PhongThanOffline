
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(20)==10 or GetTask(20)==11 or GetTask(20)==12 or GetTask(20)==13 then return 3,1 end
    if (GetTask(25)==3 and HaveEventItem(24)>=1) or (GetTask(25)==1 and HaveEventItem(21)>=1 and HaveEventItem(22)>=1 and HaveEventItem(23)>=1) then return 3,7 end
    if GetTask(25)==0 and GetPlayerType()==0 and GetLevel()>=7 then return 1,7 end
    if GetTask(25)>0 and GetTask(25)<4 then return 2,7 end
    return 0,0
end

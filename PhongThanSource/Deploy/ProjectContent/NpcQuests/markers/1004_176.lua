
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(31)==3 and HaveEventItem(29)>=1 then return 3,3 end
    if GetTask(34)==1 or (GetTask(34)==5 and HaveEventItem(30)>=1) then return 3,12 end
    if GetTask(2)==2 and HaveEventItem(15)>=1 then return 3,25 end
    if GetLevel()>=25 and GetPlayerType()==2 and GetTask(2)==0 then return 1,25 end
    if GetTask(31)==3 then return 2,3 end
    if GetTask(34)==5 then return 2,12 end
    if GetTask(2)==2 then return 2,25 end
    return 0,0
end


-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(30)==1 or GetTask(30)==3 or GetTask(30)==5 or GetTask(30)==7 then return 3,1 end
    if GetTask(35)==7 and HaveEventItem(27)>=1 then return 3,7 end
    if GetTask(35)==0 and GetPlayerType()==2 and GetLevel()>=7 then return 1,7 end
    if GetTask(35)>0 and GetTask(35)<8 then return 2,7 end
    return 0,0
end

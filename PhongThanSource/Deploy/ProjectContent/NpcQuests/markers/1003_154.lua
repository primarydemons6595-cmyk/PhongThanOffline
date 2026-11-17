
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetLevel()>=20 and GetLevel()<=50 and GetTask(897)~=0 and GetTask(898)==0 then return 3,20 end
    if GetLevel()<=50 and GetTask(897)==0 then return 1,20 end
    if GetTask(897)~=0 and GetTask(898)>0 then return 2,20 end
    return 0,0
end

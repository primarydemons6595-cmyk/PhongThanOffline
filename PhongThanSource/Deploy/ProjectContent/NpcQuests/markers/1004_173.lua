
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetTask(35)==1 or (GetTask(35)==6 and GetItemCount(28)>=3) then return 3,7 end
    if GetTask(35)==6 then return 2,7 end
    return 0,0
end

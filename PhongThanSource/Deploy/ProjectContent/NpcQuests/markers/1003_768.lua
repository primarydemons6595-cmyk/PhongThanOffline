
-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    local ks,kl=PTK_State()
    if ks==3 then return ks,kl end
    if GetTask(13)==1 and HaveNormalItem(3,9,0,0)>=5 then return 3,1 end
    if GetTask(13)==0 then return 1,1 end
    if GetTask(13)==1 then return 2,1 end
    return ks,kl
end

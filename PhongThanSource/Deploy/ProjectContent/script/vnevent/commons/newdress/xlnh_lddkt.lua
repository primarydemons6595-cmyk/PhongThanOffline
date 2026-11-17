-- Rebuilt from original \u70c8\u7130\u7194\u91d1\u88c57.lua and item 5757/2.
-- Reusable dress. Generation time is persisted by GetIBItemGenTime.
function main(itemID)
    local genre, detail, part = FindItemEx(itemID)
    if genre ~= 8 or detail ~= 5757 or part ~= 2 then return end
    local templates = {{1934,1935},{1934,1935},{1934,1935}}
    local sex = GetSex()
    local job = GetPlayerType()
    if sex < 0 or sex > 1 or job < 0 or job > 2 then return end
    local target = templates[job + 1][sex + 1]
    local morph = GetMorphType()
    if morph == 364 or IsPlayerInsideWeapon(PlayerIndex) > 0 then
        Msg2Player("Trang thai hien tai khong the doi ngoai trang.")
        return
    end
    if morph == target then
        PolyMorph(-1, 0, 0, 0, 0)
        return
    end
    local generated = GetIBItemGenTime(itemID)
    local remaining = generated + 604800 - SystemTime()
    if generated <= 0 or remaining <= 0 then
        Msg2Player("Ngoai trang da het han su dung.")
        return
    end
    if remaining > 43200 then remaining = 43200 end
    PolyMorph(target, 1, 0, -1, remaining)
end

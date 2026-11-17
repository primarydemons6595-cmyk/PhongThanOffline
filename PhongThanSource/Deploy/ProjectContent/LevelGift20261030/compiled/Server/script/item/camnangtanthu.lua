-- Project beginner gift, Lua 4. Reserved tasks: 4700 state, 4701 career+1,
-- 4702 delivered gear/potion units; legacy 4703 Xu marker retained, never paid here.
-- Existing 4700=2 characters remain completed. Existing partial progress is retained.
-- GetSeries: 0=Giap Si, 1=Dao Si, 2=Di Nhan. Item Series is a separate
-- table field (1/6/11 for these quest sets), NOT the player's career.
PT_STARTER_TASK = 4700
PT_STARTER_CAREER_TASK = 4701
PT_STARTER_PROGRESS_TASK = 4702
PT_STARTER_BAG_ID = 9173

function GetDesc(nItemIdx)
    return "<color=yellow>Tói Quµ T©n Thñ<color><enter>NhËn trùc tiÕp t¹i LÔ Quan tõ cÊp 10, mçi nh©n vËt mét lÇn: bé nhiÖm vô Cù §Çu / V©n Trung / KhuyÓn V¨n (¸o cÊp 20), vò khÝ 1x nhiÖm vô ®óng hÖ, thó c­ìi Hoµng Kim, 100 TiÓu Hång §¬n, 100 TiÓu Hoµn §¬n vµ ThÎ Quµ: Th¨ng CÊp NhËn Th­ëng. Gi¸p SÜ nhËn c¶ vò khÝ ng¾n vµ dµi."
end

function PTStarterCheck()
    if GetTask(PT_STARTER_TASK) == 2 then
        Msg2Player("B¹n ®· nhËn hÕt quµ t©n thñ.")
        return 0
    end
    if NearLeQuan() ~= 1 then
        Msg2Player("H·y ®Õn LÔ Quan t¹i T©n Thñ Th«n ®Ó nhËn quµ trùc tiÕp.")
        return 0
    end
    if GetLevel() < 10 then
        Msg2Player("CÇn ®¹t cÊp 10 ®Ó nhËn quµ t©n thñ t¹i LÔ Quan.")
        return 0
    end
    if GetTrade() ~= 0 or GetLockState() ~= 0 then
        Msg2Player("H·y kÕt thóc giao dÞch vµ më khãa nh©n vËt tr­íc khi nhËn quµ.")
        return 0
    end
    local career = GetSeries()
    if career ~= 0 and career ~= 1 and career ~= 2 then
        Msg2Player("HÖ ph¸i kh«ng hîp lÖ ®Ó nhËn quµ t©n thñ.")
        return 0
    end
    local saved = GetTask(PT_STARTER_CAREER_TASK)
    if saved ~= 0 and saved ~= career + 1 then
        Msg2Player("Quµ ®· ®­îc chèt theo hÖ ph¸i lóc b¾t ®Çu nhËn. H·y liªn hÖ GM.")
        return 0
    end
    return 1
end

-- Row: genre, detail, particular, template level, item series, luck, count.
-- Template level 1 is NOT player level 1: requirements come from item tables.
function PTStarterRewards(career)
    local rewards = {}
    local details = {2, 7, 6, 5, 9}
    local itemSeries = 1 + career * 5
    local i = 0
    for i = 1, getn(details) do
        tinsert(rewards, {0, details[i], 6 + career, 1, itemSeries, 0, 1})
    end
    if career == 0 then
        tinsert(rewards, {0, 0, 27, 1, 5, 0, 1}) -- Tram Tuong dao (N.v), ngan
        tinsert(rewards, {0, 0, 28, 1, 5, 0, 1}) -- Tam Tiem dao (N.v), dai
    else
        tinsert(rewards, {0, 0, 28 + career, 1, 5, 0, 1})
    end
    -- VNG 1000 is a template marker. Current generator resolves it to the
    -- requested valid series or series_nil=5; passing 1000 fails validation.
    tinsert(rewards, {0, 10, 24 + career, 1, 5, 0, 1}) -- Trac Ma/Tuoc/Diep
    tinsert(rewards, {1, 0, 0, 1, 0, 0, 100}) -- Tieu Hong Don
    tinsert(rewards, {1, 3, 0, 1, 0, 0, 100}) -- Tieu Hoan Don
    return rewards
end

function main(nItemIdx)
    Msg2Player("Quµ t©n thñ ®· chuyÓn sang LÔ Quan ë ba T©n Thñ Th«n. NhËn trùc tiÕp, kh«ng cÇn më tói; mçi nh©n vËt mét lÇn.")
end

function PhongThanClaimStarter()
    if GetTask(PT_STARTER_TASK)==2 then PTLGCard();return end
    -- Recheck on the callback: menu opening is not authorization to grant.
    if PTStarterCheck() == 0 then return end
    local career = GetSeries()
    local rewards = PTStarterRewards(career)
    local progress = GetTask(PT_STARTER_PROGRESS_TASK)
    local total = 0
    local i = 0
    for i = 1, getn(rewards) do total = total + rewards[i][7] end
    if progress < 0 or progress > total + 1 then
        Msg2Player("TiÕn ®é quµ t©n thñ kh«ng hîp lÖ. H·y liªn hÖ GM.")
        return
    end
    SetTask(PT_STARTER_CAREER_TASK, career + 1)
    if GetTask(PT_STARTER_TASK) == 0 then SetTask(PT_STARTER_TASK, 1) end
    local unit = 0
    local j = 0
    for i = 1, getn(rewards) do
        local row = rewards[i]
        for j = 1, row[7] do
            unit = unit + 1
            if unit > progress then
                local result = 0
                if row[1] == 0 then
                    -- Seventh argument binds the instance. AddNormalItem
                    -- returns success only after insertion into inventory.
                    result = AddNormalItem(row[1], row[2], row[3], row[4], row[5], row[6], 1)
                else
                    -- Native stacking merges each potion into stacks of 100.
                    result = AddNormalItemPile(row[1], row[2], row[3], row[4], row[5], row[6])
                end
                if result == nil or result <= 0 then
                    Msg2Player("Ch­a nhËn hÕt quµ. H·y ®Ó trèng hµnh trang F4 vµ gÆp l¹i LÔ Quan; chØ ph¸t tiÕp phÇn cßn thiÕu.")
                    return
                end
                progress = unit
                SetTask(PT_STARTER_PROGRESS_TASK, progress)
            end
        end
    end
    -- Currency and its saved marker are committed in the same native transaction.
    -- Legacy total+1 means abstract copper was paid, not that Xu items were paid.
    if PTLGCard() ~= 1 then
        Msg2Player("Ch­a nhËn ®­îc thÎ quµ th¨ng cÊp. H·y ®Ó trèng F4 vµ gÆp l¹i LÔ Quan ®Ó nhËn tiÕp; kh«ng ph¸t l¹i trang bÞ.")
        return
    end
    SetTask(PT_STARTER_PROGRESS_TASK, total + 1)
    if GetItemCount(6, PT_STARTER_BAG_ID, 1, -1, 3) > 0 then
        DelItem(1, 6, PT_STARTER_BAG_ID, 1, -1, 3)
    end
    SetTask(PT_STARTER_TASK, 2)
    Msg2Player("§· nhËn ®ñ quµ t©n thñ: trang bÞ, vò khÝ, thó c­ìi, 100 TiÓu Hång §¬n, 100 TiÓu Hoµn §¬n vµ ThÎ Quµ: Th¨ng CÊp NhËn Th­ëng!")
end

function PTStarterClose()
end

PTLG_LEQUAN_MAIN=main
Include("\\script\\phongthan\\rewards\\level_gift.lua")
main=PTLG_LEQUAN_MAIN

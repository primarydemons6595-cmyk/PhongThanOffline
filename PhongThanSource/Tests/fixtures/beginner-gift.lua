function ResetGift()
    tasks = {}; level = 1; career = 0; bag = 0; copper = 500
    delivered = {}; red = 0; blue = 0; attempts = 0
    failAt = 0; failCopper = 0; failConsume = 0; failIssue = 0
    trading = 0; locked = 0; prompts = 0
end
function GetLevel() return level end
function GetSeries() return career end
function GetTask(id) return tasks[id] or 0 end
function SetTask(id, value) tasks[id] = value end
function GetTrade() return trading end
function GetLockState() return locked end
function GetItemCount(g, d, l, s, p)
    assert(g == 6 and d == 9173 and l == 1 and s == -1 and p == 3)
    return bag
end
function Msg2Player(text) assert(type(text) == "string") end
function QuestExchange(task, old, next, needs, rewards)
    assert(task == 4700 and old == 0 and next == 1 and getn(needs) == 0)
    assert(getn(rewards) == 1 and rewards[1][1] == 6 and rewards[1][3] == 9173)
    if failIssue == 1 then return 0 end
    assert(GetTask(task) == old)
    bag = bag + 1; SetTask(task, next); return 1
end
function TestBeginnerLogin()
    ResetGift(); PTGiveBeginnerGuide(); PTGiveBeginnerGuide()
    assert(bag == 1 and GetTask(4700) == 1)
    ResetGift(); level = 2; PTGiveBeginnerGuide(); assert(bag == 0 and GetTask(4700) == 0)
    ResetGift(); level = 0; PTGiveBeginnerGuide(); assert(bag == 0)
    ResetGift(); failIssue = 1; PTGiveBeginnerGuide(); assert(bag == 0 and GetTask(4700) == 0)
    failIssue = 0; PTGiveBeginnerGuide(); assert(bag == 1)
    ResetGift(); trading = 1; PTGiveBeginnerGuide(); assert(bag == 0)
    ResetGift(); locked = 1; PTGiveBeginnerGuide(); assert(bag == 0)
end
function AddNormalItem(g, d, p, l, s, luck, bound)
    attempts = attempts + 1
    if attempts == failAt then return 0 end
    assert(g == 0 and l == 1 and luck == 0 and bound == 1)
    tinsert(delivered, {d, p, s}); return attempts
end
function AddNormalItemPile(g, d, p, l, s, luck)
    attempts = attempts + 1
    if attempts == failAt then return 0 end
    assert(g == 1 and p == 0 and l == 1 and s == 0 and luck == 0)
    if d == 0 then red = red + 1 elseif d == 3 then blue = blue + 1 else assert(nil) end
    return attempts
end
function IsHaveSpaceForCopperCash(n) assert(n == 100); return 1 end
function AddCopperCash(n)
    assert(n == 100)
    if failCopper == 1 then return 0 end
    copper = copper + n; return copper
end
function DelItem(n, g, d, l, s, p)
    assert(n == 1 and g == 6 and d == 9173 and l == 1 and s == -1 and p == 3)
    if failConsume == 1 then return 0 end
    assert(bag == 1); bag = bag - 1; return 1
end
function Say(text, n, first, second)
    assert(n == 2 and first == "Nhan qua/PhongThanClaimStarter" and second == "De sau/PTStarterClose")
    prompts = prompts + 1
end
function ReadyGift(c)
    ResetGift(); career = c; level = 10; bag = 1; SetTask(4700, 1)
end
function AssertGift(c)
    local expected = 7
    if c == 0 then expected = 8 end
    assert(GetTask(4700) == 2 and bag == 0 and copper == 600)
    assert(red == 100 and blue == 100 and getn(delivered) == expected)
    local i = 0
    local details = {2, 7, 6, 5, 9}
    for i = 1, 5 do
        assert(delivered[i][1] == details[i] and delivered[i][2] == 6 + c and delivered[i][3] == 1 + 5 * c)
    end
    if c == 0 then
        assert(delivered[6][2] == 27 and delivered[7][2] == 28)
    else
        assert(delivered[6][2] == 28 + c)
    end
    assert(delivered[expected][1] == 10 and delivered[expected][2] == 24 + c)
end
function TestBeginnerGift()
    local c = 0
    for c = 0, 2 do
        ReadyGift(c); level = 9; main(1); PhongThanClaimStarter(); assert(attempts == 0 and prompts == 0)
        level = 10; main(1); assert(prompts == 1)
        PhongThanClaimStarter(); AssertGift(c)
        PhongThanClaimStarter(); AssertGift(c)
        ReadyGift(c); level = 20; PhongThanClaimStarter(); AssertGift(c)
    end
    ReadyGift(0); bag = 0; PhongThanClaimStarter(); assert(attempts == 0)
    ReadyGift(0); SetTask(4700, 0); PhongThanClaimStarter(); assert(attempts == 0)
    ReadyGift(3); PhongThanClaimStarter(); assert(attempts == 0)
    ReadyGift(0); failAt = 5; PhongThanClaimStarter(); assert(GetTask(4702) == 4 and bag == 1)
    career = 1; PhongThanClaimStarter(); assert(GetTask(4702) == 4)
    career = 0; failAt = 0; PhongThanClaimStarter(); AssertGift(0)
    ReadyGift(1); failAt = 72; PhongThanClaimStarter(); assert(red == 64 and blue == 0)
    failAt = 0; PhongThanClaimStarter(); AssertGift(1)
    ReadyGift(2); failCopper = 1; PhongThanClaimStarter(); assert(copper == 500 and bag == 1)
    failCopper = 0; PhongThanClaimStarter(); AssertGift(2)
    ReadyGift(0); failConsume = 1; PhongThanClaimStarter(); assert(copper == 600 and bag == 1)
    failConsume = 0; PhongThanClaimStarter(); AssertGift(0)
    ReadyGift(0); main(1); trading = 1; PhongThanClaimStarter(); assert(attempts == 0)
    ReadyGift(0); locked = 1; PhongThanClaimStarter(); assert(attempts == 0)
end

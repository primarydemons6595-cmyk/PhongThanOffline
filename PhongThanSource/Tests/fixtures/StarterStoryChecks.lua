-- Actual production PTG/PT2 apply and preview functions are loaded first.
-- Only player/inventory/UI APIs are isolated; no live account is modified.
checks=0
function check(value) checks=checks+1;if not value then error("starter story check "..checks) end end
function GetTask(t) return tasks[t] or 0 end
function SetTask(t,v) tasks[t]=v end
function GetPlayerType() return faction end
function GetLevel() return 90 end
function GetContentWorldPos() return world-1000 end
function GetTrade() return trading end
function GetLockState() return locked end
function GetCash() return money end
function Earn(n) money=money+n end
function Pay(n) money=money-n end
function AddOwnExp(n) exp=exp+n end
function Msg2Player(text) lastMessage=text end
function SayButtons(text,count,first,second) lastText=text;options=count end
function CloseDialog() end
function PTGJournal() end
function PT2Journal() end
function key(g,d,p,l) return g..":"..d..":"..p..":"..l end
function HaveNormalItem(g,d,p,l) return bag[key(g,d,p,l)] or 0 end
function QuestExchange(t,from,to,needs,gifts)
    if reject or GetTask(t)~=from or from==to then return 0 end
    for i=1,getn(needs) do local r=needs[i];if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end end
    for i=1,getn(needs) do local r=needs[i];local k=key(r[1],r[2],r[3],r[4]);bag[k]=bag[k]-r[7] end
    for i=1,getn(gifts) do local r=gifts[i];local k=key(r[1],r[2],r[3],r[4]);bag[k]=(bag[k] or 0)+r[7] end
    tasks[t]=to;return 1
end
for f=0,2 do
    faction=f;tasks={};bag={};exp=0;money=0;trading=0;locked=0;reject=nil
    local steps=f==0 and PTG_STEPS or PT2_STEPS
    local task=f==0 and 21 or (f==1 and 11 or 31)
    local count=0
    for i=1,getn(steps) do
        local a=steps[i]
        if a.task==task and (f==0 or a.f==f) then
            count=count+1;world=f==0 and 1002 or (f==1 and 1003 or 1004)
            PTG_MAP=world;PT2_MAP=world;PTG_ROLE=a.role;PT2_ROLE=a.role
            check(GetTask(task)==a.from)
            local beforeExp=exp;local beforeMoney=money
            if f==0 then PTGPreview(i) else PT2Preview(i) end
            check(exp==beforeExp and money==beforeMoney and GetTask(task)==a.from)
            check(strfind(lastText,PTStarterStep(a).text,1,1)~=nil)
            check(options==2 and strfind(lastText,tostring(a.xp),1,1)~=nil)
            -- A blocked trade, lock, missing input, or rejected transaction pays nothing.
            trading=1;if f==0 then PTGApply(i) else PT2Apply(i) end
            check(exp==beforeExp and money==beforeMoney and GetTask(task)==a.from);trading=0
            locked=1;if f==0 then PTGApply(i) else PT2Apply(i) end
            check(exp==beforeExp and money==beforeMoney and GetTask(task)==a.from);locked=0
            if getn(a.needs)>0 then
                for n=1,getn(a.needs) do local r=a.needs[n];bag[key(r[1],r[2],r[3],r[4])]=0 end
                if f==0 then PTGApply(i) else PT2Apply(i) end
                check(exp==beforeExp and money==beforeMoney and GetTask(task)==a.from)
            end
            for n=1,getn(a.needs) do local r=a.needs[n];bag[key(r[1],r[2],r[3],r[4])]=r[7] end
            reject=1;if f==0 then PTGApply(i) else PT2Apply(i) end
            check(exp==beforeExp and money==beforeMoney and GetTask(task)==a.from);reject=nil
            if f==0 then PTGApply(i) else PT2Apply(i) end
            check(GetTask(task)==a.to and exp==beforeExp+a.xp and money==beforeMoney+a.money)
            check(options==1 and strfind(lastText,"EXP",1,1)~=nil)
            local paid=exp;local cash=money
            if f==0 then PTGApply(i) else PT2Apply(i) end
            check(exp==paid and money==cash)
        end
    end
    check(exp==(f==0 and 9000 or (f==1 and 7000 or 7500)))
    check(money==600)
    check(GetTask(task)==PT_STARTER_FINAL[task])
    print("PASS STARTER_CHAIN",f,"steps",count,"exp",exp,"money",money)
end
print("PASS STARTER_STORY checks",checks,"preview/no reward; locked/trading/missing/full rollback; step/final/replay; original story")

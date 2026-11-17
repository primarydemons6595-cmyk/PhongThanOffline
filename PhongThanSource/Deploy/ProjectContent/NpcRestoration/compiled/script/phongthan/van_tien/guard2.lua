PT_ID=2
PT_MAP=68
PT_LEVEL=60
PT_FIRST=134
PT_BOSS=138
PT_LABEL="Thuy"
PT_REWARD_TASK=4931
PT_ENTRY_TASK=4935
PT_CLAIM_TASK=4939
PT_REWARD_DETAIL=64
PT_ENTRY_X=1752
PT_ENTRY_Y=3567
PT_EXIT_MAP=4
PT_EXIT_X=1548
PT_EXIT_Y=3232
PT_POINTS={{51040,105568},{56128,105568},{51008,109376},{56192,109376},{53600,107488}}
PT_MOBS={{134,50400,104928},{135,51680,104928},{136,52960,104960},{137,54272,104928},{134,55520,104928},{135,56832,104896},{136,50400,106528},{137,51680,106528},{134,52960,106528},{135,54240,106528},{136,55520,106528},{137,56800,106528},{134,50400,108128},{135,51680,108128},{136,52960,108128},{137,54240,108128},{134,55520,108128},{135,56768,108096},{136,50400,109728},{137,51712,109760},{134,52960,109728},{135,54240,109728},{136,55520,109728},{137,56800,109728}}

function pt_each(fn)
    local cursor=0
    local old=PlayerIndex
    for n=1,500 do
        local player
        cursor,player=GetNextPlayer(PT_ID,cursor,0)
        if cursor==0 then break end
        if player>0 then PlayerIndex=player;fn() end
    end
    PlayerIndex=old
end
function pt_fight() SetFightState(1) end
function pt_exit()
    LockCamp(0);SetFightState(0);SetLogoutRV(0)
    NewWorld(PT_EXIT_MAP,PT_EXIT_X,PT_EXIT_Y)
end
function pt_spawn(tid,x,y,death)
    local npc=AddNpc(tid,PT_LEVEL,SubWorld,x,y,1)
    if npc==0 then return 0 end
    local owned=AddMSNpc(PT_ID,npc,0)
    if owned==nil or owned<=0 then DelNpc(npc);return 0 end
    if death~=nil then
        if SetNpcDeathScript(npc,death)~=1 then DelNpc(npc);return 0 end
    end
    return npc
end
function pt_progress()
    if GetGlobalValue(PT_ID)~=2 then return end
    for n=1,4 do
        if GetMissionV(10+n)==0 and GetMissionV(20+n)==0 then
            local p=PT_POINTS[n]
            local npc=pt_spawn(PT_FIRST+n-1,p[1],p[2],"\\script\\phongthan\\van_tien\\guard"..PT_ID..".lua")
            if npc>0 then SetMissionV(20+n,npc) end
        end
    end
    if GetMissionV(11)+GetMissionV(12)+GetMissionV(13)+GetMissionV(14)==4 and GetMissionV(30)==0 then
        local p=PT_POINTS[5]
        local npc=pt_spawn(PT_BOSS,p[1],p[2],"\\script\\phongthan\\van_tien\\boss"..PT_ID..".lua")
        if npc>0 then SetMissionV(30,npc);Msg2MSAll(PT_ID,"Da ha bon ho phap. Thong Thien Giao Chu xuat hien!") end
    end
end

function OnDeath(npc)
    if GetGlobalValue(PT_ID)~=2 then return end
    local slot=GetNpcTemplateID(npc)-PT_FIRST+1
    if slot<1 or slot>4 or GetMissionV(20+slot)~=npc or GetMissionV(10+slot)~=0 then return end
    SetMissionV(10+slot,1)
    Msg2MSAll(PT_ID,"Da ha ho phap "..slot.."/4.")
    pt_progress()
end

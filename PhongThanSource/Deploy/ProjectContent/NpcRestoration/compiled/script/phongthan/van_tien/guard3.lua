PT_ID=3
PT_MAP=69
PT_LEVEL=80
PT_FIRST=139
PT_BOSS=143
PT_LABEL="Hoa"
PT_REWARD_TASK=4932
PT_ENTRY_TASK=4936
PT_CLAIM_TASK=4940
PT_REWARD_DETAIL=65
PT_ENTRY_X=1650
PT_ENTRY_Y=3400
PT_EXIT_MAP=3
PT_EXIT_X=1709
PT_EXIT_Y=3105
PT_POINTS={{50624,103136},{55744,103136},{50624,106976},{55744,106976},{53184,105056}}
PT_MOBS={{139,49984,102496},{140,51200,102560},{141,52544,102496},{142,53824,102496},{139,55104,102496},{140,56288,102400},{141,49984,104096},{142,51296,104032},{139,52512,104128},{140,53824,104096},{141,55104,104096},{142,56288,104000},{139,49984,105696},{140,51264,105696},{141,52544,105696},{142,53824,105696},{139,55104,105696},{140,56288,105600},{141,49984,107296},{142,51264,107296},{139,52544,107296},{140,53824,107296},{141,55104,107296},{142,56288,107200}}

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

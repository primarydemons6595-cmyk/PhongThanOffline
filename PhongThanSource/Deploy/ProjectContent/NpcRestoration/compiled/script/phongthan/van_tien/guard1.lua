PT_ID=1
PT_MAP=67
PT_LEVEL=40
PT_FIRST=129
PT_BOSS=133
PT_LABEL="Tho"
PT_REWARD_TASK=4930
PT_ENTRY_TASK=4934
PT_CLAIM_TASK=4938
PT_REWARD_DETAIL=63
PT_ENTRY_X=1325
PT_ENTRY_Y=3280
PT_EXIT_MAP=2
PT_EXIT_X=1650
PT_EXIT_Y=3159
PT_POINTS={{44640,101344},{49760,101344},{44608,105152},{49760,105184},{47200,103264}}
PT_MOBS={{129,44000,100704},{130,45248,100640},{131,46560,100704},{132,47840,100704},{129,49120,100704},{130,50400,100704},{131,44000,102304},{132,45280,102304},{129,46560,102304},{130,47840,102304},{131,49120,102304},{132,50336,102240},{129,44032,103872},{130,45280,103904},{131,46528,103904},{132,47776,103840},{129,49120,103904},{130,50368,103872},{131,44000,105504},{132,45280,105504},{129,46560,105472},{130,47776,105472},{131,49120,105504},{132,50336,105536}}

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

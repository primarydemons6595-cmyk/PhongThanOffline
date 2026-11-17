PT_ID=4
PT_MAP=70
PT_LEVEL=100
PT_FIRST=144
PT_BOSS=148
PT_LABEL="Phong"
PT_REWARD_TASK=4933
PT_ENTRY_TASK=4937
PT_CLAIM_TASK=4941
PT_REWARD_DETAIL=65
PT_ENTRY_X=1671
PT_ENTRY_Y=3035
PT_EXIT_MAP=1
PT_EXIT_X=1510
PT_EXIT_Y=3290
PT_POINTS={{52832,99008},{57952,99008},{52832,102848},{57952,102848},{55392,100928}}
PT_MOBS={{144,52192,98368},{145,53472,98368},{146,54752,98368},{147,56000,98336},{144,57312,98368},{145,58336,98112},{146,52192,99968},{147,53472,99968},{144,54752,99968},{145,56032,99968},{146,57312,99968},{147,58336,99712},{144,52192,101568},{145,53472,101568},{146,54752,101568},{147,56032,101568},{144,57312,101568},{145,58336,101312},{146,52192,103168},{147,53472,103168},{144,54784,103200},{145,56032,103168},{146,57280,103136},{147,58336,102912}}

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

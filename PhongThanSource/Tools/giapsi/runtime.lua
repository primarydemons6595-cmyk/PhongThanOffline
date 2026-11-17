-- UTF-8 authoring source; build_giapsi_quests.cjs emits engine TCVN3 bytes.
-- Saved phases retain the original VNG task IDs. New slots: 4550..4569.
function PTGMap() return GetContentWorldPos()+1000 end
function PTGAllowed()
    return GetPlayerType()==0 and PTGMap()==PTG_MAP
end
function PTGReady(a)
    return PTGAllowed() and a.role==PTG_ROLE and GetLevel()>=a.level and
        GetTask(a.task)==a.from and (not a.mainPhase or GetTask(3)==a.mainPhase)
end
function PTGHave(rows)
    for i=1,getn(rows) do
        local r=rows[i]
        if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end
    end
    return 1
end
function PTGNotice(text)
    SayButtons(text,1,"Đóng/ptg_close")
end
function ptg_close() CloseDialog() end
function PTGPreview(i)
    local a=PTG_STEPS[i]
    if not PTGReady(a) then return end
    local text="<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>"
    if a.xp>0 then text=text.."<enter><color=green>Kinh nghiệm: "..a.xp.."<color>" end
    if a.money>0 then text=text.."<enter><color=green>Lượng: "..a.money.."<color>" end
    if a.rewardText then text=text.."<enter><color=green>"..a.rewardText.."<color>" end
    SayButtons(text,2,"Tiếp tục/ptg_yes"..i,"Quay lại/main")
end
function PTGApply(i)
    local a=PTG_STEPS[i]
    if not PTGReady(a) then return end
    if GetTrade()~=0 or GetLockState()~=0 then return end
    if PTGHave(a.needs)~=1 then
        PTGNotice("<color=yellow>"..a.title.."<color><enter>Chưa đủ vật phẩm.<enter><color=cyan>"..a.note.."<color>")
        return
    end
    local needs=a.needs
    if a.keepNeeds then needs={} end
    local gifts=a.gifts
    if a.ensureBoard then
        gifts={}
        local count=HaveEventItem(9)
        if count<3 then gifts={{4,9,0,0,0,0,3-count}} end
    end
    if a.randomBook then gifts={PTG_BOOKS[RANDOM(1,getn(PTG_BOOKS))]} end
    -- Earn has no result in the legacy API: verify the actual wallet delta,
    -- then roll it back if inventory/phase CAS fails. No yielded Lua calls.
    local old=GetCash()
    if a.money>0 then
        if old>2000000000-a.money then return end
        Earn(a.money)
        if GetCash()~=old+a.money then return end
    end
    if QuestExchange(a.task,a.from,a.to,needs,gifts,1)~=1 then
        if a.money>0 then Pay(a.money) end
        PTGNotice("Chưa thể giao nhiệm vụ. Hãy mở khóa vật phẩm và dành đủ ô trống trong hành trang.")
        return
    end
    if a.xp>0 then AddOwnExp(a.xp) end
    if a.credit>0 then AddCredit(a.credit) end
    if a.camp then SetCamp(a.camp) end
    if a.task==3 and a.to==84 then SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0) end
    PTGJournal()
    Msg2Player(a.note)
    if a.travel then local moved=NewRuntimeWorld(a.travel[1],a.travel[2]*8+4,a.travel[3]*16+8); if moved==1 and (a.travel[1]==1062 or a.travel[1]==1063) then SetFightState(1) end end
    PTGNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>")
end
function PTGFutureTravelReady()
    local phase=GetTask(3)
    return PTGAllowed() and GetLevel()>=80 and
        ((PTG_ROLE=="tay" and phase>=70 and phase<=74) or
         (PTG_ROLE=="dac_future" and phase>=72 and phase<=74))
end
function ptg_future_travel()
    if not PTGFutureTravelReady() or GetTrade()~=0 or GetLockState()~=0 then return end
    if GetTask(3)==70 then
        -- Accept through the original guarded quest exchange; never skip it.
        for i=1,getn(PTG_STEPS) do
            local a=PTG_STEPS[i]
            if a.role=="tay" and a.task==3 and a.from==70 and PTGReady(a) then return PTGPreview(i) end
        end
        return
    end
    CloseDialog()
    local moved=0
    if PTG_ROLE=="tay" then moved=NewRuntimeWorld(1063,198*8+4,200*16+8)
    else moved=NewRuntimeWorld(1062,203*8+4,198*16+8) end
    if moved==1 then SetFightState(1) end
end
function PTGMenu()
    PTGJournal()
    if not PTGAllowed() then
        if PTG_OLD_MAIN then return PTG_OLD_MAIN() end
        CloseDialog();return
    end
    local options={}
    if PTGFutureTravelReady() then
        if PTG_ROLE=="tay" then tinsert(options,"Dịch chuyển đến Triều Ca tương lai/ptg_future_travel")
        else tinsert(options,"Dịch chuyển đến Ngọc Hư Cung tương lai/ptg_future_travel") end
    end
    if PTSAddChoices then PTSAddChoices(options) end
    for i=1,getn(PTG_STEPS) do
        local a=PTG_STEPS[i]
        if PTGReady(a) then tinsert(options,a.label.."/ptg_a"..i) end
    end
    if PTG_ROLE=="giao" and GetLevel()>=20 then
        if GetTask(4554)==0 and ((GetLevel()<29 and (GetTask(4556)~=PTGDay() or GetTask(4557)<3)) or (GetLevel()>=29 and GetTask(4558)==0)) then tinsert(options,"Giáo Huấn/ptg_giao") end
        if GetTask(4554)>0 and GetTask(4555)>=40 then tinsert(options,"Nhận thưởng/ptg_giao_claim") end
    end
    if PTG_ROLE=="thong" and GetTask(3)==88 then tinsert(options,"Khiêu chiến/ptg_challenge") end
    if PTG_OLD_MAIN then tinsert(options,"Dịch vụ/ptg_services") end
    local text="<color=yellow>Nhiệm vụ Giáp Sĩ<color><enter>Chọn nút nhiệm vụ để xem mục tiêu và phần thưởng."
    if getn(options)==0 then text="<color=yellow>Nhiệm vụ Giáp Sĩ<color><enter>Chưa có nhiệm vụ đủ điều kiện nhận hoặc trả. Xem chỉ dẫn đang làm bằng F11." end
    SayButtons(text,getn(options),options)
end
function ptg_services()
    if PTG_OLD_MAIN then PTG_OLD_MAIN() else CloseDialog() end
end
function PTGMarker()
    if not PTGAllowed() then
        if PTG_OLD_MARK then return PTG_OLD_MARK() end
        return 0,0
    end
    local best,level=0,0
    for i=1,getn(PTG_STEPS) do
        local a=PTG_STEPS[i]
        if PTGReady(a) then
            local state=PTGHave(a.needs)==1 and (a.from==0 and 1 or 3) or 2
            if state>best then best=state;level=a.level end
        end
    end
    if PTG_ROLE=="giao" and GetLevel()>=20 then
        if GetTask(4554)>0 then return GetTask(4555)>=40 and 3 or 2,20 end
        if (GetLevel()<29 and (GetTask(4556)~=PTGDay() or GetTask(4557)<3)) or (GetLevel()>=29 and GetTask(4558)==0) then return 1,20 end
    end
    if PTG_OLD_MARK then
        local state,oldLevel=PTG_OLD_MARK()
        if state and state>best then return state,oldLevel end
    end
    return best,level
end
function PTGEvent(id,limit)
    if HaveEventItem(id)>=limit then return 0 end
    return AddEventItem(id)
end
function PTGDeath(template)
    if GetPlayerType()~=0 then return end
    local before=PTGJournalState()
    local map=PTGMap()
    for i=1,getn(PTG_KILLS) do
        local k=PTG_KILLS[i]
        if template==k[1] and map==k[2] and GetTask(k[3])==k[4] then
            if PTGEvent(k[6],1)>0 and k[4]~=k[5] then SetTask(k[3],k[5]) end
        end
    end
    -- Original one-of-each Dung Dao admission remains enforced natively.
    if template==575 and map==1007 and GetTask(25)==1 then PTGEvent(20+RANDOM(1,3),1) end
    if GetTask(4550)==1 and map==1015 then
        if template==7 then if AddIBBuff(335,10)>0 then SetTask(4551,SystemTime()+10);TopMessage(11631) else SetTask(4551,0) end end
        if template==13 and GetTask(4551)>SystemTime() and HaveIBBuff(335)==1 then PTGEvent(191,5) end
    end
    if GetTask(4550)==5 and map==1014 and template==18 then
        if HaveNormalItem(6,1,277,1)<1 and HaveNormalItem(6,1,9201,1)<1 then AddNormalItem(6,1,277,1,0,0) end
    end
    local target=GetTask(4554)
    if target>0 and map==GetTask(4559) then
        local eligible=0
        if template==target then eligible=1 end
        if target==3 and template==0 then eligible=1 end
        if eligible==1 and GetTask(4555)<40 then SetTask(4555,GetTask(4555)+1) end
    end
    -- Soul masks are persistent, idempotent, map-checked and killer-scoped.
    if GetTask(3)==87 and map==1070 and template>=541 and template<=543 then
        local bit=2^(template-541)
        local mask=GetTask(4562)
        if mod(floor(mask/bit),2)==0 then SetTask(4562,mask+bit) end
        if GetTask(4562)==7 then
            SetTask(3,88);SetTask(4563,SystemTime()+180)
            Msg2Player("Viễn Cổ Hỏa Diệm: còn 3 phút hạ Thông Thiên Giáo Chủ!")
        end
    end
    if template==2703 and map==1070 and GetTask(3)==88 then
        if SystemTime()<=GetTask(4563) and PTGEvent(174,1)>0 then SetTask(3,89) end
    end
    if before~=PTGJournalState() then PTGJournal() end
end
function PTGDay() return floor((SystemTime()+25200)/86400) end
function ptg_giao()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetLevel()<20 then return end
    if GetTask(4554)>0 then
        PTGNotice("<color=yellow>Giáo Huấn<color><enter><color=cyan>"..GetTask(4555).." / 40 "..PTG_TARGET_NAMES[GetTask(4554)].."<color><enter>Đủ mục tiêu hãy chọn nút nhận thưởng.")
        if GetTask(4555)>=40 then SayButtons("<color=green>Phần thưởng: 4600 kinh nghiệm và 3000 lượng.<color>",2,"Nhận thưởng/ptg_giao_claim","Đóng/ptg_close") end
        return
    end
    if GetLevel()>=29 then
        if GetTask(4558)~=0 then PTGNotice("Đã hoàn thành lượt Giáo Huấn cuối cùng.");return end
    elseif GetTask(4556)==PTGDay() and GetTask(4557)>=3 then
        PTGNotice("Đã nhận đủ 3 lượt Giáo Huấn hôm nay.");return
    end
    SayButtons("<color=yellow>Giáo Huấn<color><enter><color=cyan>Tiêu diệt 40 quái được giao.<color><enter><color=green>4600 kinh nghiệm và 3000 lượng.<color>",2,"Nhận nhiệm vụ/ptg_giao_take","Quay lại/main")
end
function ptg_giao_take()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetLevel()<20 or GetTask(4554)~=0 then return end
    if GetTrade()~=0 or GetLockState()~=0 then return end
    if GetLevel()>=29 then
        if GetTask(4558)~=0 then return end
        SetTask(4558,1);SetTask(4554,7);SetTask(4559,1007)
    else
        if GetTask(4556)~=PTGDay() then SetTask(4556,PTGDay());SetTask(4557,0) end
        if GetTask(4557)>=3 then return end
        SetTask(4557,GetTask(4557)+1);SetTask(4554,3);SetTask(4559,1007)
    end
    SetTask(4555,0)
    PTGJournal()
    Msg2Player("Giáo Huấn: diệt 40 "..PTG_TARGET_NAMES[GetTask(4554)]..". Mục tiêu nằm tại "..PTG_TARGET_MAPS[GetTask(4554)]..".")
    PTGMenu()
end
function ptg_giao_claim()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetTask(4554)==0 or GetTask(4555)<40 then return end
    if GetTrade()~=0 or GetLockState()~=0 or GetCash()>1999997000 then return end
    local old=GetCash();Earn(3000)
    if GetCash()~=old+3000 then return end
    if QuestExchange(4554,GetTask(4554),0,{},{})~=1 then Pay(3000);return end
    SetTask(4555,0);AddOwnExp(4600);PTGJournal()
    PTGNotice("<color=green>Đã nhận 4600 kinh nghiệm và 3000 lượng.<color>")
end
function PTGGather(kind,bit)
    local before=PTGJournalState()
    if GetPlayerType()~=0 then return end
    local phase=GetTask(4550)
    local mask=GetTask(4553)
    if kind==1 and phase==3 and PTGMap()==1016 then
        return GiapGather(0)
    elseif kind==2 and phase==5 and PTGMap()==1014 then
        return GiapGather(0)
    end
    if before~=PTGJournalState() then PTGJournal() end
end
function ptg_challenge()
    if not PTGAllowed() or PTG_ROLE~="thong" or GetTask(3)~=88 then return end
    if SystemTime()>GetTask(4563) then
        -- Retry the whole Thong Thien stage, retaining the preceding quests.
        for id=176,178 do for n=1,2 do if HaveEventItem(id)>0 then DelEventItem(id) end end end
        SetTask(3,83);SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0)
        PTGJournal()
        PTGNotice("Đã quá 3 phút. Trở lại Khương Tử Nha thời niên thiếu nhận lại Thông Thiên.")
        return
    end
    local previous=GetTask(4567)
    if previous>0 and GetNpcTemplateID(previous)==2703 and GetNpcWorldPos(previous)==1070 and GetNpcLife(previous)>0 then return end
    local npc=AddNpc(2703,95,SubWorld,204*256+128,210*512+256,1)
    if npc and npc>0 then
        if SetNpcDeathScript(npc,"\\script\\phongthan\\giapsi\\death_2703.lua")==1 then SetTask(4567,npc) else DelNpc(npc) end
    end
    CloseDialog()
end

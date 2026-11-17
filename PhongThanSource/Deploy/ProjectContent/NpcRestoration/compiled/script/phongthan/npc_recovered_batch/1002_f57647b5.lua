--description: ÍòÏÉÕó[ÍÁ]
--author: yujin
--date: 2005/4/14

function main()
	tasks = 
	{
		{"V¹n Tiªn trËn","renwu3";show=1}
	}
	if(GetGlobalValue(1)==0)then
		SayTask(11176,tasks)			
	elseif(GetGlobalValue(1)==1)then
		SayTask(11177,tasks)			
	elseif(GetGlobalValue(1)==2)then
		SayTask(11178,tasks)			
	end;
end;

function no()
		CloseDialog()
end;
--------------------------
function   renwu3()
	idx = SubWorldID2Idx(67); -- ?±£µØÍ¼ÔÚÕâÌ¨·þÎñÆ÷
	if (idx == -1) then 
		return
	end;
	SubWorld = idx; -- ?Îñ¿ªÆô±ØÐèµÄ±äÁ¿
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then
				Talk(1,"no",11179)
		elseif(GetLevel()<=29)or(GetLevel()>=51)then
				Talk(1,"no",11180)
		elseif(7~=GetCamp())or(87<GetPK())then
				Talk(1,"no","ChØ cho phÐp nh©n vËt phe xanh  vµo.")
		elseif(HaveNormalItem(3,62,0,0)>=1)or(GetTask(421)==GetMissionV(1))then
				MsgBox(11181,"yes_wxz","no")
		else
				Talk(1,"no",11182)
		end;
end;

function do_enter()
		DelHandItem(3,66,0,0)
		DelHandItem(3,67,0,0)
		DelHandItem(3,68,0,0)
		DelHandItem(3,69,0,0)
			for i=1,60 do
				if(HaveNormalItem(3,66,0,0)>=1)then
							DelNormalItem(3,66,0,0)
				elseif(HaveNormalItem(3,67,0,0)>=1)then
							DelNormalItem(3,67,0,0)
				elseif(HaveNormalItem(3,68,0,0)>=1)then
							DelNormalItem(3,68,0,0)
				elseif(HaveNormalItem(3,69,0,0)>=1)then
							DelNormalItem(3,69,0,0)
				else
							break;
				end;
			end;
	
	local mission_step = GetGlobalValue(1)
	if (mission_step == 1) then
		SetFightState(0)
	else
		SetFightState(1)
	end
		AddMSPlayer(1,1)
		SetLogoutRV(1)
		SetTask(421,GetMissionV(1))
		LockCamp(1)--Ëø¶¨ÕóÓª
		NewWorld(67,1325,3280)
		StopUsePills()
		Msg2Player("Tr¹ng th¸i tu luyÖn tù ®éng t¾t!")
		CloseDialog()
end

function  yes_ib()
	idx = SubWorldID2Idx(67); -- ?±£µØÍ¼ÔÚÕâÌ¨·þÎñÆ÷
	if (idx == -1) then 
		return
	end;
	SubWorld = idx; -- ?Îñ¿ªÆô±ØÐèµÄ±äÁ¿

	local mission_step = GetGlobalValue(1)
	local noneib_player_count = GetGlobalValue(61)
	local ib_player_count = GetGlobalValue(65)
	local player_count = noneib_player_count+ib_player_count
	local player_key = GetTask(421)
	local mission_key = GetMissionV(1)

	if (mission_step == 1) then
		if(player_count <200) then
			if(player_key ~= mission_key) then
				if(FindAValidIBItem(8,144,2,0)>=1)then
					DelNormalItem(3,62,0,0)
					CostIBItem(FindAValidIBItem(8,144,2,0))
					SetGlobalValue(65,GetGlobalValue(65)+1)
					do_enter()
				else
					Talk(1,"no","Xin lçi! Ng­¬i kh«ng cã Thæ Linh Th¹ch.")
				end
			else
				do_enter()
			end	
		else
			Talk(1,"no","Sè ng­êi trong trËn ®· ®Çy, Linh Th¹ch còng kh«ng thÓ gióp ng­¬i vµo trong.")
		end
	elseif(mission_step == 2) then
		if(player_count < 200 and player_key==mission_key) then
			do_enter()
		else
			Talk(1,"no","Ng­¬i ®Õn trÔ ma qu¸i trong trËn ®· hãa phÐp ng¨n chÆn cöa vµo!")
		end
	else
		Talk(1,"no",11183)
	end

end


function  yes_wxz()
	idx = SubWorldID2Idx(67); -- ?±£µØÍ¼ÔÚÕâÌ¨·þÎñÆ÷
	if (idx == -1) then 
		return
	end;
	SubWorld = idx; -- ?Îñ¿ªÆô±ØÐèµÄ±äÁ¿

	local mission_step = GetGlobalValue(1)
	local noneib_player_count = GetGlobalValue(61)
	local ib_player_count = GetGlobalValue(65)
	local player_count = noneib_player_count+ib_player_count
	local player_key = GetTask(421)
	local mission_key = GetMissionV(1)
	
	if (player_key == mission_key) then
		if (mission_step == 1 or mission_step == 2) then
			do_enter()
		else
			Talk(1,"no",11183)
		end
	else
		if (mission_step == 1) then
			if(player_count < 200 )then
				if(noneib_player_count < 50) then
					DelNormalItem(3,62,0,0)
					SetGlobalValue(61,noneib_player_count+1)
					do_enter()
				else
					MsgBox("V¹n Tiªn trËn ®· ®ñ ng­êi, trõ khi ng­¬i cã <color=green>Thæ Linh Th¹ch<color> ta cã thÓ ph¸ lÖ cho ng­¬i vµo!","yes_ib","no")
				end
			else
				Talk(1,"no","Ng­¬i ®Õn trÔ ma qu¸i trong trËn ®· hãa phÐp ng¨n chÆn cöa vµo!")
			end
		else
			Talk(1,"no",11183)
		end
	end
end;
-- Project no-fee recovery mode; no currency or tickets consumed.
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

function no() CloseDialog() end
function main()
    Say("Van Tien Tran ("..PT_LABEL.."): 02:20, 10:20, 13:20, 18:20, 21:20, 23:20. Dang ky 5 phut, chien dau 30 phut. Ban phuc hoi mien phi.",3,"Vao tran/pt_enter","Nhan thuong/pt_claim","Thoat/no")
end
function pt_enter()
    SubWorld=SubWorldID2Idx(PT_MAP)
    if SubWorld<0 then Say("Ban do Van Tien Tran chua san sang. Bao loi cho quan tri vien.",1,"Thoat/no");return end
    local state=GetGlobalValue(PT_ID)
    if state==0 then Say("Chua den gio mo tran. Dang ky 02:20, 10:20, 13:20, 18:20, 21:20, 23:20 (gio Viet Nam), trong 5 phut.",1,"Thoat/no");return end
    if GetLevel()<30 or GetLevel()>50 then
        Say("Cap do khong phu hop voi tran nay.",1,"Thoat/no");return
    end
    if GetTask(PT_REWARD_TASK)>0 then Say("Hay nhan thuong tran truoc truoc khi vao tran moi.",1,"Thoat/no");return end
    if state==2 and GetTask(PT_ENTRY_TASK)~=GetMissionV(1) then Say("Da het gio dang ky.",1,"Thoat/no");return end
    if NewWorld(PT_MAP,PT_ENTRY_X,PT_ENTRY_Y)~=1 then
        Say("Chua the chuyen map. Hay thu lai.",1,"Thoat/no");return
    end
    AddMSPlayer(PT_ID,1)
    SetTask(PT_ENTRY_TASK,GetMissionV(1))
    SetLogoutRV(1);LockCamp(1)
    if state==1 then SetFightState(0) else SetFightState(1) end
    if state==1 then ShowMissionCountdown(PT_ID,3001) else ShowMissionCountdown(PT_ID,3002) end
    CloseDialog()
end
function pt_claim()
    local reward=GetTask(PT_REWARD_TASK)
    if reward==0 then Say("Chua co thuong cho ban.",1,"Thoat/no");return end
    if GetTask(PT_CLAIM_TASK)==reward then SetTask(PT_REWARD_TASK,0);CloseDialog();return end
    if QuestExchange(PT_CLAIM_TASK,GetTask(PT_CLAIM_TASK),reward,{},{{3,PT_REWARD_DETAIL,0,0,0,0,2}})~=1 then
        Say("Hay de trong 2 o F4. Thuong van duoc giu, khong bi mat.",1,"Thoat/no");return
    end
    SetTask(PT_REWARD_TASK,0)
    Say("Da nhan 2 Linh Phu thuong Van Tien Tran.",1,"Thoat/no")
end

-- Atomic native admission; no consumption before a successful local transfer.
function main()
    Say("Van Tien Tran: can 1 Linh Phu dung loai cho moi luot moi. Vao lai cung luot khong thu them. Lich va ve do Studio quan ly. Thuong theo NPC nhiem vu va drop goc.",3,"Vao tran/pt_enter","Thuong ton tu ban cu/pt_claim","Ket thuc doi thoai/no")
end
function pt_enter()
    SubWorld=SubWorldID2Idx(PT_MAP)
    local result=EventEnterVanTien(PT_ID,PT_ENTRY_X,PT_ENTRY_Y)
    if result~=1 then
        local message="Chua den gio dang ky, het cho hoac dieu kien cap do chua phu hop."
        if result==-1 then message="Thieu Linh Phu dung loai trong F4. Chua tru ve."
        elseif result==-2 then message="Chua the xu ly hanh trang. Hay ket thuc giao dich va thu lai." end
        Say(message,1,"Ket thuc doi thoai/no");return
    end
    SetLogoutRV(1);LockCamp(1)
    local phase=GetGlobalValue(PT_ID)
    if phase==1 then SetFightState(0);ShowMissionCountdown(PT_ID,3001)
    else SetFightState(1);ShowMissionCountdown(PT_ID,3002) end
    CloseDialog()
end
pt_old_pending_claim=pt_claim
function pt_claim()
    local reward=GetTask(PT_REWARD_TASK)
    local now=floor(SystemTime()/60)*4
    -- Legacy authored pending payments must match the old admission generation.
    -- Never interpret arbitrary IBBuff values as a reward or clear those slots.
    if reward<now-172800 or reward>now+4 or GetTask(4934)~=reward then
        Say("Thuong tran hien tai theo nhiem vu goc tai Thien Hung va vat pham roi. Khong phat them 2 Linh Phu.",1,"Ket thuc doi thoai/no");return
    end
    return pt_old_pending_claim()
end

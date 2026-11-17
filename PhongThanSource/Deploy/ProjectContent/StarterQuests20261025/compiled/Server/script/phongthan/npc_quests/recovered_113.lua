--818,Â~»í¬±•jª¤¡¼¢uúJüR­p
--819,Â~»íÅ÷ÇP±t»Ö¡¼
--895Ây±þ¥ô°È¨C¶g¦¸¼Æ¡A896³Ì«á¤@¦¸±µ¥ô°È®É¶¡¡A897Ây±þ¹ï¶H¡A898Ây±þ¼Æ¶q
object={
		{name="Ng­u S¸t",id=13},
		{name="D¹ Xoa",id=16},
		{name="Thiªn H¹o",id=17},
		{name="Sa Hån",id=18},
		{name="Lôc Quy",id=20},
		{name="L·o Hå l«",id=23},
		{name="B¨ng Linh",id=28},
		{name="Anh Chiªu ThÇn",id=30},
	}

npc_name=
{
"KiÕm Nh©n",
"TuyÕt qu¸i",
"Háa DiÖn",
"X¹ ThÇn",
"B¨ng Lang",
"Lôc Qu¸i",
"Cuång §iªu",
"Hoµn CÈu",
"YÕm Háa",
"Th¶o Tiªn",
"Cæ §iªu",
"Cèt Tinh",
"Ng­u S¸t",
"Gi¸p Cèt",
"ThiÕt Trïng",
"D¹ Xoa",
"Thiªn H¹o",
"Sa Hån",
"H¾c Phong",
"Lôc Quy",
"§ao CÇm",
"Háa Ng­",
"L·o Hå l«",
"L¹c C¬",
"Giang Quy",
"Tr­ Tinh",
"ThiÕt Ng­",
"B¨ng Linh",
"Háa Ma",
"Anh Chiªu ThÇn",
"Cù Th¹ch",
"H¶i S©m",
"Phi Gi¸p",
"Cèt Tinh V­¬ng",
"Ngäc N÷",
"Háa Tµ",
"Hµ Cèt",
"Thanh Hång §¨ng",
"TiÔn §ao thÇn",
"L©n vò s­",
"S¬n D­¬ng yªu",
"L«i Tr¹ch thÇn",
"D· Mao thÇn",
"Th¹ch thÇn",
"Lam Cèt",
"Bè ThÇn",
"Ma N÷",
"Lam Cèt",
"§íi Tr¹i",
"Lôc Ng« ThÇn",
"Phi Thö",
"H­¬ng d©n",
"¶i Nh©n",
"ThiÕt trïng",
"Phi Gi¸p",
"Ngäc N÷",
"D· Mao",
"Lam Cèt",
"Ma N÷",
"§íi Tr¹i",
"Lôc Ng« ThÇn",
}

function OnDeath(npcindex)
	local npcchr=GetHardNpcAttrib(npcindex)
	local mob_lvl = GetNpcLevel(npcindex)
	local	kindID=GetTask(897)-1
	local	npcID=GetNpcTemplateID(npcindex)
	local	count=GetTask(898)
	if(npcchr>=0)then
		local i=GetLevel()-mob_lvl
		if(i<=10)then
			if(npcchr==0)then
				ThrowItem(npcindex, PlayerIndex,3,15,0,1,0,0)
			elseif(npcchr==1)then	
				ThrowItem(npcindex, PlayerIndex,3,17,0,1,0,0)
			elseif(npcchr==2)then	
				ThrowItem(npcindex, PlayerIndex,3,16,0,1,0,0)
			elseif(npcchr==3)then	
				ThrowItem(npcindex, PlayerIndex,3,21,0,1,0,0)
			elseif(npcchr==4)then	
				ThrowItem(npcindex, PlayerIndex,3,18,0,1,0,0)
			elseif(npcchr==5)then	
				ThrowItem(npcindex, PlayerIndex,3,20,0,1,0,0)
			elseif(npcchr==6)then	
				ThrowItem(npcindex, PlayerIndex,3,19,0,1,0,0)
			elseif(npcchr==7)then	
				ThrowItem(npcindex, PlayerIndex,3,19,0,1,0,0)
			end;
		end;
	end;

	local templateID = GetNpcTemplateID(npcindex)+1
	--¦UÃþ«ö¦a¹Ï²Õ¶¤¦@¨É¦¨ªGªº¥ô°È
	local w,x,y=GetWorldPos()
	local lvl = GetNpcLevel(npcindex)
	if(GetTeam()~=0)then
		-- ¦³¶¤¥î(¥]¬A¥u¦³¦Û¤v¤@­Ó¤Hªº)
		local oldPlayer=PlayerIndex
		local membercount=GetTeamSize()
		-- ¹M¾ú¶¤¤¤¶¤­û
		for i=1,membercount do
			PlayerIndex=GetTeamMember(i)
			--¶Ä§LÀçÂy±þ¥ô°È
			liesha_city(templateID,w,lvl)
			--¶}±Òªá¥c¥ô°È¥ÎªºÂy±þ¥ô°È
			huahui_open_task(templateID,w)
			if(kindID==npcID)and(judge_relation()==1)then
				mission_PR()
				--®v®{Ây±þ¥ô°È
			end
		end
		PlayerIndex=oldPlayer
	else
		-- µL¶¤¥î
		--¶Ä§LÀçÂy±þ¥ô°È
		liesha_city(templateID,w,lvl)
			--¶}±Òªá¥c¥ô°È¥ÎªºÂy±þ¥ô°È
		huahui_open_task(templateID,w)
	end;
	--Ê¦Í½ÁÔÉ±?Îñ
end;
--®v®{Ây±þ¥ô°È
function judge_relation()		--º¡¨¬®v®{2¤H¶¤
	local mark=0
	local n=0
	if(GetTeamSize()>0)then			-- ¦³¶¤¥î	
		if(GetTeamSize()==2)then	--2¤H¶¤
			if(IsCaptain()==0)then
				n=GetTeamMember(1)
			else
				n=GetTeamMember(2)
			end;
			mark=IsMasterPRRelation(n)
		end
	end
	return mark
end


function mission_PR()
	local	kindID=GetTask(897)
	local	count=GetTask(898)
	if count <= 0 then return end
	local	mark=HaveIBBuff(215)	--§PÂ_Ây±þ¥ô°Èªº¼Ð»xbuff
	if(mark~=0)then
		if(count>1)then
			SetTask(898,count-1)
			Msg2Player("Trõ Yªu: B¹n ph¶i giÕt"..(count-1)..". "..PTQuestNpcName(kindID).."!")
		else
			SetTask(898,0)
			TaskNote(42,8)
			Msg2Player("Trõ Yªu: B¹n ®· h¹ thµnh c«ng "..PTQuestNpcName(kindID).." ")
		end
	else
		if(count>0)then
			Msg2Player("Trõ Yªu: Vßng s¸ng ®· biÕn mÊt, nhiÖm vô cõu s¸t thÊt b¹i.")
		end
	end
end
--¶Ä§LÀçÂy±þ¥ô°È
function	liesha_city(templateID, world, lvl)
	local task_id = 852 
	if (lvl >= 75)then
		task_id = 858
	elseif(lvl >= 55)then
		task_id = 856
	elseif(lvl >= 35)then
		task_id = 854
	end
	local task_val = GetTask(task_id)
	if(task_val ~= 0) then
		local w,x,y=GetWorldPos()
		if (w == world) then
			local type1 = GetByte(task_val,1)
			local count1=	GetByte(task_val,2)
			local type2 = GetByte(task_val,3)
			local count2= GetByte(task_val,4)
			if count1 == 0 and count2 == 0 then return end
		
			if(type1 == templateID and count1 > 0) then
				count1 = count1 - 1
				if(count1>0)then
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..PTQuestNpcName(type1).."("..(50-count1).."/50)")
				else
					count1 = 0
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..PTQuestNpcName(type1).." ")
				end
				TaskNote(task_id,-1)
				TaskNote(task_id,0,PTQuestNpcName(type1),(50-count1),PTQuestNpcName(type2),(50-count2))
				SetTask(task_id, SetByte(task_val, 2, count1))
			elseif(type2 == templateID and count2 > 0)then	
				count2 = count2 - 1
				if(count2>0)then
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..PTQuestNpcName(type2).."("..(50-count2).."/50)")
				else
					count2 = 0
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..PTQuestNpcName(type2).." ")
				end
				TaskNote(task_id,-1)
				TaskNote(task_id,0,PTQuestNpcName(type1),(50-count1),PTQuestNpcName(type2),(50-count2))
				SetTask(task_id, SetByte(task_val, 4, count2))
			end
			if (type1 == templateID or type2 == templateID)and(count1 == 0 and count2 == 0) then
				TaskNote(task_id, 1)
			end
		end
	end
end



--¶}±Òªá¥c¥ô°È¥ÎªºÂy±þ¥ô°È
function huahui_open_task(templateID,world)
	local task_target = GetTask(894)
	if (task_target ~= 0) then
		local w,x,y=GetWorldPos()
		if (w == world) then
			local task_val = GetTask(889)
			local param = {894, floor(GetTask(888)/2) + 1}
			for i=1,4 do
				local t = GetByte(task_target, i)
				local c = GetByte(task_val, i)
				if(t ~= 0) then
					if (t == templateID and c < 50)then
						c = c+1
						if(c < 50)then
							Msg2Player("NhiÖm vô hoa cá: Cõu s¸t thµnh c«ng "..PTQuestNpcName(t).."("..c.."/50))")
						else
							Msg2Player("NhiÖm vô hoa cá: h¹ s¸t thµnh c«ng "..PTQuestNpcName(t).." ")
						end
						SetTask(889,SetByte(GetTask(889), i, c))
					end
					param[getn(param)+1] = c
				end
			end
			TaskNote(894, -1)
			call(TaskNote, param)
		end
	end
end 

function  no()
	CloseDialog()
end;
-- Lua 4. Reconstructed KÕ Tôc rules; provenance is in the deployment report.
-- Original task slots: 973=local day, 984=acceptances, 993=target/count, 994=kills.
PTK_TARGETS={ [0]={1,4}, [1]={2,5}, [2]={3,7} }
PTK_NAMES={ [1]="KiÕm Nh©n",[4]="X¹ Nh©n",[2]="TuyÕt Qu¸i",[5]="B¨ng Lang",[3]="Háa DiÖn",[7]="Cuång §iªu" }
PTK_MATERIAL={ [1]=10,[4]=11,[2]=13,[5]=9,[3]=12,[7]=8 }

function PTK_Day()
    return floor(LocalSystemTime()/86400)
end
function PTK_ResetDay()
    if GetTask(993)==0 and GetTask(973)~=PTK_Day() then
        SetTask(973,PTK_Day())
        SetTask(984,0)
    end
end
function PTK_Count(level)
    if level<=10 then return 10 end
    if level<=20 then return 20 end
    if level<=40 then return 40 end
    if level<=60 then return 80 end
    if level<=80 then return 150 end
    if level<=100 then return 250 end
    return 400
end
function PTK_ValidTarget(target)
    local choices=PTK_TARGETS[GetPlayerType()]
    return choices and (target==choices[1] or target==choices[2])
end
function PTK_Note(step)
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    if not PTK_ValidTarget(target) or need<=0 then return end
    local text="KÕ Tôc: tiªu diÖt "..PTK_NAMES[target].." ("..GetTask(994).."/"..need..")."
    if GetTask(994)>=need then text=text.." Trë vÒ Thñ Khè nhËn th­ëng.";step=2 end
    TaskNote(51,step,text)
end
function PTK_Show()
    if GetPlayerType()==PTK_CLASS then return 1 end
    return 0
end
function ptk_close() CloseDialog() end
function ptk_open()
    if PTK_Show()~=1 then CloseDialog() return end
    PTK_ResetDay()
    local state=GetTask(993)
    if state~=0 then
        local target=GetByte(state,1)
        local need=GetByte(state,2)*10
        if not PTK_ValidTarget(target) or need<=0 then
            Msg2Player("D÷ liÖu KÕ Tôc ch­a phï hîp. TiÕn ®é ®­îc gi÷ nguyªn; h·y liªn hÖ GM.")
            CloseDialog() return
        end
        PTK_Note(1)
        if GetTask(994)>=need then
            MsgBox("§· hoµn thµnh KÕ Tôc. NhËn 100 ®iÓm kinh nghiÖm?","ptk_finish","ptk_close")
        else
            Talk(1,"ptk_close","KÕ Tôc: ®· tiªu diÖt "..GetTask(994).."/"..need.." "..PTK_NAMES[target]..". H·y tiÕp tôc råi trë l¹i gÆp ta.")
        end
        return
    end
    if GetLevel()<1 or GetTask(984)>=5 then
        Talk(1,"ptk_close","Mçi ngµy nhËn tèi ®a 5 lÇn KÕ Tôc. H·y quay l¹i vµo ngµy mai.") return
    end
    local choices=PTK_TARGETS[GetPlayerType()]
        Say("KÕ Tôc: chän môc tiªu. CÇn diÖt "..PTK_Count(GetLevel()).." qu¸i; hoµn thµnh nhËn 100 kinh nghiÖm. H«m nay ®· nhËn "..GetTask(984).."/5 lÇn.",3,
            PTK_NAMES[choices[1]].."/ptk_take1",PTK_NAMES[choices[2]].."/ptk_take2","§ãng/ptk_close")
end
function ptk_take1() PTK_Take(1) end
function ptk_take2() PTK_Take(2) end
function PTK_Take(choice)
    if PTK_Show()~=1 or GetLevel()<1 then return end
    PTK_ResetDay()
    if GetTask(993)~=0 or GetTask(984)>=5 then ptk_open() return end
    local target=PTK_TARGETS[GetPlayerType()][choice]
    if not target then return end
    local state=SetByte(SetByte(0,1,target),2,PTK_Count(GetLevel())/10)
    local rewards={}
    if GetLevel()<=5 then rewards={{1,0,1,1,0,0,1}} end
    if QuestExchange(993,0,state,{},rewards)~=1 then
        Msg2Player("Ch­a nhËn ®­îc nhiÖm vô. H·y chõa chç trèng trong hµnh trang vµ kÕt thóc giao dÞch.") return
    end
    SetTask(994,0)
    SetTask(984,GetTask(984)+1)
    PTK_Note(0)
    Msg2Player("§· nhËn KÕ Tôc. NhÊn F11 ®Ó xem tiÕn ®é.")
    CloseDialog()
end
function ptk_finish()
    if PTK_Show()~=1 then return end
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    if state==0 or need<=0 or not PTK_ValidTarget(target) or GetTask(994)<need then return end
    if QuestExchange(993,state,0,{},{})~=1 then return end
    SetTask(994,0)
    AddOwnExp(100)
    TaskNote(51,-1)
    PTK_ResetDay()
    Msg2Player("Hoµn thµnh KÕ Tôc: nhËn 100 ®iÓm kinh nghiÖm.")
    CloseDialog()
end
function PTK_State()
    if PTK_Show()~=1 then return 0,0 end
    local state=GetTask(993)
    if state~=0 then
        local target=GetByte(state,1)
        local need=GetByte(state,2)*10
        if PTK_ValidTarget(target) and need>0 then
            if GetTask(994)>=need then return 3,0 end
            return 2,0
        end
        return 0,0
    end
    if GetLevel()>=1 and (GetTask(973)~=PTK_Day() or GetTask(984)<5) then return 1,0 end
    return 0,0
end
-- Called only after the common death dispatcher validates player and map context.
function PTK_Death(template,teamSize)
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    local count=GetTask(994)
    if need<=0 or template+1~=target or not PTK_ValidTarget(target) or count>=need then return end
    SetTask(994,count+1)
    PTK_Note(1)
    Msg2Player("KÕ Tôc: "..PTK_NAMES[target].." "..(count+1).."/"..need..".")
    -- Archived server variants use 50% solo, 40+10*party size when teamed.
    -- This rate is explicitly reconstructed, not sourced from a client Lua.
    local chance=50
    if teamSize>0 then chance=40+teamSize*10 end
    if chance>100 then chance=100 end
    if random(1,100)<=chance then AddNormalItem(3,PTK_MATERIAL[target],0,0,0,0) end
    if count+1==need then Msg2Player("§· ®ñ sè qu¸i. Trë vÒ gÆp Thñ Khè ®Ó tr¶ nhiÖm vô KÕ Tôc.") end
end

-- Project compatibility patch, appended to the hash-verified VNG normal.lua.
-- Quest IDs, targets, limits, messages and hard-monster drop tuples remain
-- those in the original source; this fixes execution context, not quest rules.

function PTQuestNpcName(templateID)
    local name = npc_name[templateID]
    if name then return name end
    name = GetNpcTempName(templateID - 1)
    if name and name ~= "" then return name end
    return tostring(templateID)
end

function OnDeath(npcindex)
    local killer = PlayerIndex
    if not killer or killer <= 0 then return end
    local world = GetNpcWorldPos(npcindex)
    local killerWorld = GetWorldPos()
    if not world or world <= 0 or killerWorld ~= world then return end

    local npcID = GetNpcTemplateID(npcindex)
    local lvl = GetNpcLevel(npcindex)
    local npcchr = GetHardNpcAttrib(npcindex)
    -- The eight original ThrowItem tuples, unchanged.
    if npcchr >= 0 and GetLevel() - lvl <= 10 then
        if npcchr == 0 then
            ThrowItem(npcindex, PlayerIndex,3,15,0,1,0,0)
        elseif npcchr == 1 then
            ThrowItem(npcindex, PlayerIndex,3,17,0,1,0,0)
        elseif npcchr == 2 then
            ThrowItem(npcindex, PlayerIndex,3,16,0,1,0,0)
        elseif npcchr == 3 then
            ThrowItem(npcindex, PlayerIndex,3,21,0,1,0,0)
        elseif npcchr == 4 then
            ThrowItem(npcindex, PlayerIndex,3,18,0,1,0,0)
        elseif npcchr == 5 then
            ThrowItem(npcindex, PlayerIndex,3,20,0,1,0,0)
        elseif npcchr == 6 then
            ThrowItem(npcindex, PlayerIndex,3,19,0,1,0,0)
        elseif npcchr == 7 then
            ThrowItem(npcindex, PlayerIndex,3,19,0,1,0,0)
        end
    end

    -- GetTeam() is nil for solo and 0 is a valid team ID in the server.
    -- Snapshot members before changing PlayerIndex; otherwise an invalid or
    -- offline member can change the team from which the next slot is read.
    local membercount = GetTeamSize()
    local members = {}
    if membercount > 0 then
        for i=1,membercount do
            local member = GetTeamMember(i)
            if member and member > 0 then
                members[getn(members)+1] = member
            end
        end
    else
        members[1] = killer
    end

    local credited = {}
    for i=1,getn(members) do
        local member = members[i]
        if not credited[member] then
            credited[member] = 1
            PlayerIndex = member
            local memberWorld = GetWorldPos()
            if memberWorld == world then
                PTK_Death(npcID, membercount)
                liesha_city(npcID+1, world, lvl)
                huahui_open_task(npcID+1, world)
                -- Read each member's own target, not the killer's task 897.
                if membercount == 2 and GetTask(897) == npcID+1 and
                    GetTask(898) > 0 and judge_relation() == 1 then
                    mission_PR()
                end
            end
        end
    end
    PlayerIndex = killer
end

pt_common_death=OnDeath
--description:¶«º£ÉñÁú
--author: yichuan
--date: 2004/7/14

function OnDeath()
	UTask_Knight=GetTask(3)
	if(UTask_Knight==31)then
				if AddEventItem(13)<=0 then return end
				Msg2Player("LÊy ®­îc r©u cña ThÇn Long, cã thÓ ®Õn TriÒu Ca t×m Hå Hû MÞ.")
				SetTask(3,32)
				TaskNote(27,14)
	end;
end;

pt_monster_death=OnDeath
function OnDeath(index)
    local killer=PlayerIndex
    if not killer or killer<=0 or GetWorldPos()~=GetNpcWorldPos(index) then return end
    pt_common_death(index)
    PlayerIndex=killer
    pt_monster_death(index)
    PlayerIndex=killer
end

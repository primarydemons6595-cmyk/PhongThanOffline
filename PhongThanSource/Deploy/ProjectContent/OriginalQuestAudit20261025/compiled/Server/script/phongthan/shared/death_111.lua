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
PTK_MATERIAL={ [1]=10,[4]=11,[2]=6,[5]=9,[3]=7,[7]=8 }

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
    if GetLevel()<=5 then
        MsgBox("NhËn KÕ Tôc: tiªu diÖt "..PTK_Count(GetLevel()).." "..PTK_NAMES[choices[1]]..". Ta tÆng mét TiÓu Hång §¬n; hoµn thµnh nhËn 100 kinh nghiÖm.","ptk_take1","ptk_close")
    else
        Say("KÕ Tôc: chän môc tiªu. CÇn diÖt "..PTK_Count(GetLevel()).." qu¸i; hoµn thµnh nhËn 100 kinh nghiÖm. H«m nay ®· nhËn "..GetTask(984).."/5 lÇn.",3,
            PTK_NAMES[choices[1]].."/ptk_take1",PTK_NAMES[choices[2]].."/ptk_take2","§ãng/ptk_close")
    end
end
function ptk_take1() PTK_Take(1) end
function ptk_take2() PTK_Take(2) end
function PTK_Take(choice)
    if PTK_Show()~=1 or GetLevel()<1 then return end
    PTK_ResetDay()
    if GetTask(993)~=0 or GetTask(984)>=5 then ptk_open() return end
    local target=PTK_TARGETS[GetPlayerType()][choice]
    if not target or (GetLevel()<=5 and choice~=1) then return end
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
--description: ÌÇ´×Óã-Ã÷Öé°µÍ¶ÈÎÎñ
--author: chensong
--date: 2004/7/13

function OnDeath(sel)
	UTask_cg_1 = GetTask(41);
	i=random(1,3); 
	if  (i>=3) then
		if(UTask_cg_1==20)then
			if AddEventItem(35)<=0 then return end
			Msg2Player("Thu ®­îc Thè Ng­ huyÕt.")
			TaskNote(20,5)
			SetTask(41,UTask_cg_1+4)
		end;
		if(UTask_cg_1==21)then
			if AddEventItem(35)<=0 then return end
			Msg2Player("Thu ®­îc Thè Ng­ huyÕt.")
			TaskNote(20,9)
			SetTask(41,UTask_cg_1+4)
		end;
		if(UTask_cg_1==22)then
			if AddEventItem(35)<=0 then return end
			Msg2Player("Thu ®­îc Thè Ng­ huyÕt.")
			TaskNote(20,8)
			SetTask(41,UTask_cg_1+4)
		end;
		if(UTask_cg_1==23)then
			if AddEventItem(35)<=0 then return end
			Msg2Player("Thu ®­îc Thè Ng­ huyÕt.")
			TaskNote(20,11)
			SetTask(41,UTask_cg_1+4)
		end;
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

-- PT_SHARED_QUESTS_20261024
PTS_ACTIONS={{role="west",title="Phi Tiªn",level=39,task=51,from=0,to=1,note="GÆp ThÇy T­íng Sè, T©y Kú (174,188).",needs={},gifts={}},{role="west_boi",title="Phi Tiªn",level=39,task=51,from=1,to=2,note="Mang B¸ L¹c Nh·n ®Õn hái Cao Minh, Xi V­u Mé (206,198).",needs={},gifts={{3,29,0,0,0,0,1}},cost=1000},{role="cao",title="Phi Tiªn",level=39,task=51,from=2,to=3,note="DiÖt Tr­ Tinh ë Hoang M¹c: (187,186), (183,192), (183,188), (187,189). LÊy B¨ng Tinh vÒ Ng­êi T©y Vùc, T©y Kú (185,192).",needs={},gifts={}},{role="west",title="Phi Tiªn",level=39,task=51,from=4,to=5,note="§· më hîp thµnh thó c­ìi. Cã thÓ t×m thªm B¨ng Tinh ®Ó tiÕp tôc hîp thµnh.",needs={{4,37,0,0,0,0,1}},gifts={},finish=1},{role="tieu",title="Vi Lao",level=57,task=50,from=0,to=1,note="Hái ¤ng L·o H¸i Thuèc, T©y Kú (178,192).",needs={},gifts={}},{role="herbalist",title="Vi Lao",level=57,task=50,from=1,to=2,note="DiÖt Cù Th¹ch ë Thñ D­¬ng S¬n (151,175), (134,185), (134,204), lÊy Huyªn Th¶o vÒ Vâ C¸t, T©y Kú (181,195).",needs={},gifts={}},{role="vocat",title="Vi Lao",level=57,task=50,from=3,to=4,note="§Õn XÝch Tinh Tö, Ngäc H­ Cung (213,197).",needs={{4,38,0,0,0,0,1}},gifts={}},{role="xich",title="Vi Lao",level=57,task=50,from=4,to=5,note="T×m 1 LiÔu Méc, 1 C«n L«n KÝnh, 2 BÊt DiÖt ThÇn §¨ng. Thi Thó: BÝch Du Cung tÇng 2 (212,211), (196,213), (190,219). Mua ®Ìn ë Cao Gi¸c, Xi V­u Mé (205,197).",needs={},gifts={}},{role="xich",title="Vi Lao",level=57,task=50,from=5,to=6,note="Mang b¶o vËt ®Õn Vâ C¸t, T©y Kú (181,195).",needs={{4,39,0,0,0,0,1},{4,40,0,0,0,0,1},{4,41,0,0,0,0,2}},gifts={},keep=1},{role="vocat",title="Vi Lao",level=57,task=50,from=6,to=7,note="Trë l¹i XÝch Tinh Tö, Ngäc H­ Cung (213,197), nhËn phÇn th­ëng.",needs={{4,39,0,0,0,0,1},{4,40,0,0,0,0,1},{4,41,0,0,0,0,2}},gifts={}},{role="xich",title="Vi Lao",level=57,task=50,from=7,to=8,note="NhËn 5000 kinh nghiÖm, 3-5 Hång Thñy Tinh vµ 1 ®Ìn cßn thõa. Vâ C¸t cã thÓ nhê gióp lÇn n÷a.",needs={},gifts={{4,41,0,0,0,0,1}},xp=5000,crystals=1,finish=1},{role="vocat",title="Vi Lao",level=57,task=50,from=8,to=4,note="§Õn XÝch Tinh Tö, Ngäc H­ Cung (213,197), gióp Vâ C¸t lÇn n÷a.",needs={},gifts={}},{role="nham_tau",title="Phu Thª",level=27,task=92,from=0,to=1,note="GÆp D­¬ng TiÔn, T©y Kú (165,185), xin BiÕn Th©n Phï.",needs={},gifts={}},{role="duong",title="Phu Thª",level=27,task=92,from=1,to=2,note="Dïng BiÕn Th©n Phï thµnh Phi Thö vµ Ngäc N÷, lÇn l­ît gÆp NhËm §¹i Ca, T©y Kú (170,195).",needs={},gifts={{6,1,11,1,0,0,3}}},{role="nham_ca",title="Phu Thª",level=27,task=92,from=2,to=3,note="BiÕn thµnh Ngäc N÷, gÆp NhËm §¹i Ca (170,195).",needs={},gifts={},morph=50},{role="nham_ca",title="Phu Thª",level=27,task=92,from=2,to=4,note="BiÕn thµnh Phi Thö, gÆp NhËm §¹i Ca (170,195).",needs={},gifts={},morph=34},{role="nham_ca",title="Phu Thª",level=27,task=92,from=3,to=5,note="BiÕn thµnh biÓu t­îng t×nh yªu, gÆp NhËm §¹i TÈu (170,195).",needs={},gifts={},morph=34},{role="nham_ca",title="Phu Thª",level=27,task=92,from=4,to=5,note="BiÕn thµnh biÓu t­îng t×nh yªu, gÆp NhËm §¹i TÈu (170,195).",needs={},gifts={},morph=50},{role="nham_tau",title="Phu Thª",level=27,task=92,from=5,to=6,note="Hoµn thµnh: 20 søc lùc vµ 5000 kinh nghiÖm.",needs={},gifts={},morph=249,weight=20,xp=5000,finish=1},{role="tong",title="Yªn Phóc",level=23,task=91,from=0,to=1,note="Hái ThÇy T­íng Sè, TriÒu Ca (214,195), vÒ tÊm quÎ.",needs={},gifts={}},{role="east_boi",title="Yªn Phóc",level=23,task=91,from=1,to=2,note="Trë l¹i Tèng DÞ Nh©n, TriÒu Ca (191,189), chän nãi thËt hoÆc mua quÎ.",needs={},gifts={}},{role="tong",title="Yªn Phóc",level=23,task=91,from=2,to=5,note="Nãi thËt, nhËn 5000 l­îng.",needs={},gifts={},money=5000,finish=1,label="Nãi thËt"},{role="tong",title="Yªn Phóc",level=23,task=91,from=2,to=3,note="Mua quÎ gi¸ 300 l­îng, mang ®Õn ThÇy T­íng Sè, TriÒu Ca (214,195).",needs={},gifts={{6,1,12,1,0,0,1}},cost=300,label="Mua quÎ"},{role="east_boi",title="Yªn Phóc",level=23,task=91,from=3,to=4,note="QuÎ kh«ng thuéc vÒ ng­¬i. BÞ trõ 1 danh väng.",needs={{6,1,12,1,0,0,1}},gifts={},credit=-1,finish=1},{role="gambler",title="Minh Ch©u",level=43,task=41,from=0,to=1,note="Hái Chñ TiÖm CÇm §å, TriÒu Ca (209,198), vÒ §Þnh H¶i B¶o Ch©u.",needs={},gifts={}},{role="pawn",title="Minh Ch©u",level=43,task=41,from=1,to=2,note="Mang B¶o Ch©u ®Õn Vâ S­ ë Phong ThÇn §µi (182,203).",needs={},gifts={{4,33,0,0,0,0,1}}},{role="library",title="Minh Ch©u",level=43,task=41,from=2,to=3,note="GÆp Cï L­u T«n, TriÒu Ca (206,197).",needs={{4,33,0,0,0,0,1}},gifts={},keep=1},{role="culuu",title="Minh Ch©u",level=43,task=41,from=3,to=20,note="DiÖt ba c¸ ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Mang ba ng­ huyÕt, 3 S¬n Th¹ch vµ 10 §ång Thau vÒ Cï L­u T«n (206,197).",needs={{4,33,0,0,0,0,1}},gifts={},keep=1},{role="pawn",title="Minh Ch©u",level=43,task=41,from=10,to=11,note="Mang B¶o Ch©u vÒ Cï L­u T«n, TriÒu Ca (206,197), kh«i phôc linh lùc lÇn n÷a.",needs={},gifts={{4,33,0,0,0,0,1}}},{role="culuu",title="Minh Ch©u",level=43,task=41,from=11,to=20,note="Thu thËp ba ng­ huyÕt ë §«ng H¶i, 3 S¬n Th¹ch vµ 10 §ång Thau, mang vÒ Cï L­u T«n (206,197).",needs={{4,33,0,0,0,0,1}},gifts={},keep=1},{role="culuu",title="Minh Ch©u",level=43,task=41,from=27,to=10,note="NhËn 5000 l­îng, cã c¬ héi nhËn Hång B¶o Th¹ch. Cã thÓ nhËn B¶o Ch©u míi ë TiÖm CÇm §å.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},gifts={},money=5000,ruby=1,finish=1},{role="gambler",title="Minh Ch©u",level=43,task=41,from=0,to=0,note="§· cho con b¹c vay 100 l­îng. Muèn t×m hiÓu B¶o Ch©u, h·y tõ chèi cho vay ë lÇn nãi chuyÖn tiÕp theo.",needs={},gifts={},cost=100,label="Cho vay",loan=1}}
PTS_JOURNALS={{task=41,header="3024",title="Minh Chu ƒm §Çu",phases={[0]={note="§· cho con b¹c vay 100 l­îng. Muèn t×m hiÓu B¶o Ch©u, h·y tõ chèi cho vay ë lÇn nãi chuyÖn tiÕp theo.",needs={},names={}},[1]={note="Hái Chñ TiÖm CÇm §å, TriÒu Ca (209,198), vÒ §Þnh H¶i B¶o Ch©u.",needs={},names={}},[2]={note="Mang B¶o Ch©u ®Õn Vâ S­ ë Phong ThÇn §µi (182,203).",needs={{4,33,0,0,0,0,1}},names={"§Þnh H¶i b¶o ch©u"}},[3]={note="GÆp Cï L­u T«n, TriÒu Ca (206,197).",needs={{4,33,0,0,0,0,1}},names={"§Þnh H¶i b¶o ch©u"}},[10]={note="Cã thÓ ®Õn Chñ TiÖm CÇm §å, TriÒu Ca (209,198), nhËn B¶o Ch©u míi.",needs={},names={}},[11]={note="Mang B¶o Ch©u vÒ Cï L­u T«n, TriÒu Ca (206,197), kh«i phôc linh lùc lÇn n÷a.",needs={{4,33,0,0,0,0,1}},names={"§Þnh H¶i b¶o ch©u"}},[20]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[21]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[22]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[23]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[24]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[25]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[26]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}},[27]={note="Thu ng­ huyÕt ë §«ng H¶i: Chóc Ng­ (225,207), Thè Ng­ (222,179), ThÓ Ng­ (204,177). Tr¶ t¹i Cï L­u T«n, TriÒu Ca (206,197), kÌm 3 S¬n Th¹ch vµ 10 §ång Thau.",needs={{4,33,0,0,0,0,1},{4,34,0,0,0,0,1},{4,35,0,0,0,0,1},{4,36,0,0,0,0,1},{3,82,0,0,0,0,3},{3,3,0,0,0,0,10}},names={"§Þnh H¶i b¶o ch©u","Chóc Ng­ huyÕt","Thè Ng­ huyÕt","ThÓ Ng­ huyÕt","S¬n th¹ch","§ång thau"}}}},{task=50,header="3021",title="Häa §Þa Vi Lao",phases={[1]={note="Hái ¤ng L·o H¸i Thuèc, T©y Kú (178,192).",needs={},names={}},[2]={note="DiÖt Cù Th¹ch ë Thñ D­¬ng S¬n (151,175), (134,185), (134,204), lÊy Huyªn Th¶o vÒ Vâ C¸t, T©y Kú (181,195).",needs={{4,38,0,0,0,0,1}},names={"Huyªn th¶o"}},[3]={note="Mang Huyªn Th¶o ®Õn Vâ C¸t, T©y Kú (181,195).",needs={{4,38,0,0,0,0,1}},names={"Huyªn th¶o"}},[4]={note="§Õn XÝch Tinh Tö, Ngäc H­ Cung (213,197), gióp Vâ C¸t lÇn n÷a.",needs={},names={}},[5]={note="T×m 1 LiÔu Méc, 1 C«n L«n KÝnh, 2 BÊt DiÖt ThÇn §¨ng. Thi Thó: BÝch Du Cung tÇng 2 (212,211), (196,213), (190,219). Mua ®Ìn ë Cao Gi¸c, Xi V­u Mé (205,197).",needs={{4,39,0,0,0,0,1},{4,40,0,0,0,0,1},{4,41,0,0,0,0,2}},names={"LiÔu méc","C«n L«n kÝnh","§Ìn thÇn"}},[6]={note="Mang b¶o vËt ®Õn Vâ C¸t, T©y Kú (181,195).",needs={{4,39,0,0,0,0,1},{4,40,0,0,0,0,1},{4,41,0,0,0,0,2}},names={"LiÔu méc","C«n L«n kÝnh","§Ìn thÇn"}},[7]={note="Trë l¹i XÝch Tinh Tö, Ngäc H­ Cung (213,197), nhËn phÇn th­ëng.",needs={},names={}}}},{task=51,header="3020",title="Thiªn Ngo¹i Phi Tiªn",phases={[1]={note="GÆp ThÇy T­íng Sè, T©y Kú (174,188).",needs={},names={}},[2]={note="Mang B¸ L¹c Nh·n ®Õn hái Cao Minh, Xi V­u Mé (206,198).",needs={},names={}},[3]={note="DiÖt Tr­ Tinh ë Hoang M¹c: (187,186), (183,192), (183,188), (187,189). LÊy B¨ng Tinh vÒ Ng­êi T©y Vùc, T©y Kú (185,192).",needs={{4,37,0,0,0,0,1}},names={"M¶nh L­u tinh"}},[4]={note="Mang B¨ng Tinh vÒ Ng­êi T©y Vùc, T©y Kú (185,192), më hîp thµnh thó c­ìi.",needs={{4,37,0,0,0,0,1}},names={"M¶nh L­u tinh"}}}},{task=91,header="3023",title="Yªn Phóc",phases={[1]={note="Hái ThÇy T­íng Sè, TriÒu Ca (214,195), vÒ tÊm quÎ.",needs={},names={}},[2]={note="Trë l¹i Tèng DÞ Nh©n, TriÒu Ca (191,189), chän nãi thËt hoÆc mua quÎ.",needs={},names={}},[3]={note="Mua quÎ gi¸ 300 l­îng, mang ®Õn ThÇy T­íng Sè, TriÒu Ca (214,195).",needs={{6,1,12,1,0,0,1}},names={"VËt phÈm"}}}},{task=92,header="3022",title="Nh©n Chi T­¬ng Tö",phases={[1]={note="GÆp D­¬ng TiÔn, T©y Kú (165,185), xin BiÕn Th©n Phï.",needs={},names={}},[2]={note="Dïng BiÕn Th©n Phï thµnh Phi Thö vµ Ngäc N÷, lÇn l­ît gÆp NhËm §¹i Ca, T©y Kú (170,195).",needs={},names={}},[3]={note="BiÕn thµnh Ngäc N÷, gÆp NhËm §¹i Ca (170,195).",needs={},names={}},[4]={note="BiÕn thµnh Phi Thö, gÆp NhËm §¹i Ca (170,195).",needs={},names={}},[5]={note="BiÕn thµnh biÓu t­îng t×nh yªu, gÆp NhËm §¹i TÈu (170,195).",needs={},names={}}}}}
PTS_MOUNTS={{0,1,29,5000},{0,2,30,5000},{0,3,31,5000},{0,4,32,5000},{0,5,33,5000},{0,6,42,5000},{0,7,43,5000},{0,8,44,5000},{0,9,45,5000},{0,10,46,5000},{3,10,47,5003},{4,10,48,5004},{5,10,49,5005},{6,10,50,5006},{7,10,51,5007},{8,10,52,5008}}
PTS_TOWERS={{1006,217,196,0,"B¾c H¶i"},{1007,213,195,1,"YÕn S¬n"},{1012,211,207,2,"Miªu C­¬ng"},{1013,225,188,3,"Cù Léc"},{1010,151,175,4,"Thñ D­¬ng S¬n"},{1009,200,200,5,"T©y C«n L«n"}}
PTS_MATERIALS={[22]="§Þa T©m",[23]="Phong LÖ",[24]="Thñy Hån",[25]="Háa Linh"}
PTS_FISH={[110]=34,[111]=35,[112]=36}
PTS_FISH_BITS={[110]=1,[111]=4,[112]=2}
-- Common side quests keep the original saved task phases for all three classes.
function PTSMap() return GetContentWorldPos()+1000 end
function PTSClass() return GetPlayerType()>=0 and GetPlayerType()<=2 end
function PTSClose() CloseDialog() end
function PTSNotice(text) SayButtons(text,0,{}) end
function PTSHave(needs)
    for n=1,getn(needs) do local r=needs[n];if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end end
    return 1
end
function PTSReady(a)
    return PTSClass() and PTSMap()==PTS_MAP and a.role==PTS_ROLE and GetLevel()>=a.level and GetTask(a.task)==a.from and (not a.morph or GetMorphType()==a.morph) and PTSHave(a.needs)==1
end
function PTSAddChoices(options)
    for i=1,getn(PTS_ACTIONS) do local a=PTS_ACTIONS[i];if PTSReady(a) then tinsert(options,(a.label or a.title).."/pts_preview"..i) end end
    if PTS_ROLE=="west" then
        if GetLevel()>=19 then tinsert(options,"Trïng Méc/pts_mount") end
        if GetLevel()>=39 and (GetTask(51)==3 or GetTask(51)==5) and HaveEventItem(37)>0 then tinsert(options,"Hîp thó c­ìi/pts_fuse") end
    end
    if PTS_ROLE=="duong" and GetLevel()>=59 then
        local value=GetTask(53);local count=GetByte(value,2);local state=GetByte(value,1)
        if count<36 and (state==0 or state==3 and HaveEventItem(44)>0) then tinsert(options,"Quy Tinh/pts_stars") end
        if state==2 and count<36 then tinsert(options,"Gäi l¹i tinh/pts_star_retry") end
    end
    if PTS_ROLE=="store_west" or PTS_ROLE=="store_east" then
        local target=GetTask(55)
        if GetLevel()>=31 and (target==0 or target>=22 and target<=25 and HaveNormalItem(3,target,0,0)>=10) then tinsert(options,"Tø T­îng/pts_elements") end
    end
    if PTS_ROLE=="caogiac" and GetTask(50)>=4 and GetTask(50)<=6 and HaveEventItem(41)<2 then tinsert(options,"Mua ®Ìn/pts_buy_lamp") end
end
function PTSMenu()
    if not PTSClass() or PTSMap()~=PTS_MAP then CloseDialog();return end
    PTSJournal()
    local choices={};PTSAddChoices(choices)
    if PTS_HAS_SERVICE==1 then tinsert(choices,"DÞch vô/pts_service") end
    SayButtons("<color=yellow>NhiÖm vô chung Tam Giíi<color><enter>ChØ hiÖn nhiÖm vô ®ñ cÊp vµ ®ñ ®iÒu kiÖn nhËn hoÆc tr¶. Xem b­íc ®ang lµm ë F11.",getn(choices),choices)
end
function pts_service() if PTS_OLD_MAIN then return PTS_OLD_MAIN() end end
function PTSPreview(i)
    local a=PTS_ACTIONS[i];if not PTSReady(a) then return end
    local text="<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>"
    if a.money then text=text.."<enter><color=green>Th­ëng "..a.money.." l­îng.<color>" end
    if a.xp then text=text.."<enter><color=green>Th­ëng "..a.xp.." kinh nghiÖm.<color>" end
    if a.cost then text=text.."<enter><color=yellow>Chi phÝ: "..a.cost.." l­îng.<color>" end
    SayButtons(text,1,"§ång ý/pts_apply"..i)
end
function PTSExchange(task,from,to,needs,gifts,money,cost)
    if GetTrade()~=0 or GetLockState()~=0 or GetCash()<(cost or 0) then return 0 end
    local old=GetCash()
    if money and money>0 then if old>2000000000-money then return 0 end;Earn(money);if GetCash()~=old+money then return 0 end end
    if cost and cost>0 then if Pay(cost)~=1 then if money and money>0 then Pay(money) end;return 0 end end
    if QuestExchange(task,from,to,needs,gifts,1)~=1 then
        if cost and cost>0 then Earn(cost) end
        if money and money>0 then Pay(money) end
        PTSNotice("Ch­a ®ñ vËt phÈm hoÆc hµnh trang ch­a ®ñ chç. NhiÖm vô vµ phÇn th­ëng ®­îc gi÷ nguyªn.");return 0
    end
    return 1
end
function PTSApply(i)
    local a=PTS_ACTIONS[i];if not PTSReady(a) then return end
    if a.loan then
        if GetTrade()~=0 or GetLockState()~=0 or GetCash()<a.cost then return end
        if Pay(a.cost)==1 then PTSNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>") end
        return
    end
    local needs=a.needs;if a.keep then needs={} end
    local gifts={};for n=1,getn(a.gifts) do tinsert(gifts,a.gifts[n]) end
    if a.crystals then local roll=RANDOM(1,10);tinsert(gifts,{3,28,0,0,0,0,roll<=5 and 3 or (roll<=9 and 4 or 5)}) end
    if a.ruby and RANDOM(1,4)==1 then tinsert(gifts,{3,79,0,0,0,0,1}) end
    if PTSExchange(a.task,a.from,a.to,needs,gifts,a.money,a.cost)~=1 then return end
    if a.weight then AddWeightMax(a.weight) end
    if a.xp then AddOwnExp(a.xp) end
    if a.credit and a.credit<0 then DecCredit(-a.credit) end
    if a.task==50 and a.to==5 then SetTask(52,1) end
    if a.task==50 and a.to==8 then SetTask(52,0) end
    PTSJournal();PTSNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>")
end
function PTSMountRecipe()
    for n=1,getn(PTS_MOUNTS) do local r=PTS_MOUNTS[n]
        if HaveNormalItem(0,10,r[1],r[2])>0 and HaveNormalItem(3,r[3],0,0)>0 then return r end
    end
end
function pts_mount()
    if not PTSClass() or PTSMap()~=1020 or GetLevel()<19 or PTS_ROLE~="west" then return end
    local r=PTSMountRecipe()
    if not r or HaveEventItem(39)<3 then PTSNotice("<color=yellow>Trïng Méc<color><enter>CÇn 3 LiÔu Méc, thó c­ìi tr¾ng vµ B¸ L¹c Nh·n cïng cÊp trong hµnh trang.");return end
    SayButtons("<color=yellow>Trïng Méc<color><enter>§æi 3 LiÔu Méc, thó c­ìi tr¾ng vµ B¸ L¹c Nh·n cïng cÊp thµnh thó c­ìi xanh.",1,"Hîp thµnh/pts_mount_apply")
end
function pts_mount_apply()
    if not PTSClass() or PTSMap()~=1020 or GetLevel()<19 or PTS_ROLE~="west" then return end
    local r=PTSMountRecipe();if not r then return end
    local phase=GetTask(4570)
    if PTSExchange(4570,phase,1-phase,{{4,39,0,0,0,0,3},{0,10,r[1],r[2],0,0,1},{3,r[3],0,0,0,0,1}},{{0,10,r[4],r[2],0,0,1}},nil,nil)==1 then PTSNotice("<color=green>§· hîp thµnh thó c­ìi xanh cïng cÊp.<color>") end
end
function pts_fuse()
    if not PTSClass() or PTSMap()~=1020 or PTS_ROLE~="west" or GetLevel()<39 or (GetTask(51)~=3 and GetTask(51)~=5) then return end
    local r=PTSMountRecipe();if not r then return end
    local phase=GetTask(51)
    if PTSExchange(51,phase,6,{{4,37,0,0,0,0,1},{0,10,r[1],r[2],0,0,1},{3,r[3],0,0,0,0,1}},{{0,10,r[4],r[2],0,0,1}},nil,nil)==1 then SetTask(51,3);PTSJournal() end
end
function pts_buy_lamp()
    if PTSMap()~=1004 or PTS_ROLE~="caogiac" or GetTask(50)<4 or GetTask(50)>6 or HaveEventItem(41)>=2 then return end
    local phase=GetTask(4571)
    if PTSExchange(4571,phase,1-phase,{},{{4,41,0,0,0,0,1}},nil,5000)==1 then PTSJournal() end
end
function pts_elements()
    if not PTSClass() or GetLevel()<31 or PTSMap()~=PTS_MAP or (PTS_ROLE~="store_west" and PTS_ROLE~="store_east") then return end
    local target=GetTask(55)
    if target==0 then
        local next=21+RANDOM(1,4)
        if PTSExchange(55,0,next,{},{},nil,nil)==1 then SetTask(54,1);PTSJournal() end
    elseif target>=22 and target<=25 and HaveNormalItem(3,target,0,0)>=10 then
        local gifts={};local roll=RANDOM(1,100000);local green=0
        if roll<=10000 then gifts={{0,4,RANDOM(6,13),3,0,0,1}}
        elseif roll<=15000 then local part=RANDOM(3,5);gifts={{0,4+RANDOM(1,3),part,4,2+5*(part-3),0,1}};green=1 end
        if PTSExchange(55,target,0,{{3,target,0,0,0,0,10}},gifts,1200,nil)==1 then AddCredit(5);SetTask(54,0);if green==1 then SetTask(811,GetTask(811)+1) end;PTSJournal();PTSNotice("<color=green>NhËn 1200 l­îng, 5 danh väng vµ phÇn th­ëng ngÉu nhiªn nÕu tróng.<color>") end
    end
end
function pts_stars()
    if not PTSClass() or PTSMap()~=1020 or PTS_ROLE~="duong" or GetLevel()<59 then return end
    local value=GetTask(53);local count=GetByte(value,2);local state=GetByte(value,1)
    if count>=36 then return end
    if state==0 then
        PTSExchange(53,value,SetByte(value,1,1),{},{{4,42,0,0,0,0,1}},nil,nil)
    elseif state==3 and HaveEventItem(44)>0 then
        local gifts={};local xp=0;local credit=1
        if count==35 then local p=GetPlayerType();local part=p==0 and (RANDOM(1,200)<100 and 31 or 32) or (p==1 and 33 or 34);gifts={{0,0,part,6,0,0,1}};xp=500000;credit=108 end
        if PTSExchange(53,value,SetByte(SetByte(value,1,0),2,count+1),{{4,44,0,0,0,0,1}},gifts,nil,nil)==1 then AddCredit(credit);if xp>0 then AddOwnExp(xp) end end
    end
    PTSJournal()
end
function pts_star_retry()
    if PTSMap()~=1020 or PTS_ROLE~="duong" then return end
    local value=GetTask(53)
    if GetByte(value,1)==2 then PTSExchange(53,value,SetByte(value,1,1),{{4,43,0,0,0,0,1}},{{4,42,0,0,0,0,1}},nil,nil);PTSJournal() end
end
function pts_tower()
    if not PTSClass() or GetLevel()<59 or PTSMap()~=PTS_TOWER_MAP then return end
    local value=GetTask(53);local count=GetByte(value,2)
    if count>=36 or floor(count/6)~=PTS_TOWER_GROUP or GetByte(value,1)~=1 or HaveEventItem(42)<1 then return end
    local tid=123;local level=60;if mod(count,6)==5 then tid=124;level=80 end
    local npc=AddNpc(tid,level,SubWorld,PTS_TOWER_X*256+160,PTS_TOWER_Y*512+288,1)
    if not npc or npc<=0 then return end
    local names={{"Thiªn Kh«i Tinh","Thiªn S¸t Tinh","Thiªn C¬ Tinh","Thiªn Nhµn Tinh","Thiªn Dòng Tinh","Thiªn Hïng Tinh"},{"Thiªn M·nh Tinh","Thiªn Uy Tinh","Thiªn Anh Tinh","Thiªn Quý Tinh","Thiªn Phóc Tinh","Thiªn M·n Tinh"},{"Thiªn Hå Tinh","Thiªn Th­¬ng Tinh","Thiªn LËp Tinh","Thiªn TiÖp Tinh","Thiªn ¸m Tinh","Thiªn H÷u Tinh"},{"Thiªn Kh«ng Tinh","Thiªn Tèc Tinh","Thiªn DÞ Tinh","Thiªn S¸t Tinh","Thiªn Vi Tinh","Thiªn Cøu Tinh"},{"Thiªn Tho¸i Tinh","Thiªn Thä Tinh","Thiªn KiÕm Tinh","Thiªn B×nh Tinh","Thiªn Téi Tinh","Thiªn Tæn Tinh"},{"Thiªn B¹i Tinh","Thiªn Lao Tinh","Thiªn HuÖ Tinh","Thiªn B¹o Tinh","Thiªn Khèc Tinh","Thiªn X¶o Tinh"}}
    SetNpcName(npc,names[floor(count/6)+1][mod(count,6)+1])
    if SetNpcDeathScript(npc,"\\script\\phongthan\\shared\\star_"..count..".lua")~=1 then DelNpc(npc);return end
    if PTSExchange(53,value,SetByte(value,1,2),{{4,42,0,0,0,0,1}},{{4,43,0,0,0,0,1}},nil,nil)~=1 then DelNpc(npc) end
    PTSJournal();CloseDialog()
end

function PTSJournal()
    if not PTSClass() then return end
    for id=20,26 do AddNote(tostring(id),2," ",0) end
    for i=1,getn(PTS_JOURNALS) do
        local q=PTS_JOURNALS[i];local phase=GetTask(q.task);local row=q.phases[phase]
        if row then
            local text="<color=yellow>"..q.title.."<color><enter><color=cyan>"..row.note.."<color>"
            if getn(row.needs)>0 then
                text=text.."<enter><color=green>"
                for n=1,getn(row.needs) do local r=row.needs[n];local count=HaveNormalItem(r[1],r[2],r[3],r[4]);if count>r[7] then count=r[7] end
                    text=text..row.names[n].." "..count.."/"..r[7].."; "
                end
                text=text.."<color>"
            end
            AddNote(q.header,1,text,phase)
        else AddNote(q.header,2," ",phase) end
    end
    local value=GetTask(53);local count=GetByte(value,2);local state=GetByte(value,1)
    if count>0 and count<36 or state>0 and count<36 then
        local tower=PTS_TOWERS[floor(count/6)+1]
        local text="<color=yellow>Thiªn C­¬ng Tinh Quy VÞ<color><enter><color=green>§· thu phôc: "..count.."/36<color><enter>"
        if state==1 then text=text.."<color=cyan>Më Phong Än Th¸p t¹i "..tower[5].." ("..tower[2]..","..tower[3]..").<color>"
        elseif state==2 then text=text.."H¹ Thiªn C­¬ng Tinh võa th¶. NÕu thÊt l¹c, gÆp D­¬ng TiÔn (165,185), T©y Kú, gäi l¹i tinh."
        else text=text.."GÆp D­¬ng TiÔn, T©y Kú (165,185), ®Ó tr¶ ch©n khÝ hoÆc nhËn B¸ch Linh Ph­ín cho l­ît kÕ tiÕp." end
        AddNote("3025",1,text,value)
    else AddNote("3025",2," ",value) end
    local target=GetTask(55)
    if target>=22 and target<=25 then
        local count=HaveNormalItem(3,target,0,0);if count>10 then count=10 end
        AddNote("3026",1,"<color=yellow>Tø T­îng §ång Quy<color><enter><color=cyan>Thu thËp "..PTS_MATERIALS[target].." t¹i Môc D· (185,190)-(188,190), råi gÆp Thñ Khè ë T©y Kú (195,188) hoÆc TriÒu Ca (221,189).<color><enter><color=green>TiÕn ®é: "..count.."/10<color>",count)
    else AddNote("3026",2," ",0) end
end
function PTSDeath(template)
    if not PTSClass() then return end
    local map=PTSMap()
    if template==108 and map==1022 and GetTask(51)==3 then
        if HaveEventItem(37)<1 then AddEventItem(37) end
        if HaveEventItem(37)>0 then SetTask(51,4) end
    elseif template==104 and map==1010 and GetTask(50)==2 then
        if HaveEventItem(38)<1 then AddEventItem(38) end
        if HaveEventItem(38)>0 then SetTask(50,3) end
    elseif template==114 and map==1043 and GetTask(50)==5 then
        if HaveEventItem(40)<1 then AddEventItem(40) end
    elseif map==1018 and template>=7 and template<=10 and GetTask(55)==template+15 then
        local target=GetTask(55)
        if HaveNormalItem(3,target,0,0)<10 then AddNormalItem(3,target,0,0,0,0) end
    end
    if map==1037 and template>=110 and template<=112 and GetTask(41)>=20 and GetTask(41)<=27 then
        local item=PTS_FISH[template]
        if HaveEventItem(item)<1 and AddEventItem(item)>0 then
            local bit=PTS_FISH_BITS[template];local mask=GetTask(41)-20
            if mod(floor(mask/bit),2)==0 then SetTask(41,20+mask+bit) end
        end
    end
    PTSJournal()
end

PTS_OLD_DEATH=OnDeath
function OnDeath(index)
 local killer=PlayerIndex
 if not killer or killer<=0 or GetNpcWorldPos(index)~=PTSMap() then return end
 if PTS_OLD_DEATH then PTS_OLD_DEATH(index) end
 PlayerIndex=killer
 PTSDeath(111)
end

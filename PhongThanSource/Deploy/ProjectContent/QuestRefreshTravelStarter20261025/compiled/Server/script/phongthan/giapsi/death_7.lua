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

-- PT_GIAPSI_20261024_BEGIN
PTG_STEPS={{role="lo",title="T©n Thøc",level=3,task=21,from=0,to=1,note="GÆp ¢u Thiªn Hãa t¹i Sïng Thµnh Doanh.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=1,to=2,note="GÆp Sïng øng B­u.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="buu",title="T©n Thøc",level=3,task=21,from=2,to=3,note="Thu thËp 10 §o¶n KiÕm råi gÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=3,to=4,note="GÆp Sïng HÇu Hæ.",needs={{3,10,0,0,0,0,10}},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="hau",title="T©n Thøc",level=3,task=21,from=4,to=5,note="Trë l¹i gÆp Lç Hïng.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="lo",title="T©n Thøc",level=3,task=21,from=5,to=6,note="GÆp Sïng H¾c Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="hac",title="T©n Thøc",level=3,task=21,from=6,to=7,note="GÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=7,to=8,note="Trë l¹i gÆp Lç Hïng nhËn th­ëng.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="lo",title="T©n Thøc",level=3,task=21,from=8,to=9,note="Hoµn thµnh T©n Thøc.",needs={},gifts={},xp=500,money=600,credit=0,label="T©n Thøc"},{role="loan",title="Dòng §ao",level=7,task=25,from=0,to=1,note="DiÖt Hoµn CÈu V­¬ng t¹i YÕn S¬n (235,208), lÊy ®ñ ba bé phËn ®ao.",needs={},gifts={},xp=0,money=0,credit=0,label="Dòng §ao"},{role="dien",title="Dòng §ao",level=7,task=25,from=1,to=2,note="Nhê ¢u Thiªn Hãa söa Dòng §ao.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,keepNeeds=1,label="Dòng §ao"},{role="au",title="Dòng §ao",level=7,task=25,from=2,to=3,note="Mang Dòng §ao vÒ TriÒu §iÒn.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},gifts={{4,24,0,0,0,0,1}},xp=0,money=0,credit=0,label="Dòng §ao"},{role="dien",title="Dòng §ao",level=7,task=25,from=3,to=4,note="Hoµn thµnh Dòng §ao.",needs={{4,24,0,0,0,0,1}},gifts={{0,4,0,1,0,0,1}},xp=0,money=0,credit=10,label="Dòng §ao",rewardText="Ph¸p b¶o cÊp 10; 10 ®iÓm danh väng."},{role="loan",title="KiÕm ƒi",level=14,task=24,from=0,to=1,note="GÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="KiÕm ƒi"},{role="au",title="KiÕm ƒi",level=14,task=24,from=1,to=2,note="DiÖt Hoµn CÈu Tinh t¹i YÕn S¬n (213,187), (202,193), (213,194); vÒ Sïng øng Loan.",needs={},gifts={},xp=0,money=0,credit=0,label="KiÕm ƒi"},{role="loan",title="KiÕm ƒi",level=14,task=24,from=3,to=4,note="Hoµn thµnh KiÕm ƒi.",needs={{4,25,0,0,0,0,1}},gifts={{5,0,0,0,0,0,5},{7,58,62,1,0,0,1}},xp=3000,money=0,credit=0,camp=7,label="KiÕm ƒi",rewardText="5 Håi Thµnh Phï; s¸ch Bµn Cæ Khai Thiªn."},{role="hau",title="Trung Thµnh",level=25,task=3,from=0,to=1,note="GÆp TrÞnh Lu©n ë Sïng Thµnh Doanh.",needs={},gifts={},xp=0,money=0,credit=0,label="Trung Thµnh"},{role="trinh",title="Trung Thµnh",level=25,task=3,from=1,to=2,note="Mang HuyÕt Th­ vÒ Sïng HÇu Hæ.",needs={},gifts={{4,11,0,0,0,0,1}},xp=0,money=0,credit=0,label="Trung Thµnh"},{role="hau",title="Trung Thµnh",level=25,task=3,from=2,to=10,note="Hoµn thµnh Trung Thµnh.",needs={{4,11,0,0,0,0,1}},gifts={},xp=20000,money=30000,credit=0,label="Trung Thµnh"},{role="hac",title="TrÇm H­¬ng",level=35,task=3,from=10,to=11,note="Mang TrÇm H­¬ng Méc tíi Hoµng Phi Hæ ë TriÒu Ca.",needs={},gifts={{4,45,0,0,0,0,1}},xp=0,money=0,credit=0,label="TrÇm H­¬ng"},{role="hoang",title="TrÇm H­¬ng",level=35,task=3,from=11,to=20,note="Hoµn thµnh TrÇm H­¬ng.",needs={{4,45,0,0,0,0,1}},gifts={{7,59,128,1,0,0,1}},xp=80000,money=0,credit=0,label="TrÇm H­¬ng",rewardText="MËt tÞch Ban M«n Léng Phñ."},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=20,to=21,note="Hái chuyÖn Lý TÞnh vµ §Æng Cöu C«ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="ly",title="Khuyªn Hµng",level=45,task=3,from=21,to=22,note="GÆp §Æng Cöu C«ng t¹i Tam S¬n.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="dang",title="Khuyªn Hµng",level=45,task=3,from=21,to=23,note="GÆp Lý TÞnh t¹i TrÇn §­êng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="dang",title="Khuyªn Hµng",level=45,task=3,from=22,to=24,note="Trë vÒ Hoµng Phi Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="ly",title="Khuyªn Hµng",level=45,task=3,from=23,to=24,note="Trë vÒ Hoµng Phi Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=24,to=25,note="DiÖt TrÊn T­íng Qu©n t¹i Môc D· (187,189), lÊy Ên tÝn Vâ Thµnh V­¬ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=26,to=27,note="B¸o tin cho Kh­¬ng Tö Nha t¹i T©y Kú.",needs={{4,12,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="khuong",title="Khuyªn Hµng",level=45,task=3,from=27,to=30,note="Hoµn thµnh Khuyªn Hµng.",needs={},gifts={{0,0,31,4,0,0,1}},xp=240000,money=0,credit=0,label="Khuyªn Hµng",rewardText="Vò khÝ hoµng kim cÊp 40."},{role="duong",title="ThÇn Long",level=55,task=3,from=30,to=31,note="DiÖt ThÇn Long t¹i Long Cung tÇng 3 (206,175), lÊy R©u ThÇn Long.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Long"},{role="dac",title="ThÇn Long",level=55,task=3,from=32,to=33,note="Mang R©u ThÇn Long cho Hå Hû MÞ.",needs={{4,13,0,0,0,0,1}},gifts={{4,9,0,0,0,0,1}},xp=0,money=0,credit=0,keepNeeds=1,label="ThÇn Long"},{role="ho",title="ThÇn Long",level=55,task=3,from=33,to=40,note="Hoµn thµnh ThÇn Long.",needs={{4,13,0,0,0,0,1}},gifts={{3,46,0,0,0,0,1}},xp=600000,money=0,credit=0,label="ThÇn Long",rewardText="B¸ L¹c Nh·n cÊp 10."},{role="dac",title="B×nh An",level=65,task=3,from=40,to=41,note="GÆp T©y V­¬ng MÉu t¹i Diªu Tr×.",needs={},gifts={},xp=0,money=0,credit=0,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=3,from=41,to=42,note="DiÖt Cù D· t¹i BÝch Du Cung tÇng 4: (181,199), (196,204), (185,217); lÊy An C­ §å.",needs={},gifts={},xp=0,money=0,credit=0,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=3,from=43,to=50,note="Hoµn thµnh B×nh An.",needs={{4,3,0,0,0,0,1}},gifts={},xp=1400000,money=0,credit=0,randomBook=1,label="B×nh An",rewardText="S¸ch kü n¨ng ngÉu nhiªn cña ba hÖ ph¸i."},{role="nong",title="ThÇn Dô KÝnh",level=70,task=3,from=50,to=51,note="DiÖt Tr¹nh Nanh t¹i Khæn Tiªn Cung tÇng 1 (197,212), lÊy ThÇn Dô KÝnh.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Dô KÝnh"},{role="nong",title="ThÇn Dô KÝnh",level=70,task=3,from=52,to=60,note="Hoµn thµnh ThÇn Dô KÝnh. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",needs={{4,4,0,0,0,0,1}},gifts={{0,0,31,7,0,0,1}},xp=2000000,money=0,credit=0,keepNeeds=1,label="ThÇn Dô KÝnh",rewardText="Vò khÝ hoµng kim cÊp 70."},{role="dac",title="Tam S¸ch",level=75,task=3,from=60,to=61,note="Mang hai b¶ng Phong ThÇn tíi Kh­¬ng Tö Nha. Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={},gifts={},xp=0,money=0,credit=0,ensureBoard=1,label="Tam S¸ch"},{role="khuong",title="Tam S¸ch",level=75,task=3,from=61,to=62,note="Mang ThÇn Dô KÝnh tíi Vâ V­¬ng.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=3,from=62,to=63,note="LÊy Huúnh §Õ Néi Kinh ë tÇng 2 (196,189); LuyÖn Kim ThuËt ë tÇng 3 (206,204); HuyÒn N÷ Binh Ph¸p ë tÇng 4 (191,210), Khæn Tiªn Cung.",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=3,from=63,to=70,note="Hoµn thµnh Tam S¸ch.",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},gifts={{0,4,14,1,0,0,1},{0,4,15,1,0,0,1},{0,4,16,1,0,0,1},{0,4,17,1,0,0,1}},xp=3000000,money=0,credit=0,label="Tam S¸ch",rewardText="Mét bé bèn ph¸p b¶o cÊp 70."},{role="tay",title="Cæ Kim",level=80,task=3,from=70,to=71,note="GÆp §¾c Kû t¹i TriÒu Ca m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1063,198,200},label="Cæ Kim"},{role="dac_future",title="Cæ Kim",level=80,task=3,from=71,to=72,note="GÆp Nguyªn Thñy Thiªn T«n t¹i Ngäc H­ Cung m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1062,203,198},label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=3,from=72,to=73,note="DiÖt Bé §Çu qu©n Chu t¹i Ngäc H­ Cung m­êi n¨m sau (206,204), lÊy Tróc Gi¶n.",needs={},gifts={},xp=0,money=0,credit=0,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=3,from=74,to=80,note="Hoµn thµnh Cæ Kim. Dïng Khø Lai Phï ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",needs={{4,10,0,0,0,0,1}},gifts={{3,41,0,0,0,0,1},{6,1,34,1,0,0,1}},xp=5000000,money=0,credit=0,label="Cæ Kim",rewardText="Lam B¶o Th¹ch; Khø Lai Phï."},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=80,to=81,note="Nãi chuyÖn víi Kh­¬ng Tö Nha thêi niªn thiÕu.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="khuong_young",title="Thiªn Duyªn",level=85,task=3,from=81,to=82,note="Trë l¹i gÆp §¾c Kû thêi niªn thiÕu.",needs={},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=82,to=85,note="DiÖt Bµn Cæ t¹i Khæn Tiªn Cung tÇng 5 (199,212), lÊy Bµn Cæ ThÇn KhÝ.",needs={},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=85,to=83,note="Hoµn thµnh Thiªn Duyªn.",needs={{4,175,0,0,0,0,1}},gifts={{0,5,6,8,4,0,1}},xp=6500000,money=0,credit=0,label="Thiªn Duyªn",rewardText="Mét trang bÞ lôc cÊp 80."},{role="khuong_young",title="Th«ng Thiªn",level=92,task=3,from=83,to=84,note="Tham Lam: Th­¬ng Nh©n, §¹i Phong (200,200). V« Tri: Cöu Di, Tam S¬n (200,200). Tµn NhÉn: dïng MËt §å t¹i B¾c H¶i (190,206).",needs={},gifts={{6,1,142,1,0,0,1},{6,1,181,1,0,0,1}},xp=0,money=0,credit=0,label="Th«ng Thiªn"},{role="merchant",title="Th«ng Thiªn",level=92,task=4560,from=0,to=1,note="NhËn Tham Lam.",needs={},gifts={{4,176,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,label="Th«ng Thiªn"},{role="cuudi",title="Th«ng Thiªn",level=92,task=4561,from=0,to=1,note="NhËn V« Tri.",needs={},gifts={{4,177,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,label="Th«ng Thiªn"},{role="thong",title="Th«ng Thiªn",level=92,task=3,from=84,to=86,note="Dïng Khø Lai Phï gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Th«ng Thiªn"},{role="khuong_young",title="Th«ng Thiªn: nhËn giµy",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,5,6,9,5,0,1}},xp=0,money=0,credit=0,rewardChoice=1,label="NhËn giµy",rewardText="Giµy lôc cÊp 90."},{role="khuong_young",title="Th«ng Thiªn: nhËn yªu ®¸i",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,6,6,9,5,0,1}},xp=0,money=0,credit=0,rewardChoice=2,label="NhËn yªu ®¸i",rewardText="Yªu ®¸i lôc cÊp 90."},{role="khuong_young",title="Th«ng Thiªn: nhËn m·o",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,7,6,9,5,0,1}},xp=0,money=0,credit=0,rewardChoice=3,label="NhËn m·o",rewardText="M·o lôc cÊp 90."},{role="doctor_manh",title="Trõ Häa: M¹nh T©n",level=18,task=4550,from=0,to=1,note="DiÖt Hoµn CÈu t¹i M¹nh T©n (188,210) ®Õn (192,210), råi trong 10 gi©y diÖt Gi¸p Cèt ngay phÝa nam ®Ó lÊy ®ñ 5 H¹p §»ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_manh",title="Trõ Häa: tr¶ H¹p §»ng",level=18,task=4550,from=1,to=2,note="§Õn §¹i Phu t¹i Tam S¬n.",needs={{4,191,0,0,0,0,5}},gifts={},xp=6000,money=0,credit=0,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: Tam S¬n",level=18,task=4550,from=2,to=3,note="H¸i 10 Nguyªn S©m t¹i Tam S¬n, tõ (196,203) ®Õn (205,203). CÈn thËn Nguyªn Tham Tinh ë gÇn ®ã.",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: tr¶ Nguyªn S©m",level=18,task=4550,from=3,to=4,note="§Õn §¹i Phu t¹i §ång Quan.",needs={{4,192,0,0,0,0,10}},gifts={},xp=8000,money=0,credit=0,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: §ång Quan",level=18,task=4550,from=4,to=5,note="DiÖt Ng­u S¸t t¹i §ång Quan (195,208) ®Õn (199,208), dïng TÞnh Hãa Phï, råi tÞnh hãa 5 S¬n Xuyªn LiÔu ë hµng (195,211) ®Õn (199,211).",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: tr¶ S¬n Xuyªn LiÔu",level=18,task=4550,from=5,to=6,note="Hoµn thµnh Trõ Häa.",needs={{4,201,0,0,0,0,5}},gifts={{0,2,6,2,1,0,1}},xp=10000,money=0,credit=0,label="Trõ Häa",rewardText="Mét trang bÞ lôc."}}
PTG_KILLS={{576,1007,24,2,3,25},{122,1018,3,25,26,12},{113,1039,3,31,32,13},{121,1045,3,42,43,3},{120,1047,3,51,52,4},{117,1048,3,63,63,7},{118,1049,3,63,63,8},{119,1050,3,63,63,6},{116,1062,3,73,74,10},{82,1051,3,85,85,175}}
PTG_BOOKS={{7,16,19,1,0,0,1},{7,17,20,1,0,0,1},{7,18,21,1,0,0,1},{7,19,22,1,0,0,1},{7,33,36,1,0,0,1},{7,34,37,1,0,0,1},{7,35,38,1,0,0,1},{7,36,39,1,0,0,1},{7,45,48,1,0,0,1},{7,46,49,1,0,0,1},{7,55,456,1,0,0,1},{7,60,1481,1,0,0,1},{7,66,1487,1,0,0,1},{7,81,1548,1,0,0,1},{7,83,1551,1,0,0,1}}
PTG_TARGET_NAMES={[3]="KiÕm Nh©n hoÆc X¹ Nh©n",[7]="Hoµn CÈu"}
PTG_TARGET_MAPS={[3]="YÕn S¬n (204,224), (227,224), qu¸i s½n cã ngoµi Sïng Thµnh",[7]="YÕn S¬n (235,226), qu¸i s½n cã ngoµi Sïng Thµnh"}
PTG_JOURNAL={{task=3,header="3004",phases={[1]={title="Trung Thµnh",note="GÆp TrÞnh Lu©n, Sïng Thµnh Doanh (200,203) ë Sïng Thµnh Doanh.",needs={},needNames={}},[2]={title="Trung Thµnh",note="Mang HuyÕt Th­ vÒ Sïng HÇu Hæ, Sïng Thµnh Doanh (212,193).",needs={{4,11,0,0,0,0,1}},needNames={"HuyÕt th­"}},[10]={title="Trung Thµnh",note="Hoµn thµnh Trung Thµnh. §¹t cÊp 35, gÆp Sïng H¾c Hæ, Sïng Thµnh Doanh (213,200) nhËn TrÇm H­¬ng.",needs={},needNames={}},[11]={title="TrÇm H­¬ng",note="Mang TrÇm H­¬ng Méc tíi Hoµng Phi Hæ, TriÒu Ca (231,185) ë TriÒu Ca.",needs={{4,45,0,0,0,0,1}},needNames={"Gç trÇm h­¬ng"}},[20]={title="TrÇm H­¬ng",note="Hoµn thµnh TrÇm H­¬ng. §¹t cÊp 45, gÆp Hoµng Phi Hæ, TriÒu Ca (231,185) nhËn Khuyªn Hµng.",needs={},needNames={}},[21]={title="Khuyªn Hµng",note="Hái chuyÖn Lý TÞnh, TrÇn §­êng (193,207) vµ §Æng Cöu C«ng.",needs={},needNames={}},[22]={title="Khuyªn Hµng",note="GÆp §Æng Cöu C«ng t¹i Tam S¬n. Sau ®ã gÆp §Æng Cöu C«ng, Tam S¬n (204,197).",needs={},needNames={}},[23]={title="Khuyªn Hµng",note="GÆp Lý TÞnh, TrÇn §­êng (193,207) t¹i TrÇn §­êng.",needs={},needNames={}},[24]={title="Khuyªn Hµng",note="Trë vÒ Hoµng Phi Hæ, TriÒu Ca (231,185).",needs={},needNames={}},[25]={title="Khuyªn Hµng",note="DiÖt TrÊn T­íng Qu©n t¹i Môc D· (187,189), lÊy Ên tÝn Vâ Thµnh V­¬ng.",needs={},needNames={}},[26]={title="Khuyªn Hµng",note="§· lÊy ®­îc vËt phÈm. Mang vÒ Hoµng Phi Hæ, TriÒu Ca (231,185).",needs={{4,12,0,0,0,0,1}},needNames={"Ên tÝn cña Vâ Thµnh v­¬ng"}},[27]={title="Khuyªn Hµng",note="B¸o tin cho Kh­¬ng Tö Nha, T©y Kú (158,189) t¹i T©y Kú.",needs={},needNames={}},[30]={title="Khuyªn Hµng",note="Hoµn thµnh Khuyªn Hµng. §¹t cÊp 55, gÆp D­¬ng TiÔn, T©y Kú (165,185) nhËn ThÇn Long.",needs={},needNames={}},[31]={title="ThÇn Long",note="DiÖt ThÇn Long t¹i Long Cung tÇng 3 (206,175), lÊy R©u ThÇn Long.",needs={},needNames={}},[32]={title="ThÇn Long",note="§· lÊy ®­îc vËt phÈm. Mang vÒ §¾c Kû, TriÒu Ca (220,179).",needs={{4,13,0,0,0,0,1}},needNames={"R©u ThÇn long"}},[33]={title="ThÇn Long",note="Mang R©u ThÇn Long cho Hå Hû MÞ, TriÒu Ca (205,184).",needs={{4,13,0,0,0,0,1}},needNames={"R©u ThÇn long"}},[40]={title="ThÇn Long",note="Hoµn thµnh ThÇn Long. §¹t cÊp 65, gÆp §¾c Kû, TriÒu Ca (220,179) nhËn B×nh An.",needs={},needNames={}},[41]={title="B×nh An",note="GÆp T©y V­¬ng MÉu t¹i Diªu Tr×. Sau ®ã gÆp T©y V­¬ng MÉu, Diªu Tr× (221,188).",needs={},needNames={}},[42]={title="B×nh An",note="DiÖt Cù D· t¹i BÝch Du Cung tÇng 4: (181,199), (196,204), (185,217); lÊy An C­ §å.",needs={},needNames={}},[43]={title="B×nh An",note="§· lÊy ®­îc vËt phÈm. Mang vÒ T©y V­¬ng MÉu, Diªu Tr× (221,188).",needs={{4,3,0,0,0,0,1}},needNames={"An C­ ®å"}},[50]={title="B×nh An",note="Hoµn thµnh B×nh An. §¹t cÊp 70, gÆp ThÇn N«ng, Háa V©n §éng (194,191) nhËn ThÇn Dô KÝnh.",needs={},needNames={}},[51]={title="ThÇn Dô KÝnh",note="DiÖt Tr¹nh Nanh t¹i Khæn Tiªn Cung tÇng 1 (197,212), lÊy ThÇn Dô KÝnh.",needs={},needNames={}},[52]={title="ThÇn Dô KÝnh",note="§· lÊy ®­îc vËt phÈm. Mang vÒ ThÇn N«ng, Háa V©n §éng (194,191).",needs={{4,4,0,0,0,0,1}},needNames={"ThÇn Dô KÝnh"}},[60]={title="ThÇn Dô KÝnh",note="Hoµn thµnh ThÇn Dô KÝnh. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch. §¹t cÊp 75, gÆp §¾c Kû, TriÒu Ca (220,179) nhËn Tam S¸ch.",needs={},needNames={}},[61]={title="Tam S¸ch",note="Mang hai b¶ng Phong ThÇn tíi Kh­¬ng Tö Nha, T©y Kú (158,189). Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={{4,9,0,0,0,0,1}},needNames={"Phong ThÇn b¶ng"}},[62]={title="Tam S¸ch",note="Mang ThÇn Dô KÝnh tíi Vâ V­¬ng, T©y Kú (159,189).",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},needNames={"ThÇn Dô KÝnh","Phong ThÇn b¶ng"}},[63]={title="Tam S¸ch",note="LÊy Huúnh §Õ Néi Kinh ë tÇng 2 (196,189); LuyÖn Kim ThuËt ë tÇng 3 (206,204); HuyÒn N÷ Binh Ph¸p ë tÇng 4 (191,210), Khæn Tiªn Cung. Sau ®ã gÆp Vâ V­¬ng, T©y Kú (159,189).",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},needNames={"HuyÒn N÷ binh ph¸p","Huúnh §Õ néi kinh","LuyÖn Kim thuËt"}},[70]={title="Tam S¸ch",note="Hoµn thµnh Tam S¸ch. §¹t cÊp 80, gÆp T©y V­¬ng MÉu, Diªu Tr× (221,188) nhËn Cæ Kim.",needs={},needNames={}},[71]={title="Cæ Kim",note="GÆp §¾c Kû t¹i TriÒu Ca m­êi n¨m sau. Sau ®ã gÆp §¾c Kû, TriÒu Ca m­êi n¨m sau (198,200).",needs={},needNames={}},[72]={title="Cæ Kim",note="GÆp Nguyªn Thñy Thiªn T«n t¹i Ngäc H­ Cung m­êi n¨m sau. Sau ®ã gÆp Nguyªn Thñy Thiªn T«n, Ngäc H­ Cung m­êi n¨m sau (203,198).",needs={},needNames={}},[73]={title="Cæ Kim",note="DiÖt Bé §Çu qu©n Chu t¹i Ngäc H­ Cung m­êi n¨m sau (206,204), lÊy Tróc Gi¶n.",needs={},needNames={}},[74]={title="Cæ Kim",note="§· lÊy ®­îc vËt phÈm. Mang vÒ Nguyªn Thñy Thiªn T«n, Ngäc H­ Cung m­êi n¨m sau (203,198).",needs={{4,10,0,0,0,0,1}},needNames={"Tróc gi¶n"}},[80]={title="Cæ Kim",note="Hoµn thµnh Cæ Kim. Dïng Khø Lai Phï ®Õn Ngäc H­ Cung m­êi n¨m tr­íc. §¹t cÊp 85, gÆp §¾c Kû, Ngäc H­ Cung m­êi n¨m tr­íc (207,203) nhËn Thiªn Duyªn.",needs={{4,9,0,0,0,0,1}},needNames={"Phong ThÇn b¶ng"}},[81]={title="Thiªn Duyªn",note="Nãi chuyÖn víi Kh­¬ng Tö Nha thêi niªn thiÕu. Sau ®ã gÆp Kh­¬ng Tö Nha, Ngäc H­ Cung m­êi n¨m tr­íc (198,201).",needs={},needNames={}},[82]={title="Thiªn Duyªn",note="Trë l¹i gÆp §¾c Kû thêi niªn thiÕu. Sau ®ã gÆp §¾c Kû, Ngäc H­ Cung m­êi n¨m tr­íc (207,203).",needs={},needNames={}},[83]={title="Thiªn Duyªn",note="Hoµn thµnh Thiªn Duyªn. §¹t cÊp 92, gÆp Kh­¬ng Tö Nha, Ngäc H­ Cung m­êi n¨m tr­íc (198,201) nhËn Th«ng Thiªn.",needs={},needNames={}},[84]={title="Th«ng Thiªn",note="Tham Lam: Th­¬ng Nh©n, §¹i Phong (200,200). V« Tri: Cöu Di, Tam S¬n (200,200). Tµn NhÉn: dïng MËt §å t¹i B¾c H¶i (190,206). Sau ®ã gÆp Th«ng Thiªn Gi¸o Chñ, ViÔn Cæ Háa DiÖm (204,210).",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},needNames={"Tham lam","V« tri","Tµn nhÉn"}},[85]={title="Thiªn Duyªn",note="DiÖt Bµn Cæ t¹i Khæn Tiªn Cung tÇng 5 (199,212), lÊy Bµn Cæ ThÇn KhÝ. Sau ®ã gÆp §¾c Kû, Ngäc H­ Cung m­êi n¨m tr­íc (207,203).",needs={{4,175,0,0,0,0,1}},needNames={"Bµn Cæ thÇn khÝ"}},[86]={title="Th«ng Thiªn",note="Dïng Khø Lai Phï gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={},needNames={}},[87]={title="Th«ng Thiªn",note="DiÖt Thiªn Hån, §Þa Hån, MÖnh Hån t¹i ViÔn Cæ Háa DiÖm (202,209), (203,209), (204,209). §ñ ba hån sÏ b¾t ®Çu giíi h¹n 3 phót.",needs={},needNames={}},[88]={title="Th«ng Thiªn",note="Trong 3 phót kÓ tõ lóc h¹ ®ñ ba hån, bÊm Khiªu chiÕn ë Th«ng Thiªn Gi¸o Chñ, ViÔn Cæ Háa DiÖm (204,210), råi h¹ b¶n thÓ chiÕn ®Êu ®Ó lÊy Ngäc Thiªn C¬.",needs={},needNames={}},[89]={title="Th«ng Thiªn",note="Mang Ngäc Thiªn C¬ vÒ Kh­¬ng Tö Nha, Ngäc H­ Cung m­êi n¨m tr­íc (198,201); chän mét phÇn th­ëng.",needs={{4,174,0,0,0,0,1}},needNames={"Hµn B¨ng nguyªn khÝ"}}}},{task=21,header="3000",phases={[1]={title="T©n Thøc",note="GÆp ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193) t¹i Sïng Thµnh Doanh.",needs={},needNames={}},[2]={title="T©n Thøc",note="GÆp Sïng øng B­u, Sïng Thµnh Doanh (211,198).",needs={},needNames={}},[3]={title="T©n Thøc",note="Mua ®ñ 10 §o¶n KiÕm ë ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193). Sau ®ã giao cho ¢u Thiªn Hãa.",needs={{3,10,0,0,0,0,10}},needNames={"§o¶n KiÕm"}},[4]={title="T©n Thøc",note="GÆp Sïng HÇu Hæ, Sïng Thµnh Doanh (212,193).",needs={},needNames={}},[5]={title="T©n Thøc",note="Trë l¹i gÆp Lç Hïng, Sïng Thµnh Doanh (194,199).",needs={},needNames={}},[6]={title="T©n Thøc",note="GÆp Sïng H¾c Hæ, Sïng Thµnh Doanh (213,200).",needs={},needNames={}},[7]={title="T©n Thøc",note="GÆp ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193).",needs={},needNames={}},[8]={title="T©n Thøc",note="Trë l¹i gÆp Lç Hïng, Sïng Thµnh Doanh (194,199) nhËn th­ëng.",needs={},needNames={}}}},{task=24,header="3002",phases={[1]={title="KiÕm ƒi",note="GÆp ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193).",needs={},needNames={}},[2]={title="KiÕm ƒi",note="DiÖt Hoµn CÈu Tinh t¹i YÕn S¬n (213,187), (202,193), (213,194); vÒ Sïng øng Loan.",needs={},needNames={}},[3]={title="KiÕm ƒi",note="§· lÊy ®­îc vËt phÈm. Mang vÒ Sïng øng Loan, Sïng Thµnh Doanh (212,198).",needs={{4,25,0,0,0,0,1}},needNames={"Cuèc"}}}},{task=25,header="3001",phases={[1]={title="Dòng §ao",note="DiÖt Hoµn CÈu V­¬ng t¹i YÕn S¬n (235,208), lÊy ®ñ ba bé phËn ®ao. Tr¶ t¹i TriÒu §iÒn, Sïng Thµnh Doanh (218,198).",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},needNames={"L­ìi ®ao","Th©n ®ao","C¸n ®ao"}},[2]={title="Dòng §ao",note="Nhê ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193) söa Dòng §ao.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},needNames={"L­ìi ®ao","Th©n ®ao","C¸n ®ao"}},[3]={title="Dòng §ao",note="Mang Dòng §ao vÒ TriÒu §iÒn, Sïng Thµnh Doanh (218,198).",needs={{4,24,0,0,0,0,1}},needNames={"Dòng §ao"}}}},{task=4550,header="3003",phases={[1]={title="Trõ Häa",note="DiÖt Hoµn CÈu t¹i M¹nh T©n (188,210) ®Õn (192,210), råi trong 10 gi©y diÖt Gi¸p Cèt ngay phÝa nam ®Ó lÊy ®ñ 5 H¹p §»ng. Sau ®ã gÆp §¹i Phu, M¹nh T©n (192,212).",needs={{4,191,0,0,0,0,5}},needNames={"H¹p §»ng"}},[2]={title="Trõ Häa",note="§Õn §¹i Phu t¹i Tam S¬n. Sau ®ã gÆp §¹i Phu, Tam S¬n (204,200).",needs={},needNames={}},[3]={title="Trõ Häa",note="H¸i 10 Nguyªn S©m t¹i Tam S¬n, tõ (196,203) ®Õn (205,203). CÈn thËn Nguyªn Tham Tinh ë gÇn ®ã. Sau ®ã gÆp §¹i Phu, Tam S¬n (204,200).",needs={{4,192,0,0,0,0,10}},needNames={"Nguyªn S©m"}},[4]={title="Trõ Häa",note="§Õn §¹i Phu t¹i §ång Quan. Sau ®ã gÆp §¹i Phu, §ång Quan (197,210).",needs={},needNames={}},[5]={title="Trõ Häa",note="DiÖt Ng­u S¸t t¹i §ång Quan (195,208) ®Õn (199,208), dïng TÞnh Hãa Phï, råi tÞnh hãa 5 S¬n Xuyªn LiÔu ë hµng (195,211) ®Õn (199,211). Sau ®ã gÆp §¹i Phu, §ång Quan (197,210).",needs={{4,201,0,0,0,0,5}},needNames={"S¬n Xuyªn LiÔu"}}}}}
PTG_PROGRESS={3,4,6,7,8,10,11,12,13,21,22,23,25,174,175,176,177,178,191,192,201}
-- Literal records use dedicated headers; legacy journal templates stay intact.
function PTGJournalState()
    local value=""
    for i=1,getn(PTG_JOURNAL) do value=value..":"..GetTask(PTG_JOURNAL[i].task) end
    for i=1,getn(PTG_PROGRESS) do value=value..":"..HaveEventItem(PTG_PROGRESS[i]) end
    value=value..":"..GetTask(4554)..":"..GetTask(4555)..":"..GetTask(4562)
    return value
end
function PTGJournal()
    if GetPlayerType()~=0 then return end
    -- Keep the accepted target/count when moving old distant objectives home.
    if GetTask(4554)>0 and GetTask(4559)~=1007 then SetTask(4559,1007) end
    if GetTask(4554)==13 then SetTask(4554,3) end
    if GetTask(4554)==16 then SetTask(4554,7) end
    -- Retire the former technical "Task 27" placeholder.
    AddNote("27",2," ",0)
    for i=1,getn(PTG_JOURNAL) do
        local q=PTG_JOURNAL[i]
        local phase=GetTask(q.task)
        local row=q.phases[phase]
        if row then
            local text="<color=yellow>"..row.title.."<color><enter><color=cyan>"..row.note.."<color>"
            for n=1,getn(row.needs) do
                local need=row.needs[n]
                local count=HaveNormalItem(need[1],need[2],need[3],need[4])
                if count>need[7] then count=need[7] end
                text=text.."<enter><color=green>"..row.needNames[n]..": "..count.."/"..need[7].."<color>"
            end
            if q.task==3 and phase==87 then
                local mask=GetTask(4562)
                text=text.."<enter><color=green>Thiªn Hån "..mod(mask,2).."/1; §Þa Hån "..mod(floor(mask/2),2).."/1; MÖnh Hån "..mod(floor(mask/4),2).."/1<color>"
            end
            AddNote(q.header,1,text,phase)
        else AddNote(q.header,2," ",phase) end
    end
    if GetTask(4554)>0 then
        local text="<color=yellow>Gi¸o HuÊn<color><enter><color=cyan>DiÖt "..PTG_TARGET_NAMES[GetTask(4554)].." t¹i "..PTG_TARGET_MAPS[GetTask(4554)]..".<color><enter><color=green>TiÕn ®é: "..GetTask(4555).."/40<color><enter>Tr¶ t¹i T¹p Hãa Th­¬ng Nh©n, Sïng Thµnh Doanh (207,198)."
        AddNote("3005",1,text,GetTask(4555))
    else AddNote("3005",2," ",0) end
end

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
    SayButtons(text,1,"§ãng/ptg_close")
end
function ptg_close() CloseDialog() end
function PTGPreview(i)
    local a=PTG_STEPS[i]
    if not PTGReady(a) then return end
    local text="<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>"
    if a.xp>0 then text=text.."<enter><color=green>Kinh nghiÖm: "..a.xp.."<color>" end
    if a.money>0 then text=text.."<enter><color=green>L­îng: "..a.money.."<color>" end
    if a.rewardText then text=text.."<enter><color=green>"..a.rewardText.."<color>" end
    SayButtons(text,2,"TiÕp tôc/ptg_yes"..i,"Quay l¹i/main")
end
function PTGApply(i)
    local a=PTG_STEPS[i]
    if not PTGReady(a) then return end
    if GetTrade()~=0 or GetLockState()~=0 then return end
    if PTGHave(a.needs)~=1 then
        PTGNotice("<color=yellow>"..a.title.."<color><enter>Ch­a ®ñ vËt phÈm.<enter><color=cyan>"..a.note.."<color>")
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
        PTGNotice("Ch­a thÓ giao nhiÖm vô. H·y më khãa vËt phÈm vµ dµnh ®ñ « trèng trong hµnh trang.")
        return
    end
    if a.xp>0 then AddOwnExp(a.xp) end
    if a.credit>0 then AddCredit(a.credit) end
    if a.camp then SetCamp(a.camp) end
    if a.task==3 and a.to==84 then SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0) end
    PTGJournal()
    Msg2Player(a.note)
    if a.travel then NewWorld(a.travel[1],a.travel[2]*8+4,a.travel[3]*16+8) end
    PTGNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>")
end
function PTGMenu()
    PTGJournal()
    if not PTGAllowed() then
        if PTG_OLD_MAIN then return PTG_OLD_MAIN() end
        CloseDialog();return
    end
    local options={}
    if PTSAddChoices then PTSAddChoices(options) end
    for i=1,getn(PTG_STEPS) do
        local a=PTG_STEPS[i]
        if PTGReady(a) and (getn(a.needs)==0 or PTGHave(a.needs)==1) then tinsert(options,a.label.."/ptg_a"..i) end
    end
    if PTG_ROLE=="giao" and GetLevel()>=20 then
        if GetTask(4554)==0 and ((GetLevel()<29 and (GetTask(4556)~=PTGDay() or GetTask(4557)<3)) or (GetLevel()>=29 and GetTask(4558)==0)) then tinsert(options,"Gi¸o HuÊn/ptg_giao") end
        if GetTask(4554)>0 and GetTask(4555)>=40 then tinsert(options,"NhËn th­ëng/ptg_giao_claim") end
    end
    if PTG_ROLE=="thong" and GetTask(3)==88 then tinsert(options,"Khiªu chiÕn/ptg_challenge") end
    if PTG_OLD_MAIN then tinsert(options,"DÞch vô/ptg_services") end
    local text="<color=yellow>NhiÖm vô Gi¸p SÜ<color><enter>Chän nót nhiÖm vô ®Ó xem môc tiªu vµ phÇn th­ëng."
    if getn(options)==0 then text="<color=yellow>NhiÖm vô Gi¸p SÜ<color><enter>Ch­a cã nhiÖm vô ®ñ ®iÒu kiÖn nhËn hoÆc tr¶. Xem chØ dÉn ®ang lµm b»ng F11." end
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
            Msg2Player("ViÔn Cæ Háa DiÖm: cßn 3 phót h¹ Th«ng Thiªn Gi¸o Chñ!")
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
        PTGNotice("<color=yellow>Gi¸o HuÊn<color><enter><color=cyan>"..GetTask(4555).." / 40 "..PTG_TARGET_NAMES[GetTask(4554)].."<color><enter>§ñ môc tiªu h·y chän nót nhËn th­ëng.")
        if GetTask(4555)>=40 then SayButtons("<color=green>PhÇn th­ëng: 4600 kinh nghiÖm vµ 3000 l­îng.<color>",2,"NhËn th­ëng/ptg_giao_claim","§ãng/ptg_close") end
        return
    end
    if GetLevel()>=29 then
        if GetTask(4558)~=0 then PTGNotice("§· hoµn thµnh l­ît Gi¸o HuÊn cuèi cïng.");return end
    elseif GetTask(4556)==PTGDay() and GetTask(4557)>=3 then
        PTGNotice("§· nhËn ®ñ 3 l­ît Gi¸o HuÊn h«m nay.");return
    end
    SayButtons("<color=yellow>Gi¸o HuÊn<color><enter><color=cyan>Tiªu diÖt 40 qu¸i ®­îc giao.<color><enter><color=green>4600 kinh nghiÖm vµ 3000 l­îng.<color>",2,"NhËn nhiÖm vô/ptg_giao_take","Quay l¹i/main")
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
    Msg2Player("Gi¸o HuÊn: diÖt 40 "..PTG_TARGET_NAMES[GetTask(4554)]..". Môc tiªu n»m t¹i "..PTG_TARGET_MAPS[GetTask(4554)]..".")
    PTGMenu()
end
function ptg_giao_claim()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetTask(4554)==0 or GetTask(4555)<40 then return end
    if GetTrade()~=0 or GetLockState()~=0 or GetCash()>1999997000 then return end
    local old=GetCash();Earn(3000)
    if GetCash()~=old+3000 then return end
    if QuestExchange(4554,GetTask(4554),0,{},{})~=1 then Pay(3000);return end
    SetTask(4555,0);AddOwnExp(4600);PTGJournal()
    PTGNotice("<color=green>§· nhËn 4600 kinh nghiÖm vµ 3000 l­îng.<color>")
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
        PTGNotice("§· qu¸ 3 phót. Trë l¹i Kh­¬ng Tö Nha thêi niªn thiÕu nhËn l¹i Th«ng Thiªn.")
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

PTG_OLD_DEATH=OnDeath
function OnDeath(index)
 local killer=PlayerIndex
 if not killer or killer<=0 then return end
 local world=GetNpcWorldPos(index)
 if world~=PTGMap() then return end
 PTG_OLD_DEATH(index)
 PlayerIndex=killer
 PTGDeath(7)
 PlayerIndex=killer
end

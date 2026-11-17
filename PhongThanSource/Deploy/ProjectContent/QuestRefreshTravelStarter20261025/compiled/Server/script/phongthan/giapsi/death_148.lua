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
	if(GetTeam()~=0)then			-- ¦³¶¤¥î	
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
	local	mark=HaveIBBuff(215)	--§PÂ_Ây±þ¥ô°Èªº¼Ð»xbuff
	if(mark~=0)then
		if(count>1)then
			SetTask(898,count-1)
			Msg2Player("Trõ Yªu: B¹n ph¶i giÕt"..(count-1)..". "..npc_name[kindID].."!")
		else
			SetTask(898,0)
			TaskNote(42,8)
			Msg2Player("Trõ Yªu: B¹n ®· h¹ thµnh c«ng "..npc_name[kindID].." ")
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
		
			if(type1 == templateID and count1 > 0) then
				count1 = count1 - 1
				if(count1>0)then
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..npc_name[type1].."("..(50-count1).."/50)")
				else
					count1 = 0
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..npc_name[type1].." ")
				end
				TaskNote(task_id,-1)
				TaskNote(task_id,0,npc_name[type1],(50-count1),npc_name[type2],(50-count2))
				SetTask(task_id, SetByte(task_val, 2, count1))
			elseif(type2 == templateID and count2 > 0)then	
				count2 = count2 - 1
				if(count2>0)then
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..npc_name[type2].."("..(50-count2).."/50)")
				else
					count2 = 0
					Msg2Player("NhiÖm vô lÝnh ®¸nh thuª: h¹ s¸t thµnh c«ng "..npc_name[type2].." ")
				end
				TaskNote(task_id,-1)
				TaskNote(task_id,0,npc_name[type1],(50-count1),npc_name[type2],(50-count2))
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
							Msg2Player("NhiÖm vô hoa cá: Cõu s¸t thµnh c«ng "..npc_name[t].."("..c.."/50))")
						else
							Msg2Player("NhiÖm vô hoa cá: h¹ s¸t thµnh c«ng "..npc_name[t].." ")
						end
						SetTask(889,SetByte(task_val, i, c))
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
-- PT_GIAPSI_20261024_BEGIN
PTG_STEPS={{role="lo",title="T©n Thøc",level=3,task=21,from=0,to=1,note="GÆp ¢u Thiªn Hãa t¹i Sïng Thµnh Doanh.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=1,to=2,note="GÆp Sïng øng B­u.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="buu",title="T©n Thøc",level=3,task=21,from=2,to=3,note="Thu thËp 10 §o¶n KiÕm råi gÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=3,to=4,note="GÆp Sïng HÇu Hæ.",needs={{3,10,0,0,0,0,10}},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="hau",title="T©n Thøc",level=3,task=21,from=4,to=5,note="Trë l¹i gÆp Lç Hïng.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="lo",title="T©n Thøc",level=3,task=21,from=5,to=6,note="GÆp Sïng H¾c Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="hac",title="T©n Thøc",level=3,task=21,from=6,to=7,note="GÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=7,to=8,note="Trë l¹i gÆp Lç Hïng nhËn th­ëng.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="lo",title="T©n Thøc",level=3,task=21,from=8,to=9,note="Hoµn thµnh T©n Thøc.",needs={},gifts={},xp=500,money=600,credit=0,label="T©n Thøc"},{role="loan",title="Dòng §ao",level=7,task=25,from=0,to=1,note="DiÖt Hoµn CÈu V­¬ng t¹i YÕn S¬n (235,208), lÊy ®ñ ba bé phËn ®ao.",needs={},gifts={},xp=0,money=0,credit=0,label="Dòng §ao"},{role="dien",title="Dòng §ao",level=7,task=25,from=1,to=2,note="Nhê ¢u Thiªn Hãa söa Dòng §ao.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,keepNeeds=1,label="Dòng §ao"},{role="au",title="Dòng §ao",level=7,task=25,from=2,to=3,note="Mang Dòng §ao vÒ TriÒu §iÒn.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},gifts={{4,24,0,0,0,0,1}},xp=0,money=0,credit=0,label="Dòng §ao"},{role="dien",title="Dòng §ao",level=7,task=25,from=3,to=4,note="Hoµn thµnh Dòng §ao.",needs={{4,24,0,0,0,0,1}},gifts={{0,4,0,1,0,0,1}},xp=0,money=0,credit=10,label="Dòng §ao",rewardText="Ph¸p b¶o cÊp 10; 10 ®iÓm danh väng."},{role="loan",title="KiÕm ƒi",level=14,task=24,from=0,to=1,note="GÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="KiÕm ƒi"},{role="au",title="KiÕm ƒi",level=14,task=24,from=1,to=2,note="DiÖt Hoµn CÈu Tinh t¹i YÕn S¬n (213,187), (202,193), (213,194); vÒ Sïng øng Loan.",needs={},gifts={},xp=0,money=0,credit=0,label="KiÕm ƒi"},{role="loan",title="KiÕm ƒi",level=14,task=24,from=3,to=4,note="Hoµn thµnh KiÕm ƒi.",needs={{4,25,0,0,0,0,1}},gifts={{5,0,0,0,0,0,5},{7,58,62,1,0,0,1}},xp=3000,money=0,credit=0,camp=7,label="KiÕm ƒi",rewardText="5 Håi Thµnh Phï; s¸ch Bµn Cæ Khai Thiªn."},{role="hau",title="Trung Thµnh",level=25,task=3,from=0,to=1,note="GÆp TrÞnh Lu©n ë Sïng Thµnh Doanh.",needs={},gifts={},xp=0,money=0,credit=0,label="Trung Thµnh"},{role="trinh",title="Trung Thµnh",level=25,task=3,from=1,to=2,note="Mang HuyÕt Th­ vÒ Sïng HÇu Hæ.",needs={},gifts={{4,11,0,0,0,0,1}},xp=0,money=0,credit=0,label="Trung Thµnh"},{role="hau",title="Trung Thµnh",level=25,task=3,from=2,to=10,note="Hoµn thµnh Trung Thµnh.",needs={{4,11,0,0,0,0,1}},gifts={},xp=20000,money=30000,credit=0,label="Trung Thµnh"},{role="hac",title="TrÇm H­¬ng",level=35,task=3,from=10,to=11,note="Mang TrÇm H­¬ng Méc tíi Hoµng Phi Hæ ë TriÒu Ca.",needs={},gifts={{4,45,0,0,0,0,1}},xp=0,money=0,credit=0,label="TrÇm H­¬ng"},{role="hoang",title="TrÇm H­¬ng",level=35,task=3,from=11,to=20,note="Hoµn thµnh TrÇm H­¬ng.",needs={{4,45,0,0,0,0,1}},gifts={{7,59,128,1,0,0,1}},xp=80000,money=0,credit=0,label="TrÇm H­¬ng",rewardText="MËt tÞch Ban M«n Léng Phñ."},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=20,to=21,note="Hái chuyÖn Lý TÞnh vµ §Æng Cöu C«ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="ly",title="Khuyªn Hµng",level=45,task=3,from=21,to=22,note="GÆp §Æng Cöu C«ng t¹i Tam S¬n.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="dang",title="Khuyªn Hµng",level=45,task=3,from=21,to=23,note="GÆp Lý TÞnh t¹i TrÇn §­êng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="dang",title="Khuyªn Hµng",level=45,task=3,from=22,to=24,note="Trë vÒ Hoµng Phi Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="ly",title="Khuyªn Hµng",level=45,task=3,from=23,to=24,note="Trë vÒ Hoµng Phi Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=24,to=25,note="DiÖt TrÊn T­íng Qu©n t¹i Môc D· (187,189), lÊy Ên tÝn Vâ Thµnh V­¬ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=26,to=27,note="B¸o tin cho Kh­¬ng Tö Nha t¹i T©y Kú.",needs={{4,12,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="khuong",title="Khuyªn Hµng",level=45,task=3,from=27,to=30,note="Hoµn thµnh Khuyªn Hµng.",needs={},gifts={{0,0,31,4,1,0,1}},xp=240000,money=0,credit=0,label="Khuyªn Hµng",rewardText="Vò khÝ hoµng kim cÊp 40."},{role="duong",title="ThÇn Long",level=55,task=3,from=30,to=31,note="DiÖt ThÇn Long t¹i Long Cung tÇng 3 (206,175), lÊy R©u ThÇn Long.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Long"},{role="dac",title="ThÇn Long",level=55,task=3,from=32,to=33,note="Mang R©u ThÇn Long cho Hå Hû MÞ.",needs={{4,13,0,0,0,0,1}},gifts={{4,9,0,0,0,0,1}},xp=0,money=0,credit=0,keepNeeds=1,label="ThÇn Long"},{role="ho",title="ThÇn Long",level=55,task=3,from=33,to=40,note="Hoµn thµnh ThÇn Long.",needs={{4,13,0,0,0,0,1}},gifts={{3,46,0,0,0,0,1}},xp=600000,money=0,credit=0,label="ThÇn Long",rewardText="B¸ L¹c Nh·n cÊp 10."},{role="dac",title="B×nh An",level=65,task=3,from=40,to=41,note="GÆp T©y V­¬ng MÉu t¹i Diªu Tr×.",needs={},gifts={},xp=0,money=0,credit=0,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=3,from=41,to=42,note="DiÖt Cù D· t¹i BÝch Du Cung tÇng 4: (181,199), (196,204), (185,217); lÊy An C­ §å.",needs={},gifts={},xp=0,money=0,credit=0,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=3,from=43,to=50,note="Hoµn thµnh B×nh An.",needs={{4,3,0,0,0,0,1}},gifts={},xp=1400000,money=0,credit=0,randomBook=1,label="B×nh An",rewardText="S¸ch kü n¨ng ngÉu nhiªn cña ba hÖ ph¸i."},{role="nong",title="ThÇn Dô KÝnh",level=70,task=3,from=50,to=51,note="DiÖt Tr¹nh Nanh t¹i Khæn Tiªn Cung tÇng 1 (197,212), lÊy ThÇn Dô KÝnh.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Dô KÝnh"},{role="nong",title="ThÇn Dô KÝnh",level=70,task=3,from=52,to=60,note="Hoµn thµnh ThÇn Dô KÝnh. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",needs={{4,4,0,0,0,0,1}},gifts={{0,0,31,7,1,0,1}},xp=2000000,money=0,credit=0,keepNeeds=1,label="ThÇn Dô KÝnh",rewardText="Vò khÝ hoµng kim cÊp 70."},{role="dac",title="Tam S¸ch",level=75,task=3,from=60,to=61,note="Mang hai b¶ng Phong ThÇn tíi Kh­¬ng Tö Nha. Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={},gifts={},xp=0,money=0,credit=0,ensureBoard=1,label="Tam S¸ch"},{role="khuong",title="Tam S¸ch",level=75,task=3,from=61,to=62,note="Mang ThÇn Dô KÝnh tíi Vâ V­¬ng.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=3,from=62,to=63,note="LÊy Huúnh §Õ Néi Kinh ë tÇng 2 (196,189); LuyÖn Kim ThuËt ë tÇng 3 (206,204); HuyÒn N÷ Binh Ph¸p ë tÇng 4 (191,210), Khæn Tiªn Cung.",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=3,from=63,to=70,note="Hoµn thµnh Tam S¸ch.",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},gifts={{0,4,14,1,0,0,1},{0,4,15,1,0,0,1},{0,4,16,1,0,0,1},{0,4,17,1,0,0,1}},xp=3000000,money=0,credit=0,label="Tam S¸ch",rewardText="Mét bé bèn ph¸p b¶o cÊp 70."},{role="tay",title="Cæ Kim",level=80,task=3,from=70,to=71,note="GÆp §¾c Kû t¹i TriÒu Ca m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1063,198,200},label="Cæ Kim"},{role="dac_future",title="Cæ Kim",level=80,task=3,from=71,to=72,note="GÆp Nguyªn Thñy Thiªn T«n t¹i Ngäc H­ Cung m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1062,203,198},label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=3,from=72,to=73,note="DiÖt Bé §Çu qu©n Chu t¹i Ngäc H­ Cung m­êi n¨m sau (206,204), lÊy Tróc Gi¶n.",needs={},gifts={},xp=0,money=0,credit=0,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=3,from=74,to=80,note="Hoµn thµnh Cæ Kim. Dïng Khø Lai Phï ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",needs={{4,10,0,0,0,0,1}},gifts={{3,41,0,0,0,0,1},{6,1,34,1,0,0,1}},xp=5000000,money=0,credit=0,label="Cæ Kim",rewardText="Lam B¶o Th¹ch; Khø Lai Phï."},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=80,to=81,note="Nãi chuyÖn víi Kh­¬ng Tö Nha thêi niªn thiÕu.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="khuong_young",title="Thiªn Duyªn",level=85,task=3,from=81,to=82,note="Trë l¹i gÆp §¾c Kû thêi niªn thiÕu.",needs={},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=82,to=85,note="DiÖt Bµn Cæ t¹i Khæn Tiªn Cung tÇng 5 (199,212), lÊy Bµn Cæ ThÇn KhÝ.",needs={},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=85,to=83,note="Hoµn thµnh Thiªn Duyªn.",needs={{4,175,0,0,0,0,1}},gifts={{0,5,6,8,1,0,1}},xp=6500000,money=0,credit=0,label="Thiªn Duyªn",rewardText="Mét trang bÞ lôc cÊp 80."},{role="khuong_young",title="Th«ng Thiªn",level=92,task=3,from=83,to=84,note="T×m Tham Lam t¹i Th­¬ng Nh©n §¹i Phong, V« Tri t¹i Cöu Di Tam S¬n; dïng MËt §å ë B¾c H¶i t×m Tµn NhÉn.",needs={},gifts={{6,1,142,1,0,0,1},{6,1,181,1,0,0,1}},xp=0,money=0,credit=0,label="Th«ng Thiªn"},{role="merchant",title="Th«ng Thiªn",level=92,task=4560,from=0,to=1,note="NhËn Tham Lam.",needs={},gifts={{4,176,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,label="Th«ng Thiªn"},{role="cuudi",title="Th«ng Thiªn",level=92,task=4561,from=0,to=1,note="NhËn V« Tri.",needs={},gifts={{4,177,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,label="Th«ng Thiªn"},{role="thong",title="Th«ng Thiªn",level=92,task=3,from=84,to=86,note="Dïng Khø Lai Phï gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Th«ng Thiªn"},{role="khuong_young",title="Th«ng Thiªn: nhËn giµy",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,5,6,9,1,0,1}},xp=0,money=0,credit=0,rewardChoice=1,label="NhËn giµy",rewardText="Giµy lôc cÊp 90."},{role="khuong_young",title="Th«ng Thiªn: nhËn yªu ®¸i",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,6,6,9,1,0,1}},xp=0,money=0,credit=0,rewardChoice=2,label="NhËn yªu ®¸i",rewardText="Yªu ®¸i lôc cÊp 90."},{role="khuong_young",title="Th«ng Thiªn: nhËn m·o",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,7,6,9,1,0,1}},xp=0,money=0,credit=0,rewardChoice=3,label="NhËn m·o",rewardText="M·o lôc cÊp 90."},{role="doctor_manh",title="Trõ Häa: M¹nh T©n",level=18,task=4550,from=0,to=1,note="DiÖt Hoµn CÈu t¹i M¹nh T©n (188,210) ®Õn (192,210), råi trong 10 gi©y diÖt Gi¸p Cèt ngay phÝa nam ®Ó lÊy ®ñ 5 H¹p §»ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_manh",title="Trõ Häa: tr¶ H¹p §»ng",level=18,task=4550,from=1,to=2,note="§Õn §¹i Phu t¹i Tam S¬n.",needs={{4,191,0,0,0,0,5}},gifts={},xp=6000,money=0,credit=0,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: Tam S¬n",level=18,task=4550,from=2,to=3,note="H¸i 10 Nguyªn S©m t¹i Tam S¬n, tõ (196,203) ®Õn (205,203). CÈn thËn Nguyªn Tham Tinh ë gÇn ®ã.",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: tr¶ Nguyªn S©m",level=18,task=4550,from=3,to=4,note="§Õn §¹i Phu t¹i §ång Quan.",needs={{4,192,0,0,0,0,10}},gifts={},xp=8000,money=0,credit=0,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: §ång Quan",level=18,task=4550,from=4,to=5,note="DiÖt Ng­u S¸t t¹i §ång Quan (195,208) ®Õn (199,208), dïng TÞnh Hãa Phï, råi tÞnh hãa 5 S¬n Xuyªn LiÔu ë hµng (195,211) ®Õn (199,211).",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: tr¶ S¬n Xuyªn LiÔu",level=18,task=4550,from=5,to=6,note="Hoµn thµnh Trõ Häa.",needs={{4,201,0,0,0,0,5}},gifts={{0,2,6,2,1,0,1}},xp=10000,money=0,credit=0,label="Trõ Häa",rewardText="Mét trang bÞ lôc."}}
PTG_KILLS={{576,1007,24,2,3,25},{122,1018,3,25,26,12},{113,1039,3,31,32,13},{121,1045,3,42,43,3},{120,1047,3,51,52,4},{117,1048,3,63,63,7},{118,1049,3,63,63,8},{119,1050,3,63,63,6},{116,1062,3,73,74,10},{82,1051,3,85,85,175}}
PTG_BOOKS={{7,16,19,1,0,0,1},{7,17,20,1,0,0,1},{7,18,21,1,0,0,1},{7,19,22,1,0,0,1},{7,33,36,1,0,0,1},{7,34,37,1,0,0,1},{7,35,38,1,0,0,1},{7,36,39,1,0,0,1},{7,45,48,1,0,0,1},{7,46,49,1,0,0,1},{7,55,456,1,0,0,1},{7,60,1481,1,0,0,1},{7,66,1487,1,0,0,1},{7,81,1548,1,0,0,1},{7,83,1551,1,0,0,1}}
PTG_TARGET_NAMES={[13]="Gi¸p Cèt",[16]="Thiªn H¹o"}
PTG_TARGET_MAPS={[13]="M¹nh T©n (190,210)",[16]="§ång Quan (198,208)"}
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
    TaskNote(27,a.to)
    Msg2Player(a.note)
    if a.travel then NewWorld(a.travel[1],a.travel[2]*8+4,a.travel[3]*16+8) end
    PTGNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>")
end
function PTGMenu()
    if not PTGAllowed() then
        if PTG_OLD_MAIN then return PTG_OLD_MAIN() end
        CloseDialog();return
    end
    local options={}
    for i=1,getn(PTG_STEPS) do
        local a=PTG_STEPS[i]
        if PTGReady(a) then tinsert(options,a.label.."/ptg_a"..i) end
    end
    if PTG_ROLE=="giao" then tinsert(options,"Gi¸o HuÊn/ptg_giao") end
    if PTG_ROLE=="thong" and GetTask(3)==88 then tinsert(options,"Khiªu chiÕn/ptg_challenge") end
    if PTG_OLD_MAIN then tinsert(options,"DÞch vô kh¸c/ptg_services") end
    tinsert(options,"§ãng/ptg_close")
    SayButtons("<color=yellow>NhiÖm vô Gi¸p SÜ<color><enter>Chän nót nhiÖm vô ®Ó xem môc tiªu vµ phÇn th­ëng.",getn(options),options)
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
    if GetTask(4554)>0 and map==GetTask(4559) and template==GetTask(4554) then
        if GetTask(4555)<40 then SetTask(4555,GetTask(4555)+1) end
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
    if template==148 and map==1070 and GetTask(3)==88 then
        if SystemTime()<=GetTask(4563) and PTGEvent(174,1)>0 then SetTask(3,89) end
    end
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
        SetTask(4558,1);SetTask(4554,16);SetTask(4559,1014)
    else
        if GetTask(4556)~=PTGDay() then SetTask(4556,PTGDay());SetTask(4557,0) end
        if GetTask(4557)>=3 then return end
        SetTask(4557,GetTask(4557)+1);SetTask(4554,13);SetTask(4559,1015)
    end
    SetTask(4555,0)
    Msg2Player("Gi¸o HuÊn: diÖt 40 "..PTG_TARGET_NAMES[GetTask(4554)]..". Môc tiªu n»m t¹i "..PTG_TARGET_MAPS[GetTask(4554)]..".")
    PTGMenu()
end
function ptg_giao_claim()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetTask(4554)==0 or GetTask(4555)<40 then return end
    if GetTrade()~=0 or GetLockState()~=0 or GetCash()>1999997000 then return end
    local old=GetCash();Earn(3000)
    if GetCash()~=old+3000 then return end
    if QuestExchange(4554,GetTask(4554),0,{},{})~=1 then Pay(3000);return end
    SetTask(4555,0);AddOwnExp(4600)
    PTGNotice("<color=green>§· nhËn 4600 kinh nghiÖm vµ 3000 l­îng.<color>")
end
function PTGGather(kind,bit)
    if GetPlayerType()~=0 then return end
    local phase=GetTask(4550)
    local mask=GetTask(4553)
    if kind==1 and phase==3 and PTGMap()==1016 then
        return GiapGather(0)
    elseif kind==2 and phase==5 and PTGMap()==1014 then
        return GiapGather(0)
    end
end
function ptg_challenge()
    if not PTGAllowed() or PTG_ROLE~="thong" or GetTask(3)~=88 then return end
    if SystemTime()>GetTask(4563) then
        -- Retry the whole Thong Thien stage, retaining the preceding quests.
        for id=176,178 do for n=1,2 do if HaveEventItem(id)>0 then DelEventItem(id) end end end
        SetTask(3,83);SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0)
        PTGNotice("§· qu¸ 3 phót. Trë l¹i Kh­¬ng Tö Nha thêi niªn thiÕu nhËn l¹i Th«ng Thiªn.")
        return
    end
    local previous=GetTask(4567)
    if previous>0 and GetNpcTemplateID(previous)==148 and GetNpcWorldPos(previous)==1070 and GetNpcLife(previous)>0 then return end
    local npc=AddNpc(148,95,SubWorld,204*256+128,210*512+256,1)
    if npc and npc>0 then
        if SetNpcDeathScript(npc,"\\script\\phongthan\\giapsi\\death_148.lua")==1 then SetTask(4567,npc) else DelNpc(npc) end
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
 PTGDeath(148)
 PlayerIndex=killer
end

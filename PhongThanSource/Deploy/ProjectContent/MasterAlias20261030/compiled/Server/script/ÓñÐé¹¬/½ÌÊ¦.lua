-- Original API name; use persistent native master status, never a stub.
IsMaster=IsMantleMaster
--description: ¼¼ÄÜ½ÌÊ¦
--author: yichuan
--date: 2004/5/12

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

function main(sel)
	tasks =
	{
		{"Häc kü n¨ng","jineng";show=1},
		{"Trõ Yªu","mission_PR";show=0},
		{"Hoµn thµnh","mission_PR_end";show=0},
		{"Hñy bá","mission_PR_cancel";show=0},
		{"T×m hiÓu n/v","mission_PR_intro";show=1}
	}
	if(GetLevel()<=50)and(GetTask(897)==0)then
		tasks[2].show=1;
	end
	if(GetLevel()>=20)and(GetLevel()<=50)and(GetTask(897)~=0)and(GetTask(898)==0)then
		tasks[3].show=1;
	end
	if(GetLevel()>=20)and(IsMaster()==0)and(GetTask(897)~=0)and(GetTask(898)~=0)then
		tasks[4].show=1;
	end
	if(songxin()==0)then
		SayTask("D¹o nµy yªu ma léng hµnh. Nguyªn Thñy Thiªn T«n kªu gäi <color=green>Trõ yªu<color>. NhiÖm vô nµy cÇn <color=green>s­ ®å phèi hîp<color>!",tasks)
	end
end;

function mission_PR_intro()
	tasks1 =
	{
		{"T×m hiÓu","PR1_intro";show=1},
		{"Xem n/v","PR2_intro";show=1},
		{"§¹o lı","PR3_intro";show=1},
		{"XuÊt s­","PR4_intro";show=1}
	}
	SayTask("§Ö tö ®¹t cÊp 20 cã thÓ lµm nhiÖm vô s­ m«n gåm <color=green>Trõ Yªu, Thİ luyÖn mª cung, §¹o lı<color>. Sau khi hoµn thµnh nhiÖm vô kh«ng nh÷ng vâ nghÖ t¨ng c­êng mµ cßn nhËn ®­îc b¶o vËt cña tiªn gia.",tasks1)
end

function PR1_intro()
	Talk(2,"no","Kh¶o nghiÖm <color=green>Trõ Yªu<color> chİnh lµ thö lßng b¸t ¸i cña ng­¬i, trong thêi gian quy ®Şnh ph¶i giÕt ®óng yªu ma nÕu kh«ng chóng sÏ lµm h¹i b¸ t¸nh.","1 tuÇn nhËn <color=red>4 lÇn<color> nhiÖm vô. Mçi <color=green>thø hai<color> nhiÖm vô Trõ Yªu ®­îc tİnh l¹i tõ ®Çu.")
end

function PR2_intro()
	Talk(2,"PR2_intro1","Kh¶o nghiÖm <color=green>Thİ luyÖn mª cung<color> chİnh lµ thö lßng dòng c¶m cña ng­¬i, trong thêi gian quy ®Şnh ph¶i kh¸m ph¸ hÕt 5 tÇng mª cung. NÕu ng­¬i kh¸m ph¸ ®­îc <color=green>2<color> mª cung trë lªn cã thÓ nhËn nhiÖm vô <color=green>§¹o lı<color>.","NÕu kh«ng ®ñ cÊp sÏ kh«ng thÓ nhËn nhiÖm vô. CÇn <color=green>s¾p xÕp thêi gian thİ luyÖn hîp lı<color>!")
end

function PR2_intro1()
	Talk(2,"no","<color=red>Tõ cÊp 26 ®Õn 35<color> ®Õn gÆp <color=green>Thæ Hµnh T«n ë TriÒu Ca<color> nhËn n/v thİ luyÖn hoang m¹c. <color=red>Tõ cÊp 31 ®Õn 40 <color> ®Õn gÆp <color=green>Na Tra ë T©y Kú<color> nhËn n/v thİ luyÖn §«ng H¶i","<color=red>Tõ cÊp 36 ®Õn 45<color> ®Õn gÆp <color=green>Hoµng Thiªn Hãa ë TriÒu Ca<color> nhËn n/v thİ luyÖn Hiªn Viªn. <color=red>Tõ cÊp 41 ®Õn 50<color> gÆp <color=green>D­¬ng TiÔn ë T©y Kú<color> nhËn n/v thİ luyÖn B¨ng Xuyªn")
end

function PR3_intro()
	Talk(1,"no","<color=green>§¹o lı<color> kh«ng ph¶i ai còng lÜnh ngé ®­îc. Khi ng­¬i ®¹t <color=yellow>cÊp 50<color> sÏ cã 1 c¬ héi, <color=green>Hoµng Phi Hæ<color> sÏ h­íng dÉn ng­¬i. Hoµn thµnh nhiÖm vô nµy cã thÓ xuÊt s­")
end

function PR4_intro()
	Talk(1,"no","§Ö tö ®¹t <color>cÊp 50<color> ®· cã thÓ xuÊt s­, tù lËp m«n hé. NÕu ®Õn cÊp 51 vÉn ch­a kh¸m ph¸ thµnh c«ng <color=green>2<color> mª cung trë lªn th× kh«ng thÓ lÜnh ngé <color=green>§¹o ®øc th¸nh hiÒn<color>, kh«ng ®­îc xuÊt s­ nh­ng cã thÓ ®Õn Hoµng Phi Hæ <color=red>xin xuÊt s­<color>!")
end

function mission_PR_cancel()
	MsgBox("§¹o h¹nh cña ng­¬i kh«ng ®ñ ®Ó hµng phôc yªu ma. H¬n n÷a <color=green>Trõ Yªu<color> mçi tuÇn chØ <color=red>4 lÇn<color>, ng­¬i muèn hñy kh«ng?","mission_PR_cancel_confirm","no")
end

function mission_PR_cancel_confirm()
	SetTask(898,0)
	SetTask(897,0)
	RemoveIBBuff(215)
	TaskNote(42,-1)
	Talk(1,"no","Ng­¬i ®· hñy nhiÖm vô truy s¸t (s­ ®å). TuÇn nµy ng­¬i ®· nhËn Trõ Yªu <color=green>"..GetTask(895).."<color> lÇn!")
end

function mission_PR_end()
	local mark1=judge_relation()
	if(mark1==1)then
		local oldPlayer=PlayerIndex
		mission_PR_end_P()
		if(GetTeamSize()==2)then	--2¤H¶¤
			if(IsCaptain()==0)then
				n=GetTeamMember(1)
			else
				n=GetTeamMember(2)
			end;
			PlayerIndex=n
			mission_PR_end_M()
		end
		PlayerIndex=oldPlayer
	else
		Talk(1,"no","NhiÖm vô truy s¸t cÇn ph¶i 2 ng­êi tæ ®éi, xin x¸c nhËn l¹i tæ ®éi cña m×nh!")
	end
end

function mission_PR_end_P()
	local exp1=GetNextExp()-GetExp()
	local exp2=floor(12*GetNextExp()/10)
	AddOwnExp(exp1)
	AddOwnExp(exp2-exp1)
	SetTask(898,0)
	SetTask(897,0)
	TaskNote(42,-1)
	TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>"..exp2.."kinh nghiÖm")
	Talk(1,"no","Ng­¬i thËt xuÊt s¾c!")
end

function mission_PR_end_M()
	AddMasterPRValue(2)
	TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>2 ®iÓm s­ ®å")
end

function mission_PR()
	local mark1=judge_relation()
	if(GetLevel()>=20)then
		if(mark1==1)then
			local mark2=judge_times()
			if(mark2==0)then
				if(GetLevel()<=50)then
					mission_PR_confirm()
				else
					Talk(1,"no","NhiÖm vô s­ ®å chØ cã ®Ö tö míi lµm ®­îc, ng­¬i mau kªu ®Ö tö ®Õn nhËn nhiÖm vô!")
				end		
			else
				Talk(1,"no","NhiÖm vô truy s¸t (s­ ®å) mçi tuÇn chØ lµm ®­îc 4 lÇn!")
			end
		else
			Talk(1,"no","NhiÖm vô truy s¸t cÇn ph¶i 2 ng­êi tæ ®éi, xin x¸c nhËn l¹i tæ ®éi cña m×nh!")
		end
	else
		Talk(1,"no","§¼ng cÊp hiÖn t¹i cña ng­¬i ch­a ®ñ søc truy s¸t, ®¹t cÊp 20 cã thÓ cïng víi s­ phô ®Õn chç ta thö søc!")
	end
end

function mission_PR_confirm()
	local rank_kind=GetLevel()
	local rank_count=0
	local i
	local kind
	local count
	local name
	local k
	if(rank_kind<25)then
		i=random(1,2)
	elseif(rank_kind>=25)and(rank_kind<30)then
		i=3
	elseif(rank_kind>=30)and(rank_kind<35)then
		i=4
	elseif(rank_kind>=35)and(rank_kind<40)then
		i=5
	elseif(rank_kind>=40)and(rank_kind<45)then
		i=6
	elseif(rank_kind>=45)and(rank_kind<50)then
		i=7
	else
		i=8
	end
	if(GetTeamSize()==2)then
		RemoveIBBuff(215)
		local done=AddIBBuff(215)		--µ¹Âù­¿¸gÅçªº¼Ğ»xbuff
		if(done==1)then
			local oldPlayer=PlayerIndex
			for k=1,2 do
				PlayerIndex=GetTeamMember(k)
				rank_count=rank_count+GetLevel()
				SetTask(895,GetTask(895)+1)
				SetTask(896,SystemTime())
				SetTask(906,GetWeekDay())
				kind=object[i].id
				SetTask(897,kind)
			end	
			PlayerIndex=oldPlayer
			count=100+(floor(rank_count/10)-5)*40
			name=object[i].name
			SetTask(898,count)
			TaskNote(42,i-1)
			Talk(1,"no","D­íi sù b¶o hé cña vßng s¸ng <color=yellow>Nh©n gi¶<color>, giÕt qu¸i nhËn ®­îc nh©n ®«i kinh nghiÖm. HiÖn ng­¬i ph¶i hµng phôc "..count..". "..name..". Chóc ng­¬i may m¾n!")
		else
			Talk(1,"no","Tr¹ng th¸i cña ng­¬i kh«ng phï hîp, kh«ng thÓ tiÕp nhËn nhiÖm vô s­ ®å, khi kh¸c h·y quay l¹i!")
		end
	end
end

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

function judge_times()
	local mark=0
	local oldPlayer=PlayerIndex
	for i=1,2 do
		PlayerIndex=GetTeamMember(i)
		mark=judge_times_do()+mark
	end
	PlayerIndex=oldPlayer
	return	mark
end

function judge_times_do()		--Âú×ãÒ»¸öĞÇÆÚ×î¶à4´Î		?§OÒ»¸öĞÇÆÚ×ö?ÎñíèÓĞ´ïµ½ÉÏÏŞªöÎª0
	local mark=1
	local c=GetTask(895)
	local t=GetTask(896)
	local weekday=GetTask(906)
	if(GetWeekDay()<weekday)then	--±g¶ş¸öĞÇÆÚ
		SetTask(895,0)					--Õâ¸öĞÇÆÚ×ö?Îñ´ÎÊıÇåÁã
		mark=0
	elseif(GetWeekDay()==weekday)and(SystemTime()-t>86400)then
		SetTask(895,0)					--Õâ¸öĞÇÆÚ×ö?Îñ´ÎÊıÇåÁã
		mark=0
	elseif(GetWeekDay()>weekday)and(SystemTime()-t>86400*7)then	--±g¶ş¸öĞÇÆÚ
		SetTask(895,0)					--Õâ¸öĞÇÆÚ×ö?Îñ´ÎÊıÇåÁã
		mark=0
	else							--»êÒ»¸öĞÇÆÚ
		if(c<4)then					--Õâ¸öĞÇÆÚ×ö?Îñ´ÎÊıÉÙÓÚ4
			mark=0
		end
	end
	return mark
end

function jineng()
		if (GetPlayerType() == 1) then
				MsgBox(10553,"yes_2","no")
		else
				Talk(1,"no",10554)
		end;
end

function songxin()
	local task_id = 872
	local map_id = 2

	local task_val = GetTask(task_id)
	local type1 = GetByte(task_val,1) 
	local type2 = GetByte(task_val,2)
	local finish = GetByte(task_val,3)

	local setbit = 0
	if (type1 == map_id) then
		setbit = 7
	elseif (type2 == map_id)then
		setbit = 8
	end

	if (setbit ~= 0) then
	 	if (GetBit(finish, setbit) == 0)then
	 		Talk(1,"no","VÊt v¶ cho ng­¬i qu¸!")
			SetTask(task_id, SetByte(task_val,3,SetBit(finish, setbit, 1)))
	 		return 1
	 	end
	end
	return 0
end
function yes_2()
		CloseDialog()
		Sale(9);
end;

function no()
		CloseDialog()
end;



PTVS_RuntimePos=GetWorldPos
GetWorldPos=GetContentWorldPos
SubWorldIdx2ID=SubWorldIdx2ContentID
-- Keep the original teacher quest/dialogue and Sale(9) inventory.
PTVS_OriginalMain=main
PTVS_Contacts={}
function PTVSNear()
 local contact=PTVS_Contacts[PlayerIndex or 0]
 if not contact or contact.owner~=PlayerId or GetNpcID(contact.npc)~=contact.id then return nil end
 if GetNpcTemplateID(contact.npc)~=154 then return nil end
 local world,x,y=PTVS_RuntimePos()
 local map,nx,ny=GetNpcPos(contact.npc)
 return world==1003 and map==world and (x-nx)*(x-nx)+(y-ny)*(y-ny)<=144
end
function main(npc)
 if not npc or npc<=0 or GetNpcTemplateID(npc)~=154 then return end
 PTVS_Contacts[PlayerIndex or 0]={npc=npc,id=GetNpcID(npc),owner=PlayerId}
 if not PTVSNear() then return end
 return PTVS_OriginalMain(npc)
end
function jineng()
 if not PTVSNear() then return end
 if GetPlayerType()~=1 then Talk(1,"no",10554);return end
 MsgBox(10553,"yes_2","no")
end
function yes_2()
 if not PTVSNear() or GetPlayerType()~=1 or GetTrade()~=0 or GetLockState()~=0 then return end
 CloseDialog();Sale(9)
end

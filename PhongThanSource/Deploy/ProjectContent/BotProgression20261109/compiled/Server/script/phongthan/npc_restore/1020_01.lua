-- Original VNG source payload; provenance in deployment report.
--description:Ñîê¯-¼×Ê¿Ö÷Ïß?Îñ
--author: yichuan
--date:2004/7/14
--902,±ù´¨Ì½ÏÕ

function  main()
	local tasks = 
	{
		{"Quy Tinh","renwu1";show=0},
		{"Phu Thª","renwu2";show=0},
		{"ThÇn Long","renwu3";show=0},
		{"T×m hiÓu","renwu4";show=1},
		{"B¨ng Xuyªn ThÝ LuyÖn","shitu_1";show=0}
	}
	UTask_Knight = GetTask(3);
	if(GetPlayerType()==0)  and  (GetLevel()>=55)  and  (UTask_Knight==30)then
			tasks[3].show=1
	end;
	local task_step = GetByte(GetTask(53),2)
	if(task_step < 36)and(GetLevel()>=59)then
		 tasks[1].show=1
	end;

	UTask_world_2=GetTask(92);	
	if(UTask_world_2==1)then
			tasks[2].show=1
	end;

	if(GetLevel()>40)and(GetLevel()<=50)and(GetTask(902)<=7)then	--®{§Ìµ¥¯Åok¡A®{§Ì¨S¦³§¹¦¨¹L
		tasks[5].show=1;
	end;
			
	SayTask(10449,tasks)
end;

function  shitu_1()
	local mark=judge_relation()
	if(mark==1)then		--º¡¨¬®v®{2¤H¶¤ 
		if(GetTask(902)==0)then		--±µ¥ô°È
			MsgBox("S­ phô cã nãi muèn t¨ng nhanh n¨ng lùc chØ cÇn v­ît qua nh÷ng mª cung ma qu¸i. Ng­¬i cã muèn thö kh«ng#¿","shitu_1_begin","no")
		elseif(GetTask(902)==6)then	--§¹¦¨¥ô°È
			MsgBox("Chóc mõng! Cã ph¶i ng­¬i c¶m thÊy m¹nh h¬n tr­íc nhiÒu? ","shitu_1_end","no")
		elseif(GetTask(902)==7)then
			Talk(1,"no","Ng­¬i ®· hoµn thµnh nhiÖm vô ThÝ LuyÖn B¨ng Xuyªn. NÕu ng­¬i muèn xuÊt s­ cã thÓ ®Õn TriÒu Ca gÆp Hoµng Phi Hæ! ")
		else				--©ñ±ó¥ô°È
			MsgBox("Dôc tèc bÊt ®¹t! HiÖn n¨ng lùc cña ng­¬i ch­a ®ñ ®Ó hoµn thµnh thö th¸ch nµy. H·y quay l¹i sau!","shitu_1_cancel","no")
		end
	else
		if(GetTask(902)==0)then
			Talk(1,"no","N¬i nµy v« cïng nguy hiÓm. NÕu ng­¬i ®¹t cÊp 41 cã thÓ lËp tæ ®éi víi s­ phô ®Ó v­ît mª cung. ")
		elseif(GetTask(902)==6)then
			Talk(1,"no","NhiÖm vô s­ ®å th¸m hiÓm cÇn 2 s­ ®å t¹o thµnh 1 nhãm. Xin x¸c nhËn tæ ®éi ®· h×nh thµnh!")
		elseif(GetTask(902)==7)then
			Talk(1,"no","Ng­¬i ®· hoµn thµnh nhiÖm vô ThÝ LuyÖn B¨ng Xuyªn. NÕu ng­¬i muèn xuÊt s­ cã thÓ ®Õn TriÒu Ca gÆp Hoµng Phi Hæ! ")
		else
			MsgBox("Dôc tèc bÊt ®¹t! HiÖn n¨ng lùc cña ng­¬i ch­a ®ñ ®Ó hoµn thµnh thö th¸ch nµy. H·y quay l¹i sau!","shitu_1_cancel","no")
		end
	end
end

function shitu_1_begin()
	local mark=judge_relation()
	if(mark==1)then		--º¡¨¬®v®{2¤H¶¤ 
		RemoveIBBuff(216)
		local done=AddIBBuff(216)	--¼Ð»xbuff
		if(done==1)then
			SetTask(902,1)
			TaskNote(46,5)
			if(GetTask(900)<7)then
				SetTask(900,0)
				TaskNote(44,-1)
			end
			if(GetTask(901)<7)then
				SetTask(901,0)
				TaskNote(45,-1)
			end
			if(GetTask(899)<7)then
				SetTask(899,0)
				TaskNote(43,-1)
			end
			Talk(2,"no","Ng­¬i hiÖn ®øng trong vßng <color=yellow>Dòng gi¶<color>, tr­íc khi vßng trßn nµy mÊt ®i ph¶i phôc mÖnh <color=green>§¹i phu<color> ë mçi tÇng Mª cung B¨ng Xuyªn, nÕu hoµn thµnh xem nh­ thÝ luyÖn thµnh c«ng!","BÊt cø tæn th­¬ng nµo còng cã thÓ khiÕn vßng trßn cña ng­¬i mÊt ®i. C¶ qu¸ tr×nh nµy ng­¬i ph¶i cïng víi s­ phô thùc hiÖn. §¹i phu cña mçi tÇng sÏ trÞ th­¬ng gióp ng­¬i. ")
		else
			Talk(1,"no","Tr¹ng th¸i cña ng­¬i hiÖn kh«ng thÓ tiÕp nhËn nhiÖm vô s­ ®å. H·y quay l¹i sau nhÐ!")
		end
	else
		Talk(1,"no","NhiÖm vô s­ ®å th¸m hiÓm cÇn 2 s­ ®å t¹o thµnh 1 nhãm. Xin x¸c nhËn tæ ®éi ®· h×nh thµnh!")
	end
end

function shitu_1_end()
	local mark=judge_relation()
	if(mark==1)then		--º¡¨¬®v®{2¤H¶¤ 
		shitu_1_end_P()
		local oldPlayer=PlayerIndex
		if(GetTeamSize()==2)then	--2¤H¶¤
			if(IsCaptain()==0)then
				n=GetTeamMember(1)
			else
				n=GetTeamMember(2)
			end;
			PlayerIndex=n
			shitu_1_end_M()
		end
		PlayerIndex=oldPlayer
	else
		Talk(1,"no","NhiÖm vô s­ ®å th¸m hiÓm cÇn 2 s­ ®å t¹o thµnh 1 nhãm. Xin x¸c nhËn tæ ®éi ®· h×nh thµnh!")
	end
end

function step_complete()
	local mark=0
	local step={}
	for i=1,4 do
		step[i]=8-GetTask(898+i)
		if(step[i]==1)then
			mark=mark+1
		end
	end
	return mark
end

function shitu_1_end_P()
	local exp1=GetNextExp()-GetExp()
	local exp2=2*GetNextExp()
	local exp=2*GetNextExp()
	AddOwnExp(exp1)
	exp2=exp2-exp1
	exp1=GetNextExp()-GetExp()
	if(exp1<exp2)then
		AddOwnExp(exp1)
		AddOwnExp(exp2-exp1)
	else
		AddOwnExp(exp2)
	end
	SetTask(902,7)
	TaskNote(46,6)
	TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>"..exp.."kinh nghiÖm")
	local mark=step_complete()
	if(mark==1)and(GetTask(907)==0)then	--§¹¦¨¤F¤@¦¸±´ÀI¥ô°È¥B¨S¦³»â¨ú¹Lºñ¦âÀY²¯
		AddNormalItem(0,7,GetPlayerType()+6,4,0,0)
		SetTask(907,1)
		Talk(1,"no","Ng­¬i thËt xuÊt s¾c! Xin nhËn phÇn th­ëng")
		AddGlobalCountNews("<color=green>"..GetName().."<color> vµ s­ phô hoµn thµnh nhiÖm vô th¸m hiÓm, nhËn ®­îc <color=green>®Çu kh«i<color>",20)
--	elseif(mark==2)and(GetTask(903)==0)then		--§¹¦¨¤F¤G¦¸±´ÀI¥ô°È¥B¨S¦³»â¨ú¹LÂÅ¦â§¤ÃM
--		AddNormalItem(3,14,0,0,0,0)
--		SetTask(903,1)
--		Talk(1,"no","·¨áÖ¡G¯u¬O¤£Â²³æ¡A§A¤w¸g§¹¦¨¤F¨â¶µ«iÂô°g®cªº¸Õ½m¡A§Ú³o¸Ì¦³¤@¤Ç¯«¾s¡A´NÃØ»P§A¤F¡I")
	elseif(mark==4)and(GetTask(904)==0)then		--§¹¦¨¤F¥|¦¸±´ÀI¥ô°È¥B¨S¦³»â¨ú¹L50ÂÅ§¤ÃM
		AddNormalItem2(0,10,GetPlayerType()+15,9,0,0)
		SetTask(904,1)
		Talk(1,"no","§óng lµ anh hïng xuÊt thiÕu niªn! Xin nhËn phÇn th­ëng!")
		AddGlobalCountNews("<color=green>"..GetName().."<color>vµ s­ phô hoµn thµnh nhiÖm vô th¸m hiÓm, nhËn ®­îc <color=green>thó c­ìi cÊp 50<color>",20)
	else
		Talk(1,"no","S­ ®å 2 ng­êi phèi hîp thËt xuÊt s¾c!")
	end
end

function shitu_1_end_M()
	local step=GetTask(902)
	if(step<100)then
		AddMasterPRValue(10)
		SetTask(902,100)
		TopMessage("B¹n nhËn ®­îc <color=green>10 ®iÓm s­ ®å")
	else
		AddMasterPRValue(5)
		TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>5 ®iÓm s­ ®å")
		Talk(1,"no","12930:LÇn tr­íc ng­¬i ®· nhËn th­ëng, nªn phÇn th­ëng lÇn nµy kh«ng ®­îc nhiÒu. ")
	end
end

function shitu_1_cancel()
	RemoveIBBuff(216)
	SetTask(902,0)
	TaskNote(46,-1)
	Talk(1,"no","B¨ng Xuyªn rÊt nguy hiÓm. Ng­¬i ch­a ®ñ søc. H·y rÌn luyÖn thªm mét thêi gian n÷a!")
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


function  renwu3()
				Talk(1,"no",10450)
				Msg2Player("H·y mang r©u ThÇn Long vÒ cho Hå Hû MÞ ")
				SetTask(3,31)
				TaskNote(27,13)
end;

map_names=
{
	"B¾c H¶i",
	"YÕn S¬n",
	"Miªu C­¬ng",
	"Cù Léc",
	"Thñ D­¬ng s¬n",
	"T©y C«n L«n",
}

function  renwu1()
	local task_val = GetTask(53)
	local task_step = GetByte(task_val, 2)
	local task_state = GetByte(task_val, 1)
	if (task_state == 0)then
		MsgBox(10454,"yes_1","no")
	elseif(task_state == 1) then
		if (HaveEventItem(42) >= 1) then
			Talk(1,"no","T¹i <color=green>"..map_names[floor(task_step/6)+1].." <color> cã thÓ t×m thÊy <color=green>Phong Ên th¸p<color>, dïng <color=green>B¸ch Linh Ph­ín<color> th¶ <color=green>Thiªn C­¬ng Ma Tinh<color> ra. Sau khi thu phôc nã sÏ nhËn ®­îc <color=green>Thiªn C­¬ng ch©n khÝ<color>.")
		else
			Talk(1,"no","B¸ch Linh ph­ín ®· bÞ mÊt, h·y ®Õn <color=green>nhµ kh¶o cæ<color> xem thö. ")
		end
	elseif(task_state == 2)then
		if HaveEventItem(43) >= 1 then
			DelEventItem(43)
			AddEventItem(42)
			Msg2Player("Ch­a giÕt ®­îc Thiªn C­¬ng Tinh ®µo tÈu, B¸ch linh ph­ín tiÕp tôc phong Ên.")
			SetTask(53,SetByte(task_val,1,1))
			Talk(1,"no",10453)
		else
			Talk(1,"no","B¸ch Linh ph­ín ®· bÞ mÊt, h·y ®Õn <color=green>nhµ kh¶o cæ<color> xem thö.")
		end
	elseif(task_state == 3) then
		if HaveEventItem(44 )>= 1 then
			if(task_step == 35) then
				DelEventItem(44)
				AddOwnExp(500000)----½±Àø40000µã¾­Ñé
				Msg2Player("B¹n nhËn ®­îc ®iÓm danh väng")
				TaskNote(23,-1)
				AddCredit(108)----½±Àø108µãÉùÍû
				local playertype = GetPlayerType()
				if (playertype == 0) then
					if(random(1,200)<100) then
						AddNormalItem(0,0,31,6,0,0,0)
					else
						AddNormalItem(0,0,32,6,0,0,0)
					end
				elseif (playertype == 1) then
					AddNormalItem(0,0,33,6,0,0,0)
				elseif (playertype == 2) then
					AddNormalItem(0,0,34,6,0,0,0)
				end
			
				SetTask(53,SetByte(SetByte(task_val,1,0),2,task_step+1))
				Talk(1,"no","36 viªn Thiªn C­¬ng Tinh ®Òu quy tô vÒ! Lµm tèt l¾m! Xin nhËn phÇn th­ëng!")
			else
				DelEventItem(44)
				AddCredit(1)
				Msg2Player("Hoµn thµnh nhiÖm vô, nhËn ®­îc phÇn th­ëng!")
				TaskNote(23,-1)
				SetTask(53,SetByte(SetByte(task_val,1,0),2,task_step+1))
				Talk(1,"no",10452)
			end
		else
			Talk(1,"no","Thiªn C­¬ng Ch©n KhÝ ®· bÞ mÊt, h·y ®Õn <color=green>nhµ kh¶o cæ<color> xem thö. ")
		end
	end	
end;

function  yes_1()
	local task_val = GetTask(53)
	local task_step = GetByte(task_val, 2)
	local task_state = GetByte(task_val, 1)
	if (task_state == 0) then
		AddEventItem(42)
		Msg2Player("NhËn ®­îc B¸ch Linh ph­ín, ®i th¶ Thiªn C­¬ng Tinh")
		TaskNote(23, floor(task_step/6)+1)
		SetTask(53,SetByte(task_val,1,1))
		Talk(1,"no","T¹i <color=green>"..map_names[floor(task_step/6)+1].." <color> cã thÓ t×m thÊy <color=green>Phong Ên th¸p<color>, dïng <color=green>B¸ch Linh Ph­ín<color> th¶ <color=green>Thiªn C­¬ng Ma Tinh<color> ra. Sau khi thu phôc nã sÏ nhËn ®­îc <color=green>Thiªn C­¬ng ch©n khÝ<color>.")
	end
end;

function   renwu2()
		Talk(3,"next",10455,10456,10457)
end;

function next()
		Talk(2,"no",10459,10460)
		AddNormalItem(6,1,11,1,0,0)
		AddNormalItem(6,1,11,1,0,0)
		AddNormalItem(6,1,11,1,0,0)
		Msg2Player("H·y BiÕn thµnh Phi Thö vµ Ngäc N÷ ®Õn gÆp NhËm §¹i Ca, sau ®ã biÕn thµnh biÓu t­îng t×nh yªu ®Õn gÆp NhËm §¹i TÈu")
		TaskNote(26,1)
		SetTask(92,2)
end;


function no()
		CloseDialog()
end;

function  renwu4()
	local task_step = GetByte(GetTask(53), 2)
	if task_step > 35 then
		Talk(1,"no",10119)
	else	
		Talk(1,"one",10120)
	end;
end;

function  one()
		local  p={}
		local  n=0
		for n = 250,261 do
				if(GetTask(n)==0)then
						p[n-249]="green"
				elseif(GetTask(n)==1)then
						p[n-249]="red"
				end;
		end;
		Talk(1,"two","<color="..p[1]..">Thiªn Kh«i Tinh<color>-----<color="..p[2]..">Thiªn C­¬ng Tinh<color>-----<color="..p[3]..">Thiªn C¬ Tinh<color>-----<color="..p[4]..">Thiªn Nhµn Tinh<color>-----<color="..p[5]..">Thiªn Dòng Tinh<color>-----<color="..p[6]..">Thiªn Hïng Tinh<color>-----<color="..p[7]..">Thiªn M·nh Tinh<color>-----<color="..p[8]..">Thiªn Uy Tinh<color>-----<color="..p[9]..">Thiªn Anh Tinh<color>-----<color="..p[10]..">Thiªn Quý Tinh<color>-----<color="..p[11]..">Thiªn Phóc Tinh<color>-----<color="..p[12]..">Thiªn M·n Tinh<color>")
end;

function  two()
		local  p={}
		local  n=0
		for n = 262,273 do
				if(GetTask(n)==0)then
						p[n-249]="green"
				elseif(GetTask(n)==1)then
						p[n-249]="red"
				end;
		end;
		Talk(1,"three","<color="..p[13]..">Thiªn C« Tinh<color>-----<color="..p[14]..">Thiªn S¬n Tinh<color>-----<color="..p[15]..">Thiªn VÞ Tinh<color>-----<color="..p[16]..">Thiªn TiÖp Tinh<color>-----<color="..p[17]..">Thiªn ¸m Tinh<color>-----<color="..p[18]..">Thiªn H÷u Tinh<color>-----<color="..p[19]..">Thiªn Kh«ng Tinh<color>-----<color="..p[20]..">Thiªn Tèc Tinh<color>-----<color="..p[21]..">Thiªn DÞ Tinh<color>-----<color="..p[22]..">Thiªn S¸t Tinh<color>-----<color="..p[23]..">Thiªn Vi Tinh<color>-----<color="..p[24]..">Thiªn Cøu Tinh<color>")
end;

function  three()
		local  p={}
		local  n=0
		for n = 274,285 do
				if(GetTask(n)==0)then
						p[n-249]="green"
				elseif(GetTask(n)==1)then
						p[n-249]="red"
				end;
		end;
		Talk(1,"no","<color="..p[25]..">Thiªn Thèi Tinh<color>-----<color="..p[26]..">Thiªn Thä Tinh<color>-----<color="..p[27]..">Thiªn KiÕm Tinh<color>-----<color="..p[28]..">Thiªn B×nh Tinh<color>-----<color="..p[29]..">Thiªn Téi Tinh<color>-----<color="..p[30]..">Thiªn Tæn Tinh<color>-----<color="..p[31]..">Thiªn B¹i Tinh<color>-----<color="..p[32]..">Thiªn Lao Tinh<color>-----<color="..p[33]..">Thiªn TuÖ Tinh<color>-----<color="..p[34]..">Thiªn B¹o Tinh<color>-----<color="..p[35]..">Thiªn Chó Tinh<color>-----<color="..p[36]..">Thiªn X¶o Tinh<color>")
end;

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1020_01; not a claim of original appearance.
function pt_guard()
    local w = GetContentWorldPos()+1000
    if w ~= 1020 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("§Çu mèi Thiªn C­¬ng Tinh Quy VÞ vµ Phu Thª t¹i T©y Kú. Tªn trong Lua gèc lµ D­¬ng TiÔn, kh«ng ph¶i ®­êng dÉn NhÞ Lang ThÇn ë Diªu Tr×.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Loi Chan Tu (163/187); Vo Vuong (159/189); Khuong Tu Nha (158/189); Thay Tuong So (174/188); Nham Dai Ca (170/195)", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_status()
    if pt_guard() == 0 then return end
    Say("Cã nh¸nh Lua VNG gèc trong PAK. C¸c ®iÒu kiÖn vµ giao dÞch do nh¸nh gèc kiÓm tra; ch­a nghiÖm thu toµn bé nhiÖm vô.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_close()
    CloseDialog()
end
function pt_original()
    if pt_guard() == 0 then return end
    pt_original_main()
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
        if PTGReady(a) then tinsert(options,a.label.."/ptg_a"..i) end
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

PTG_MAP=1020
PTG_ROLE="duong"
PTG_OLD_MAIN=main
PTG_OLD_MARK=GetPlayerTaskState
function main() return PTGMenu() end
function GetPlayerTaskState() return PTGMarker() end
PTG_OLD_SAYTASK=SayTask
function SayTask(text,choices)
 if GetPlayerType()==0 then for n=1,getn(choices) do local t=choices[n][1];if PTG_OLD_TITLES[t] then choices[n].show=0 end end end
 return PTG_OLD_SAYTASK(text,choices)
end
PTG_OLD_TITLES={["T©n Thøc"]=1,["Dòng §ao"]=1,["KiÕm ƒi"]=1,["Trung Thµnh"]=1,["TrÇm H­¬ng"]=1,["Khuyªn Hµng"]=1,["ThÇn Long"]=1,["B×nh An"]=1,["ThÇn Dô KÝnh"]=1,["Tam S¸ch"]=1,["Cæ Kim"]=1,["Thiªn Duyªn"]=1,["Th«ng Thiªn"]=1,["Trõ Häa"]=1,["Khuyªn hµng"]=1,["Tam s¸ch"]=1}
function ptg_a1() PTGPreview(1) end
function ptg_yes1() PTGApply(1) end
function ptg_a2() PTGPreview(2) end
function ptg_yes2() PTGApply(2) end
function ptg_a3() PTGPreview(3) end
function ptg_yes3() PTGApply(3) end
function ptg_a4() PTGPreview(4) end
function ptg_yes4() PTGApply(4) end
function ptg_a5() PTGPreview(5) end
function ptg_yes5() PTGApply(5) end
function ptg_a6() PTGPreview(6) end
function ptg_yes6() PTGApply(6) end
function ptg_a7() PTGPreview(7) end
function ptg_yes7() PTGApply(7) end
function ptg_a8() PTGPreview(8) end
function ptg_yes8() PTGApply(8) end
function ptg_a9() PTGPreview(9) end
function ptg_yes9() PTGApply(9) end
function ptg_a10() PTGPreview(10) end
function ptg_yes10() PTGApply(10) end
function ptg_a11() PTGPreview(11) end
function ptg_yes11() PTGApply(11) end
function ptg_a12() PTGPreview(12) end
function ptg_yes12() PTGApply(12) end
function ptg_a13() PTGPreview(13) end
function ptg_yes13() PTGApply(13) end
function ptg_a14() PTGPreview(14) end
function ptg_yes14() PTGApply(14) end
function ptg_a15() PTGPreview(15) end
function ptg_yes15() PTGApply(15) end
function ptg_a16() PTGPreview(16) end
function ptg_yes16() PTGApply(16) end
function ptg_a17() PTGPreview(17) end
function ptg_yes17() PTGApply(17) end
function ptg_a18() PTGPreview(18) end
function ptg_yes18() PTGApply(18) end
function ptg_a19() PTGPreview(19) end
function ptg_yes19() PTGApply(19) end
function ptg_a20() PTGPreview(20) end
function ptg_yes20() PTGApply(20) end
function ptg_a21() PTGPreview(21) end
function ptg_yes21() PTGApply(21) end
function ptg_a22() PTGPreview(22) end
function ptg_yes22() PTGApply(22) end
function ptg_a23() PTGPreview(23) end
function ptg_yes23() PTGApply(23) end
function ptg_a24() PTGPreview(24) end
function ptg_yes24() PTGApply(24) end
function ptg_a25() PTGPreview(25) end
function ptg_yes25() PTGApply(25) end
function ptg_a26() PTGPreview(26) end
function ptg_yes26() PTGApply(26) end
function ptg_a27() PTGPreview(27) end
function ptg_yes27() PTGApply(27) end
function ptg_a28() PTGPreview(28) end
function ptg_yes28() PTGApply(28) end
function ptg_a29() PTGPreview(29) end
function ptg_yes29() PTGApply(29) end
function ptg_a30() PTGPreview(30) end
function ptg_yes30() PTGApply(30) end
function ptg_a31() PTGPreview(31) end
function ptg_yes31() PTGApply(31) end
function ptg_a32() PTGPreview(32) end
function ptg_yes32() PTGApply(32) end
function ptg_a33() PTGPreview(33) end
function ptg_yes33() PTGApply(33) end
function ptg_a34() PTGPreview(34) end
function ptg_yes34() PTGApply(34) end
function ptg_a35() PTGPreview(35) end
function ptg_yes35() PTGApply(35) end
function ptg_a36() PTGPreview(36) end
function ptg_yes36() PTGApply(36) end
function ptg_a37() PTGPreview(37) end
function ptg_yes37() PTGApply(37) end
function ptg_a38() PTGPreview(38) end
function ptg_yes38() PTGApply(38) end
function ptg_a39() PTGPreview(39) end
function ptg_yes39() PTGApply(39) end
function ptg_a40() PTGPreview(40) end
function ptg_yes40() PTGApply(40) end
function ptg_a41() PTGPreview(41) end
function ptg_yes41() PTGApply(41) end
function ptg_a42() PTGPreview(42) end
function ptg_yes42() PTGApply(42) end
function ptg_a43() PTGPreview(43) end
function ptg_yes43() PTGApply(43) end
function ptg_a44() PTGPreview(44) end
function ptg_yes44() PTGApply(44) end
function ptg_a45() PTGPreview(45) end
function ptg_yes45() PTGApply(45) end
function ptg_a46() PTGPreview(46) end
function ptg_yes46() PTGApply(46) end
function ptg_a47() PTGPreview(47) end
function ptg_yes47() PTGApply(47) end
function ptg_a48() PTGPreview(48) end
function ptg_yes48() PTGApply(48) end
function ptg_a49() PTGPreview(49) end
function ptg_yes49() PTGApply(49) end
function ptg_a50() PTGPreview(50) end
function ptg_yes50() PTGApply(50) end
function ptg_a51() PTGPreview(51) end
function ptg_yes51() PTGApply(51) end
function ptg_a52() PTGPreview(52) end
function ptg_yes52() PTGApply(52) end
function ptg_a53() PTGPreview(53) end
function ptg_yes53() PTGApply(53) end
function ptg_a54() PTGPreview(54) end
function ptg_yes54() PTGApply(54) end
function ptg_a55() PTGPreview(55) end
function ptg_yes55() PTGApply(55) end
function ptg_a56() PTGPreview(56) end
function ptg_yes56() PTGApply(56) end
function ptg_a57() PTGPreview(57) end
function ptg_yes57() PTGApply(57) end
function ptg_a58() PTGPreview(58) end
function ptg_yes58() PTGApply(58) end
function ptg_a59() PTGPreview(59) end
function ptg_yes59() PTGApply(59) end
function ptg_a60() PTGPreview(60) end
function ptg_yes60() PTGApply(60) end
function ptg_a61() PTGPreview(61) end
function ptg_yes61() PTGApply(61) end
function ptg_a62() PTGPreview(62) end
function ptg_yes62() PTGApply(62) end

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
        if GetLevel()>=31 and (target==0 or target>=22 and target<=25) then tinsert(options,"Tø T­îng/pts_elements") end
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
        if q.task==41 and phase==0 then row=nil end
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

PTS_MAP=1020
PTS_ROLE="duong"
PTS_OLD_MAIN=main
PTS_HAS_SERVICE=1
PTS_OLD_SAYTASK=SayTask
PTS_OLD_TITLES={["Phi Tiªn"]=1,["Trïng Méc"]=1,["Vi Lao"]=1,["Phu Thª"]=1,["Yªn Phóc"]=1,["Minh Ch©u"]=1,["Quy Tinh"]=1,["Tø T­îng"]=1}
function SayTask(text,choices)
 for n=1,getn(choices) do if PTS_OLD_TITLES[choices[n][1]] then choices[n].show=0 end end
 return PTS_OLD_SAYTASK(text,choices)
end
function main() if GetPlayerType()==0 then return PTGMenu() end;return PTSMenu() end
function pts_preview1() PTSPreview(1) end
function pts_apply1() PTSApply(1) end
function pts_preview2() PTSPreview(2) end
function pts_apply2() PTSApply(2) end
function pts_preview3() PTSPreview(3) end
function pts_apply3() PTSApply(3) end
function pts_preview4() PTSPreview(4) end
function pts_apply4() PTSApply(4) end
function pts_preview5() PTSPreview(5) end
function pts_apply5() PTSApply(5) end
function pts_preview6() PTSPreview(6) end
function pts_apply6() PTSApply(6) end
function pts_preview7() PTSPreview(7) end
function pts_apply7() PTSApply(7) end
function pts_preview8() PTSPreview(8) end
function pts_apply8() PTSApply(8) end
function pts_preview9() PTSPreview(9) end
function pts_apply9() PTSApply(9) end
function pts_preview10() PTSPreview(10) end
function pts_apply10() PTSApply(10) end
function pts_preview11() PTSPreview(11) end
function pts_apply11() PTSApply(11) end
function pts_preview12() PTSPreview(12) end
function pts_apply12() PTSApply(12) end
function pts_preview13() PTSPreview(13) end
function pts_apply13() PTSApply(13) end
function pts_preview14() PTSPreview(14) end
function pts_apply14() PTSApply(14) end
function pts_preview15() PTSPreview(15) end
function pts_apply15() PTSApply(15) end
function pts_preview16() PTSPreview(16) end
function pts_apply16() PTSApply(16) end
function pts_preview17() PTSPreview(17) end
function pts_apply17() PTSApply(17) end
function pts_preview18() PTSPreview(18) end
function pts_apply18() PTSApply(18) end
function pts_preview19() PTSPreview(19) end
function pts_apply19() PTSApply(19) end
function pts_preview20() PTSPreview(20) end
function pts_apply20() PTSApply(20) end
function pts_preview21() PTSPreview(21) end
function pts_apply21() PTSApply(21) end
function pts_preview22() PTSPreview(22) end
function pts_apply22() PTSApply(22) end
function pts_preview23() PTSPreview(23) end
function pts_apply23() PTSApply(23) end
function pts_preview24() PTSPreview(24) end
function pts_apply24() PTSApply(24) end
function pts_preview25() PTSPreview(25) end
function pts_apply25() PTSApply(25) end
function pts_preview26() PTSPreview(26) end
function pts_apply26() PTSApply(26) end
function pts_preview27() PTSPreview(27) end
function pts_apply27() PTSApply(27) end
function pts_preview28() PTSPreview(28) end
function pts_apply28() PTSApply(28) end
function pts_preview29() PTSPreview(29) end
function pts_apply29() PTSApply(29) end
function pts_preview30() PTSPreview(30) end
function pts_apply30() PTSApply(30) end
function pts_preview31() PTSPreview(31) end
function pts_apply31() PTSApply(31) end
function pts_preview32() PTSPreview(32) end
function pts_apply32() PTSApply(32) end

function PTSMarker()
    if not PTSClass() or PTSMap()~=PTS_MAP then return 0,0 end
    local best,level=0,0
    for i=1,getn(PTS_ACTIONS) do
        local a=PTS_ACTIONS[i]
        if a.role==PTS_ROLE and GetLevel()>=a.level and GetTask(a.task)==a.from then
            local state=PTSHave(a.needs)==1 and (not a.morph or GetMorphType()==a.morph) and (a.from==0 and 1 or 3) or 2
            if state>best then best=state;level=a.level end
        end
    end
    if PTS_ROLE=="duong" and GetLevel()>=59 then
        local v=GetTask(53);local state=GetByte(v,1);local count=GetByte(v,2)
        if count<36 then local mark=state==0 and 1 or (state==3 and HaveEventItem(44)>0 and 3 or 2);if mark>best then best=mark;level=59 end end
    end
    if (PTS_ROLE=="store_west" or PTS_ROLE=="store_east") and GetLevel()>=31 then
        local target=GetTask(55);local mark=target==0 and 1 or (target>=22 and target<=25 and HaveNormalItem(3,target,0,0)>=10 and 3 or 2)
        if mark>best then best=mark;level=31 end
    end
    return best,level
end

PTS_PREVIOUS_MARK=GetPlayerTaskState
function GetPlayerTaskState()
    local state,level=PTSMarker()
    if PTS_PREVIOUS_MARK then local old,required=PTS_PREVIOUS_MARK();if old and old>state then return old,required end end
    return state,level
end

Include("\\script\\phongthan\\restoration\\tutuong.lua")

-- Bot capability metadata; no quest or reward logic is replaced.
Include("\\script\\phongthan\\bots\\npc_goals.lua")

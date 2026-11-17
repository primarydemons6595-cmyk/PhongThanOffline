
--description:ÄÄß¸
--author: yichuan
--date:2004/7/11
--900,¶«º£Ì½ÏÕ

function  main()
	tasks = 
	{
		{" §æi søc lùc","panduan";show=0},
		{"§«ng H¶i ThÝ LuyÖn","shitu_1";show=0}
	}
	if(GetLevel()>20)then
		tasks[1].show=1
	end
	if(GetLevel()>30)and(GetLevel()<=40)and(GetTask(900)<=7)then	--®{§Ìµ¥¯Åok¡A®{§Ì¨S¦³§¹¦¨¹L
		tasks[2].show=1;
	end;
	SayTask(10424,tasks)
end;

function  shitu_1()
	local mark=judge_relation()
	if(mark==1)then		--º¡¨¬®v®{2¤H¶¤ 
		if(GetTask(900)==0)then		--±µ¥ô°È
			MsgBox("S­ phô cã nãi: muèn t¨ng nhanh n¨ng lùc tu chØ cÇn v­ît qua nh÷ng mª cung ma qu¸i. Ng­¬i cã muèn thö kh«ng#¿","shitu_1_begin","no")
		elseif(GetTask(900)==6)then	--§¹¦¨¥ô°È
			MsgBox("Chóc mõng ng­¬i! Cã ph¶i ng­¬i c¶m thÊy m¹nh h¬n tr­íc nhiÒu? ","shitu_1_end","no")
		elseif(GetTask(900)==7)then
			Talk(1,"no","Ng­¬i ®· hoµn thµnh nhiÖm vô ThÝ LuyÖn §«ng H¶i. NÕu muèn n©ng cao n¨ng lùc cã thÓ ®Õn TriÒu Ca gÆp s­ huynh Na Tra cña ta!")
		else				--©ñ±ó¥ô°È
			MsgBox("Dôc tèc bÊt ®¹t! HiÖn n¨ng lùc cña ng­¬i ch­a ®ñ ®Ó hoµn thµnh thö th¸ch nµy. H·y quay l¹i sau!","shitu_1_cancel","no")
		end
	else
		if(GetTask(900)==0)then
			Talk(1,"no","Muèn t¨ng nhanh n¨ng lùc chØ cÇn x«ng qua nh÷ng mª cung  ma qu¸i. NÕu ng­¬i ®¹t cÊp 31 cã thÓ lËp tæ ®éi víi s­ phô ®Ó v­ît mª cung. ")
		elseif(GetTask(900)==6)then
			Talk(1,"no","NhiÖm vô s­ ®å th¸m hiÓm cÇn 2 s­ ®å t¹o thµnh 1 nhãm. Xin x¸c nhËn tæ ®éi ®· h×nh thµnh!")
		elseif(GetTask(900)==7)then
			Talk(1,"no","Ng­¬i ®· hoµn thµnh nhiÖm vô ThÝ LuyÖn §«ng H¶i. NÕu muèn n©ng cao n¨ng lùc cã thÓ ®Õn TriÒu Ca gÆp s­ huynh Hoµng Thiªn Hãa cña ta!")
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
			SetTask(900,1)
			TaskNote(44,5)
			if(GetTask(899)<7)then
				SetTask(899,0)
				TaskNote(43,-1)
			end
			if(GetTask(901)<7)then
				SetTask(901,0)
				TaskNote(45,-1)
			end
			if(GetTask(902)<7)then
				SetTask(902,0)
				TaskNote(46,-1)
			end
			Talk(2,"no","Ng­¬i hiÖn ®øng trong vßng <color=yellow>Dòng gi¶<color>, tr­íc khi vßng trßn nµy mÊt ®i ph¶i phôc mÖnh <color=green>§¹i phu<color> ë mçi tÇng Mª cung §«ng H¶i, nÕu hoµn thµnh xem nh­ thÝ luyÖn thµnh c«ng!","BÊt cø tæn th­¬ng nµo còng cã thÓ khiÕn vßng trßn cña ng­¬i mÊt ®i. C¶ qu¸ tr×nh nµy ng­¬i ph¶i cïng víi s­ phô thùc hiÖn. §¹i phu cña mçi tÇng sÏ trÞ th­¬ng gióp ng­¬i. ")
		else
			Talk(1,"no","Tr¹ng th¸i cña ng­¬i hiÖn kh«ng thÓ tiÕp nhËn nhiÖm vô s­ ®å. Sau khi tr¹ng th¸i nµy kÕt thóc h·y quay l¹i!")
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
	SetTask(900,7)
	TaskNote(44,6)
	TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>"..exp.."kinh nghiÖm")
	local mark=step_complete()
	if(mark==1)and(GetTask(907)==0)then	--§¹¦¨¤F¤@¦¸±´ÀI¥ô°È¥B¨S¦³»â¨ú¹Lºñ¦âÀY²¯
		AddNormalItem(0,7,GetPlayerType()+6,4,0,0)
		SetTask(907,1)
		Talk(1,"no","Ng­¬i thËt xuÊt s¾c! Xin  nhËn phÇn th­ëng!")
		AddGlobalCountNews("<color=green>"..GetName().."<color> vµ s­ phô hoµn thµnh nhiÖm vô th¸m hiÓm, nhËn ®­îc <color=green>®Çu kh«i<color>",20)
--	elseif(mark==2)and(GetTask(903)==0)then		--§¹¦¨¤F¤G¦¸±´ÀI¥ô°È¥B¨S¦³»â¨ú¹LÂÅ¦â§¤ÃM
--		AddNormalItem(3,14,0,0,0,0)
--		SetTask(903,1)
--		Talk(1,"no","­þ¦\¡G¯u¬O¤£Â²³æ¡A§A¤w¸g§¹¦¨¤F¨â¶µ«iÂô°g®cªº¸Õ½m¡A§Ú³o¸Ì¦³¤@¤Ç¯«¾s¡A´NÃØ»P§A¤F¡I")
	elseif(mark==4)and(GetTask(904)==0)then		--§¹¦¨¤F¥|¦¸±´ÀI¥ô°È¥B¨S¦³»â¨ú¹L50ÂÅ§¤ÃM
		AddNormalItem2(0,10,GetPlayerType()+15,9,0,0)
		SetTask(904,1)
		Talk(1,"no","Ng­¬i thËt xuÊt s¾c! Xin  nhËn phÇn th­ëng!")
		AddGlobalCountNews("<color=green>"..GetName().."<color>vµ s­ phô hoµn thµnh nhiÖm vô th¸m hiÓm, nhËn ®­îc <color=green>thó c­ìi cÊp 50<color>",20)
	else
		Talk(1,"no","ThÇy trß 2 ng­êi phèi hîp thËt xuÊt s¾c!")
	end
end

function shitu_1_end_M()
	local step=GetTask(900)
	if(step<100)then
		AddMasterPRValue(7)
		SetTask(900,100)
		TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>7 ®iÓm s­ ®å")
	else
		AddMasterPRValue(3)
		TopMessage("Chóc mõng! B¹n nhËn ®­îc <color=green>3 ®iÓm s­ ®å")
		Talk(1,"no","LÇn tr­íc ng­¬i ®· nhËn th­ëng, nªn phÇn th­ëng lÇn nµy kh«ng ®­îc nhiÒu. ")
	end
end

function shitu_1_cancel()
	RemoveIBBuff(216)
	SetTask(900,0)
	TaskNote(44,-1)
	Talk(1,"no","§«ng H¶i rÊt nguy hiÓm. N¨ng lùc ng­¬i vÉn ch­a ®ñ. H·y rÌn luyÖn thªm mét thêi gian n÷a!")
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

function  getmaxweight1(level)
	if(level>20)and(level<=25)then
		return 350
	elseif(level>25)and(level<=30)then
		return 400
	elseif(level>30)and(level<=35)then
		return 450
	elseif(level>35)and(level<=40)then
		return 500
	elseif(level>40)and(level<=45)then
		return 550
	elseif(level>45)and(level<=50)then
		return 600
	elseif(level>50)and(level<=55)then
		return 650
	elseif(level>55)and(level<=60)then
		return 700
	elseif(level>60)and(level<=65)then
		return 750
	elseif(level>65)and(level<=70)then
		return 800
	elseif(level>70)and(level<=75)then
		return 850
	elseif(level>75)and(level<=80)then
		return 900
	elseif(level>80)and(level<=85)then
		return 950
	elseif(level>85)and(level<=90)then
		return 1000
	elseif(level>90)and(level<=95)then
		return 1050
	elseif(level>95)and(level<=100)then
		return 1100
	elseif(level>100)and(level<=105)then
		return 1150
	elseif(level>105)and(level<=110)then
		return 1200
	elseif(level>110)and(level<=115)then
		return 1250
	elseif(level>115)and(level<=120)then
		return 1300
	elseif(level>120)and(level<=125)then
		return 1350
	elseif(level>125)and(level<=130)then
		return 1400
	elseif(level>130)and(level<=135)then
		return 1450
	elseif(level>135)and(level<=140)then
		return 1500
	elseif(level>140)and(level<=145)then
		return 1550
	elseif(level>145)and(level<=150)then
		return 1600
	elseif(level>150)and(level<=155)then
		return 1650
	elseif(level>155)and(level<=160)then
		return 1700
	elseif(level>160)and(level<=165)then
		return 1750
	elseif(level>165)and(level<=170)then
		return 1800
	end
end

function getmaxweight(level)
	local i=level*10+100
	if(i>1800)then
		i=1800
	end
	return  i
end

function  panduan()
	local maxweight=getmaxweight(GetLevel())
	if(GetNativeWeightMax()>=maxweight)then
		Talk(1,"no1",10431)
	else
		fuzhong()
	end
end

function  fuzhong()
		if(GetNativeWeightMax()<400)then
				MsgBox(10425,"zj1","no1")
		elseif(GetNativeWeightMax()<600)then
				MsgBox(10426,"zj2","no1")
		elseif(GetNativeWeightMax()<700)then
				MsgBox(10427,"zj3","no1")
		elseif(GetNativeWeightMax()<800)then
				MsgBox(10428,"zj4","no1")
		elseif(GetNativeWeightMax()<850)then
				MsgBox("§æi 1 ®iÓm søc lùc cÇn <color=red>40 ®iÓm danh väng, 1 ®iÓm s­ ®å vµ "..(10*GetNativeWeightMax()).." l­îng<color>,  x¸c ®Þnh chø?","zj5","no1")
		elseif(GetNativeWeightMax()<900)then
				MsgBox("§æi 1 ®iÓm søc lùc cÇn <color=red>80 ®iÓm danh väng, 1 ®iÓm s­ ®å vµ "..(10*GetNativeWeightMax()).." l­îng<color>,  x¸c ®Þnh chø?","zj6","no1")
		elseif(GetNativeWeightMax()<1000)then
				MsgBox("§æi 1 ®iÓm søc nÆng cÇn <color=red>100 ®iÓm danh väng, 1 ®iÓm s­ ®å vµ "..(20*GetNativeWeightMax()).." l­îng<color>,  x¸c ®Þnh chø?","zj7","no1")
		elseif(GetNativeWeightMax()<1100)then
				MsgBox("§æi 1 ®iÓm søc nÆng cÇn <color=red>100 ®iÓm danh väng, 1 ®iÓm s­ ®å vµ "..(50*GetNativeWeightMax()).." l­îng<color>,  x¸c ®Þnh chø?","zj8","no1")	
		else
				MsgBox("§æi 1 ®iÓm søc nÆng cÇn <color=red>150 ®iÓm danh väng, 2 ®iÓm s­ ®å vµ "..(100*GetNativeWeightMax()).." l­îng<color>,  x¸c ®Þnh chø?","zj9","no1")
		end;
end;

function   zj1()
		if(GetCredit()<5)then
				Talk(1,"no1",10430)
		else
				DecCredit(5)
				AddWeightMax(1)
				MsgBox(10432,"panduan","no1")
		end;
end;

function   zj2()
		if(GetCredit()<10)then
				Talk(1,"no1",10430)
		else
				DecCredit(10)
				AddWeightMax(1)
				MsgBox(10432,"panduan","no1")
		end;
end;


function   zj3()
		if(GetCredit()<20)then
				Talk(1,"no1",10430)
		else
				DecCredit(20)
				AddWeightMax(1)
				MsgBox(10432,"panduan","no1")
		end;
end;


function   zj4()
		if(GetCredit()<40)then
				Talk(1,"no1",10430)
		else
				DecCredit(40)
				AddWeightMax(1)
				MsgBox(10432,"panduan","no1")
		end;
end;

function   zj5()
		if(GetCredit()<40)then
				Talk(1,"no1",10430)
		else
			if(GetMasterPRValue()>=1)then
				if(GetCash()>=10*GetNativeWeightMax())then
					DecCredit(40)
					DecMasterPRValue(1)
					Pay(10*GetNativeWeightMax())
					AddWeightMax(1)
					MsgBox(10432,"panduan","no1")
				else
					Talk(1,"no1","Ng­¬i kh«ng ®ñ tiÒn!")
				end
			else
				Talk(1,"no1","1 ®iÓm søc lùc t­¬ng øng víi 1 ®iÓm s­ ®å. Ng­¬i kh«ng ®ñ ®iÓm s­ ®å.")
			end
		end;
end

function   zj6()
		if(GetCredit()<80)then
				Talk(1,"no1",10430)
		else
			if(GetMasterPRValue()>=1)then
				if(GetCash()>=10*GetNativeWeightMax())then
					DecCredit(80)
					DecMasterPRValue(1)
					Pay(10*GetNativeWeightMax())
					AddWeightMax(1)
					MsgBox(10432,"panduan","no1")
				else
					Talk(1,"no1","Ng­¬i kh«ng ®ñ tiÒn!")
				end
			else
				Talk(1,"no1","1 ®iÓm søc lùc t­¬ng øng víi 1 ®iÓm s­ ®å. Ng­¬i kh«ng ®ñ ®iÓm s­ ®å.")
			end
		end;
end

function   zj7()
		if(GetCredit()<100)then
				Talk(1,"no1",10430)
		else
			if(GetMasterPRValue()>=1)then
				if(GetCash()>=20*GetNativeWeightMax())then
					DecCredit(100)
					DecMasterPRValue(1)
					Pay(20*GetNativeWeightMax())
					AddWeightMax(1)
					MsgBox(10432,"panduan","no1")
				else
					Talk(1,"no1","Ng­¬i kh«ng ®ñ tiÒn!")
				end
			else
				Talk(1,"no1","1 ®iÓm søc lùc t­¬ng øng víi 1 ®iÓm s­ ®å. Ng­¬i kh«ng ®ñ ®iÓm s­ ®å.")
			end
		end;
end

function   zj8()
		if(GetCredit()<100)then
				Talk(1,"no1",10430)
		else
			if(GetMasterPRValue()>=1)then
				if(GetCash()>=50*GetNativeWeightMax())then
					DecCredit(100)
					DecMasterPRValue(1)
					Pay(50*GetNativeWeightMax())
					AddWeightMax(1)
					MsgBox(10432,"panduan","no1")
				else
					Talk(1,"no1","Ng­¬i kh«ng ®ñ tiÒn!")
				end
			else
				Talk(1,"no1","1 ®iÓm søc lùc t­¬ng øng víi 1 ®iÓm s­ ®å. Ng­¬i kh«ng ®ñ ®iÓm s­ ®å.")
			end
		end;
end

function   zj9()
		if(GetCredit()<150)then
				Talk(1,"no1",10430)
		else
			if(GetMasterPRValue()>=2)then
				if(GetCash()>=100*GetNativeWeightMax())then
					DecCredit(150)
					DecMasterPRValue(2)
					Pay(100*GetNativeWeightMax())
					AddWeightMax(1)
					MsgBox(10432,"panduan","no1")
				else
					Talk(1,"no1","Ng­¬i kh«ng ®ñ tiÒn!")
				end
			else
				Talk(1,"no1","1 ®iÓm søc lùc t­¬ng øng víi 2 ®iÓm s­ ®å. Ng­¬i kh«ng ®ñ ®iÓm s­ ®å.")
			end
		end;
end

--function   zj5()
--		if(GetCredit()<100)then
--				Talk(1,"no1",10430)
--		elseif(GetNativeWeightMax()>=(GetLevel()*10+100))then
--				Talk(1,"no1",10431)
--		else
--				DecCredit(100)
--				AddWeightMax(1)
--				MsgBox(10432,"fuzhong","no1")
--		end;
--end;

function no1()
		CloseDialog()
end;

function no()
	CloseDialog()
end
-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1020 then CloseDialog(); return end
    return pt_original_main()
end

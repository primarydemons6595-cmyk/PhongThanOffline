--description: ²·¹ìÊ¦¡ª¡ªÎ÷Æç¢ÁÊ±ÍÚ¹ìÑ°±¦¡¢³¯¸è»î¶¯Ïà¹Ø
--author: liuhongbo
--date: 2005/5/26

--382£¬¼ÇÂ¼ÍÚ¹ì?Îñx×ø±ê
--381£¬¼ÇÂ¼ÍÚ¹ì±¦²ØµØµã
--383£¬¼ÇÂ¼ÍÚ¹ì?Îñy×ø±ê
--380£¬¼ÇÂ¼ÍÚ¹ì?Îñ½Óµ½µÄÊ±¼ä
--384£¬¼ÇÂ¼ÍÚ¹ìËùÏûºÄµÄÊ±¼ä
--826,827,828,829,830,831,832,833·Ö±ğ¼ÇÂ¼ÊÔÁ·?ÎñµÄ×Ü±äÁ¿\µØ¡¢Ë®¡¢»ğ¡¢É½¡¢Ôó¡¢·ç¡¢À×±ê¼Ç
--561¼ÇÂ¼ÍÚ±¦±È?²½Öè;562ÅĞ¶Ï½Ó±È??ÎñÏµÍ³Ê±¼ä;563¼ÇÂ¼ÍÚ±¦ËùÓÃÊ±¼ä;564ÅĞ¶ÏÊÇ²»ÊÇÓÃÌì¹ìÍÚ±¦;565¼ÇÂ¼·ÅÆú±È?Ê±¼ä;

mapname = {
			[1]="Phong ThÇn ®µi",
			[2]="Sïng thµnh ®¹i doanh",
			[3]="Ngäc H­ cung",
			[4]="Xi V­u Mé",
			[5]="ChiÕn tr­êng Phong Yªn",
			[6]="ChiÕn tr­êng Phong Yªn",
			[7]="ChiÕn tr­êng Phong Yªn",
			[8]="Mang Mang TuyÕt Nguyªn",
			[9]="Mang Mang TuyÕt Nguyªn",
			[10]="Mang Mang TuyÕt Nguyªn",
			[11]="Lôc L©m Th©m Xø",
			[12]="Lôc L©m Th©m Xø",
			[13]="Lôc L©m Th©m Xø",
			[14]="Phong DiÖp Thô L©m",
			[15]="Th¹ch KiÒu TiÓu Kİnh",
			[16]="Th¹ch L©m TiÔu Bİch",
			[17]="Phong DiÖp Thô L©m",
			[18]="Th¹ch KiÒu TiÓu Kİnh",
			[19]="TuyÖt Long lÜnh",
			[20]="T©y Kú",
			[21]="TriÒu Ca",
			[22]="Hoang m¹c",
			[23]="Sa M¹c Thæ Thµnh",
			[24]="Sa M¹c Phong Than",
			[25]="Sa M¹c Lôc Ch©u",
			[26]="Sa M¹c chÕt",
			[27]="Hiªn Viªn ®éng tÇng 1 ",
			[28]="Hiªn Viªn ®éng tÇng 2 ",
			[29]="Hiªn Viªn ®éng tÇng 3 ",
			[30]="Hiªn Viªn ®éng tÇng 4 ",
			[31]="Hiªn Viªn ®éng tÇng 5 ",
			[32]="Ngäc TuyÒn B¨ng Xuyªn",
			[33]="B¨ng Xuyªn TuyÕt Cèc",
			[34]="§¹i Phong B¨ng Xuyªn",
			[35]="§¹i Th¹ch B¨ng Xuyªn",
			[36]="B¨ng Xuyªn Chi Cùc",
			[37]="§«ng H¶i Thñy Vùc",
			[38]="§«ng H¶i Long Cung",
			[39]="§«ng H¶i H¶i C©u",
			[40]="§«ng H¶i Long Vùc",
			[41]="§«ng H¶i Long Uyªn",
			[42]="Bİch Du cung tÇng 1 ",
			[43]="Bİch Du cung tÇng 2",
			[44]="Bİch Du cung tÇng 3",
			[45]="Bİch Du cung tÇng 4",
			[46]="Bİch Du cung tÇng 5",
			[47]="Khæn Tiªn cung tÇng 1",
			[48]="Khæn Tiªn cung tÇng 2",
			[49]="Khæn Tiªn cung tÇng 3",
			[50]="Khæn Tiªn cung tÇng 4",
			[51]="Khæn Tiªn cung tÇng 5",
			[52]="Diªu Tr×",
			[53]="§¹i H¶i",
			[54]="Bång Lai",
			[55]="§«ng Doanh",
			[56]="Ph­¬ng Tr­îng",
			[57]="Kho¸ng tr­êng",
			[58]="D­îc V­¬ng cèc",
			[59]="Sïng thµnh kho¸ng tr­êng",
			[65]="Vïng ®Êt Èm ­ít"
		}

	itemname={
			[1]="L­ h­¬ng",
			[2]="KiÕm gç",
			[3]="Bïa trõ tµ"
		}

	guaname={
			[1]="QuÎ Kh«n",
			[2]="QuÎ Kh¶m",
			[3]="QuÎ Ly",
			[4]="QuÎ CÊn",
			[5]="QuÎ §oµi",
			[6]="QuÎ Tèn",
			[7]="QuÎ ChÊn"
		}

function main()
	local m,x,y=GetWorldPos()
	if(m==20)then
		xiqi()
	elseif(m==21)then
		chaoge()
	end
end;

function chaoge()
	tasks = 
	{
	 {"T×m b¶o","treasure";show=0},
	 {"Hñy bá","quxiao";show=0},
	 {"Phôc Ma","tiangua";show=0},
	 {"B¸t qu¸i","jieshao";show=1},
	 {"MËt tŞch da dª","yangpi";show=1}
	}
	local condition=0
	if(GetTask(561)==0)then
		condition=1
	end
	if(GetTask(826)==0)and(condition==1)then		--¦³«õ7ºØ¨öªº¸Õ½m¥ô°È®É­Ô,¤£¯à¶i¦æ´¶³qªº«õ¨ö¦æ¬°(¥]¬A±µ¨ü©M¨ú®ø)
		if (GetTask(382)==0)then	
			if (HaveNormalItem(3,15,0,0)>=1) or (HaveNormalItem(3,16,0,0)>=1)  or (HaveNormalItem(3,17,0,0)>=1) or (HaveNormalItem(3,18,0,0)>=1) or (HaveNormalItem(3,19,0,0)>=1) or (HaveNormalItem(3,20,0,0)>=1) or (HaveNormalItem(3,21,0,0)>=1) then
				tasks[1].show=1
			end;
			if(HaveNormalItem(3,14,0,0)>=1)and(GetLevel()>=45)then	--¦³¤Ñ¨ö
				tasks[3].show=1
			end
		else
			if(GetTask(849)==1)and(GetLevel()>=45)then		--±µ¹L¤K¨ö¥ñÅ]ªº¥ô°È
				tasks[3].show=1
			end
			if(GetTask(849)==0)then
				tasks[2].show=1
			end
		end
		SayTask("Thiªn §Şa V« Cùc, V« Cùc Sinh H÷u Cùc, H÷u Cùc Sinh Th¸i Cùc, Th¸i Cùc Sinh L­ìng Nghi, L­ìng Nghi Sinh Tø T­îng, Tø T­îng Sinh B¸t Qu¸i. NÕu ng­¬i cã thÓ mang vßng B¸t Qu¸i vÒ, ta sÏ tİnh gióp ng­¬i.",tasks)
	elseif(GetTask(826)~=0)then
		Talk(1,"no","Ng­¬i ®ang nhËn nhiÖm vô <color=green>ThÊt Qu¶i<color>, kh«ng thÓ lµm nhiÖm vô kh¸c!")
	elseif(condition==0)then
		Talk(1,"no","Ng­¬i ®ang nhËn nhiÖm vô <color=green>Thi t×m b¸t qu¸i<color>, kh«ng thÓ lµm nhiÖm vô kh¸c!")
	end;
end

function xiqi()
	tasks = 
	{
	 {"ThÊt Qu¶i","search_jisi";show=0},
	 {"Hñy bá thİ luyÖn","cancel_jisi";show=0},
	 {"Hoµn thµnh thİ luyÖn","complete_jisi";show=0},
	 {"Thi t×m kho b¸u","rate_treature";show=0},		--¥[¤J´MÄ_¤ñÁÉ¿ï¶µ2006-8-7;         
	 {"ThÊt Qu¶i","intr_jisi";show=1},
	 {"Hîp tranh","hecheng";show=1}
	}
--	local h,m,s=GetHMS()
--	if(h==20)then
		if(GetTask(382)==0)and(GetTask(826)<7)and(GetTask(561)==0)and (GetLevel()>=45)then
			tasks[1].show=1
		end
		if(GetTask(826)~=0)and (GetLevel()>=45) then			--ÀH®É¥i¥H¨ú®ø¥ô°È¡A§_«h¥i¯à¥X²{ª±®a¦b¥i¥Î®É¶¡¤º¤£¯à°Ñ¥[¤ñÁÉªº°İÃD
			tasks[2].show=1		
					--·í¦³¸Õ½m¥ô°Èªº®É­Ô¤£¯à¶i¦æ¤K¨ö¤ñÁÉ
		end
		if (GetTask(826)==0)and(GetLevel()>=45)then
		      tasks[4].show=1
                end
		if(GetTask(826)==7)and(GetTask(833)==2)and(GetTask(561)==0)and (GetLevel()>=45)then
			tasks[3].show=1	
		end
	
--	end
	SayTask("B¸t qu¸i chia lµm Cµn, Kh«n, Tèn, ChÊn, Kh¶m, Ly, CÊn, §oµi. §èi øng víi vßng b¸t qu¸i lµ Thiªn, §Şa, Phong, L«i, Thñy, Háa, S¬n, Tr¹ch. Ta ®· quy Èn nhiÒu n¨m ng­¬i cã thÓ ®Õn TriÒu Ca t×m huynh ®Ö cña ta gi¶i quÎ.",tasks)
end
function hecheng()
        
	MsgBox("Muèn hîp thµnh m¶nh tranh b¸t qu¸i cÇn <color=green>1 Chuéc hån ®¨ng<color>, <color=green>tiÒn giÊy<color> vµ <color=green>Hoµng töu<color> lµm nguyªn liÖu, ng­¬i muèn hîp thµnh kh«ng?","shi1","no")

end
function shi1()
        local j=FindAValidIBItem(8,196,2,0)
        if (j~=0)and(HaveNormalItem(3,97,0,0)>=1)and (HaveNormalItem(3,98,0,0)>=1)then
        	AddNormalItem(3,86,0,1,0,0)
		DelNormalItem(3,97,0,0)
		DelNormalItem(3,98,0,0)
		CostIBItem(j)
                MsgBox("M¶nh tranh b¸t qu¸i ®· hîp thµnh xong, xin nhËn lÊy!","no")
	else
         MsgBox("Muèn hîp thµnh m¶nh tranh b¸t qu¸i cÇn <color=green>1 Chuéc hån ®¨ng<color>, <color=green>tiÒn giÊy<color> vµ <color=green>Hoµng töu<color>. Ng­¬i ch­a ®ñ!","no")
	 end
end
	 

function rate_treature()			--´MÄ_¤ñÁÉµæ³æ
	tasks = 
		{
		 {"B¸o danh","rate";show=0},
		 {"Kh«ng tham gia","cancel_rate";show=0},
		 {"Thi ®Êu xong","rateend";show=0},
		 {"Thêi gian","lasttime";show=0},
		 {"XÕp h¹ng","seerank";show=1},	
		 {"PhÇn th­ëng","reward";show=0},		--»P¤ñÁÉ¤U¤@¨B¤£¯à¦P®É¥X²{
		 {"B­íc kÕ tiÕp","next";show=0}
		 }
		local h,m,s=GetHMS()
		if (GetWeekDay()==7) and (h>=13) and (h<=17) then
			if(GetTask(561)==0)and(GetTask(382)==0)and(GetTask(562)~=floor(SystemTime()/(86400*7)))and (GetLevel()>=45) then
				tasks[1].show=1
			end;
			if (GetTask(561)>0)and(GetTask(561)<9)and(GetTask(562)==floor(SystemTime()/(86400*7)))and (GetLevel()>=45)then	--¦]¬°Ïä¬ñ¨B¥u¬O¨t²Î­p¼Æªº¦w±Æ¡Aª±®a¨Æ¹ê¤W¥uª¾¹D?8­Ó¦a¤è«õ­y¡A¤]´N¬Oª±®a¨Æ¹ê¤W²z¸Ñ¬O8¨B¡A?”¸561==9ªí©ú?°È¤w¸g§¹¦¨¡A¦b¦³®Ä®É¶¡¬q¤º¤£»İ­n©ñ±ó?°È
				tasks[2].show=1
				tasks[4].show=1
				if(GetTask(382)==0)then
					tasks[7].show=1
				end
			end;
			if (GetTask(561)==9)and(GetTask(382)==0)then              --¥ô°È¨BÆJÅÜ¶q(9=¤ñÁÉµ²§ô)
				tasks[3].show=1
			end;
			if (GetTask(561)==0)and(GetTask(562)==floor(SystemTime()/(86400*7)))and(GetTask(565)==0) then 
				tasks[4].show=1
			end;
		elseif(GetWeekDay()==7) and (h>=18) and (h<=23) and(GetTask(850)~=floor(SystemTime()/86400))and(GetLevel()>=45)then 
			tasks[6].show=1
			if(GetTask(561)>0)and(GetTask(561)<=9)then
				tasks[2].show=1
			end
		end
		if(GetTask(561)>0)and(GetTask(561)<=9)then
			tasks[2].show=1
		end		
	SayTask("Mçi chiÒu chñ nhËt tõ <color=green>1 giê<color> ®Õn <color=green>6 giê<color>,  ta ®Òu tæ chøc cuéc thi t×m kho b¸u. ChØ cÇn ®¹t cÊp 45, cã <color=green>ThÊt Qu¶i<color> vµ <color=green>tranh b¸t qu¸i<color> cã thÓ ®Õn b¸o danh tham gia. 10 ng­êi hoµn thµnh nhiÖm vô nhanh nhÊt sÏ nhËn ®­îc phÇn th­ëng <color=green>quÎ cµn<color>!",tasks)
end;
----------¹î¬İ²Ö­p®É¶¡----------------------------------------------------------------------------
function lasttime()
	local step=GetTask(561)
	local time=SystemTime()-GetTask(563)
	local s1=""
	if(step==0)then
		time=GetTask(563)
		s1="Ng­¬i ®· hoµn thµnh cuéc thi t×m kiÕm b¸t qu¸i "
	else
		s1="Ng­¬i ®· hoµn thµnh b­íc thø <color=green>"..step.."<color>"
	end
	Talk(1,"no",s1.."B¹n ®· sö dông <color=green>"..time.."<color> gi©y.")

end;
----------¹î¬İ²Ö­p®É¶¡---------------------------------------------------------------------------end

-----------«õ¨ö¤ñÁÉ----------------------------------------------------------------------------------
function rate()
	if (GetLevel()>=45) then
		if (GetTask(561)==0) then
			if (GetTask(562)~=floor(SystemTime()/(86400*7))) then
				if (GetTask(382)==0)then
					if (HaveNormalItem(3,87,0,0)>=1) then
						next()
					else
						MsgBox("Ph¶i cã <color=yellow>tranh b¸t qu¸i<color> míi cã thÓ tham gia thi ®Êu.","no")
					end;
				else
					MsgBox("Ng­¬i ®ang trong tr¹ng th¸i t×m kiÕm kho b¸u, hoµn thµnh xong míi cã thÓ tham gia thi ®Êu.","no")
				end;
			else
				MsgBox("Mçi ng­êi mét tuÇn chØ cã thÓ tham gia thi t×m kho b¸u mét lÇn, tuÇn sau h·y tíi ®i!","no")
			end;
		elseif(GetTask(561)==10) then
			MsgBox("TuÇn nµy ng­¬i ®· tham gia thi ®Êu råi!","no")
		else
			MsgBox("Ng­¬i ®ang trong tr¹ng th¸i thi ®Êu.","no")
		end;
	else
		MsgBox("§¹t cÊp 45 míi cã thÓ tham gia!","no")
	end;
end;

function next()
	local step = GetTask(561)
	local flag = GetTask(566)
	local n = GetTask(567)
	local last = GetTask(568)
	local gn={"QuÎ Kh«n","QuÎ Kh¶m","QuÎ Ly","QuÎ CÊn","QuÎ §oµi","QuÎ Tèn","QuÎ ChÊn"}
	if(step<=6) then					--¤ñÁÉ¤¤1¨ì7¨B
		if(flag==0) then
			flag = random(1,7)			--²Ä¤@¨B
			if(HaveNormalItem(3,87,0,0)>=1)then
				if(HaveNormalItem(3,14+flag,0,0)>=1)then
					SetTask(562,SystemTime()/(86400*7))
					SetTask(563,SystemTime())
					SetTask(565,0)
					SetTask(566,flag)			--°O¿ıªì¦¸´MÄ_ªº¦aÂI
					SetTask(561,1)				--¨BÆJ¸m¦ì
					SetTask(567,random(1,6))		--°O¿ı¦aÂI¼WªøÅv­È
					SetTask(568,flag)			--refrush¤W¤@¦¸ªº´MÄ_¦aÂI
					DelNormalItem(3,87,0,0)
					if(flag==1)then
						yv1()
					elseif(flag==2)then
						yv2()
					elseif(flag==3)then
						yv3()				
					elseif(flag==4)then
						yv4()
					elseif(flag==5)then
						yv5()
					elseif(flag==6)then
						yv6()
					elseif(flag==7)then
						yv7()
					end
					Talk(1,"no","Cuéc thi t×m b¸t qu¸i ®ang tiÕn hµnh b­íc <color=yellow>1<color>, ng­¬i ph¶i dïng <color=green>"..gn[flag].."<color> ®Ó t×m kho b¸u.")
				else
					Talk(1,"no","B¸o danh tham gia thi ®Êu ph¶i cã <color=green>quÎ kh«n, quÎ kh¶m, quÎ ly, quÎ cÊn, quÎ ®oµi, quÎ tèn, quÎ chÊn<color> mçi thø mét c¸i. Ng­¬i cßn thiÕu <color=yellow>"..gn[flag].."<color>, t×m ®ñ råi ®Õn nhĞ!")
				end
			else
				Talk(1,"no","Ph¶i cã <color=yellow>tranh b¸t qu¸i<color> míi cã thÓ tham gia thi ®Êu.")
			end
		else
			local point=mod(last+n,7)		--¨D³o¤@¦¸ªº´MÄ_¦aÂI
			if(point==0)then
				point = 7
			end
			if(HaveNormalItem(3,14+point,0,0)>=1)then
				if(point==1)then
					yv1()
				elseif(point==2)then
					yv2()
				elseif(point==3)then
					yv3()				
				elseif(point==4)then
					yv4()
				elseif(point==5)then
					yv5()
				elseif(point==6)then
					yv6()
				elseif(point==7)then
					yv7()
				end
				SetTask(561,step+1)			--¨BÆJ+1
				SetTask(568,last+n)			--refrush¤W¤@¦¸ªº´MÄ_¦aÂI
				Talk(1,"no","Cuéc thi t×m b¸t qu¸i ®ang tiÕn hµnh b­íc <color=yellow>"..GetTask(561).."<color>, ng­¬i ph¶i dïng <color=green>"..gn[point].."<color> ®Ó t×m kho b¸u.")
			else
--				Msg2Player(point)
				Talk(1,"no","Thi t×m b¸t qu¸i ph¶i cã <color=green>quÎ kh«n, quÎ kh¶m, quÎ ly, quÎ cÊn, quÎ ®oµi, quÎ tèn, quÎ chÊn<color> mçi thø mét c¸i, h×nh nh­ ng­¬i kh«ng mang theo <color=yellow>"..gn[point].."<color>, t×m ®ñ råi ®Õn nhĞ!")
			end
		end;
	elseif(step==7) then					--²Ä¤K¨Bµ´ÀsÀ­
		SetTask(561,step+1)				--¨BÆJ+1
		MsgBox("ë TuyÖt Long LÜnh tõng cã mét d· long bŞ <color=red>S¸t Ma<color> giÕt chÕt, ®©y lµ <color=green>v¶y Rång<color>, cã thÓ gióp ng­¬i t×m <color=green>Long Hån th¹ch<color>, mang viªn ®¸ ®ã vÒ ®©y ®Ó hoµn tÊt b­íc cuèi cïng cña cuéc thi!","no")
		local maps ={ 
				 
						{mapid=19,x=1401,y=3063,r=20},
						{mapid=19,x=1402,y=3164,r=20},
						{mapid=19,x=1362,y=3388,r=20},
						{mapid=19,x=1430,y=3455,r=20},
						{mapid=19,x=1488,y=3339,r=20},
						{mapid=19,x=1517,y=3180,r=20},
						{mapid=19,x=1522,y=3109,r=20},
						{mapid=19,x=1586,y=2965,r=20},
						{mapid=19,x=1634,y=3384,r=20},
						{mapid=19,x=1818,y=3344,r=20},
						{mapid=19,x=1832,y=3035,r=20},
						{mapid=19,x=1747,y=2961,r=20},
						{mapid=19,x=1731,y=3101,r=20}
					}
		local num=random(1,13)
		local px=maps[num].x
		local py=maps[num].y
		local r=random(0,3)
		if(r==0)then
			px=px+random(maps[num].r)
			py=py+random(maps[num].r)
		elseif(r==1)then
			px=px-random(maps[num].r)
			py=py-random(maps[num].r)
		elseif(r==2)then
			px=px+random(maps[num].r)
			py=py-random(maps[num].r)
		else
			px=px-random(maps[num].r)
			py=py+random(maps[num].r)
		end;
		SetTask(381,19)
		SetTask(382,px)
		SetTask(383,py)
		SetTask(378,0)
		SetTask(380,SystemTime())
		SetTask(395,0)
		AddNormalItem(6,1,179,0,0,0)
	end;
end;
-----------«õ¨ö¤ñÁÉend------------------------------------------------------------------------------
----------¹î¬İ±Æ¦W--------------------------------------------------------------------------------
function seerank()
	local name=LoadIniString("guabisai",11)
	if(name=="Kh«ng t×m thÊy ")or(name==nil)then
		Talk(1,"no","TuÇn nµy ch­a ai xÕp h¹ng, cè g¾ng lÇn sau nhĞ!")
	else
		local s="<color=green>H¹ng 1 ®Õn h¹ng 5<color> gåm\n"
		for i=1,5 do
			local n=LoadIniString("guabisai",10+i)
			local t=LoadIniInteger("guabisai",i)
			if(n==nil)or(t==nil)then
				break
			else
				s=s.."h¹ng <color=yellow>"..i.."<color>: <color=green>"..n.."<color>Thêi gian lµ<color=red>"..t.."<color> gi©y\n"
			end
		end;
		MsgBox(s,"xiayiye")
	end
end;

function xiayiye()
	local name=LoadIniString("guabisai",16)
	if(name=="Kh«ng t×m thÊy ")or(name==nil)then
		CloseDialog()
	else
		local s1="<color=green>H¹ng 6 ®Õn h¹ng 10<color> gåm\n"
		for i=6,10 do
			local n=LoadIniString("guabisai",10+i)
			local t=LoadIniInteger("guabisai",i)
			if(n==nil)or(t==nil)then
				break
			else
				s1=s1.."h¹ng <color=yellow>"..i.."<color>: <color=green>"..n.."<color>Thêi gian lµ<color=red>"..t.."<color> gi©y\n"
			end
		end;
		MsgBox(s1,"no")
	end
end;
----------¹î¬İ±Æ¦W--------------------------------------------------------------------------------end
-----------------¦pªG¦³¦W¦¸¡A«hµ¹¤Ñ¨ö-----------------------------------------------------------------
function reward()
	local name=GetName()
	for i=1,10 do
		local flag=LoadIniString("guabisai",i+10)
		if(flag==name) then
			local k=i
			if(k==1)then
				AddNormalItem(3,14,0,0,0,1)
				AddNormalItem(3,14,0,0,0,1)
				AddNormalItem(0,4,32,1,0,1)
				MsgBox("Chóc mõng ng­¬i xÕp h¹ng "..k..", phÇn th­ëng 2 <color=green>quÎ cµn<color> vµ <color=green>kinh Vu Lan Bån<color>!","no")
                                AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> xÕp h¹ng "..k.."Chóc mõng b¹n nhËn ®­îc 2 <color=green>quÎ cµn<color> vµ <color=green>kinh Vu Lan Bån<color>##",3)
				SetTask(850,floor(SystemTime()/86400))
				return
			elseif(k==2)then
				AddNormalItem(3,14,0,0,0,1)
				AddNormalItem(3,14,0,0,0,1)
				MsgBox("Chóc mõng ng­¬i xÕp h¹ng "..k..", phÇn th­ëng 2 <color=green>quÎ cµn<color>! Hy väng cuéc thi lÇn sau ng­¬i ®¹t ®­îc thµnh tİch cao h¬n!","no")
                                AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> xÕp h¹ng "..k.."Chóc mõng b¹n nhËn ®­îc 2 <color=green>quÎ cµn<color>##",3)
				SetTask(850,floor(SystemTime()/86400))
				return
			else
				AddNormalItem(3,14,0,0,0,1)
				MsgBox("Chóc mõng ng­¬i xÕp h¹ng "..k.." , phÇn th­ëng 1 <color=green>quÎ cµn<color>! Hy väng cuéc thi lÇn sau ng­¬i ®¹t ®­îc thµnh tİch cao h¬n!","no")
                                AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> xÕp h¹ng "..k.."Chóc mõng b¹n nhËn ®­îc 1 <color=green>quÎ cµn<color>##",3)
				SetTask(850,floor(SystemTime()/86400))	--¼ÇÂ¼ÒÑ¾­Áì?¹ı½±¢»
				return
			end
		end;
	end;
	MsgBox("Xin lçi! Ng­¬i kh«ng nhËn ®­îc thø h¹ng nµo trong cuéc thi nµy, cè g¾ng h¬n nhĞ!","no")
end;
-----------------¦pªG¦³¦W¦¸¡A«hµ¹¤Ñ¨ö-----------------------------------------------------------------end

-------©ñ±ó¤ñÁÉ------------------------------------------------------------------------------------
function cancel_rate()
	MsgBox("Cuéc thi mçi tuÇn diÔn ra mét lÇn, ng­¬i x¸c ®Şnh hñy bá cuéc thi ph¶i kh«ng?","yes_cancel_rate","no")
end;

function yes_cancel_rate()
	SetTask(561,0)              --¥ô°È¨BÆJÅÜ¶q(0=¨S¦³±µ¥ô°È¡A1=±µ¨ü¥ô°È¨BÆJ1¡K¡K¡K¡K¡K¡K¡K¡K7=±µ¨ü¥ô°È¨BÆJ7¡A8=¥ñªk¥ô°È¨BÆJ8¡A9=§¹¦¨¥ô°È¡A10=©ñ±ó¥ô°È)
--	SetTask(562,0)              --¥Î©ó­­¨î¨C¶g°µ¥ô°È¦¸¼ÆªºÅÜ¶q
	SetTask(563,0)              --§¹¦¨¨CÀô¥ô°È©Ò»İªº®É¶¡ÅÜ¶q
	SetTask(566,0)
	SetTask(567,0)
	SetTask(568,0)
	SetTask(381,0)
	SetTask(382,0)
	SetTask(383,0)
	local tianshu=SystemTime()/86400
	SetTask(565,floor(tianshu))	--°O¿ı©ñ±ó¥ô°Èªº®É¶¡(¥H¤Ñ°O)
	MsgBox("Ng­¬i kh«ng tham gia thi t×m kho b¸u cña tuÇn nµy!","no")
end;
-------©ñ±ó¤ñÁÉ--------------------------------------------------------------------------------------end
----------¤ñÁÉµ²§ô¡A¨ú±o¸gÅç¼úÀy¦P®É±Æ¦W---------------------------------------------------------------
function rateend()
	if (GetTask(561)==9) and(HaveEventItem(165)>=1)then
		local exp=GetLevel()*8000
		local t=SystemTime()-GetTask(563)
		local name=GetName()
		SetTask(563,t)
		SetTask(561,0)
		SetTask(566,0)
		SetTask(567,0)
		SetTask(568,0)
		DelEventItem(165)
		AddOwnExp(exp)
		MsgBox("§©y lµ phÇn th­ëng tÆng ng­¬i, ng­¬i ®· sö dông hÕt <color=green>"..t.."<color> gi©y. NÕu nh­ ng­¬i ®¹t thø h¹ng cao, <color=green>tõ 18 giê ®Õn 24 giê<color> ®Õn chç ta nhËn phÇn th­ëng ®Æc biÖt.","no")
		for i=1,10 do						--°O¿ı¦W¦¸©M¹ïÀ³ªº®É¶¡
			local flagT=LoadIniInteger("guabisai",i)
			local flagN=LoadIniString("guabisai",i+10)
			if(flagT==nil)then
				flagT=0
			end
			if(flagN==nil)then
				flagN="Kh«ng t×m thÊy "
			end
			if(flagT==0)or(t<flagT) then
					SaveIniInteger("guabisai",i,t)
					SaveIniString("guabisai",i+10,name)
					if(i<10)then
						for k=9,i+1,-1 do
							local flag1=LoadIniInteger("guabisai",k)
							local flag2=LoadIniString("guabisai",k+10)
							SaveIniInteger("guabisai",k+1,flag1)
							SaveIniString("guabisai",k+11,flag2)
						end;
						SaveIniInteger("guabisai",i+1,flagT)
						SaveIniString("guabisai",i+11,flagN)
						break;
					end
			end
		end;
	else
		Talk(1,"no","TuyÖt Long LÜnh lµ n¬i cã ®Şa thÕ hiÓm trë, ta nghÜ Long Hån th¹ch ®­îc cÊt giÊu rÊt kü, ng­¬i thö t×m l¹i xem!")
	end;
end;
----------¤ñÁÉµ²§ô¡A¨ú±o¸gÅç¼úÀy¦P®É±Æ¦W-----------------------------------------------------------end

------------------------------¤Ñ¨ö¥ñÅ]begin-------------------------------------------------------------------------
function tiangua()
	if(HaveNormalItem(3,14,0,0)>=1)and(GetTask(849)==0)and (GetLevel()>=45)then
		MsgBox("Ta nghÜ ChÊn ma th¹ch cña Bİch Du Cung ®· bŞ <color=red>S¸t Ma<color> ph¸ hñy! NÕu ng­¬i muèn cøu b¸ t¸nh cã thÓ mang <color=green>quÎ cµn<color> ®Õn, ta sÏ gióp ng­¬i xem h¾n ®ang Èn nóp ë ®©u!","tiangua_begin","no")
	elseif(GetTask(849)==1)then
		MsgBox("S¸t Ma cã phĞp thuËt cao siªu cã thÓ Èn m×nh bÊt cø n¬i nµo, ng­¬i muèn hñy nhiÖm vô hµng phôc yªu ma ­?","tiangua_cancel","no")
	end
end

function tiangua_begin()	--±µ¥ô°È
		local maps =	{ 
							{mapid=43,x=1331,y=3259,r=5},
							{mapid=43,x=1432,y=3341,r=5},
							{mapid=43,x=1429,y=3255,r=5},
							{mapid=43,x=1392,y=3124,r=5},
							{mapid=43,x=1636,y=3311,r=5},
							{mapid=43,x=1698,y=3384,r=5},
							{mapid=43,x=1576,y=3183,r=5},
							{mapid=43,x=1480,y=3304,r=5},
					}
		if (HaveNormalItem(3,14,0,0)>=1) then
			DelNormalItem(3,14,0,0)
			SetTask(849,1)
			AddNormalItem(6,1,149,0,1,0) --µ¹¥ñÅ]¨÷¶b
			Msg2Player("B¹n nhËn ®­îc mËt tŞch phôc ma cña ThÇy bãi.")
			local num=random(1,8)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","Mang theo mËt tŞch cã thÓ t×m ra tung tİch <color=green>S¸t Ma<color>, <color=green>S¸t Ma<color> cã ph¸p lùc cao c­êng. Ng­¬i cã thÓ mêi nhiÒu ng­êi tiªu diÖt h¾n.")
		else
			Talk(1,"no","ChØ cã <color=yellow>quÎ cµn<color> míi t×m ra n¬i Èn nÊu cña <color=green>S¸t Ma<color>, nÕu ng­¬i kh«ng mang theo ta kh«ng thÓ gióp ®­îc g×.")
		end;
end

function tiangua_cancel()
			SetTask(381,0)
			SetTask(382,0)
			SetTask(383,0)
			SetTask(849,0)
			SetTask(378,0)		--distance
			SetTask(380,0)		--½Ó?ÎñÊ±¼ä
			SetTask(395,0)		--Ñ°±¦ËùÓÃÊ±¼ä
			Talk(1,"no","Ng­¬i ®· hñy nhiÖm vô <color=green> Phôc Ma<color>!")
end
------------------------------¤Ñ¨ö¥ñÅ]end-------------------------------------------------------------------------

function complete_jisi()		--§¹¦¨¸Õ½m¡A¥²¼úÀyÅ«»î¿O¤@­Ó
	AddNormalItem(8,196,2,0,1,0)
	for j=1,8 do
		SetTask(825+j,0)
	end
	local k=GetLevel()*3000
	AddOwnExp(k)
	TopMessage("nhËn ®­îc <color=green>"..k.."<color> ®iÓm kinh nghiÖm!")
	Talk(1,"no","Ng­¬i ®· hoµn thµnh nhiÖm vô ThÊt Qu¶i, <color=yellow>Chuéc hån ®¨ng<color> nµy tÆng cho ng­¬i!")
end

function cancel_jisi()			--¨ú®ø¬Y¤@¨BÆJ¡A«e­±¤w¸g§¹¦¨ªº¨BÆJ«O¯d
	local i=GetTask(826)
	MsgBox("B­íc tiÕp theo: ®i t×m <color=green>"..guaname[i].."<color> nÕu hñy bá ng­¬i ph¶i lµm l¹i b­íc nµy!","confirm_cancel_jisi","no")
end

function confirm_cancel_jisi()
	local i=GetTask(826)
	SetTask(826,i-1)
	SetTask(826+i,0)
	SetTask(381,0)
	SetTask(382,0)
	SetTask(383,0)
	Talk(1,"no","Ng­¬i ®· hñy bá t×m <color=green>"..guaname[i].."<color>!")
end

function cancel_jisi_bak()		--¨ú®ø¾ã­Ó¸Õ½m¶iµ{
	local i=GetTask(826)
	MsgBox("B­íc tiÕp theo: ®i t×m <color=green>"..guaname[i].."<color>,  nÕu hñy bá ng­¬i ph¶i lµm l¹i tõ ®Çu. TiÕp tôc nhiÖm vô ThÊt Qu¶i chø?","confirm_cancel_jisi_bak","no")
end

function intr_jisi()
	Talk(2,"no","NÕu ng­¬i hoµn thµnh nhiÖm vô ThÊt Qu¶i sÏ nhËn ®­îc Chuéc hån ®¨ng.","ChØ cÇn t×m ®­îc <color=green>quÎ kh«n, quÎ kh¶m, quÎ ly, quÎ cÊn, quÎ ®oµi, quÎ tèn, quÎ chÊn<color>, míi ®­îc xem lµ hoµn thµnh nhiÖm vô!")
end

function confirm_cancel_jisi_bak()
	SetTask(826,0)
	Talk(1,"no","Ng­¬i ®· hñy nhiÖm vô ThÊt Qu¶i!")
end

function search_jisi()			--«ö¶¶§Ç¤À°t»İ­n¤°»ò¨ö¡A§PÂ_¸Õ½m¨BÆJ

	if(GetTask(826)==0)then
	         local today=floor(SystemTime()/86400)
		 local taskday=GetTask(915)
		 if(today ~= taskday) then
			if(HaveNormalItem(3,15,0,0)>=1)then	--¦³¦a¨ö
				MsgBox("Tham gia chø? <color=yellow>B­íc 1<color>: ®i t×m <color=green>quÎ kh«n<color>!","step_1","no")
			else
				Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ kh«n<color> vÒ ®©y!")
			end
		else
			Talk(1,"no","Mçi ngµy chØ cã 1 c¬ héi thö nghiÖm nhiÖm vô, hoµn thµnh sÏ nhËn ®­îc Chuéc hån ®¨ng, nÕu muèn thö l¹i nhiÖm vô ThÊt Qu¶i th× ngµy mai h·y ®Õn!")
		end
	elseif(GetTask(826)==1)and(GetTask(827)==2)then			--¤W¤@¨B¤w¸g§¹¦¨¡]¦a¨ö¥È¾÷¥H¸Ñ¡^
		if(HaveNormalItem(3,16,0,0)>=1)then	--¦³¤ô¨ö
			MsgBox("<color=yellow>b­íc 2<color>: ®i t×m <color=green>quÎ kh¶m<color>!","step_2","no")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ kh¶m<color> vÒ ®©y!")
		end
	elseif(GetTask(826)==2)and(GetTask(828)==2)then			--¤W¤@¨B¤w¸g§¹¦¨¡]¤ô¨ö¥È¾÷¥H¸Ñ¡^
		if(HaveNormalItem(3,17,0,0)>=1)then	--¦³¤õ¨ö
			MsgBox("<color=yellow>b­íc 3<color>: ®i t×m <color=green>quÎ ly<color>!","step_3","no")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ ly<color> vÒ ®©y!")
		end
	elseif(GetTask(826)==3)and(GetTask(829)==2)then			--¤W¤@¨B¤w¸g§¹¦¨¡]¤õ¨ö¥È¾÷¥H¸Ñ¡^
		if(HaveNormalItem(3,18,0,0)>=1)then	--¦³¤s¨ö
			MsgBox("<color=yellow>b­íc 4<color>: ®i t×m <color=green>quÎ cÊn<color>!","step_4","no")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ cÊn<color> vÒ ®©y!")
		end
	elseif(GetTask(826)==4)and(GetTask(830)==2)then			--¤W¤@¨B¤w¸g§¹¦¨¡]¤s¨ö¥È¾÷¥H¸Ñ¡^
		if(HaveNormalItem(3,19,0,0)>=1)then	--¦³¿A¨ö
			MsgBox("<color=yellow>b­íc 5<color>: ®i t×m <color=green>quÎ ®oµi<color>!","step_5","no")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ ®oµi<color> vÒ ®©y!")
		end
	elseif(GetTask(826)==5)and(GetTask(831)==2)then			--¤W¤@¨B¤w¸g§¹¦¨¡]¿A¨ö¥È¾÷¥H¸Ñ¡^
		if(HaveNormalItem(3,20,0,0)>=1)then	--¦³­·¨ö
			MsgBox("<color=yellow>b­íc 6<color>: ®i t×m <color=green>quÎ tèn<color>!","step_6","no")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ tèn<color> vÒ ®©y!")
		end
	elseif(GetTask(826)==6)and(GetTask(832)==2)then			--¤W¤@¨B¤w¸g§¹¦¨¡]­·¨ö¥È¾÷¥H¸Ñ¡^
		if(HaveNormalItem(3,21,0,0)>=1)then	--¦³¹p¨ö
			MsgBox("<color=yellow>b­íc cuèi cïng<color>: ®i t×m <color=green>quÎ chÊn<color>!","step_7","no")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ chÊn<color> vÒ ®©y!")
		end
	end
end

function step_1()
		local maps =	{ 
							{mapid=16,x=1399,y=3450,r=20},
							{mapid=16,x=1395,y=3338,r=20},
							{mapid=16,x=1494,y=3122,r=20},
							{mapid=16,x=1478,y=3202,r=20},
							{mapid=16,x=1502,y=3306,r=20},
							{mapid=16,x=1534,y=3418,r=20},
							{mapid=16,x=1674,y=3346,r=20},
							{mapid=16,x=1648,y=3438,r=20},

				}
	 local today=floor(SystemTime()/86400)
	 local taskday=GetTask(915)
	 if(today ~= taskday) then
		if (HaveNormalItem(3,15,0,0)>=1) then
			DelNormalItem(3,15,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(827,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")		
			local num=random(1,8)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			SetTask(915,today)
			Talk(1,"no","T×m ®­îc <color=green>quÎ kh«n<color> lµ b­íc 1 ®Çu tiªn, hy väng ng­¬i cã thÓ hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ kh«n<color> vÒ ®©y!")
		end;
	else
	       Talk(1,"no","Mçi ngµy chØ cã 1 c¬ héi thö nghiÖm nhiÖm vô, hoµn thµnh sÏ nhËn ®­îc Chuéc hån ®¨ng, nÕu muèn thö l¹i nhiÖm vô ThÊt Qu¶i th× ngµy mai h·y ®Õn!")
	end
end

function step_2()
		local maps =	{ 
							{mapid=65,x=1554,y=2760,r=20},
							{mapid=65,x=1562,y=3233,r=20},
							{mapid=65,x=1642,y=3209,r=20},
							{mapid=65,x=1661,y=3095,r=20},
							{mapid=65,x=1818,y=2911,r=20},
							{mapid=65,x=1707,y=2799,r=20},
							{mapid=65,x=1605,y=2766,r=20},
							{mapid=65,x=1469,y=2830,r=20},
							{mapid=65,x=1333,y=2850,r=20},
						}
		if (HaveNormalItem(3,16,0,0)>=1) then
			DelNormalItem(3,16,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(828,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
			local num=random(1,9)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","T×m ®­îc <color=green>quÎ kh¶m<color> lµ b­íc 2. Hy väng ng­¬i sím hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ kh¶m<color> vÒ ®©y!")
		end;
end

function step_3()
		local maps =	{ 
							{mapid=8,x=1740,y=2976,r=20},
							{mapid=8,x=1816,y=2944,r=20},
							{mapid=8,x=1860,y=2805,r=20},
							{mapid=8,x=1948,y=3044,r=20},
							{mapid=8,x=2021,y=3021,r=20},
							{mapid=8,x=1905,y=3071,r=20},
							{mapid=8,x=1780,y=3312,r=20},
							{mapid=8,x=1674,y=3348,r=20},
							{mapid=8,x=1717,y=3491,r=20},
							{mapid=8,x=1965,y=3223,r=20},
						}
		if (HaveNormalItem(3,17,0,0)>=1) then
			DelNormalItem(3,17,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(829,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","T×m ®­îc <color=green>quÎ ly<color> lµ b­íc 3 cña thİ luyÖn, . Hy väng ng­¬i sím hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ ly<color> vÒ ®©y!")
		end;
end

function step_4()
		local maps =	{ 
							{mapid=15,x=1413,y=3519,r=20},
							{mapid=15,x=1410,y=3289,r=20},
							{mapid=15,x=1410,y=3105,r=20},
							{mapid=15,x=1547,y=3050,r=20},
							{mapid=15,x=1575,y=3219,r=20},
							{mapid=15,x=1612,y=3373,r=20},
							{mapid=15,x=1597,y=3535,r=20},
							{mapid=15,x=1653,y=3472,r=20},
							{mapid=15,x=1714,y=3289,r=20},
							{mapid=15,x=1758,y=3090,r=20},
						}
		if (HaveNormalItem(3,18,0,0)>=1) then
			DelNormalItem(3,18,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(830,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","T×m ®­îc <color=green>quÎ cÊn<color> lµ b­íc 4. Hy väng ng­¬i sím hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ cÊn<color> vÒ ®©y!")
		end;
end

function step_5()
		local maps =	{ 
							{mapid=14,x=1713,y=3613,r=20},
							{mapid=14,x=1661,y=3437,r=20},
							{mapid=14,x=1633,y=3293,r=20},
							{mapid=14,x=1673,y=3173,r=20},
							{mapid=14,x=1564,y=3189,r=20},
							{mapid=14,x=1500,y=3253,r=20},
							{mapid=14,x=1518,y=3419,r=20},
							{mapid=14,x=1506,y=3555,r=20},
							{mapid=14,x=1390,y=3683,r=20},
							{mapid=14,x=1347,y=3494,r=20},
				}
		if (HaveNormalItem(3,19,0,0)>=1) then
			DelNormalItem(3,19,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(831,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","T×m ®­îc <color=green>quÎ ®oµi<color> lµ b­íc 5. Hy väng ng­¬i sím hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ ®oµi<color> vÒ ®©y!")
		end;
end

function step_6()
		local maps =	{ 
							{mapid=11,x=1819,y=3580,r=20},
							{mapid=11,x=1823,y=3365,r=20},
							{mapid=11,x=1815,y=3229,r=20},
							{mapid=11,x=1706,y=3126,r=20},
							{mapid=11,x=1702,y=3342,r=20},
							{mapid=11,x=1710,y=3567,r=20},
							{mapid=11,x=1642,y=3567,r=20},
							{mapid=11,x=1610,y=3503,r=20},
							{mapid=11,x=1642,y=3390,r=20},
							{mapid=11,x=1630,y=3286,r=20},
						}
		if (HaveNormalItem(3,20,0,0)>=1) then
			DelNormalItem(3,20,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(832,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","T×m ®­îc <color=green>quÎ tèn<color> lµ b­íc 6. Hy väng ng­¬i sím hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ tèn<color> vÒ ®©y!")
		end;
end


function step_7()
		local maps =	{ 
							{mapid=5,x=1674,y=2858,r=20},
							{mapid=5,x=1585,y=3075,r=20},
							{mapid=5,x=1584,y=3210,r=20},
							{mapid=5,x=1667,y=3177,r=20},
							{mapid=5,x=1746,y=3205,r=20},
							{mapid=5,x=1803,y=3190,r=20},
							{mapid=5,x=1815,y=3044,r=20},
				}
		if (HaveNormalItem(3,21,0,0)>=1) then
			DelNormalItem(3,21,0,0)
			SetTask(826,GetTask(826)+1)
			SetTask(833,1)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
			local num=random(1,7)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
			Talk(1,"no","T×m ®­îc <color=green>quÎ chÊn<color> lµ b­íc cuèi cïng. Hy väng ng­¬i sím hoµn thµnh!")
		else
			Talk(1,"no","Muèn hoµn thµnh kh¶o nghiÖm cña ta ph¶i hiÓu râ huyÒn c¬ cña c¸c lo¹i quÎ, ng­¬i mau gióp ta t×m <color=green>quÎ chÊn<color> vÒ ®©y!")
		end;
end
-----------------------------------------------7¨ö¸Õ½m¥ô°Èend---------------------------------------------------------
-----------------------------------------------´¶³q´MÄ_begin----------------------------------------------------------
function quxiao()
	MsgBox("B¹n thËt sù muèn bá nhiÖm vô t×m b¶o?","cancel","no")
end;

function cancel()
		SetTask(381,0)
		SetTask(382,0)
		SetTask(383,0)
		SetTask(378,0)
		SetTask(380,0)
		SetTask(395,0)
		CloseDialog()
end;

function jieshao()
	Talk(2,"no","B¸t qu¸i chia lµm Cµn, Kh«n, Tèn, ChÊn, Kh¶m, Ly, CÊn, §oµi. §èi øng víi vßng b¸t qu¸i lµ Thiªn, §Şa, Phong, L«i, Thñy, Háa, S¬n, Tr¹ch. ChØ cã ai khuÊt phôc ®­îc <color=red>Lam Qu¸i<color> míi cã ®­îc quÎ t­îng nµy. Khi nµo ng­¬i cã h·y ®Õn t×m ta.","Trong b¸t qu¸i hµm chøa nh÷ng søc m¹nh thÇn kú kh¸c nhau, trong ®ã quÎ Thiªn lµ khã gÆp nhÊt. NÕu Thİ chñ lµ ng­êi h÷u duyªn, khi nµo tËp hîp ®ñ b¸t qu¸i sÏ cã sù kú diÖu x¶y ra.")
end;

function yangpi()
	Talk(3,"no","MËt tŞch da dª ghi l¹i n¬i cÊt giÊu b¶o vËt. Khi b¶o vËt bŞ ph¸t hiÖn, mËt tŞch sÏ biÕn mÊt.","V× b¶o vËt vµ mËt tŞch da dª cã liªn quan ®Õn sè mÖnh cña con ng­êi nªn ng­¬i kh«ng thÓ cïng lóc më 2 b¶o vËt.","B¶o vËt chØ tÆng cho ng­êi cã lßng, mËt tŞch da dª nµy rÊt hiÕm thÊy!")
end;

function treasure()
		Say("NÕu ng­¬i ®­a l¸ th¨m cho ta, ta sÏ chØ ng­¬i c¸ch t×m b¶o vËt.",8,"QuÎ Kh«n/v1","QuÎ Kh¶m/v2","QuÎ Ly/v3","QuÎ CÊn/v4","QuÎ §oµi/v5","QuÎ Tèn/v6","QuÎ ChÊn/v7","Hñy bá/no")
end;

function v1()
		if (HaveNormalItem(3,15,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ Kh«n<color> cho ta?","yv1","no")		
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ Kh«n<color>!")
		end;
end;

function yv1()
		local maps =	{ 
							{mapid=16,x=1399,y=3450,r=20},
							{mapid=16,x=1395,y=3338,r=20},
							{mapid=16,x=1494,y=3122,r=20},
							{mapid=16,x=1478,y=3202,r=20},
							{mapid=16,x=1502,y=3306,r=20},
							{mapid=16,x=1534,y=3418,r=20},
							{mapid=16,x=1674,y=3346,r=20},
							{mapid=16,x=1648,y=3438,r=20},

						}
		if (HaveNormalItem(3,15,0,0)>=1) then
			DelNormalItem(3,15,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
                        AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
		else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,8)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function v2()
		if (HaveNormalItem(3,16,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ Kh¶m<color> cho ta?","yv2","no")
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ Kh¶m<color>!")
		end;
end;

function yv2()
		local maps =	{ 
							{mapid=65,x=1554,y=2760,r=20},
							{mapid=65,x=1562,y=3233,r=20},
							{mapid=65,x=1642,y=3209,r=20},
							{mapid=65,x=1661,y=3095,r=20},
							{mapid=65,x=1818,y=2911,r=20},
							{mapid=65,x=1707,y=2799,r=20},
							{mapid=65,x=1605,y=2766,r=20},
							{mapid=65,x=1469,y=2830,r=20},
							{mapid=65,x=1333,y=2850,r=20},
						}
		if (HaveNormalItem(3,16,0,0)>=1) then
			DelNormalItem(3,16,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
			AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
		else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,9)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function v3()
		if (HaveNormalItem(3,17,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ Ly<color> cho ta?","yv3","no")
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ Ly<color>!")
		end;
end;

function yv3()
		local maps =	{ 
							{mapid=8,x=1740,y=2976,r=20},
							{mapid=8,x=1816,y=2944,r=20},
							{mapid=8,x=1860,y=2805,r=20},
							{mapid=8,x=1948,y=3044,r=20},
							{mapid=8,x=2021,y=3021,r=20},
							{mapid=8,x=1905,y=3071,r=20},
							{mapid=8,x=1780,y=3312,r=20},
							{mapid=8,x=1674,y=3348,r=20},
							{mapid=8,x=1717,y=3491,r=20},
							{mapid=8,x=1965,y=3223,r=20},
							{mapid=9,x=1830,y=3708,r=20},
							{mapid=9,x=1810,y=3588,r=20},
							{mapid=9,x=1716,y=3614,r=20},
							{mapid=9,x=1617,y=3609,r=20},
							{mapid=9,x=1677,y=3751,r=20},
							{mapid=9,x=1576,y=3450,r=20},
							{mapid=9,x=1652,y=3346,r=20},
							{mapid=9,x=1654,y=3248,r=20},
							{mapid=9,x=1749,y=3263,r=20},
							{mapid=9,x=1777,y=3354,r=20},
							{mapid=10,x=1566,y=3194,r=20},
							{mapid=10,x=1631,y=3234,r=20},
							{mapid=10,x=1608,y=3346,r=20},
							{mapid=10,x=1512,y=3408,r=20},
							{mapid=10,x=1464,y=3507,r=20},
							{mapid=10,x=1378,y=3361,r=20},
							{mapid=10,x=1306,y=3254,r=20},
							{mapid=10,x=1251,y=3145,r=20},
							{mapid=10,x=1084,y=3153,r=20},
							{mapid=10,x=1019,y=3241,r=20},
						}
		if (HaveNormalItem(3,17,0,0)>=1) then
			DelNormalItem(3,17,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
			AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
		else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function v4()
		if (HaveNormalItem(3,18,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ CÊn<color> cho ta?","yv4","no")
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ CÊn<color>!")
		end;
end;

function yv4()
		local maps =	{ 
							{mapid=15,x=1413,y=3519,r=20},
							{mapid=15,x=1410,y=3289,r=20},
							{mapid=15,x=1410,y=3105,r=20},
							{mapid=15,x=1547,y=3050,r=20},
							{mapid=15,x=1575,y=3219,r=20},
							{mapid=15,x=1612,y=3373,r=20},
							{mapid=15,x=1597,y=3535,r=20},
							{mapid=15,x=1653,y=3472,r=20},
							{mapid=15,x=1714,y=3289,r=20},
							{mapid=15,x=1758,y=3090,r=20},
							{mapid=18,x=1683,y=2950,r=20},
							{mapid=18,x=1534,y=3397,r=20},
							{mapid=18,x=1551,y=3311,r=20},
							{mapid=18,x=1507,y=3159,r=20},
							{mapid=18,x=1515,y=2935,r=20},
							{mapid=18,x=1603,y=2895,r=20},
							{mapid=18,x=1567,y=3007,r=20},
							{mapid=18,x=1568,y=3302,r=20},
							{mapid=18,x=1580,y=3438,r=20},
							{mapid=18,x=1696,y=3430,r=20},
						}
		if (HaveNormalItem(3,18,0,0)>=1) then
			DelNormalItem(3,18,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
			AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function v5()
		if (HaveNormalItem(3,19,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ §oµi<color> cho ta?","yv5","no")
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ §oµi<color>!")
		end;
end;

function yv5()
		local maps =	{ 
							{mapid=14,x=1713,y=3613,r=20},
							{mapid=14,x=1661,y=3437,r=20},
							{mapid=14,x=1633,y=3293,r=20},
							{mapid=14,x=1673,y=3173,r=20},
							{mapid=14,x=1564,y=3189,r=20},
							{mapid=14,x=1500,y=3253,r=20},
							{mapid=14,x=1518,y=3419,r=20},
							{mapid=14,x=1506,y=3555,r=20},
							{mapid=14,x=1390,y=3683,r=20},
							{mapid=14,x=1347,y=3494,r=20},
							{mapid=17,x=1613,y=3595,r=20},
							{mapid=17,x=1613,y=3291,r=20},
							{mapid=17,x=1645,y=3123,r=20},
							{mapid=17,x=1693,y=3099,r=20},
							{mapid=17,x=1713,y=3187,r=20},
							{mapid=17,x=1729,y=3291,r=20},
							{mapid=17,x=1729,y=3419,r=20},
							{mapid=17,x=1733,y=3571,r=20},
							{mapid=17,x=1817,y=3579,r=20},
							{mapid=17,x=1897,y=3427,r=20},
						}
		if (HaveNormalItem(3,19,0,0)>=1) then
			DelNormalItem(3,19,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
			AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function v6()
		if (HaveNormalItem(3,20,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ Tèn<color> cho ta?","yv6","no")
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ Tèn<color>!")
		end;
end;

function yv6()
		local maps =	{ 
							{mapid=11,x=1819,y=3580,r=20},
							{mapid=11,x=1823,y=3365,r=20},
							{mapid=11,x=1815,y=3229,r=20},
							{mapid=11,x=1706,y=3126,r=20},
							{mapid=11,x=1702,y=3342,r=20},
							{mapid=11,x=1710,y=3567,r=20},
							{mapid=11,x=1642,y=3567,r=20},
							{mapid=11,x=1610,y=3503,r=20},
							{mapid=11,x=1642,y=3390,r=20},
							{mapid=11,x=1630,y=3286,r=20},
							{mapid=12,x=1499,y=3544,r=20},
							{mapid=12,x=1523,y=3368,r=20},
							{mapid=12,x=1523,y=3216,r=20},
							{mapid=12,x=1523,y=3064,r=20},
							{mapid=12,x=1571,y=2959,r=20},
							{mapid=12,x=1619,y=3159,r=20},
							{mapid=12,x=1672,y=3367,r=20},
							{mapid=12,x=1680,y=3543,r=20},
							{mapid=12,x=1749,y=3527,r=20},
							{mapid=12,x=1727,y=3181,r=20},
							{mapid=13,x=1601,y=3219,r=20},
							{mapid=13,x=1619,y=2992,r=20},
							{mapid=13,x=1615,y=2863,r=20},
							{mapid=13,x=1687,y=2871,r=20},
							{mapid=13,x=1703,y=3031,r=20},
							{mapid=13,x=1707,y=3199,r=20},
							{mapid=13,x=1895,y=2831,r=20},
							{mapid=13,x=1920,y=2951,r=20},
							{mapid=13,x=1901,y=3071,r=20},
							{mapid=13,x=1942,y=3119,r=20},
						}
		if (HaveNormalItem(3,20,0,0)>=1) then
			DelNormalItem(3,20,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
			AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,10)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function v7()
		if (HaveNormalItem(3,21,0,0)>=1) then
			if (GetTask(382)==0) then 
				MsgBox("Ng­¬i quyÕt ®Şnh ®­a <color=green>QuÎ ChÊn<color> cho ta?","yv7","no")
			end;
		else
			Talk(1,"no","Ng­¬i kh«ng cã <color=green>QuÎ ChÊn<color>!")
		end;
end;

function yv7()
		local maps =	{ 	{mapid=5,x=1674,y=2858,r=20},
							{mapid=5,x=1585,y=3075,r=20},
							{mapid=5,x=1584,y=3210,r=20},
							{mapid=5,x=1667,y=3177,r=20},
							{mapid=5,x=1746,y=3205,r=20},
							{mapid=5,x=1803,y=3190,r=20},
							{mapid=5,x=1815,y=3044,r=20},
							{mapid=6,x=1886,y=3068,r=20},
							{mapid=6,x=1550,y=3059,r=20},
							{mapid=6,x=1552,y=2905,r=20},
							{mapid=6,x=1560,y=2795,r=20},
							{mapid=6,x=1669,y=2811,r=20},
							{mapid=6,x=1820,y=2797,r=20},
							{mapid=6,x=1686,y=2931,r=20},
							{mapid=6,x=1660,y=3120,r=20},
							{mapid=6,x=1793,y=3257,r=20},
							{mapid=6,x=1886,y=3244,r=20},
							{mapid=7,x=1617,y=3089,r=20},
							{mapid=7,x=1600,y=3349,r=20},
							{mapid=7,x=1609,y=3444,r=20},
							{mapid=7,x=1629,y=3564,r=20},
							{mapid=7,x=1709,y=3552,r=20},
							{mapid=7,x=1745,y=3423,r=20},
							{mapid=7,x=1868,y=3335,r=20},
							{mapid=7,x=1791,y=3241,r=20},
							{mapid=7,x=1766,y=3049,r=20},
							{mapid=7,x=1844,y=3009,r=20},
						}
		if (HaveNormalItem(3,21,0,0)>=1) then
			DelNormalItem(3,21,0,0)
			AddNormalItem(6,1,110,0,1,0) --»ñµÃÎï¢»Ñò¢³¾íÖá
			Msg2Player("NhËn ®­îc MËt tŞch da dª")
		if (GetTask(395)>0)and(GetTask(395)<200)and(GetTask(381)~=0)then
			local exp=2000-GetTask(395)*10
			AddOwnExp(exp)
			TopMessage("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. LÇn tr­íc ng­¬i ®Õn <color=red>"..mapname[GetTask(381)].."<color> t×m b¶o vËt hÕt <color=red>"..GetTask(395).."<color> gi©y, ®©y lµ <color=green>"..exp.."<color> ®iÓm kinh nghiÖm ng­¬i nhËn ®­îc.","no")
			Msg2Player("B¹n nhËn ®­îc "..exp.." ®iÓm kinh nghiÖm.")
			else
			MsgBox("§©y chİnh lµ <color=green>MËt tŞch da dª<color> chøa tin tøc liªn quan ®Õn kho b¸u. Hµo quang cña nã sÏ dÉn ng­¬i ®i","no")
		end;
			local num=random(1,7)
			local px=maps[num].x
			local py=maps[num].y
			local r=random(0,3)
			if(r==0)then
				px=px+random(maps[num].r)
				py=py+random(maps[num].r)
			elseif(r==1)then
				px=px-random(maps[num].r)
				py=py-random(maps[num].r)
			elseif(r==2)then
				px=px+random(maps[num].r)
				py=py-random(maps[num].r)
			else
				px=px-random(maps[num].r)
				py=py+random(maps[num].r)
			end;
			SetTask(381,maps[num].mapid)
			SetTask(382,px)
			SetTask(383,py)
			SetTask(378,0)
			SetTask(380,SystemTime())
			SetTask(395,0)
		end;
end;

function no()
		CloseDialog()
end;


Include("\\script\\phongthan\\daily\\reward.lua")

pt_daily_original_seven=complete_jisi
function complete_jisi()
 if GetTask(826)~=7 or GetTask(833)~=2 or GetTask(561)~=0 or GetLevel()<45 then return end
 for i=827,833 do if GetTask(i)~=2 then return end end
 pt_daily_original_seven()
 if GetTask(826)==0 then PTDailyHope(3,PTDailyNextReceipt(3)) end
end

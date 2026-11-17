--description: °Ø¼ø
--author: yichuan
--date: 2005/3/10

--Task583==7 ?×ö·û
--Task583==8 ×öÍê³Ñ·û£¬ĞèÒª·û½ø?¶«ÒÄ
--Task583>=9 ´«ËÍ¹ıÒ»´Î£¬²»ĞèÒª·û¿ÉÒÔ½ø?¶«ÒÄ



function main(sel)
			tasks =
			{
				{"Kh«ng Minh L­u ChuyÓn","kongming";show=0},
				{"ChÕ t¹o Linh phï","lingfu";show=0},
				{"§i §«ng Di","qwdy";show=0},
				{"LÊy ®­îc qu¶","guoshi";show=0},
				{"Trång c©y","chaxun";show=1},
				{"Quû nguyÖt","guijie";show=0}
			}
			local mark=0
--			if(GetTask(597)>=6)then
--				if(GetTask(597)==6)then
--					tasks[1].show=1
--					mark=1
--				elseif (GetTask(597)==7)then
--					tasks[2].show=1
--					mark=1
--				elseif (GetTask(597)>=8)then
--					tasks[3].show=1
--					mark=1
--				end;
--			end;
			if(GetLevel()>=35) then
				local ZZtype=GetTask(804)
				if((ZZtype==1) and (HaveEventItem(49)>=1)) then
					tasks[4].show=1
					mark=1
				elseif ((ZZtype==2) and (HaveEventItem(164)>=1)) then
					tasks[4].show=1
					mark=1
				end
			end;
			if(mark>0)then
				SayTask("Trêi ®Êt mªnh m«ng, m©y bay bÒnh bång, m×nh ta ngao du!",tasks)
			else
				SayTask(11185,tasks)
			end;
end;


function guijie()
		Talk(1,"jishinext","§Õn ngµy Quû NguyÖt, nhiÒu linh hån ch­a ®­îc siªu tho¸t biÕn thµnh ¸c ma. Ng­¬i ph¶i thu thËp mét sè <color=green>vËt tÕ<color> siªu ®é chóng")	
end

function jishinext()
	local LastTime=GetTask(821)
	local NowTime=SystemTime()  
	local tasks =
		{
			{"TÕ quû nguyÖt","jishi";show=1},
			{"T×m hiÓu","intro";show=1}
		}
	if (GetTask(820)==0) then
		SayTask("LÇn cóng tÕ ®Çu tiªn ph¶i cã <color=green>¢m D­¬ng Kİnh<color>, <color=green>Ngäc H­ Phï<color>, <color=green>§Ìn ThÇn<color>, ta sÏ më thiªn nh·n cho ng­¬i giao tiÕp víi chóng. ",tasks)
	elseif (GetTask(822)==0) and (floor(NowTime/86400)>floor(LastTime/86400))then
		SayTask("Ng­¬i ®· më thiªn nh·n, cÇn ph¶i ®­a <color=green>§Ìn ThÇn<color> ta míi gióp ng­¬i duy tr× ph¸p lùc cña thiªn nh·n, nÕu kh«ng ng­¬i kh«ng thÓ hoµn tÊt viÖc cóng tÕ h«m nay.",tasks)
	elseif (GetTask(822)==0) and (floor(NowTime/86400)==floor(LastTime/86400)) then
		MsgBox("Ng­¬i ®· hoµn tÊt viÖc cóng tÕ h«m nay, mai h·y quay l¹i!","no")
	elseif (GetTask(822)>0) then
		MsgBox("Ng­¬i ch­a lµm lÔ siªu ®é, mau ®Õn chiÕn tr­êng ViÔn Cæ hoµn tÊt viÖc cóng tÕ, ta míi cã thÓ gióp ng­¬i duy tr× ph¸p lùc thiªn nh·n.","no")
	end
end


function intro()
	Talk(1,"intronext","Qua bao cuéc chiÕn, nhiÒu sinh linh ch­a ®­îc siªu tho¸t biÕn thµnh ¸c ma. Ng­¬i ph¶i ®Õn chiÕn tr­êng ViÔn Cæ siªu ®é chóng. §èi víi ¸c ma kh«ng thÓ siªu ®é ta sÏ tiªu diÖt chóng ®Ó khái g©y häa nh©n gian.")
end

function intronext()
	Talk(1,"introover","Ng­¬i ph¶i ®­a <color=green>¢m D­¬ng Kİnh<color>, <color=green>Ngäc H­ Phï<color>, <color=green>§Ìn ThÇn<color> ta míi cã thÓ gióp ng­¬i më thiªn nh·n giao tiÕp víi c¸c hån ma. Mçi ngµy chØ tÕ ®­îc <color=green>1 lÇn<color>, 1 lÇn chia ra <color=green>5 b­íc<color>.")
end

function introover()
	Talk(1,"introquit","Sau khi më thiªn nh·n, mçi ngµy b¾t ®Çu cóng tÕ ph¶i ®­a cho ta mét <color=green>§Ìn ThÇn<color> dïng ®Ó duy tr× ph¸p lùc thiªn nh·n.")
end

function introquit()
	Talk(1,"no","VËt tÕ gåm <color=green>L­ h­¬ng<color>, <color=green>Bïa trõ tµ<color>, <color=green>KiÕm gç<color>, <color=green>Chuéc hån ®¨ng<color>, <color=green>TiÒn giÊy<color>, <color=green>Hoµng töu<color>. TiÒn giÊy vµ Hoµng töu lµ 2 thø cÇn thiÕt cho viÖc cóng tÕ.")
end



function jishi()
	local LastTime=GetTask(821)--°O¿ı¬¡°Ê¨C¤Ñ²½ªÁ¶}©lªº®É¶¡
	local NowTime=SystemTime()  
	if (GetTask(820)==0) then  --ªí¥ÜÁÙ¨S¦³¶}±Ò°­¸`¬¡°Ê
		beging()
	elseif (GetTask(822)>=1) and (GetTask(822)<=5)  then--ªí¥Ü¤§«eªº²½ªÁÁÙ¨S¦³²½ªÁ§¹
		MsgBox("Ng­¬i ch­a lµm lÔ siªu ®é, mau ®Õn chiÕn tr­êng ViÔn Cæ hoµn tÊt viÖc cóng tÕ, ta míi cã thÓ gióp ng­¬i duy tr× ph¸p lùc thiªn nh·n.","no")
	elseif (floor(NowTime/86400)>floor(LastTime/86400)) and (GetTask(822)==0)  then--ªí¥Ü¶}©l·sªº¤@¤Ñªº²½ªÁ¤F
		if (HaveEventItem(41)>=1)  then
			MsgBox("RÊt tèt! Giao <color=green>§Ìn ThÇn<color> cho ta, ta sÏ ®­a ng­¬i ®Õn chiÕn tr­êng ViÔn Cæ siªu ®é ma quû","jishihuodong","no")
		else
			MsgBox("Thiªn nh·n cña ng­¬i ®· ®­îc më. Mau ®­a <color=green>§Ìn ThÇn<color> ta sÏ gióp ng­¬i duy tr× ph¸p lùc thiªn nh·n. §õng nªn bá qua dŞp cóng tÕ nµy!","no")
		end
	elseif (floor(NowTime/86400)==floor(LastTime/86400)) and (GetTask(822)==0)  then--ªí¥Ü¤µ¤Ñªº²½ªÁ§¹¦¨¤F
		MsgBox("Ng­¬i ®· hoµn tÊt viÖc cóng tÕ h«m nay, mai h·y quay l¹i!","no")
	end
end

function beging()
	if (GetTask(820)==0) then 
		if (HaveEventItem(41)>=1) and (GetItemLevel2(0,4,6)>=1) and (GetItemLevel2(0,4,21)>=1) then
			MsgBox("RÊt tèt! Giao <color=green>¢m D­¬ng Kİnh<color>, <color=green>Ngäc H­ Phï<color>, <color=green>§Ìn ThÇn<color> ta sÏ gióp ng­¬i më thiªn nh·n vµ ®­a tíi chiÕn tr­êng ViÔn Cæ siªu ®é ma quû","begingnext","no")
		else
			MsgBox("Ph¶i cã <color=green>¢m D­¬ng Kİnh<color>, <color=green>Ngäc H­ Phï<color>, <color=green>§Ìn ThÇn<color> ta míi cã thÓ gióp ng­¬i më thiªn nh·n","no")
		end
	end
end

function begingnext()
	CloseDialog()
	local LastTime=GetTask(821)
	local NowTime=SystemTime()  
	if (GetTask(820)==0) then 
		if (HaveEventItem(41)>=1) and (GetItemLevel2(0,4,6)>=1) and (GetItemLevel2(0,4,21)>=1) then
			local  YYLevel=GetItemLevel2(0,4,6)
			local  YXLevel=GetItemLevel2(0,4,21)
			DelItem2(0,4,6,YYLevel)--³±¶§Ãè
			DelItem2(0,4,21,YXLevel)--¥Éµê²Å
			DelEventItem(41)--¤£·À¯«¿O
			SetTask(820,1)
			SetTask(821,NowTime)
			SetTask(822,1)
			SetTask(823,0)
			NewWorld(64,1600,3238)
			SetFightState(1)
			Msg2Player("B¸ Gi¸m ®· më thiªn nh·n vµ ®­a b¹n ®Õn chiÕn tr­êng ViÔn Cæ.")
		else
			MsgBox("Ph¶i cã <color=green>¢m D­¬ng Kİnh<color>, <color=green>Ngäc H­ Phï<color>, <color=green>§Ìn ThÇn<color> ta míi cã thÓ gióp ng­¬i më thiªn nh·n","no")
		end
	end
end


function jishihuodong()
	CloseDialog()
	local LastTime=GetTask(821)
	local NowTime=SystemTime()  
	if (floor(NowTime/86400)>floor(LastTime/86400)) and (GetTask(822)==0) and (HaveEventItem(41)>=1) then--ªí¥Ü¶}©l·sªº¤@¤Ñªº²½ªÁ¤F
		SetTask(821,NowTime)
		SetTask(822,1)
		SetTask(823,0)
		DelEventItem(41)
		NewWorld(64,1600,3238)
		SetFightState(1)
		Msg2Player("B¸ Gi¸m ®· duy tr× ph¸p lùc thiªn nh·n vµ ®­a b¹n ®Õn chiÕn tr­êng ViÔn Cæ.")	
	else
		MsgBox("X¶y ra sai xãt trong viÖc cóng tÕ, xin h·y thö l¹i!","no")
	end
end

function chaxun()
	MsgBox("Tæng sè c©y ®¹t ®é tr­ëng thµnh tõ 55 trë lªn cã "..GetTask(806)..", ®é tr­ëng thµnh tõ 50 - 55 cã "..GetTask(807).." , ®é tr­ëng thµnh tõ 45 - 50 cã "..GetTask(808)..", ®é tr­ëng thµnh tõ 40 - 45 cã "..GetTask(809).." ","no")
end;


function guoshi()
	MsgBox("Ph¸p thuËt cña ta cã thÓ lµm cho mÇm c©y lËp tøc khai hoa kÕt tr¸i. MÇm cña ng­¬i tæng céng tr­ëng thµnh <color=green>"..floor(GetTask(320)/2).."<color> lÇn, hiÖn ®é sinh tr­ëng lµ <color=green>"..GetTask(327).."<color>, ng­¬i x¸c nhËn c©y ®· ®ñ lín?","renwu","no")
end;


----´«ËÍ¶«ÒÄ----
function kongming()
	if (GetTask(597)==6) then
		Talk(1,"no","ThuËt <color=yellow>Kh«ng Minh L­u ChuyÓn<color> sÏ tæn h¹i lín søc kháe cña ng­¬i! Ng­¬i h·y ®i thu thËp <color=yellow>Ngäc H­ phï, Hång B¶o th¹ch, 1w l­îng<color>, råi ®Õn t×m ta.")
		SetTask(597,7)
		TaskNote(35,7)
	end;
end;

function lingfu()
	if (GetTask(597)==7)then
		if (GetCash()>=10000)and(GetTask(597)==7)and (GetItemLevel2(0,4,21)>=1)and (HaveNormalItem(3,79,0,0)>=1)then
			MsgBox("Hay l¾m! Ta sÏ lËp tøc chÕ linh phï cho ng­¬i! Xin ®îi mét l¸t.","shaodeng","no")
		else
			Talk(1,"no","Kh«ng ®ñ nguyªn liÖu kh«ng thÓ lµm linh phï. Lµm hé th©n phï cÇn: <color=yellow>Ngäc H­ phï, Hång B¶o th¹ch, 1w l­îng<color>.")
		end;
	end;
end;

function shaodeng()
	if (GetCash()>=10000)and(GetTask(597)==7) and (GetItemLevel2(0,4,21)>=1)and (HaveNormalItem(3,79,0,0)>=1) then
		Talk(1,"no","Xong råi! §©y lµ <color=yellow>hé th©n phï<color>. Linh phï nµy c¨n cø vµo thÓ chÊt cña ng­¬i mµ lµm.")
			Pay(10000)
		DelNormalItem(3,79,0,0)
		local nLevel=GetItemLevel2(0,4,21)
		DelItem2(0,4,21,nLevel)
		AddEventItem(108)
		local exp=GetNextExp()-GetExp()
			if(exp>=100000)then
			AddOwnExp(100000) --¾­Ñé½±Àø
		else
			AddOwnExp(exp)
			AddOwnExp(100000-exp)
		end;
		Msg2Player("B¹n nhËn ®­îc 10w ®iÓm kinh nghiÖm!")
		TopMessage("PhÇn th­ëng: <color=green>10w ®iÓm kinh nghiÖm<color>!")
		Msg2Player("B¹n nhËn ®­îc Hé th©n phï!")
		--TopMessage(""ÄãµÃµ½³Ñ<color=green>»¤ÉíÁé·û<color>")
		SetTask(597,8)
		TaskNote(35,8)
		Msg2Player("B¹n cã thÓ nhê vµo phĞp Kh«ng Minh L­u ChuyÓn ®Õn ¶i Giai Méng.")
	else
		Talk(1,"no","Kh«ng ®ñ nguyªn liÖu kh«ng thÓ lµm linh phï. Lµm hé th©n phï cÇn: <color=yellow>Ngäc H­ phï, Hång B¶o th¹ch, 1w l­îng<color>.")

	end;
end;

function qwdy()
	if (GetTask(597)==8) then
		if (HaveEventItem(108)>=1) then
			MsgBox("Ng­¬i muèn ®Õn <color=red>¶i Giai Méng<color> ­?","chuansong","no")
		else
			Talk(1,"no","Ng­¬i kh«ng ®em hé th©n phï, ta kh«ng thÓ gióp ng­¬i thi triÓn Kh«ng Minh L­u ChuyÓn thuËt.")
		end;
	elseif (GetTask(597)>=9) then
		if (GetCash()>=1000) then
			MsgBox("§Õn ¶i Giai Méng cÇn <color=yellow>1000 l­îng<color>. §i chø?","chuansong","no")
		else
			MsgBox("Kh«ng ®ñ tiÒn kh«ng thÓ ®i ®­îc","no")
		end;
	end;
end;

function chuansong()
	if(GetTask(597)==8)then
		if (HaveEventItem(108)>=1) then
			DelEventItem(108)
			SetTask(597,9)
			TaskNote(35,9)
			PolyMorph(-1,0,0,0,0)
			NewWorld(73,1615,3888)
			CloseDialog()
		end;
	elseif(GetTask(597)>=9)then
		Pay(1000)
		PolyMorph(-1,0,0,0,0)
		NewWorld(73,1615,3888)
		CloseDialog()
	end;
end;

----´«ËÍ¶«ÒÄ----

function no()
		CloseDialog()
end;

function renwu()
	local l=GetName()
	local ZZtype=GetTask(804)
	if((ZZtype==1) and (HaveEventItem(49)>=1)) or ((ZZtype==2) and (HaveEventItem(164)>=1)) then
						if((ZZtype==1) and (HaveEventItem(49)>=1)) then
							DelEventItem(49)
						elseif((ZZtype==2) and (HaveEventItem(164)>=1)) then
							DelEventItem(164)
						end
						local i=GetTask(327)
						if (GetTask(813)==1) then
							AddOwnExp(i*25*GetLevel())
						else
							AddOwnExp(i*5*GetLevel())
						end


						if (i>=55)then
							SetGlobalValue(6,1)
							SetTask(806,GetTask(806)+1)--°O¿ıÀò±o¤@µ¥¼úªº¦¸¼Æ
							local   k=random(1,12)
							if(k==1)then
								AddNormalItem(0,0,13,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 75 (V¹n KiÕp)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 75 <color=green>V¹n KiÕp<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ V¹n KiÕp cÊp 75 <color=green>"..l.."<color>.",20)
							elseif(k==2)then
								AddNormalItem(0,0,16,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 75 (B¹ch §iÓu)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 75 <color=green>B¹ch §iÓu<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ B¹ch §iÓu cÊp 75 <color=green>"..l.."<color>.",20)
							elseif(k==3)then
								AddNormalItem(0,0,19,1,0,0)
								Msg2Player("B¹n nhËn ®­îc mét vò khİ cÊp 75 (Nh©n gian)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 75 <color=green>Nh©n Gian<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ Nh©n Gian cÊp 75 <color=green>"..l.."<color>.",20)
							elseif(k==4)then
								AddNormalItem(0,0,22,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 75 (NguyÖt ¶nh)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 75 <color=green>NguyÖt ¶nh<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ NguyÖt ¶nh cÊp 75 <color=green>"..l.."<color>.",20)
							elseif(k==5)then
								AddNormalItem(0,0,14,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 85 (VŞ ¦¬ng)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 85 <color=green>VŞ ¦¬ng<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ VŞ ¦¬ng cÊp 85 <color=green>"..l.."<color>.",20)
							elseif(k==6)then
								AddNormalItem(0,0,17,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 85 (Tinh ThÇn)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 85 <color=green>Tinh ThÇn<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ Tinh ThÇn cÊp 85 <color=green>"..l.."<color>.",20)
							elseif(k==7)then
								AddNormalItem(0,0,20,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 85 (V« L­îng)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 85 <color=green>V« L­îng<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ V« L­îng cÊp 85 <color=green>"..l.."<color>.",20)
							elseif(k==8)then
								AddNormalItem(0,0,23,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 85 (BÊt Hèi)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 85 <color=green>BÊt Hèi<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ BÊt Hèi cÊp 85 <color=green>"..l.."<color>.",20)
							elseif(k==9)then
								AddNormalItem(0,0,15,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 95 (Chİ T«n)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 95 <color=green>Chİ T«n<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ Chİ T«n cÊp 95 <color=green>"..l.."<color>.",20)
							elseif(k==10)then
								AddNormalItem(0,0,18,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 95 (Phiªu MiÓu)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 95 <color=green>Phiªu MiÓu<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ Phiªu MiÓu cÊp 95 <color=green>"..l.."<color>.",20)
							elseif(k==11)then
								AddNormalItem(0,0,21,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 95 (Tam Giíi)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 95 <color=green>Tam Giíi<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ Tam Giíi cÊp 95 <color=green>"..l.."<color>.",20)
							elseif(k==12)then
								AddNormalItem(0,0,24,1,0,0)
								Msg2Player("B¹n nhËn ®­îc vò khİ cÊp 95 (§a T×nh)")
								TopMessage("B¹n nhËn ®­îc vò khİ cÊp 95 <color=green>§a T×nh<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc vò khİ §a T×nh cÊp 95 <color=green>"..l.."<color>.",20)
							end;
						elseif (i>=50)then
							SetTask(807,GetTask(807)+1)--°O¿ıÀò±o¤Gµ¥¼úªº¦¸¼Æ
							AddNormalItem(3,49,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 13")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 13<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc B¸ L¹c Kİnh cÊp 13 <color=green>"..l.."<color>.",20)
						elseif (i>=45)then
							SetTask(808,GetTask(808)+1)--°O¿ıÀò±o¤Tµ¥¼úªº¦¸¼Æ
							local   j=random(1,3)
							if(j==1)then
								AddNormalItem(3,46,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc mét B¸ L¹c nh·n cÊp 10")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 10<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc B¸ L¹c Kİnh cÊp 10 <color=green>"..l.."<color>.",20)
							elseif(j==2)then
								AddNormalItem(3,47,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 11")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 11<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc B¸ L¹c Kİnh cÊp 11 <color=green>"..l.."<color>.",20)
							elseif(j==3)then
								AddNormalItem(3,48,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 12")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 12<color>")
								AddGlobalCountNews("<color=green>"..l.."<color>, nhËn ®­îc B¸ L¹c Kİnh cÊp 12 <color=green>"..l.."<color>.",20)
							end;
						elseif (i>=40)then
							SetTask(809,GetTask(809)+1)--°O¿ıÀò±o¥|µ¥¼úªº¦¸¼Æ
							local   x=random(1,4)
							if(x==1)then
								AddNormalItem(3,42,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 6")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 6<color>")
							elseif(x==2)then
								AddNormalItem(3,43,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 7")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 7<color>")
							elseif(x==3)then
								AddNormalItem(3,44,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 8")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 8<color>")
							elseif(x==4)then
								AddNormalItem(3,45,0,1,0,0)
								Msg2Player("B¹n nhËn ®­îc B¸ L¹c nh·n cÊp 9")
								TopMessage("B¹n nhËn ®­îc <color=green>B¸ L¹c Nh·n cÊp 9<color>")
							end;
						elseif (i>=35)then
							AddEventItem(48)
							Msg2Player("B¹n nhËn ®­îc 1 H¹t thÇn bİ.")
							TopMessage("B¹n nhËn ®­îc <color=green>H¹t thÇn bİ<color>")
						elseif (i>=30)then
							AddEventItem(39)
							Msg2Player("B¹n nhËn ®­îc 1 nh¸nh LiÔu Méc")
							TopMessage("B¹n nhËn ®­îc <color=green>LiÔu méc<color>")
						end;

						SetTask(320,0)--Á`¦¸¼Æ
						SetTask(321,0)--¼å¤ô©Ò»İ§÷®Æ
						SetTask(322,0)--¬IªÎ©Ò»İ§÷®Æ
						SetTask(323,0)--§ìÂÎ©Ò»İªkÄ_
						SetTask(324,0)--¼å¤ô¥¢±Ñ²v
						SetTask(325,0)--¬IªÎ¥¢±Ñ²v
						SetTask(326,0)--§ìÂÎ¥¢±Ñ²v
						SetTask(327,0)--¦¨ªø«×
						SetTask(815,0)--»¡©úºØ¾ğµ²§ô
						SetTask(813,2)--ªí¥Ü²Ä¤@Àôµ²§ô
						SetTask(804,0)
						Talk(1,"no",11186)
	else
						Talk(1,"no",11187)
	end;
end;

Include("\\script\\phongthan\\daily\\reward.lua")

pt_daily_original_tree=renwu
function renwu()
 local kind=GetTask(804)
 local item=49
 if kind==2 then item=164 end
 local before=HaveEventItem(item)
 pt_daily_original_tree()
 if (kind==1 or kind==2) and before>0 and HaveEventItem(item)<before and GetTask(804)==0 then PTDailyHope(6,PTDailyNextReceipt(6)) end
end

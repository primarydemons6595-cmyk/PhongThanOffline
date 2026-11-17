--description:»õÉÌÍõ--ò¿ÓÈÄ¹»õÉÌNPC
--author:?½¯·É
--data:2004.7.19
--modify:Áõºê²¨
--data:2005.3.24

function RandomTask()
	CloseDialog()
	local GNum=GetTask(766)--°O¿ı¶]°Ó¥ô°Èªº¦¸¼Æ
	local GLastTime=GetTask(769)--°O¿ı¨C¤Ñ¶]°Ó¥ô°È¶}©l²Ä¤@¦¸ªº®É¶¡
	local NowTime=SystemTime()
	if (GetTask(333)>0) then
		local Pgoods,var
		for i=0,10,2 do
			if ( i == 10) then
				var = 10
				break
			end;
			Pgoods=GetTask(110+i)
			if (Pgoods==GetTask(334)) then
				if (GetTask(110+i+1) >= GetTask(335)) then
					var = i
					break
				end;
			end;
		end;
		if (var < 10) and (GetTask(333)==3) then
		-- Íê³É³Ñ?ÎñÌõ¼ş
			MsgBox(10743,"complete_task","no")
		else
			local string = "Ng­¬i ®· nhËn "
			local Num=GetTask(766)+1
			if (GetTask(333)==1) then
				string = string.."<color=green>Sïng Thµnh §¹i Doanh"..trader_name[1];
			elseif (GetTask(333)==2) then
				string = string.."<color=green>Ngäc H­ Cung"..trader_name[2];
			elseif (GetTask(333)==3) then
				string = string.."<color=green>Xi V­u Mé"..trader_name[3];
			elseif (GetTask(333)==4) then
				string = string.."<color=green>TriÒu Ca"..trader_name[4];
			elseif (GetTask(333)==5) then
				string = string.."<color=green>T©y Kú"..trader_name[5];
			elseif (GetTask(333)==6) then
				string = string.."<color=green>Sa m¹c Phong Than"..trader_name[6];
			elseif (GetTask(333)==7) then
				string = string.."<color=green>§«ng H¶i H¶i C©u"..trader_name[7];
			elseif (GetTask(333)==8) then
				string = string.."<color=green>Hiªn Viªn ®éng tÇng 3"..trader_name[8];
			elseif (GetTask(333)==9) then
				string = string.."<color=green>§¹i Phong B¨ng Xuyªn"..trader_name[9];
			end;
			string = string.."<color>, cÇn <color=green>"..GetGoodsName(GetTask(334)).."<color> [<color=red>"..GetTask(335).."<color>] "..". Mau ®i t×m gióp «ng ta, ®õng ®Ó l·ng phİ thêi gian"

			Talk(1,"no",string)
		end;
	elseif (floor(NowTime/86400)>floor(GLastTime/86400))  then
		if ((GetTask(333)~=0)) then
			if ((GetTask(333)>0) and (GetTask(333)<=9) ) then
				local string = "Ng­¬i ®· nhËn "
				local Num=GetTask(766)+1
				if (GetTask(333)==1) then
					string = string.."<color=green>Sïng Thµnh §¹i Doanh"..trader_name[1];
				elseif (GetTask(333)==2) then
					string = string.."<color=green>Ngäc H­ Cung"..trader_name[2];
				elseif (GetTask(333)==3) then
					string = string.."<color=green>Xi V­u Mé"..trader_name[3];
				elseif (GetTask(333)==4) then
					string = string.."<color=green>TriÒu Ca"..trader_name[4];
				elseif (GetTask(333)==5) then
					string = string.."<color=green>T©y Kú"..trader_name[5];
				elseif (GetTask(333)==6) then
					string = string.."<color=green>Sa m¹c Phong Than"..trader_name[6];
				elseif (GetTask(333)==7) then
					string = string.."<color=green>§«ng H¶i H¶i C©u"..trader_name[7];
				elseif (GetTask(333)==8) then
					string = string.."<color=green>Hiªn Viªn ®éng tÇng 3"..trader_name[8];
				elseif (GetTask(333)==9) then
					string = string.."<color=green>§¹i Phong B¨ng Xuyªn"..trader_name[9];
				end;
				string = string.."<color>, cÇn <color=green>"..GetGoodsName(GetTask(334)).."<color> [<color=red>"..GetTask(335).."<color>] "..". Mau ®i t×m gióp «ng ta, ®õng ®Ó l·ng phİ thêi gian"

				Talk(1,"no",string)
			else
				Talk(1,"no",10739)
			end;
		else
			SetTask(766,0)
			paoshangrenwu()
		end
	elseif(GNum<5) and (GetTask(333)==0) then
		paoshangrenwu()
	elseif(GNum<20) and (GetTask(333)==0) then
		if (HaveNormalItem(8,137,2,0)>=1) then
			MsgBox("Ta ®ang rÊt cÇn <color=green>c¸i tói<color> cña ng­¬i ®Ó ®ùng hµng hãa, cø tÆng ta 1 c¸i ta sÏ cho ng­¬i 1 lÇn vËn tiªu. Sao h¶?","paoshangrenwu","no")
		else
			MsgBox("Mçi ngµy chØ cã thÓ vËn chuyÓn ®­îc <color=green>5 lÇn<color>. Ta ®ang cÇn 1 <color=green>c¸i tói<color> ®Ó ®ùng hµng hãa, ng­¬i tÆng ta 1 c¸i, ta sÏ cho ng­¬i thªm 1 lÇn vËn tiªu","no")
		end
	elseif (GNum>=20) and (floor(NowTime/86400)==floor(GLastTime/86400)) then
		MsgBox("Mçi ng­êi mçi ngµy chØ cã thÓ chuyÓn hµng <color=green>20 lÇn<color>. H«m nay ng­¬i ®· ®ñ, ngµy mai h·y quay l¹i!","no")
	end
end;

function paoshangrenwu()
	CloseDialog()
	local goodsrand = {1,2,3,4,5,6,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27}
	local level = GetLevel()
	local Gindex,goodsnum
	if (level<40) then
			Gindex=random(1,12)
	elseif (level<50) then
		local j=random(1,2)
		if (j==1) then
			Gindex=random(13,15)
		else
			Gindex=random(1,12)
		end
	elseif (level<60) then
		local j=random(1,4)
		if (j==1) then
			Gindex=random(16,18)
		elseif (j==2) then
			Gindex=random(13,15)
		else
			Gindex=random(1,12)
		end
	elseif (level<70) then
		local j=random(1,6)
		if (j==1) then
			Gindex=random(16,18)
		elseif (j==2) then
			Gindex=random(13,15)
		elseif (j==3) then
			Gindex=random(19,21)
		else
			Gindex=random(1,12)
		end
	else
		local j=random(1,8)
		if (j==1) then
			Gindex=random(16,18)
		elseif (j==2) then
			Gindex=random(13,15)
		elseif (j==3) then
			Gindex=random(19,21)
		elseif (j==4) then
			Gindex=random(22,24)
		else
			Gindex=random(1,12)
		end
	end
	local goodsindex = goodsrand[Gindex]

	if (Gindex==1) or (Gindex==4) or (Gindex==7)or (Gindex==10)or (Gindex==13)or (Gindex==16)or (Gindex==19)or (Gindex==22) then
		goodsnum = 20
	elseif (Gindex==2) or (Gindex==5) or (Gindex==8)or (Gindex==11)or (Gindex==14)or (Gindex==17)or (Gindex==20)or (Gindex==23) then
		goodsnum = 2
	elseif (Gindex==3) or (Gindex==6) or (Gindex==9)or (Gindex==12)or (Gindex==15)or (Gindex==18)or (Gindex==21)or (Gindex==24) then
		goodsnum = 1
	end
	SetTask(334,goodsindex)             ----¼ÇÂ¼Íæ¼Ò?ÎñĞèÇóµÄÉÌ¢»±àºÅ
	SetTask(335,goodsnum)               ----¼ÇÂ¼Íæ¼Ò?ÎñĞèÇóµÄÉÌ¢»¸öÊı

	if (GetTask(766)<20) and (GetTask(766)>=5) and (GetTask(333)==0)  then
		if  (HaveNormalItem(8,137,2,0)>=1)then
			SetTask(333,3)						----¼ÇÂ¼ÉÌµê±àºÅ£¨±àºÅÎª0ËµÃ÷íèÓĞ½Óµ½Ëæ»ú?Îñ£©
			if (GetTask(766)==0) then
				SetTask(769,SystemTime())
			end
			CostIBItem(FindAValidIBItem(8,137,2,0))
			MsgBox("ChØ cÇn ng­¬i ®em ®Õn cho ta <color=green>"..GetGoodsName(goodsindex).."<color> [<color=red>"..goodsnum.."<color>], sÏ cã phÇn th­ëng xøng ®¸ng","no")
			Msg2Player("§©y lµ nhiÖm vô thø "..(GetTask(766)+1)..".H·y mau ®i hoµn thµnh!")
		else
			MsgBox("Mçi ngµy chØ cã thÓ vËn chuyÓn ®­îc <color=green>5 lÇn<color>. Ta ®ang cÇn 1 <color=green>c¸i tói<color> ®Ó ®ùng hµng hãa, ng­¬i tÆng ta 1 c¸i, ta sÏ cho ng­¬i thªm 1 lÇn vËn tiªu","no")
		end
	elseif (GetTask(766)<5) and (GetTask(333)==0) then
			SetTask(333,3)						----¼ÇÂ¼ÉÌµê±àºÅ£¨±àºÅÎª0ËµÃ÷íèÓĞ½Óµ½Ëæ»ú?Îñ£©
			if (GetTask(766)==0) then
				SetTask(769,SystemTime())
			end
			MsgBox("ChØ cÇn ng­¬i ®em ®Õn cho ta <color=green>"..GetGoodsName(goodsindex).."<color> [<color=red>"..goodsnum.."<color>], sÏ cã phÇn th­ëng xøng ®¸ng","no")
			Msg2Player("§©y lµ nhiÖm vô thø "..(GetTask(766)+1)..".H·y mau ®i hoµn thµnh!")
	end
end;

function complete_task()
	CloseDialog()
	local var,Pgoods
	for i=0,10,2 do
		if ( i == 10) then
			var = 10
			break
		end;
		Pgoods=GetTask(110+i)
		if (Pgoods==GetTask(334)) then
			if (GetTask(110+i+1) >= GetTask(335)) then
				var = i
				break
			end;
		end;
	end;
	if (var < 10) then
		-- Íê³É³Ñ?ÎñÌõ¼ş£¬ÕâÀï¼ÓÉÏ½±Àø
		SetTask(110+var+1, GetTask(110+var+1)-GetTask(335))
		SetTask(120, GetTask(120)-GetGoodsWeight(GetTask(334)) * GetTask(335))
		if (GetTask(110+var+1)<=0) then
			SetTask(110+var, 0)
		end;
		Msg2Player("B¹n giao cho t¹p hãa V­¬ng "..GetTask(335)..". "..GetGoodsName(GetTask(334)))
		local r = random(1,500)
		local string=""
		local Money,Exp,Chage
		local Level=GetLevel()
		local Num=GetTask(766)
		Chage=100
		if (Level<40) then
			Money=200*Chage
			Exp=Level*100
		elseif (Level<50) then
			Money=250*Chage
			Exp=Level*150
		elseif (Level<60) then
			Money=300*Chage
			Exp=Level*200
		elseif (Level<70) then
			Money=350*Chage
			Exp=Level*250
		else
			Money=400*Chage
			Exp=Level*300
		end
		local string
		if (r==1) then
			AddOwnExp(Exp)
			Earn(Money)
			AddNormalItem(8,148,2,1,0,0)
			string="Cã chót thï lao <color=green>"..Money.."<color>, <color=green>®iÓm kinh nghiÖm"..Exp.."<color>. Ng­¬i lµm rÊt tèt, ta tÆng thªm ng­¬i <color=green>M«n bµi giao dŞch<color>"
			Msg2Player("B¹n nh©n ®­îc ".."TiÒn"..Money.."®iÓm kinh nghiÖm"..Exp.."vµ M«n bµi giao dŞch")
		else
			AddOwnExp(Exp)
			Earn(Money)
			string="Cã chót thï lao <color=green>"..Money.."<color>, <color=green>®iÓm kinh nghiÖm"..Exp.."<color> "
			Msg2Player("B¹n nh©n ®­îc ".."TiÒn"..Money.."®iÓm kinh nghiÖm"..Exp..". ")
		end;
		SetTask(333,0)
		SetTask(334,0)
		SetTask(335,0)
		SetTask(766,GetTask(766)+1)
		SetTask(803,GetTask(803)+1)--?Îñ±äÁ¿803ÓÃÀ´¼ÇÂ¼Íæ¼Ò³É¹¦µÄÅÜÉÌÀÛ¼Æ´ÎÊı
		Talk(1,"no","C¶m ¬n! Xin nhËn chót quµ män!"..string)
	end;
end;

function cancel_task()
	SetTask(333,0)
	SetTask(334,0)
	SetTask(335,0)
	SetTask(766,GetTask(766)+1)
	local LastTime=GetTask(769)  --¨ú¨ì§K¶O¶]°Óªº¶}©l®É¶¡
	local NowTime=SystemTime()  --¨ãÅé®É¶¡
	if (GetLevel()>=25) and  (floor(NowTime/86400)>floor(LastTime/86400)) and (HaveIBBuff(160)==1)then
		Talk(1,"no","Ng­¬i ®· bá nhiÖm vô cña ngµy h«m qua. Cã thÓ b¾t ®Çu nhiÖm vô h«m nay!")
		SetTask(766,0)
	else
		if (GetTask(766)>=20) then
			Talk(1,"no","Ng­¬i ®· hñy bá nhiÖm vô vËn chuyÓn lÇn nµy, nh­ng vÉn bŞ tİnh ®ñ <co lor=green>20 lÇn<color>. Mai l¹i ®Õn nhĞ!")
		elseif (GetTask(766)>=5) then
			Talk(1,"no","Ng­¬i ®· hñy bá nhiÖm vô vËn chuyÓn lÇn nµy nh­ng vÉn bŞ tİnh. NÕu cã <color=green>tói ®ùng hµng<color> th× sè lÇn vËn chuyÓn sÏ ®­îc céng thªm <color=green>"..(20-GetTask(766)).."<color>. B¹n trÎ ®õng n¶n chİ! Cè lªn nhĞ!")
		else
			Talk(1,"no","Ng­¬i ®· hñy bá nhiÖm vô vËn chuyÓn lÇn nµy nh­ng vÉn bŞ tİnh. H«m nay cßn l¹i <color=green>"..(5-GetTask(766)).."<color> lÇn. Sau khi ®ñ 5 lÇn, ng­¬i cã thÓ tÆng ta <color=green>tói ®ùng hµng<color> ®Ó tiÕp tôc nhËn vËn chuyÓn")
		end
	end
end;

trader_name=
{
"T¹p hãa Lı ","T¹p hãa Tr­¬ng ","T¹p hãa V­¬ng ","T¹p hãa Chu ","T¹p hãa Hå ","T¹p hãa TriÖu ","T¹p hãa L­u ","T¹p hãa Gi¶ ","T¹p hãa Ng« "
}

function query_task()
	if ((GetTask(333)>0) and (GetTask(333)<=9) ) then
		local LastTime=GetTask(769)  --¨ú¨ì§K¶O¶]°Óªº¶}©l®É¶¡
		local NowTime=SystemTime()  --¨ãÅé®É¶¡
		local string = "Ng­¬i ®· nhËn "
		local Num=GetTask(766)+1
		if (GetTask(333)==1) then
			string = string..trader_name[1];
		elseif (GetTask(333)==2) then
			string = string..trader_name[2];
		elseif (GetTask(333)==3) then
			string = string..trader_name[3];
		elseif (GetTask(333)==4) then
			string = string..trader_name[4];
		elseif (GetTask(333)==5) then
			string = string..trader_name[5];
		elseif (GetTask(333)==6) then
			string = string..trader_name[6];
		elseif (GetTask(333)==7) then
			string = string..trader_name[7];
		elseif (GetTask(333)==8) then
			string = string..trader_name[8];
		elseif (GetTask(333)==9) then
			string = string..trader_name[9];
		end;
		if (GetLevel()>=25) and  (floor(NowTime/86400)>floor(LastTime/86400)) and (HaveIBBuff(160)==1)then
			string = string.."cÇn <color=green>"..GetGoodsName(GetTask(334)).."<color> [<color=red>"..GetTask(335).."<color>] "..". §©y lµ nhiÖm vô h«m qua cña b¹n. B¹n cÇn ph¶i hoµn thµnh hoÆc hñy bá nhiÖm vô nµy míi cã thÓ nhËn nhiÖm vô h«m nay"..GetTask(803).." "
		else
			string = string.."cÇn <color=green>"..GetGoodsName(GetTask(334)).."<color> [<color=red>"..GetTask(335).."<color>] "..". §©y lµ nhiÖm vô thø <color=green> "..Num.."<color> h«m nay."..GetTask(803).." "
		end
		Talk(1,"no",string)
	else
		Talk(1,"no",10739)
	end;
end;

function main()
	tasks =
	{
 	{"Mua vËt phÈm","buyin";show=1},
 	{"B¸n vËt phÈm","sellout";show=1},
	{"VËn chuyÓn","RandomTask";show=0},
 	{"NhiÖm vô","query_task";show=0},
 	{"Hñy bá","cancel_task";show=0},
 	{"Tr¶ l¹c ®µ","yes_3";show=1}
	}
	local playername,guardindex
	playername=GetName()
	guardindex=GetTGuardIndexByPlayerName(playername)
	local LastTime=GetTask(765)  --¨ú¨ì§K¶O¶]°Óªº¶}©l®É¶¡
	local NowTime=SystemTime()  --¨ãÅé®É¶¡
	if (guardindex ~= 0) then
		Talk(1,"no",10740)
	elseif (GetIBBuffCount()>=16) and (HaveIBBuff(160)==0) then
		if (GetMorphType()==364) then
			local tasks8 =
				{
				{"Tr¶ l¹c ®µ","yes_4";show=1}
				}
			SayTask("Ng­¬i hiÖn kh«ng thÓ nhËn nhiÖm vô. Ng­¬i cã thÓ nhÊn nót tr¶ L¹c ®µ ®Ó xãa bá tr¹ng th¸i chuyÓn hµng!. ",tasks8)		
		else
			Talk(1,"no","Ng­¬i hiÖn kh«ng thÓ nhËn nhiÖm vô chuyÓn hµng. Sau nµy cã thÓ ®Õn t×m ta.")
		end
	elseif (GetLevel()>=20) and  (floor(NowTime/86400)>floor(LastTime/86400)) and (HaveIBBuff(160)==0)then		--Î´½ÓÅÜÉÌ?Îñ×´Ì¬£¨ÏŞ¶¨20¼¶ÒÔÉÏ²Å¿ÉÒÔÅÜÉÌ£©
		local tasks1 =
		{
		{"Th­¬ng vô","yes_1";show=0},
		{"Tr¶ l¹c ®µ","yes_4";show=0}
		}
		if ( GetTask(60)==0 ) then
			tasks1[1].show=1
			SayTask(10741,tasks1)
		else
			tasks1[2].show=1
			SayTask("Nh÷ng ai ch­a tiÕp nhËn thùc hiÖn th­¬ng vô vµ thuª l¹c ®µ trong ngµy sÏ bŞ h¹n chÕ trao ®æi hµng hãa. CÇn ph¶i trao tr¶ vµ thuª l¹i l¹c ®µ th× míi cã thÓ tiÕp tôc thùc hiÖn",tasks1)
		end
	elseif (GetLevel()>=20) and  (floor(NowTime/86400)==floor(LastTime/86400)) and (HaveIBBuff(160)==0)then
		if (HaveNormalItem(8,148,2,0)>=1) then
			if ( GetTask(60)==0 ) then
				local tasks2 =
				{
				{"Th­¬ng vô","select_camel";show=1},
				{"Tr¶ l¹c ®µ","yes_3";show=0}
				}
				SayTask("TÆng ta m«n bµi giao dŞch, ng­¬i sÏ ®­îc phĞp trao ®æi hµng hãa thªm 1 giê!",tasks2)
			else
				local tasks3 =
				{
				{"Th­¬ng vô","yes_5";show=1},
				{"Tr¶ l¹c ®µ","yes_3";show=1}
				}
				SayTask("TÆng ta m«n bµi giao dŞch, ng­¬i sÏ ®­îc phĞp trao ®æi hµng hãa thªm 1 giê!",tasks3)
			end
		else
			Talk(1,"xukezheng","HiÖn nay ®a sè L¹c ®µ ®Òu ®­îc Th¸i s­ tr­ng dông, cho nªn mçi ng­êi mçi ngµy chØ cã thÓ trao ®æi hµng hãa 1 giê.")
		end
	elseif (GetLevel()>=20)  and (HaveIBBuff(160)==1)then
		if (GetTask(60)>0) then
			if (GetLevel()>=25) then
				if (GetTask(333)>0) then
					tasks[5].show=1
					tasks[4].show=1
				end
				tasks[3].show=1
			end
			SayTask(10745,tasks)
		elseif ( GetTask(60)==0) then
			local tasks5 =
				{
				{"Th­¬ng vô","yes_1";show=1},
				{"Tr¶ l¹c ®µ","yes_3";show=0}
				}
			SayTask(10741,tasks5)
		end
	else
		MsgBox("N¨ng lùc ng­¬i ch­a ®ñ. §¹t cÊp 20 h·y ®Õn t×m ta. Ta cÇn nhiÒu ng­êi gióp chuyÓn hµng","no")
	end
end;

function xukezheng()
	local tasks4 =
	{
	{"Tr¶ l¹c ®µ","yes_3";show=0}
	}
	if ( GetTask(60)==0 ) then
	else
		tasks4[1].show=1
	end
	SayTask("NÕu cã <color=red>1 M«n bµi giao dŞch<color> th× mçi ngµy ng­¬i sÏ ®­îc céng thªm 1 giê trao ®æi hµng hãa",tasks4)
end;

function yes_1()
    MsgBox(10746,"select_camel","no")
end;


function yes_5()
	local LastTime=GetTask(765)
	local NowTime=SystemTime()
	if (GetLevel()>=20) and  (floor(NowTime/86400)==floor(LastTime/86400)) and (HaveIBBuff(160)==0) and (HaveNormalItem(8,148,2,0)>=1) then
		local NowTime=SystemTime()
		SetTask(765,NowTime)
		SetTask(60,3)
		AddIBBuff(160, 7200)
		CostIBItem(FindAValidIBItem(8,148,2,0))
		MsgBox("§· khÊu trõ cña ng­¬i 1 M«n bµi giao dŞch, ng­¬i cã thÓ tiÕp tôc trao ®æi hµng hãa. H·y nhanh lªn! ChØ cã 1 giê th«i ®Êy!","no")
	else
		MsgBox("H«m nay ng­¬i ®· trao ®æi hµng hãa ®­îc 1 giê. Muèn tiÕp tôc cÇn ph¶i cã M«n bµi Giao dŞch","no")
	end
end

function select_camel()
	Talk(1,"select_camel_next","<color=green>ThÇn hµnh L¹c ®µ<color> lµ lo¹i tèt nhÊt võa mang tõ T©y Vùc vÒ, <color=green>tèc ®é nhanh h¬n<color> so víi L¹c ®µ th­êng, mçi lÇn thuª cÇn <color=green>1 Phi Mao phï<color>.")
end

function select_camel_next()
	local level = GetLevel();
		if (level < 25 ) then
			Say("Xin chän lo¹i L¹c ®µ muèn thuª:",3,"L¹c ®µ cÊp 1 ( tèc ®é 15, søc lùc 100, tiÒn thuª 1000)/camel_level1","L¹c ®µ cÊp 1 ( tèc ®é 20, søc lùc100, tiÒn thuª 1000)/camel_level6","Hñy bá/no")
		elseif (level < 35) then
			Say("Xin chän lo¹i L¹c ®µ muèn thuª:",5,"L¹c ®µ cÊp 1 ( tèc ®é 15, søc lùc 100, tiÒn thuª 1000)/camel_level1","L¹c ®µ cÊp 1 ( tèc ®é 20, søc lùc100, tiÒn thuª 1000)/camel_level6","L¹c ®µ cÊp 2 ( tèc ®é 15, søc lùc 125, tiÒn thuª 2000)/camel_level2","L¹c ®µ cÊp 2 ( tèc ®é 20, søc lùc 125, tiÒn thuª 2000)/camel_level7","Hñy bá/no")
		elseif (level < 45) then
			Say("Xin chän lo¹i L¹c ®µ muèn thuª:",7,"L¹c ®µ cÊp 1 ( tèc ®é 15, søc lùc 100, tiÒn thuª 1000)/camel_level1","L¹c ®µ cÊp 1 ( tèc ®é 20, søc lùc100, tiÒn thuª 1000)/camel_level6","L¹c ®µ cÊp 2 ( tèc ®é 15, søc lùc 125, tiÒn thuª 2000)/camel_level2","L¹c ®µ cÊp 2 ( tèc ®é 20, søc lùc 125, tiÒn thuª 2000)/camel_level7","L¹c ®µ cÊp 3 ( tèc ®é 15, søc lùc 150, tiÒn thuª 5000)/camel_level3","L¹c ®µ cÊp 3 ( tèc ®é 20, søc lùc 150, tiÒn thuª 5000)/camel_level8","Hñy bá/no")
		elseif (level < 55) then
			Say("Xin chän lo¹i L¹c ®µ muèn thuª:",9,"L¹c ®µ cÊp 1 ( tèc ®é 15, søc lùc 100, tiÒn thuª 1000)/camel_level1","L¹c ®µ cÊp 1 ( tèc ®é 20, søc lùc100, tiÒn thuª 1000)/camel_level6","L¹c ®µ cÊp 2 ( tèc ®é 15, søc lùc 125, tiÒn thuª 2000)/camel_level2","L¹c ®µ cÊp 2 ( tèc ®é 20, søc lùc 125, tiÒn thuª 2000)/camel_level7","L¹c ®µ cÊp 3 ( tèc ®é 15, søc lùc 150, tiÒn thuª 5000)/camel_level3","L¹c ®µ cÊp 3 ( tèc ®é 20, søc lùc 150, tiÒn thuª 5000)/camel_level8","L¹c ®µ cÊp 4 ( tèc ®é 15, søc lùc 175, tiÒn thuª 20000)/camel_level4","L¹c ®µ cÊp 4 ( tèc ®é 20, søc lùc 175, tiÒn thuª 20000)/camel_level9","Hñy bá/no")
		else
			Say("Xin chän lo¹i L¹c ®µ muèn thuª:",11,"L¹c ®µ cÊp 1 ( tèc ®é 15, søc lùc 100, tiÒn thuª 1000)/camel_level1","L¹c ®µ cÊp 1 ( tèc ®é 20, søc lùc100, tiÒn thuª 1000)/camel_level6","L¹c ®µ cÊp 2 ( tèc ®é 15, søc lùc 125, tiÒn thuª 2000)/camel_level2","L¹c ®µ cÊp 2 ( tèc ®é 20, søc lùc 125, tiÒn thuª 2000)/camel_level7","L¹c ®µ cÊp 3 ( tèc ®é 15, søc lùc 150, tiÒn thuª 5000)/camel_level3","L¹c ®µ cÊp 3 ( tèc ®é 20, søc lùc 150, tiÒn thuª 5000)/camel_level8","L¹c ®µ cÊp 4 ( tèc ®é 15, søc lùc 175, tiÒn thuª 20000)/camel_level4","L¹c ®µ cÊp 4 ( tèc ®é 20, søc lùc 175, tiÒn thuª 20000)/camel_level9","L¹c ®µ cÊp 5 ( tèc ®é 15, søc lùc 200, tiÒn thuª 10w)/camel_level5","L¹c ®µ cÊp 5 ( tèc ®é 20, søc lùc 200, tiÒn thuª 10w)/camel_level10","Hñy bá/no")
		end;
end;


function camel_level1()
	yes_2(1)
end;

function camel_level2()
	yes_2(2)
end;

function camel_level3()
	yes_2(3)
end;

function camel_level4()
	yes_2(4)
end;

function camel_level5()
	yes_2(5)
end;

function camel_level6()
	if (HaveNormalItem(8,136,2,0)>=1) then
		yes_2(6)
	else
		MsgBox("<color=green>ThÇn hµnh L¹c ®µ<color> lµ lo¹i tèt nhÊt võa mang tõ T©y Vùc vÒ, <color=green>tèc ®é nhanh h¬n<color> so víi L¹c ®µ th­êng, mçi lÇn thuª cÇn <color=green>1 Phi Mao phï<color>. Cã <color=green>Phi Mao phï<color> ta sÏ giao L¹c ®µ cho ng­¬i.","no")
	end
end;

function camel_level7()
	if (HaveNormalItem(8,136,2,0)>=1) then
		yes_2(7)
	else
		MsgBox("<color=green>ThÇn hµnh L¹c ®µ<color> lµ lo¹i tèt nhÊt võa mang tõ T©y Vùc vÒ, <color=green>tèc ®é nhanh h¬n<color> so víi L¹c ®µ th­êng, mçi lÇn thuª cÇn <color=green>1 Phi Mao phï<color>. Cã <color=green>Phi Mao phï<color> ta sÏ giao L¹c ®µ cho ng­¬i.","no")
	end
end;

function camel_level8()
	if (HaveNormalItem(8,136,2,0)>=1) then
		yes_2(8)
	else
		MsgBox("<color=green>ThÇn hµnh L¹c ®µ<color> lµ lo¹i tèt nhÊt võa mang tõ T©y Vùc vÒ, <color=green>tèc ®é nhanh h¬n<color> so víi L¹c ®µ th­êng, mçi lÇn thuª cÇn <color=green>1 Phi Mao phï<color>. Cã <color=green>Phi Mao phï<color> ta sÏ giao L¹c ®µ cho ng­¬i.","no")
	end
end;

function camel_level9()
	if (HaveNormalItem(8,136,2,0)>=1) then
		yes_2(9)
	else
		MsgBox("<color=green>ThÇn hµnh L¹c ®µ<color> lµ lo¹i tèt nhÊt võa mang tõ T©y Vùc vÒ, <color=green>tèc ®é nhanh h¬n<color> so víi L¹c ®µ th­êng, mçi lÇn thuª cÇn <color=green>1 Phi Mao phï<color>. Cã <color=green>Phi Mao phï<color> ta sÏ giao L¹c ®µ cho ng­¬i.","no")
	end
end;

function camel_level10()
	if (HaveNormalItem(8,136,2,0)>=1) then
		yes_2(10)
	else
		MsgBox("<color=green>ThÇn hµnh L¹c ®µ<color> lµ lo¹i tèt nhÊt võa mang tõ T©y Vùc vÒ, <color=green>tèc ®é nhanh h¬n<color> so víi L¹c ®µ th­êng, mçi lÇn thuª cÇn <color=green>1 Phi Mao phï<color>. Cã <color=green>Phi Mao phï<color> ta sÏ giao L¹c ®µ cho ng­¬i.","no")
	end
end;

function yes_2(camel_level)
	CloseDialog()
	local cash_req = 0;
	if (camel_level == 1) or (camel_level==6) then
		cash_req = 1000
	elseif (camel_level == 2) or (camel_level==7)  then
		cash_req = 2000
	elseif (camel_level == 3) or (camel_level==8)  then
		cash_req = 5000
	elseif (camel_level == 4) or (camel_level==9)  then
		cash_req = 20000
	elseif (camel_level == 5) or (camel_level==10)  then
		cash_req = 100000
	end;
	local LastTime=GetTask(765)
	local NowTime=SystemTime()
	if (camel_level<=5) then
		if (GetCash()>=cash_req) then
			local  skill,type = GetCreatureInfo()
			if(type>=0)then
				SetCreatureType(skill,type)
			end;
			Talk(1,"no",10733)
			PolyMorph(364,0,1,15,0)                                 --±äÉíÎªÂæÍÕ
			SetDeathScript("\\script\\item\\death.lua")
			SetTask(120,0)
			Pay(cash_req)
			SetTask(60,3)
			SetTask(332,camel_level)
			if (HaveIBBuff(160)==0)then
				if  (floor(NowTime/86400)>floor(LastTime/86400))  then
					AddIBBuff(160)
					SetTask(765,NowTime)
				elseif  (floor(NowTime/86400)==floor(LastTime/86400)) and (HaveNormalItem(8,148,2,0)>=1)then
					AddIBBuff(160)
					SetTask(765,NowTime)
					CostIBItem(FindAValidIBItem(8,148,2,0))
				end
			end
		else
			Talk(1,"no",10734)
		end;
	elseif (camel_level>5) and (camel_level<=10) and (HaveNormalItem(8,136,2,0)>=1) then
		if (GetCash()>=cash_req) then
			camel_level=camel_level-5
			local  skill,type = GetCreatureInfo()
			if(type>=0)then
				SetCreatureType(skill,type)
			end;
			Talk(1,"no",10733)
			PolyMorph(364,0,1,20,0)                                 --±äÉíÎªÂæÍÕ
			SetDeathScript("\\script\\item\\death.lua")
			SetTask(120,0)
			Pay(cash_req)
			CostIBItem(FindAValidIBItem(8,136,2,0))
			SetTask(60,3)
			SetTask(332,camel_level)
			if (HaveIBBuff(160)==0)then
				if  (floor(NowTime/86400)>floor(LastTime/86400))  then
					AddIBBuff(160)
					SetTask(765,NowTime)
				elseif  (floor(NowTime/86400)==floor(LastTime/86400)) and (HaveNormalItem(8,148,2,0)>=1)then
					AddIBBuff(160)
					SetTask(765,NowTime)
					CostIBItem(FindAValidIBItem(8,148,2,0))
				end
			end
		else
			Talk(1,"no",10734)
		end;
	end
end;

function yes_3()                                                 --?Ïû±äÉí×´Ì¬(ÅĞ¶¨Íæ¼ÒÉÌ¢»ÊÇ·ñÎª¿Õ£©
		MsgBox("VËt phÈm vµ nhiÖm vô ®· hÕt hiÖu lùc, khi nµo cÇn l¹c ®µ h·y ®Õn t×m ta.","yes_4","no")
end;

function yes_4()
	Talk(1,"no","Khi nµo cÇn L¹c ®µ quay l¹i t×m ta.")
	PolyMorph(-1,1,0,6,0)
	SetTask(60,0)
	SetTask(120,0)
	SetTask(110,0)
	SetTask(111,0)
	SetTask(112,0)
	SetTask(113,0)
	SetTask(114,0)
	SetTask(115,0)
	SetTask(116,0)
	SetTask(117,0)
	SetTask(118,0)
	SetTask(119,0)
	--Çå³ıËæ»ú?Îñ
	Msg2Player("B¹n hoµn tr¶ l¹c ®µ")
end;

function fuzhong(camel_level)
	if (camel_level == 1) then
		return 100
	elseif (camel_level == 2) then
		return 125
    elseif (camel_level == 3) then
		return 150
	elseif (camel_level == 4) then
		return 175
	elseif (camel_level == 5) then
		return 200
	end;
end;

----------------------------------------------------------------------------------------------------------ºjÉÌ¢»
--------------------------------------------------------------------ºj½øÉÌ¢»ÁĞ±í
function buyin()
	name1=GetGoodsName(7)
    name2=GetGoodsName(8)
    name3=GetGoodsName(9)
    weight1=GetGoodsWeight(7)
    weight2=GetGoodsWeight(8)
    weight3=GetGoodsWeight(9)
    local weight=fuzhong(GetTask(332))-GetTask(120)
    local cash=GetCash()
    local price1=GetGoodsPurchasePrice(3,7)
	local price2=GetGoodsPurchasePrice(3,8)
    local price3=GetGoodsPurchasePrice(3,9)
    local num1=GetGoodsNum(3,7)
    local num2=GetGoodsNum(3,8)
    local num3=GetGoodsNum(3,9)
	Say("Sè tiÒn hiÖn cã "..cash.."Søc nÆng cßn l¹i "..weight,4,name1.."Gi¸ "..price1.."Hµng tån: "..num1.."Träng l­îng: "..weight1.."/buyin1",name2.."Gi¸ "..price2.."Hµng tån: "..num2.."Träng l­îng: "..weight2.."/buyin2",name3.."Gi¸ "..price3.."Hµng tån: "..num3.."Träng l­îng: "..weight3.."/buyin3","sè l­îng b¸n ra/sellout")
end;

function buyin1()
    r = goodsfull(7)                         ----------------------------ÂæÍÕÊÇ·ñÂúµÄÅĞ¶Ï
	if (r==0) then
	    buyin()
		return
	end;
    InputDialog("Xin nhËp sè l­îng cÇn mua.",2,"X¸c nhËn/inputok1","Hñy bá/no_buy")
end;

function buyin2()
    r = goodsfull(8)
	if (r==0) then
	    buyin()
		return
	end;
    InputDialog("Xin nhËp sè l­îng cÇn mua.",2,"X¸c nhËn/inputok2","Hñy bá/no_buy")
end;

function buyin3()
    r = goodsfull(9)
	if (r==0) then
	    buyin()
		return
	end;
    InputDialog("Xin nhËp sè l­îng cÇn mua.",2,"X¸c nhËn/inputok3","Hñy bá/no_buy")
end;

------------------ÅĞ¶Ï»õÎïÀ¸ÊÇ·ñÒÑÂú
function goodsfull(goodsindex)
	local var,Pgoods
	for i=0,10,2 do
	     Pgoods=GetTask(110+i)
		 		var=i
		 if (Pgoods==0) or (Pgoods==goodsindex) then
			  break
		 end;
	end;
	if (var==10) then
		 Msg2Player("L¹c ®µ chë tèi ®a ®­îc 5 lo¹i th­¬ng phÈm")
		 TopMessage("<color=red>L¹c ®µ chØ cã thÓ chë tèi ®a 5 lo¹i th­¬ng phÈm <color>")
	     return 0
	end;
	return 1
end;

function inputok1(buynum)
    dobuy(7,buynum)
end;

function inputok2(buynum)
    dobuy(8,buynum)
end;

function inputok3(buynum)
    dobuy(9,buynum)
end;

----------------¿ªÊ¼ºj?»õÎï
function dobuy(goodsindex,buynum)
	local var,Pgoods,Pgoodstaskindex
	for i=0,10,2 do
		 if ( i == 10) then
		 	  var = 10
			  break
		 end;
		 Pgoods=GetTask(110+i)
		 if (Pgoods==goodsindex)  then
		      Pgoodstaskindex=110+i
		 	  var = i
			  break
	     end;
    end;
    if (var ~= 10) then
	     buyover(Pgoodstaskindex,goodsindex,buynum)
		 buyin()
		 return
	end;
    for i=0,10,2 do
		 Pgoods=GetTask(110+i)
		 if (Pgoods==0) then
		      Pgoodstaskindex=110+i
              var=i
			  break
         end;
	end;
	if (var~=10) then
         buyover(Pgoodstaskindex,goodsindex,buynum)
		 buyin()
	end;
end;

function buyover(Pgoodstaskindex,goodsindex,buynum)
    local num=GetGoodsNum(3,goodsindex)
    if (num==0) then
	     Msg2Player("VËt phÈm nµy ®· b¸n hÕt")
	     TopMessage("<color=red>Lo¹i nµy ®· hÕt<color>")
		 return
	end;
	if (buynum>999) or (buynum<=0) then
         Msg2Player("NhËp sè liÖu kh«ng ®óng")
	 TopMessage("<color=red>Sè b¹n nhËp kh«ng cã t¸c dông<color>")
		 return
	end;
    if (buynum>num)then
         Msg2Player("TiÖm T¹p hãa kh«ng ®ñ hµng")
	 TopMessage("<color=red>Kh«ng ®ñ hµng<color>")
	     return
	end;
    local cash=GetCash()
    local price=GetGoodsPurchasePrice(3,goodsindex)
	local name=GetGoodsName(goodsindex)
	local Pgoodsnumindex,Pnum
    Pgoodsnumindex=Pgoodstaskindex+1
	Pnum=GetTask(Pgoodsnumindex)
	payment=buynum*price
	if (payment>cash)then
         Msg2Player("Quı kh¸ch kh«ng ®ñ tiÒn")
	 TopMessage("<color=red>B¹n kh«ng ®ñ tiÒn<color>")
	     return
	end;

	local cur_weight = buynum * GetGoodsWeight(goodsindex)

	cur_weight = cur_weight + GetTask(120)
	if (cur_weight > fuzhong(GetTask(332))) then
		Msg2Player("L¹c ®µ chë kh«ng næi")
		TopMessage("<color=red>Hµng qu¸ nÆng<color>")
		return
	end;
	SetTask(120, cur_weight)	--ÉèÂ÷µ±Ç°ÂæÍÕ¸ºÖØ

	Pay(payment)
	local  thispay=GetTask(132)
	SetTask(132,thispay+payment)
	Msg2Player("B¹n bá ra  "..payment.."Mua: "..name..buynum..". ")
	Pnum=Pnum+buynum
	buynum=-buynum
	SetTask(Pgoodsnumindex,Pnum)
	SetTask(Pgoodstaskindex,goodsindex)
	ChangeGoodsNum(3,goodsindex,buynum)
	-- ĞèÒªµÄ¸ºÖØ£¨?§O¸ºÖØ²»¹»£¬¼ÓÉÏ¸ºÖØ²»¹»µÄÅĞ¶¨£©
end;

function no_buy()
	buyin()
end;

---------------------------------------------------------------------------------------------------------------Âô³öÉÌ¢»
----------------------------------------------------------------------Âô³öÉÌ¢»ÁĞ±í
function sellout()
	local Pname1,Pprice1,Pnum1,Pweight1=goodslist(110)
	local Pname2,Pprice2,Pnum2,Pweight2=goodslist(112)
	local Pname3,Pprice3,Pnum3,Pweight3=goodslist(114)
	local Pname4,Pprice4,Pnum4,Pweight4=goodslist(116)
	local Pname5,Pprice5,Pnum5,Pweight5=goodslist(118)
	local cash=GetCash()
	local weight=fuzhong(GetTask(332)) - GetTask(120)
    Say("Sè tiÒn hiÖn cã "..cash.."Søc nÆng cßn l¹i "..weight,6,Pname1.."Gi¸: "..Pprice1.." Sè l­îng: "..Pnum1.."Träng l­îng: "..Pweight1.."/sellout1",Pname2.."Gi¸: "..Pprice2.." Sè l­îng: "..Pnum2.."Träng l­îng: "..Pweight2.."/sellout2",Pname3.."Gi¸: "..Pprice3.." Sè l­îng: "..Pnum3.."Träng l­îng: "..Pweight3.."/sellout3",Pname4.."Gi¸: "..Pprice4.." Sè l­îng: "..Pnum4.."Träng l­îng: "..Pweight4.."/sellout4",Pname5.."Gi¸: "..Pprice5.." Sè l­îng: "..Pnum5.."Träng l­îng: "..Pweight5.."/sellout5","Mua hµng/buyin")
end;

function goodslist(Pgoodstaskindex)
	local Pgoods=GetTask(Pgoodstaskindex)
	local Pgoodsnumindex=Pgoodstaskindex+1
	local Pname,Pprice,Pnum,Pweight
	if (Pgoods==0)then
	     Pname="---------"
         Pprice=0
		 Pnum=0
		 Pweight=0
	else
	     Pname=GetGoodsName(Pgoods)
	     Pprice=GetGoodsSalePrice(3,Pgoods)
	     Pnum=GetTask(Pgoodsnumindex)
	     Pweight=GetGoodsWeight(Pgoods)
	end;
    return Pname,Pprice,Pnum,Pweight
end;

function sellout1()
	local Pgoods=GetTask(110)
	if (Pgoods==0) then
         sellout()
		 return
	end;
	InputDialog("Xin nhËp sè l­îng cÇn b¸n.",2,"X¸c nhËn/output1","Hñy bá/no_sell")
end;

function sellout2()
	local Pgoods=GetTask(112)
	if (Pgoods==0) then
         sellout()
		 return
	end;
	InputDialog("Xin nhËp sè l­îng cÇn b¸n.",2,"X¸c nhËn/output2","Hñy bá/no_sell")
end;

function sellout3()
	local Pgoods=GetTask(114)
	if (Pgoods==0) then
         sellout()
		 return
	end;
	InputDialog("Xin nhËp sè l­îng cÇn b¸n.",2,"X¸c nhËn/output3","Hñy bá/no_sell")
end;

function sellout4()
	local Pgoods=GetTask(116)
	if (Pgoods==0) then
         sellout()
		 return
	end;
	InputDialog("Xin nhËp sè l­îng cÇn b¸n.",2,"X¸c nhËn/output4","Hñy bá/no_sell")
end;

function sellout5()
	local Pgoods=GetTask(118)
	if (Pgoods==0) then
         sellout()
		 return
	end;
	InputDialog("Xin nhËp sè l­îng cÇn b¸n.",2,"X¸c nhËn/output5","Hñy bá/no_sell")
end;

function output1(sellnum)
    dosell(110,sellnum)
end;

function output2(sellnum)
    dosell(112,sellnum)
end;

function output3(sellnum)
    dosell(114,sellnum)
end;

function output4(sellnum)
    dosell(116,sellnum)
end;

function output5(sellnum)
    dosell(118,sellnum)
end;

function dosell(Pgoodstaskindex,sellnum)
	if (sellnum>1000) or (sellnum<=0) then
         Msg2Player("NhËp sè liÖu kh«ng ®óng")
	 TopMessage("<color=red>Sè b¹n nhËp kh«ng cã t¸c dông<color>")
		 sellout()
		 return
	end;
	local Pgoods=GetTask(Pgoodstaskindex)
	local Pgoodsnumindex=Pgoodstaskindex+1
	local Pnum=GetTask(Pgoodsnumindex)
	local Pname=GetGoodsName(Pgoods)
    local Pprice=GetGoodsSalePrice(3,Pgoods)
	if (sellnum>Pnum) then
	     Msg2Player("B¹n kh«ng ®ñ vËt phÈm")
	     TopMessage("<color=red>B¹n kh«ng ®ñ hµng<color>")
		 sellout()
		 return
	end;
	local getmoney=sellnum*Pprice
	--ÓĞ¸ºÖØµÄÊ±ºòÒª¼õµô?Îï¸ºÖØ
	local cur_weight = sellnum * GetGoodsWeight(Pgoods)
	cur_weight = GetTask(120) - cur_weight
	if (cur_weight < 0) then
		cur_weight = 0
	end;
	SetTask(120, cur_weight)
	Earn(getmoney)
	local  thisearn=GetTask(131)
	SetTask(131,thisearn+getmoney)
	Pnum=Pnum-sellnum
	if (Pnum==0) then
	     SetTask(Pgoodstaskindex,0)
	end;
	SetTask(Pgoodsnumindex,Pnum)
	Msg2Player("b¸n "..Pname..sellnum.."thu ®­îc: "..getmoney)
	PlayerSellGoods(3,Pgoods,sellnum)
	sellout()
end;

function no_sell()
    sellout()
end;

function no()
		CloseDialog()
end;
Include("\\script\\phongthan\\daily\\reward.lua")

pt_daily_original_trade=complete_task
function complete_task()
 if GetTask(333)<=0 or GetTask(334)<=0 or GetTask(335)<=0 or GetLevel()<25 then return end
 local old=GetTask(803)
 pt_daily_original_trade()
 if GetTask(803)==old+1 and GetTask(333)==0 then PTDailyHope(1,PTDailyNextReceipt(1)) end
end

--description: Î÷Æç¡ª¡ªÓ¦Ç©¹ÜÀíÔ±
--author: chensong
--date: 2004/7/14

function main()
	--UTask_cp_0 = GetTask(70);
		Say(11109,2,"Mua 1 l¸ th¨m/yes_1","Mua 5 l¸ th¨m/yes_5")
end;

function yes_1()
	local  thisnumber=GetTask(136)
	SetTask(136,thisnumber+1)
	if (GetLevel()>=5)then
		if (GetCash()>=1000) then
			Talk(1,"no",11110)
			Pay(1000)
			AddNormalItem(6,1,12,1,0,0,0)
		else
			Talk(1,"no",11111)
		end;
	else
		Talk(1,"no",11112)
	end;
end;

function yes_5()
	local  thisnumber=GetTask(136)
	SetTask(136,thisnumber+5)
	if (GetLevel()>=5)then
		if (GetCash()>=5000) then
			Talk(1,"no",11113)
			Pay(5000)
			AddNormalItem(6,1,12,1,0,0,0)
			AddNormalItem(6,1,12,1,0,0,0)
			AddNormalItem(6,1,12,1,0,0,0)
			AddNormalItem(6,1,12,1,0,0,0)
			AddNormalItem(6,1,12,1,0,0,0)
		else
			Talk(1,"no",11111)
		end;
	else
		Talk(1,"no",11114)
	end;
end;

function no()
		CloseDialog()
end;

function main()
 Say("øng Tiªm Th­¬ng: mua Th¨m May M¾n, 1000 l­îng/l¸. Th¨m cã thÓ nhËn tiÒn, Thæ Linh Phï hoÆc Thñy Linh Phï.",3,
 "Mua 1 l¸: 1000 l­îng/yes_1","Mua 5 l¸: 5000 l­îng/yes_5","KÕt thóc/no")
end
function pt_buy(count)
 if EventVttBuyLottery(count)==1 then Msg2Player("§· mua Th¨m May M¾n. NhÊn chuét ph¶i vµo th¨m trong F4 ®Ó më.")
 else Msg2Player("Ch­a mua ®­îc. CÇn cÊp 5, ®ñ tiÒn, ®ñ chç trong F4 vµ hµnh trang kh«ng khãa.") end
end
function yes_1() pt_buy(1) end
function yes_5() pt_buy(5) end

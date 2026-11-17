--description: ¼¼ÄÜ½ÌÊ¦
--author: yichuan
--date: 2004/5/12

function main(sel)
	if(songxin()==0)then
		local tasks = 
		{
			{"S¸ch","yes_2";show=1},
			{"§ãng s¸ch","books";show=1}
		}
		SayTask(10069,tasks)
	end
end;

function songxin()
	local task_id = 872
	local map_id = 4

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
		Sale(14)
end;

function books()
	CloseDialog()
	if (GetPlayerType()==0) then
	local jiashibooks=
		{
			"Hoµnh Kh«ng Tr¶m/jiashi",
			"Tinh th«ng ®o¶n ®ao/jiashi",
			"Tinh th«ng tr­êng ®ao/jiashi",
			"Tam §Çu Lôc Thñ/jiashi",
			"HuyÒn B¨ng Tr¶m/jiashi",
			"Háa Quang Tr¶m/jiashi",
			"Liªn Hoµn Tr¶m/jiashi",
			"L¹c §Þa Tr¶m/jiashi",
			"ThuÇn D­¬ng Hé ThÓ/jiashi",
			"Thiªn Qu©n Tr¶m/jiashi",
			"Khuynh Thµnh NhÊt KÝch/jiashi",
			"Bµn Cæ Khai Thiªn/jiashi",
			"Ban M«n Léng Phñ/jiashi"
		}
		Say("NÕu cã 4 trang b¹ch kh«ng th­ ta sÏ gióp ng­¬i hãa phÐp thµnh kü n¨ng MËt tÞch, nh­ng còng kh«ng ch¾c ch¾n l¾m.",getn(jiashibooks),jiashibooks)
	elseif (GetPlayerType()==1) then
	local daoshibooks=
		{
			"Tinh Th«ng Thæ HÖ/daoshi",
			"ThiÕt M· B¨ng Qua/daoshi",
			"Tinh Th«ng Háa HÖ/daoshi",
			"Phong V©n L«i §éng/daoshi",
			"Ngò Nh¹c TriÒu T«ng/daoshi",
			"Tinh Th«ng B¨ng HÖ/daoshi",
			"ThËp Ph­¬ng LiÖt Háa/daoshi",
			"L«i Phong Gi¸p/daoshi",
			"Thiªn B¨ng §Þa LiÖt/daoshi",
			"B¨ng Phong B¹o/daoshi",
			"Chóc Dung Ch©n KhÝ/daoshi",
			"L«i §éng Cöu Thiªn/daoshi",
			"HuyÒn N÷ Bæ Thiªn/daoshi",
			"B¨ng Phong V¹n Lý/daoshi",
			"Tam Muéi Ch©n Háa/daoshi",
			"Bµn Cæ Khai Thiªn/daoshi",
			"Ban M«n Léng Phñ/daoshi"
		}
		Say("NÕu cã 4 trang b¹ch kh«ng th­ ta sÏ gióp ng­¬i hãa phÐp thµnh kü n¨ng MËt tÞch, nh­ng còng kh«ng ch¾c ch¾n l¾m.",getn(daoshibooks),daoshibooks)
	elseif (GetPlayerType()==2) then
	local yirenbooks=
		{
			"C­êng C«ng Chó/yiren",
			"Ph¸ Gi¸p Chó/yiren",
			"Bå §Ò Chó/yiren",
			"Tr¶m T©m Chó/yiren",
			"TËt Phong Chó/yiren",
			"V¹n Cèt Toµn Kh«/yiren",
			"Liªn Nç tÕ/yiren",
			"Háa L«i tÕ/yiren",
			"To¸i Cèt tÕ/yiren",
			"L­u Tinh tÕ/yiren",
			"Truy Hån tÕ/yiren",
			"Phong QuyÓn Tµn V©n/yiren",
			"Bµn Cæ Khai Thiªn/yiren",
			"Ban M«n Léng Phñ/yiren"
		}
		Say("NÕu cã 4 trang b¹ch kh«ng th­ ta sÏ gióp ng­¬i hãa phÐp thµnh kü n¨ng MËt tÞch, nh­ng còng kh«ng ch¾c ch¾n l¾m.",getn(yirenbooks),yirenbooks)
	else
		MsgBox("Häc tËp ph¶i kiªn nhÉn míi ®­îc, sau nµy l¹i ®Õn nhÐ.","no")
	end
end;


function jiashi(JNum)
	CloseDialog()
	if (JNum==0) then
		MsgBox("<color=green>MËt tÞch Hoµnh Kh«ng Tr¶m<color> cÇn <color=green> 1 trang b¹ch kh«ng th­ <color> vµ <color=green>1 m¶nh Hång thñy tinh <color>, ng­¬i cã ®ñ ch­a?","JCheck0","no")
	elseif (JNum==1) then
		MsgBox("<color=green>MËt tÞch Tinh th«ng ®o¶n ®ao<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck1","no")
	elseif (JNum==2) then
		MsgBox("<color=green>MËt tÞch Tinh th«ng tr­êng ®ao<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck2","no")
	elseif (JNum==3) then
		MsgBox("<color=green>MËt tÞch Tam §Çu Lôc Thñ<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck3","no")
	elseif (JNum==4) then
		MsgBox("<color=green>MËt tÞch HuyÒn B¨ng Tr¶m<color> cÇn <color=green> 1 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck4","no")
	elseif (JNum==5) then
		MsgBox("<color=green>MËt tÞch Háa Quang Tr¶m<color> cÇn <color=green> 1 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck5","no")
	elseif (JNum==6) then
		MsgBox("<color=green>MËt tÞch Liªn Hoµn Tr¶m<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck6","no")
	elseif (JNum==7) then
		MsgBox("<color=green>MËt tÞch L¹c §Þa Tr¶m<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","JCheck7","no")
	elseif (JNum==8) then
		MsgBox("<color=green>MËt tÞch ThuÇn D­¬ng Hé ThÓ<color> cÇn <color=green> 1 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","JCheck8","no")
	elseif (JNum==9) then
		MsgBox("<color=green>MËt tÞch Thiªn Qu©n Tr¶m<color> cÇn <color=green> 2 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","JCheck9","no")
	elseif (JNum==10) then
		MsgBox("<color=green>MËt tÞch Khuynh Thµnh NhÊt KÝch<color> cÇn <color=green> 1 trang hoµng kh«ng th­<color> vµ <color=green>3 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","JCheck10","no")
	elseif (JNum==11) then
		MsgBox("<color=green>MËt tÞch Bµn Cæ Khai Thiªn<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color>, ng­¬i cã ®ñ ch­a?","PTCheck0","no")
	elseif (JNum==12) then
		MsgBox("<color=green>MËt tÞch Ban M«n Léng Phñ<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color>, ng­¬i cã ®ñ ch­a?","PTCheck1","no")
	end
end;

function JCheck0()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=1) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch Hoµnh Kh«ng Tr¶m ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Hoµnh Kh«ng Tr¶m ®· xuÊt hiÖn.")
			AddNormalItem(7,29,32,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>MËt tÞch Hoµnh Kh«ng Tr¶m<color>, cÇn 1 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh. ","no")
	end
end;


function JCheck1()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tinh th«ng ®o¶n ®ao ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Tinh th«ng ®o¶n ®ao ®· xuÊt hiÖn.")
			AddNormalItem(7,30,33,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>MËt tÞch Tinh th«ng ®o¶n ®ao<color>, cÇn 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh .","no")
	end
end;	
	
function JCheck2()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tinh th«ng tr­êng ®ao ®· xuÊt hiÖn.")
			TopMessage("MËt tÞch Tinh th«ng tr­êng ®ao ®· xuÊt hiÖn.")
			AddNormalItem(7,31,34,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>MËt tÞch Tinh th«ng tr­êng ®ao<color>, cÇn 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh. ","no")
	end
end;		

function JCheck3()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,100)
		if (i<=5) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tam §Çu Lôc Thñ ®· xuÊt hiÖn.")
			TopMessage("MËt tÞch Tam §Çu Lôc Thñ ®· xuÊt hiÖn")
			AddNormalItem(7,32,35,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>MËt tÞch Tam §Çu Lôc Thñ<color>, cÇn 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh. ","no")
	end
end;

function JCheck4()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=1) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch HuyÒn B¨ng Tr¶m ®· xuÊt hiÖn")
			TopMessage("MËt tÞch HuyÒn B¨ng Tr¶m ®· xuÊt hiÖn.")
			AddNormalItem(7,33,36,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>HuyÒn B¨ng Tr¶m<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;
		
function JCheck5()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=1) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch Háa Quang Tr¶m ®· xuÊt hiÖn.")
			TopMessage("MËt tÞch Háa Quang Tr¶m ®· xuÊt hiÖn.")
			AddNormalItem(7,34,37,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Háa Quang Tr¶m<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;

function JCheck6()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch Liªn Hoµn Tr¶m ®· xuÊt hiÖn.")
			TopMessage("MËt tÞch Liªn Hoµn Tr¶m ®· xuÊt hiÖn.")
			AddNormalItem(7,35,38,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Liªn Hoµn Tr¶m<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;

function JCheck7()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch L¹c §Þa Tr¶m ®· xuÊt hiÖn.")
			TopMessage("MËt tÞch L¹c §Þa Tr¶m ®· xuÊt hiÖn.")
			AddNormalItem(7,36,39,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>L¹c §Þa Tr¶m<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;

function JCheck8()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch ThuÇn D­¬ng Hé ThÓ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch ThuÇn D­¬ng Hé ThÓ ®· xuÊt hiÖn.")
			AddNormalItem(7,37,40,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>ThuÇn D­¬ng Hé ThÓ<color>, cÇn 1 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch.","no")
	end
end;

function JCheck9()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=2) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch Thiªn Qu©n Tr¶m ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Thiªn Qu©n Tr¶m ®· xuÊt hiÖn.")
			AddNormalItem(7,38,41,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Thiªn Qu©n Tr¶m<color>, cÇn 2 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch","no")
	end
end;

function JCheck10()
	CloseDialog()
	if (HaveNormalItem(8,142,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=3) then
		CostIBItem(FindAValidIBItem(8,142,2,0))
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=1) then
			Talk(1,"no","Vui qu¸! MËt tÞch Khuynh Thµnh NhÊt KÝch ®· xuÊt hiÖn.")
			TopMessage("MËt tÞch Khuynh Thµnh NhÊt KÝch ®· xuÊt hiÖn.")
			AddNormalItem(7,39,42,1,0,0)
		else
			local j=random(1,2)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï cao cÊp.")
				TopMessage("Di ngo¹i phï cao cÊp ®· xuÊt hiÖn.")
				AddNormalItem(8,159,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phong Linh phï.")
				TopMessage("Phong Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,65,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Khuynh Thµnh NhÊt KÝch<color>, cÇn 1 trang hoµng kh«ng th­, 3 Hång B¶o Th¹ch","no")
	end
end;


function daoshi(DNum)
	CloseDialog()
	if (DNum==0) then
		MsgBox("<color=green>MËt tÞch Tinh Th«ng Thæ HÖ<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck0","no")
	elseif (DNum==1) then
		MsgBox("<color=green>MËt tÞch ThiÕt M· B¨ng Qua<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck1","no")
	elseif (DNum==2) then
		MsgBox("<color=green>MËt tÞch Tinh Th«ng Háa HÖ<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck2","no")
	elseif (DNum==3) then
		MsgBox("<color=green>MËt tÞch Phong V©n L«i §éng<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck3","no")
	elseif (DNum==4) then
		MsgBox("<color=green>MËt tÞch Ngò Nh¹c TriÒu T«ng<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck4","no")
	elseif (DNum==5) then
		MsgBox("<color=green>MËt tÞch Tinh Th«ng B¨ng HÖ<color> cÇn <color=green> 1 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck5","no")
	elseif (DNum==6) then
		MsgBox("<color=green>MËt tÞch ThËp Ph­¬ng LiÖt Háa<color> cÇn <color=green> 1 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck6","no")
	elseif (DNum==7) then
		MsgBox("<color=green>MËt tÞch L«i Phong Gi¸p<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck7","no")
	elseif (DNum==8) then
		MsgBox("<color=green>MËt tÞch Thiªn B¨ng §Þa LiÖt<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck8","no")
	elseif (DNum==9) then
		MsgBox("<color=green>MËt tÞch B¨ng Phong B¹o<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","DCheck9","no")
	elseif (DNum==10) then
		MsgBox("<color=green>MËt tÞch Chóc Dung Ch©n KhÝ<color> cÇn <color=green> 1 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","DCheck10","no")
	elseif (DNum==11) then
		MsgBox("<color=green>MËt tÞch L«i §éng Cöu Thiªn<color> cÇn <color=green> 1 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","DCheck11","no")
	elseif (DNum==12) then
		MsgBox("<color=green>MËt tÞch HuyÒn N÷ Bæ Thiªn<color> cÇn <color=green> 2 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","DCheck12","no")
	elseif (DNum==13) then
		MsgBox("<color=green>MËt tÞch B¨ng Phong V¹n Lý<color> cÇn <color=green> 2 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","DCheck13","no")
	elseif (DNum==14) then
		MsgBox("<color=green>MËt tÞch Tam Muéi Ch©n Háa<color> cÇn <color=green> 1 trang hoµng kh«ng th­<color> vµ <color=green>3 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","DCheck14","no")
	elseif (DNum==15) then
		MsgBox("<color=green>MËt tÞch Bµn Cæ Khai Thiªn<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color>, ng­¬i cã ®ñ ch­a?","PTCheck0","no")
	elseif (DNum==16) then
		MsgBox("<color=green>MËt tÞch Ban M«n Léng Phñ<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color>, ng­¬i cã ®ñ ch­a?","PTCheck1","no")
	end
end;

function DCheck0()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=1) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tinh Th«ng Thæ HÖ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Tinh Th«ng Thæ HÖ ®· xuÊt hiÖn. ")
			AddNormalItem(7,9,12,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Tinh Th«ng Thæ HÖ<color>, cÇn 1 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh. ","no")
	end
end;

function DCheck1()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=1) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=5) then
			Talk(1,"no","Vui qu¸! MËt tÞch ThiÕt M· B¨ng Qua ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch ThiÕt M· B¨ng Qua ®· xuÊt hiÖn.")
			AddNormalItem(7,10,13,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>ThiÕt M· B¨ng Qua<color>, cÇn 1 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh.","no")
	end
end;

function DCheck2()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tinh Th«ng Háa HÖ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Tinh Th«ng Háa HÖ ®· xuÊt hiÖn råi!")
			AddNormalItem(7,11,14,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Tinh Th«ng Háa HÖ<color>, cÇn 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh.","no")
	end
end;

function DCheck3()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Phong V©n L«i §éng ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Phong V©n L«i §éng ®· xuÊt hiÖn.")
			AddNormalItem(7,12,15,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Phong V©n L«i §éng<color>, cÇn 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh ","no")
	end
end;

function DCheck4()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch Ngò Nh¹c TriÒu T«ng ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Ngò Nh¹c TriÒu T«ng ®· xuÊt hiÖn.")
			AddNormalItem(7,13,16,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Ngò Nh¹c TriÒu T«ng<color>, cÇn 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh ","no")
	end
end;

function DCheck5()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=1) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tinh Th«ng B¨ng HÖ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Tinh Th«ng B¨ng HÖ ®· xuÊt hiÖn")
			AddNormalItem(7,14,17,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Tinh Th«ng B¨ng HÖ<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;

function DCheck6()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=1) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch ThËp Ph­¬ng LiÖt Háa ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch ThËp Ph­¬ng LiÖt Háa ®· xuÊt hiÖn.")
			AddNormalItem(7,15,18,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>ThËp Ph­¬ng LiÖt Háa<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;

function DCheck7()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch L«i Phong Gi¸p ®· xuÊt hiÖn!")
			TopMessage("MËt tÞch L«i Phong Gi¸p ®· xuÊt hiÖn!")
			AddNormalItem(7,16,19,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>L«i Phong Gi¸p<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;

function DCheck8()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Thiªn B¨ng §Þa LiÖt ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Thiªn B¨ng §Þa LiÖt ®· xuÊt hiÖn")
			AddNormalItem(7,17,20,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Thiªn B¨ng §Þa LiÖt<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;

function DCheck9()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch B¨ng Phong B¹o ®· xuÊt hiÖn!")
			TopMessage("MËt tÞch B¨ng Phong B¹o ®· xuÊt hiÖn")
			AddNormalItem(7,18,21,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>B¨ng Phong B¹o<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;

function DCheck10()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch Chóc Dung Ch©n KhÝ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Chóc Dung Ch©n KhÝ ®· xuÊt hiÖn")
			AddNormalItem(7,19,22,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Chóc Dung Ch©n KhÝ<color>, cÇn 1 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch","no")
	end
end;

function DCheck11()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch L«i §éng Cöu Thiªn ®· xuÊt hiÖn!")
			TopMessage("MËt tÞch L«i §éng Cöu Thiªn ®· xuÊt hiÖn")
			AddNormalItem(7,20,23,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>L«i §éng Cöu Thiªn<color>,cÇn cã 1 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch","no")
	end
end;

function DCheck12()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=2) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch HuyÒn N÷ Bæ Thiªn ®· xuÊt hiÖn!")
			TopMessage("MËt tÞch HuyÒn N÷ Bæ Thiªn ®· xuÊt hiÖn")
			AddNormalItem(7,21,24,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>HuyÒn N÷ Bæ Thiªn<color>, cÇn 2 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch","no")
	end
end;

function DCheck13()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=2) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch B¨ng Phong V¹n Lý ®· xuÊt hiÖn!")
			TopMessage("MËt tÞch B¨ng Phong V¹n Lý ®· xuÊt hiÖn.")
			AddNormalItem(7,22,25,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>B¨ng Phong V¹n Lý<color>, cÇn 2 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch.","no")
	end
end;

function DCheck14()
	CloseDialog()
	if (HaveNormalItem(8,142,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=3) then
		CostIBItem(FindAValidIBItem(8,142,2,0))
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=1) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tam Muéi Ch©n Háa ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Tam Muéi Ch©n Háa ®· xuÊt hiÖn.")
			AddNormalItem(7,23,26,1,0,0)
		else
			local j=random(1,2)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï cao cÊp.")
				TopMessage("Di ngo¹i phï cao cÊp ®· xuÊt hiÖn.")
				AddNormalItem(8,159,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phong Linh phï.")
				TopMessage("Phong Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,65,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Tam Muéi Ch©n Háa<color>, cÇn 1 trang hoµng kh«ng th­, 3 Hång B¶o Th¹ch","no")
	end
end;

function yiren(YNum)
	CloseDialog()
	if (YNum==0) then
		MsgBox("<color=green>MËt tÞch C­êng C«ng Chó<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","YCheck0","no")
	elseif (YNum==1) then
		MsgBox("<color=green>MËt tÞch Ph¸ Gi¸p Chó<color> cÇn <color=green> 1 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","YCheck1","no")
	elseif (YNum==2) then
		MsgBox("<color=green>MËt tÞch Bå §Ò Chó<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","YCheck2","no")
	elseif (YNum==3) then
		MsgBox("<color=green>MËt tÞch Tr¶m T©m Chó<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","YCheck3","no")
	elseif (YNum==4) then
		MsgBox("<color=green>MËt tÞch TËt Phong Chó<color> cÇn <color=green> 2 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","YCheck4","no")
	elseif (YNum==5) then
		MsgBox("<color=green>MËt tÞch V¹n Cèt Toµn Kh«<color> cÇn <color=green> 1 trang hoµng kh«ng th­<color> vµ <color=green>3 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","YCheck5","no")
	elseif (YNum==6) then
		MsgBox("<color=green>MËt tÞch Liªn Nç tÕ<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh vì<color>, ng­¬i cã ®ñ ch­a?","YCheck6","no")
	elseif (YNum==7) then
		MsgBox("<color=green>MËt tÞch Háa L«i<color> cÇn <color=green> 2 trang b¹ch kh«ng th­<color> vµ <color=green>1 m¶nh Hång Thñy Tinh vì<color>, ng­¬i cã ®ñ ch­a?","YCheck7","no")
	elseif (YNum==8) then
		MsgBox("<color=green>MËt tÞch To¸i Cèt<color> cÇn <color=green> 1 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","YCheck8","no")
	elseif (YNum==9) then
		MsgBox("<color=green>MËt tÞch L­u Tinh<color> cÇn <color=green> 2 trang lam kh«ng th­<color> vµ <color=green>1 Hång Thñy Tinh<color>, ng­¬i cã ®ñ ch­a?","YCheck9","no")
	elseif (YNum==10) then
		MsgBox("<color=green>MËt tÞch Truy Hån tÕ<color> cÇn <color=green> 1 trang hång kh«ng th­<color> vµ <color=green>1 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","YCheck10","no")
	elseif (YNum==11) then
		MsgBox("<color=green>MËt tÞch Phong QuyÓn Tµn V©n<color> cÇn <color=green> 1 trang hoµng kh«ng th­<color> vµ <color=green>3 Hång B¶o Th¹ch<color>, ng­¬i cã ®ñ ch­a?","YCheck11","no")
	elseif (YNum==12) then
		MsgBox("<color=green>MËt tÞch Bµn Cæ Khai Thiªn<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color>, ng­¬i cã ®ñ ch­a?","PTCheck0","no")
	elseif (YNum==13) then
		MsgBox("<color=green>MËt tÞch Ban M«n Léng Phñ<color> cÇn <color=green> 1 trang b¹ch kh«ng th­<color>, ng­¬i cã ®ñ ch­a?","PTCheck1","no")
	end
end;

function YCheck0()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch C­êng C«ng Chó ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch C­êng C«ng Chó ®· xuÊt hiÖn.")
			AddNormalItem(7,43,46,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>C­êng C«ng Chó<color>,  cÇn cã 2 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh. ","no")
	end
end;

function YCheck1()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=1) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch Ph¸ Gi¸p Chó ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Ph¸ Gi¸p Chó ®· xuÊt hiÖn.")
			AddNormalItem(7,44,47,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Ph¸ Gi¸p Chó<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;

function YCheck2()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch Bå §Ò Chó ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Bå §Ò Chó ®· xuÊt hiÖn råi.")
			AddNormalItem(7,45,48,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Bå §Ò Chó<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;

function YCheck3()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch Tr¶m T©m Chó ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Tr¶m T©m Chó ®· xuÊt hiÖn.")
			AddNormalItem(7,46,49,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Tr¶m T©m Chó<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;

function YCheck4()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=2) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=2) then
			Talk(1,"no","Vui qu¸! MËt tÞch TËt Phong Chó ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch TËt Phong Chó ®· xuÊt hiÖn")
			AddNormalItem(7,47,50,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>TËt Phong Chó<color>, cÇn 2 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch","no")
	end
end;

function YCheck5()
	CloseDialog()
	if (HaveNormalItem(8,142,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=3) then
		CostIBItem(FindAValidIBItem(8,142,2,0))
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=1) then
			Talk(1,"no","Vui qu¸! MËt tÞch V¹n Cèt Toµn Kh« ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch V¹n Cèt Toµn Kh« ®· xuÊt hiÖn. ")
			AddNormalItem(7,48,51,1,0,0)
		else
			local j=random(1,2)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï cao cÊp.")
				TopMessage("Di ngo¹i phï cao cÊp ®· xuÊt hiÖn.")
				AddNormalItem(8,159,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phong Linh phï.")
				TopMessage("Phong Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,65,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>V¹n Cèt Toµn Kh«<color>, cÇn 1 trang hoµng kh«ng th­, 3 Hång B¶o Th¹ch","no")
	end
end;

function YCheck6()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=1) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,100)
		if (i<=5) then
			Talk(1,"no","Vui qu¸! MËt tÞch Liªn Nç tÕ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Liªn Nç tÕ ®· xuÊt hiÖn.")
			AddNormalItem(7,52,453,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Liªn Nç tÕ<color>, cÇn 1 trang b¹ch kh«ng th­, 1 m¶nh Hång Thñy Tinh ","no")
	end
end;

function YCheck7()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=2) and (HaveNormalItem(3,77,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		CostIBItem(FindAValidIBItem(8,139,2,0))
		DelNormalItem(3,77,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch Háa L«i tÕ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch Háa L«i tÕ ®· xuÊt hiÖn.")
			AddNormalItem(7,53,454,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn håi thµnh phï.")
				TopMessage("Håi thµnh phï xuÊt hiÖn råi.")
				AddNormalItem(5,0,0,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn th¨m nµy!")
				TopMessage("§· cã th¨m råi.")
				AddNormalItem(6,1,12,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thæ Linh Phï.")
				TopMessage("Thæ Linh Phï xuÊt hiÖn råi.")
				AddNormalItem(3,62,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Háa L«i tÕ<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;


function YCheck8()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=1) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,10)
		if (i<=3) then
			Talk(1,"no","Vui qu¸! MËt tÞch To¸i Cèt tÕ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch To¸i Cèt tÕ ®· xuÊt hiÖn.")
			AddNormalItem(7,54,455,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>To¸i Cèt tÕ<color>, cÇn 1 trang lam kh«ng th­, 1 Hång Thñy Tinh","no")
	end
end;

function YCheck9()
	CloseDialog()
	if (HaveNormalItem(8,140,2,0)>=2) and (HaveNormalItem(3,28,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,140,2,0))
		CostIBItem(FindAValidIBItem(8,140,2,0))
		DelNormalItem(3,28,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch L­u Tinh tÕ ®· xuÊt hiÖn råi!")
			TopMessage("MËt tÞch L­u Tinh tÕ ®· xuÊt hiÖn")
			AddNormalItem(7,55,456,1,0,0)
		else
			local j=random(1,4)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn TuyÕt Qu¸i phï")
				TopMessage("TuyÕt Qu¸i BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
				AddNormalItem(8,57,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa DiÖn phï.")
				TopMessage("Háa DiÖn BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
				AddNormalItem(8,104,2,1,0,0)
			elseif (j==3) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn KiÕm Nh©n phï")
				TopMessage("KiÕm Nh©n BiÕn th©n phï ®· xuÊt hiÖn.")
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
				AddNormalItem(8,85,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Thñy Linh phï")
				TopMessage("Thñy Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,63,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>To¸i Cèt tÕ<color>, cÇn 2 trang lam kh«ng th­, 1 Hång Thñy Tinh.","no")
	end
end;


function YCheck10()
	CloseDialog()
	if (HaveNormalItem(8,141,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=1) then
		CostIBItem(FindAValidIBItem(8,141,2,0))
		DelNormalItem(3,79,0,0)
		local i=random(1,100)
		if (i<=15) then
			Talk(1,"no","Vui qu¸! MËt tÞch Truy Hån tÕ ®· xuÊt hiÖn")
			TopMessage("MËt tÞch Truy Hån tÕ ®· xuÊt hiÖn")
			AddNormalItem(7,56,457,1,0,0)
		else
			local j=random(1,3)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï.")
				TopMessage("Di Ngo¹i phï ®· xuÊt hiÖn.")
				AddNormalItem(8,35,2,1,0,0)
			elseif (j==2) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phóc Hé phï")
				TopMessage("Phóc Hé phï xuÊt hiÖn.")
				AddNormalItem(8,48,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Háa Linh phï.")
				TopMessage("Háa Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,64,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Truy Hån tÕ<color>, cÇn 1 trang hång kh«ng th­, mét viªn Hång B¶o Th¹ch","no")
	end
end;


function YCheck11()
	CloseDialog()
	if (HaveNormalItem(8,142,2,0)>=1) and (HaveNormalItem(3,79,0,0)>=3) then
		CostIBItem(FindAValidIBItem(8,142,2,0))
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		DelNormalItem(3,79,0,0)
		local i=random(1,10)
		if (i<=1) then
			Talk(1,"no","Vui qu¸! MËt tÞch Phong QuyÓn Tµn V©n ®· xuÊt hiÖn")
			TopMessage("MËt tÞch Phong QuyÓn Tµn V©n ®· xuÊt hiÖn.")
			AddNormalItem(7,57,458,1,0,0)
		else
			local j=random(1,2)
			if (j==1) then
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Di ngo¹i phï cao cÊp.")
				TopMessage("Di ngo¹i phï cao cÊp ®· xuÊt hiÖn.")
				AddNormalItem(8,159,2,1,0,0)
			else
				Talk(1,"no","Xuýt tý n÷a lµ tÈu háa nhËp ma, mau nhËn Phong Linh phï.")
				TopMessage("Phong Linh Phï ®· xuÊt hiÖn.")
				AddNormalItem(3,65,0,1,0,0)
			end
		end
	else
		MsgBox("<color=green>Phong QuyÓn Tµn V©n<color>, cÇn 1 trang hoµng kh«ng th­, 3 Hång B¶o Th¹ch","no")
	end
end;


function PTCheck0()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		Talk(1,"no","Vui qu¸! MËt tÞch Bµn Cæ Khai Thiªn ®· xuÊt hiÖn.")
		TopMessage("MËt tÞch Bµn Cæ Khai Thiªn ®· xuÊt hiÖn")
		AddNormalItem(7,58,62,1,0,0)
	else
		MsgBox("<color=green>Bµn Cæ Khai Thiªn<color>, cÇn 1 trang b¹ch kh«ng th­","no")
	end
end;


function PTCheck1()
	CloseDialog()
	if (HaveNormalItem(8,139,2,0)>=1) then
		CostIBItem(FindAValidIBItem(8,139,2,0))
		Talk(1,"no","Vui qu¸! MËt tÞch Ban M«n Léng Phñ ®· xuÊt hiÖn")
		TopMessage("MËt tÞch Ban M«n Léng Phñ ®· xuÊt hiÖn.")
		AddNormalItem(7,59,128,1,0,0)
	else
		MsgBox("<color=green>Ban M«n Léng Phñ<color>, cÇn 1 trang b¹ch kh«ng th­.","no")
	end
end;



function no()
	CloseDialog()
end;
-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1021 then CloseDialog(); return end
    return pt_original_main()
end

pt_book_DCheck0=DCheck0
function DCheck0()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck0()
end

pt_book_DCheck1=DCheck1
function DCheck1()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck1()
end

pt_book_DCheck10=DCheck10
function DCheck10()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck10()
end

pt_book_DCheck11=DCheck11
function DCheck11()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck11()
end

pt_book_DCheck12=DCheck12
function DCheck12()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck12()
end

pt_book_DCheck13=DCheck13
function DCheck13()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck13()
end

pt_book_DCheck14=DCheck14
function DCheck14()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck14()
end

pt_book_DCheck2=DCheck2
function DCheck2()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck2()
end

pt_book_DCheck3=DCheck3
function DCheck3()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck3()
end

pt_book_DCheck4=DCheck4
function DCheck4()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck4()
end

pt_book_DCheck5=DCheck5
function DCheck5()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck5()
end

pt_book_DCheck6=DCheck6
function DCheck6()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck6()
end

pt_book_DCheck7=DCheck7
function DCheck7()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck7()
end

pt_book_DCheck8=DCheck8
function DCheck8()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck8()
end

pt_book_DCheck9=DCheck9
function DCheck9()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_DCheck9()
end

pt_book_JCheck0=JCheck0
function JCheck0()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck0()
end

pt_book_JCheck1=JCheck1
function JCheck1()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck1()
end

pt_book_JCheck10=JCheck10
function JCheck10()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck10()
end

pt_book_JCheck2=JCheck2
function JCheck2()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck2()
end

pt_book_JCheck3=JCheck3
function JCheck3()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck3()
end

pt_book_JCheck4=JCheck4
function JCheck4()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck4()
end

pt_book_JCheck5=JCheck5
function JCheck5()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck5()
end

pt_book_JCheck6=JCheck6
function JCheck6()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck6()
end

pt_book_JCheck7=JCheck7
function JCheck7()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck7()
end

pt_book_JCheck8=JCheck8
function JCheck8()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck8()
end

pt_book_JCheck9=JCheck9
function JCheck9()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_JCheck9()
end

pt_book_PTCheck0=PTCheck0
function PTCheck0()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_PTCheck0()
end

pt_book_PTCheck1=PTCheck1
function PTCheck1()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_PTCheck1()
end

pt_book_YCheck0=YCheck0
function YCheck0()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck0()
end

pt_book_YCheck1=YCheck1
function YCheck1()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck1()
end

pt_book_YCheck10=YCheck10
function YCheck10()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck10()
end

pt_book_YCheck11=YCheck11
function YCheck11()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck11()
end

pt_book_YCheck2=YCheck2
function YCheck2()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck2()
end

pt_book_YCheck3=YCheck3
function YCheck3()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck3()
end

pt_book_YCheck4=YCheck4
function YCheck4()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck4()
end

pt_book_YCheck5=YCheck5
function YCheck5()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck5()
end

pt_book_YCheck6=YCheck6
function YCheck6()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck6()
end

pt_book_YCheck7=YCheck7
function YCheck7()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck7()
end

pt_book_YCheck8=YCheck8
function YCheck8()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck8()
end

pt_book_YCheck9=YCheck9
function YCheck9()
    if pt_runtime_world_pos()~=1021 then CloseDialog();return end
    if CalcFreeItemCellCount(1,1)<1 then
        Say("H·y ®Ó trèng Ýt nhÊt mét « trong F4 ®Ó nhËn s¸ch hoÆc vËt phÈm.",1,"Thoat/no")
        return
    end
    return pt_book_YCheck9()
end
-- PT_CONFIGURABLE_BOOK_PITY_BEGIN
Include("\\script\\phongthan\\book_policy.lua")
PT_BOOK_WORLD=1021
PT_BOOK_RECIPES={
{name="Hoµnh Kh«ng Tr¶m",class=0,base=2000,needs={{8,139,2,0,0,0,1},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,29,32,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Tinh Th«ng §o¶n §ao",class=0,base=3000,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,30,33,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Tinh Th«ng Tr­êng §ao",class=0,base=3000,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,31,34,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Tam §Çu Lôc Thñ",class=0,base=500,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,32,35,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="HuyÒn B¨ng tr¶m",class=0,base=3000,needs={{8,140,2,0,0,0,1},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,33,36,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Háa Quang Tr¶m",class=0,base=2000,needs={{8,140,2,0,0,0,1},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,34,37,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Liªn Hoµn Tr¶m",class=0,base=1500,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,35,38,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="L¹c §Þa Tr¶m",class=0,base=1500,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,36,39,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="ThuÇn D­¬ng Hé ThÓ",class=0,base=1500,needs={{8,141,2,0,0,0,1},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,37,40,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="Thiªn Qu©n Tr¶m",class=0,base=1500,needs={{8,141,2,0,0,0,2},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,38,41,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="Khuynh Thµnh NhÊt KÝch",class=0,base=1000,needs={{8,142,2,0,0,0,1},{3,79,0,0,0,0,3}},materials={"Hoµng Kh«ng th­","Hång b¶o th¹ch"},reward={7,39,42,1,0,0,1},failures={{8,159,2,1,0,0,1},{3,65,0,1,0,0,1}}},
{name="Tinh Th«ng Thæ HÖ",class=1,base=3000,needs={{8,139,2,0,0,0,1},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,9,12,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="ThiÕt M· B¨ng Qua",class=1,base=5000,needs={{8,139,2,0,0,0,1},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,10,13,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Tinh Th«ng Háa HÖ",class=1,base=2000,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,11,14,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Phong V©n L«i §éng",class=1,base=3000,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,12,15,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Ngò Nh¹c TriÒu T«ng",class=1,base=1500,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,13,16,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Tinh Th«ng B¨ng HÖ",class=1,base=3000,needs={{8,140,2,0,0,0,1},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,14,17,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="ThËp Ph­¬ng LiÖt Háa",class=1,base=1500,needs={{8,140,2,0,0,0,1},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,15,18,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="L«i Phong Gi¸p",class=1,base=2000,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,16,19,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Thiªn B¨ng ®Þa liÖt",class=1,base=3000,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,17,20,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="B¨ng Phong B¹o",class=1,base=3000,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,18,21,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Chóc Dung Ch©n KhÝ",class=1,base=2000,needs={{8,141,2,0,0,0,1},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,19,22,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="L«i §éng Cöu thiªn",class=1,base=1500,needs={{8,141,2,0,0,0,1},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,20,23,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="HuyÒn N÷ Bæ Thiªn",class=1,base=2000,needs={{8,141,2,0,0,0,2},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,21,24,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="B¨ng Phong V¹n Lý",class=1,base=1500,needs={{8,141,2,0,0,0,2},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,22,25,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="Tam Muéi Ch©n Háa",class=1,base=1000,needs={{8,142,2,0,0,0,1},{3,79,0,0,0,0,3}},materials={"Hoµng Kh«ng th­","Hång b¶o th¹ch"},reward={7,23,26,1,0,0,1},failures={{8,159,2,1,0,0,1},{3,65,0,1,0,0,1}}},
{name="C­êng C«ng chó",class=2,base=3000,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,43,46,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Ph¸ Gi¸p chó",class=2,base=2000,needs={{8,140,2,0,0,0,1},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,44,47,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Bå §Ò chó",class=2,base=3000,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,45,48,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Tr¶m T©m Chó",class=2,base=2000,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,46,49,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="TËt Phong chó",class=2,base=2000,needs={{8,141,2,0,0,0,2},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,47,50,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="V¹n Cèt Toµn Kh«",class=2,base=1000,needs={{8,142,2,0,0,0,1},{3,79,0,0,0,0,3}},materials={"Hoµng Kh«ng th­","Hång b¶o th¹ch"},reward={7,48,51,1,0,0,1},failures={{8,159,2,1,0,0,1},{3,65,0,1,0,0,1}}},
{name="Liªn Nç TÕ",class=2,base=500,needs={{8,139,2,0,0,0,1},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,52,453,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="Háa L«i TÕ",class=2,base=1500,needs={{8,139,2,0,0,0,2},{3,77,0,0,0,0,1}},materials={"B¹ch Kh«ng th­","M¶nh Hång thñy tinh"},reward={7,53,454,1,0,0,1},failures={{5,0,0,1,0,0,1},{6,1,12,1,0,0,1},{3,62,0,1,0,0,1}}},
{name="To¸i Cèt tÕ",class=2,base=3000,needs={{8,140,2,0,0,0,1},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,54,455,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="L­u Tinh tÕ",class=2,base=1500,needs={{8,140,2,0,0,0,2},{3,28,0,0,0,0,1}},materials={"Lam Kh«ng Th­","Hång thñy tinh"},reward={7,55,456,1,0,0,1},failures={{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,57,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,104,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{8,85,2,1,0,0,1},{3,63,0,1,0,0,1}}},
{name="Truy Hån tÕ",class=2,base=1500,needs={{8,141,2,0,0,0,1},{3,79,0,0,0,0,1}},materials={"Hång Kh«ng Th­","Hång b¶o th¹ch"},reward={7,56,457,1,0,0,1},failures={{8,35,2,1,0,0,1},{8,48,2,1,0,0,1},{3,64,0,1,0,0,1}}},
{name="Phong QuyÓn Tµn V©n",class=2,base=1000,needs={{8,142,2,0,0,0,1},{3,79,0,0,0,0,3}},materials={"Hoµng Kh«ng th­","Hång b¶o th¹ch"},reward={7,57,458,1,0,0,1},failures={{8,159,2,1,0,0,1},{3,65,0,1,0,0,1}}},
{name="Bµn Cæ khai thiªn",class=3,base=10000,needs={{8,139,2,0,0,0,1}},materials={"B¹ch Kh«ng th­"},reward={7,58,62,1,0,0,1},failures={}},
{name="Ban M«n Léng Phñ",class=3,base=10000,needs={{8,139,2,0,0,0,1}},materials={"B¹ch Kh«ng th­"},reward={7,59,128,1,0,0,1},failures={}},
}
Include("\\script\\phongthan\\book_service.lua")
books=pt_book_menu
function JCheck0() return pt_book_make(1) end
function JCheck1() return pt_book_make(2) end
function JCheck2() return pt_book_make(3) end
function JCheck3() return pt_book_make(4) end
function JCheck4() return pt_book_make(5) end
function JCheck5() return pt_book_make(6) end
function JCheck6() return pt_book_make(7) end
function JCheck7() return pt_book_make(8) end
function JCheck8() return pt_book_make(9) end
function JCheck9() return pt_book_make(10) end
function JCheck10() return pt_book_make(11) end
function DCheck0() return pt_book_make(12) end
function DCheck1() return pt_book_make(13) end
function DCheck2() return pt_book_make(14) end
function DCheck3() return pt_book_make(15) end
function DCheck4() return pt_book_make(16) end
function DCheck5() return pt_book_make(17) end
function DCheck6() return pt_book_make(18) end
function DCheck7() return pt_book_make(19) end
function DCheck8() return pt_book_make(20) end
function DCheck9() return pt_book_make(21) end
function DCheck10() return pt_book_make(22) end
function DCheck11() return pt_book_make(23) end
function DCheck12() return pt_book_make(24) end
function DCheck13() return pt_book_make(25) end
function DCheck14() return pt_book_make(26) end
function YCheck0() return pt_book_make(27) end
function YCheck1() return pt_book_make(28) end
function YCheck2() return pt_book_make(29) end
function YCheck3() return pt_book_make(30) end
function YCheck4() return pt_book_make(31) end
function YCheck5() return pt_book_make(32) end
function YCheck6() return pt_book_make(33) end
function YCheck7() return pt_book_make(34) end
function YCheck8() return pt_book_make(35) end
function YCheck9() return pt_book_make(36) end
function YCheck10() return pt_book_make(37) end
function YCheck11() return pt_book_make(38) end
function PTCheck0() return pt_book_make(39) end
function PTCheck1() return pt_book_make(40) end

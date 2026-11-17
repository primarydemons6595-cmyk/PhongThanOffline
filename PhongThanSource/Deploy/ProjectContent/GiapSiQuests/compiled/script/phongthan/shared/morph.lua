--description: PolyMorph
--author: yichuan
--date: 2004/7/13

function  main()
	local  w,x,y = GetWorldPos()
	if (w==71) then
		Msg2Player("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	--elseif (IsInWar()==1) then
	--	TopMessage("¹úÕ½µØÍ¼²»¿ÉÒÔÊ¹ÓÃ±äÉí·û")
	elseif (w==73) then
		TopMessage("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	elseif (w==74) then
		TopMessage("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	elseif (w==75) then
		TopMessage("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	elseif (w==76) then
		TopMessage("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	elseif (w==77) then
		TopMessage("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	elseif (w==78) then
		TopMessage("Kh«ng thÓ sö dông biÕn th©n phï ë ®©y.")
	else
		SayButtons(10626,8,"Kh«i phôc/esc","C¸t T­êng/zhuzhu","TuyÕt Qu¸i/fs","Háa ng­/mms","Ngäc N÷/xh","Phi Thè/bz","T×nh yªu/pty","Love/qe")
	end;
end;

function  esc()
		if ( GetMorphType()==364)then          --Éæ¼°?Îñ£¬ÓÐµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿?Îñ½â³ý£¬¹ý³ÌÖÐ²»¿ÉÊ¹ÓÃ±äÉí·û
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(-1,0,2,10,1800)		--Íê?±£»¤£¬ËÙ¶È10£¬Ê±¼ä1800Ãë
		end;
		CloseDialog()
end;

function  zhuzhu()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(411,0,2,10,1800)
		end;
		CloseDialog()
end;

function  fs()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(1,0,2,10,1800)
		end;
		CloseDialog()
end;

function  mms()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(21,0,2,10,1800)
		end;
		CloseDialog()
end;

function  xh()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(34,0,2,10,1800)
		end;
		CloseDialog()
end;

function bz()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(50,0,2,10,1800)
		end;
		CloseDialog()
end;

function  pty()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(248,0,2,10,1800)
		end;
		CloseDialog()
end;

function  qe()
		if ( GetMorphType()==364)then
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ sö dông biÕn th©n phï.")
				AddNormalItem(6,1,11,1,0,0)
		else	
				PolyMorph(249,0,2,10,1800)
		end;
		CloseDialog()
end;

function   no()
		CloseDialog()
		AddNormalItem(6,1,11,1,0,0)
end;

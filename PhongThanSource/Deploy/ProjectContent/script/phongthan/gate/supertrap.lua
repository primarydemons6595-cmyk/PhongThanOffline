--description: SuperTrap
--author: yichuan
--date: 2004/6/29

function main(sel)
	local  w,x,y = GetWorldPos()
	if ((GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0))and((w==1)or(w==52)or(w==66))then
			NewWorld(21,1723,3075)
	else
		if(cantrans()==1)then
			local cost = gettranscost()
			Say("§¼ng cÊp hiÖn t¹i cña b¹n muèn chuyÓn ®i ph¶i tèn <color=red>"..cost.."l­îng<color>. B¹n muèn chuyÓn ®Õn ®©u?",10,"Sïng Thµnh ®¹i doanh/ccdy","Ngäc H­ cung/yxg","Xi V­u mé/cym","TriÒu Ca/cg","T©y Kú/xq","Phong ThÇn ®µi/fst","Diªu Tr×/yc","ChiÕn tr­êng ViÔn Cæ/ygzc","Thanh §ång s¬n/v58","Tr­ Lung Thµnh tr¹i/v100")
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
	end;
end;

function gettranscost()
	local penny
	if(GetLevel()<=30)then
		penny=200
	elseif(GetLevel()>30)and(GetLevel()<=50)then
		penny=500  
	elseif(GetLevel()>50)and(GetLevel()<=70)then
		penny=1000 
	elseif(GetLevel()>70)and(GetLevel()<=90)then
		penny=2000
	else
		penny=5000
	end;
	return penny
end

function cantrans()
	local mark
	if (GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
		mark=0
	else
		mark=1
	end;
	return mark
end;

function ccdy()--³ç³Ç
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(2,1608,3196)
				end;
				if(i==2)then
						NewWorld(2,1591,3165)
				end;
				if(i==3)then
						NewWorld(2,1623,3213)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function yxg()--ÓñĞé¹¬
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(3,1692,3138)
				end;
				if(i==2)then
						NewWorld(3,1689,3153)
				end;
				if(i==3)then
						NewWorld(3,1710,3110)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function cym()--ò¿ÓÈÄ¹
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(4,1573,3229)
				end;
				if(i==2)then
						NewWorld(4,1554,3227)
				end;
				if(i==3)then
						NewWorld(4,1573,3262)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function  cg()--³¯¸è
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(21,1723,3075)
				end;
				if(i==2)then
						NewWorld(21,1735,3027)
				end;
				if(i==3)then
						NewWorld(21,1725,3106)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function  xq()--Î÷áª
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(20,1452,3086)
				end;
				if(i==2)then
						NewWorld(20,1437,3093)
				end;
				if(i==3)then
						NewWorld(20,1470,3106)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function fst()--·âÉñÌ¨
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(1,1536,3254)
				end;
				if(i==2)then
						NewWorld(1,1523,3241)
				end;
				if(i==3)then
						NewWorld(1,1549,3269)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function yc()--Ñş³Ø
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(52,1541,3190)
				end;
				if(i==2)then
						NewWorld(52,1556,3202)
				end;
				if(i==3)then
						NewWorld(52,1540,3205)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function ygzc()--Ô¶¹ÅÕ½³¡
		local cost = gettranscost()
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		elseif(GetPK()>=88)then
				Msg2Player("Ng­êi ch¬i tªn ch÷ ®á kh«ng thÓ chuyÓn ®Õn ViÔn Cæ")
		elseif(GetCamp()==0)then
				Msg2Player("T©n thñ kh«ng thÓ vµo ViÔn Cæ")
		elseif(GetCash()>=cost)then
				NewWorld(64,1600,3238)
				SetFightState(0)
				SetRevPos(64,216)
				Pay(cost)
		else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
		end;
		CloseDialog()
end;

function   no()
		CloseDialog()
end;

function  v58()--ÇàÍ­É½
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				local i=random(1,3)
				if ( i==1)then
						NewWorld(57,1608,3094)
				end;
				if(i==2)then
						NewWorld(57,1609,3075)
				end;
				if(i==3)then
						NewWorld(57,1604,3115)
				end;
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function  v2()--Î´À´³¯¸è
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				NewWorld(63,1506,3298)
				SetFightState(1)
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function  v3()--Î´À´ÓñĞé¹¬
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				NewWorld(62,1627,3322)
				SetFightState(1)
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function  v4()--¹ıÈ¥ÓñĞé¹¬
		if(cantrans()==1)then
			local cost = gettranscost()
			if(GetCash()>=cost)then
				NewWorld(61,1597,3333)
				SetFightState(1)
				Pay(cost)
			else
				Msg2Player("B¹n kh«ng ®ñ tiÒn!")
			end;
		else
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		end;
		CloseDialog()
end;

function v100()--ÖíÁı³ÇÕ¯
		if(GetLevel()<30)then
			if(cantrans()==1)then
				local cost = gettranscost()
				if(GetCash()>=cost)then
					NewWorld(66,1545,3348)
					SetFightState(1)
					Pay(cost)
				else
					Msg2Player("B¹n kh«ng ®ñ tiÒn!")
				end;
			else
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
			end;
			CloseDialog()
		else
			Talk(1,"no",10827)
		end;
end;

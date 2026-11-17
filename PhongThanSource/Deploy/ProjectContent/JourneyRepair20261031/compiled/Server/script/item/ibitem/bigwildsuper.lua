--description: wildSuper-Ò°Íâ´«ËÍ·û50´Î
--author: liuying
--date: 2006/3/20
--Task713¼ÇÂ¼ĞèÒªÉ¾³ıÎïÆ·µÄID

function main(itemID)
	local  w,x,y = GetWorldPos()
	if(w==64) then
		Talk(1,"no",10625)
	elseif(71==w) then
		Talk(1,"no",11061)
	elseif(w==66)then
		Talk(1,"no",11062)
	elseif(w==72)then
		Talk(1,"no","§¶o nµy n»m trong trËn ph¸p kú dŞ, linh phï trªn tay b¹n ®· mÊt ph¸p lùc.")
	else
		pt_wild_page1()
	end;
	SetExeState(0)
end;

function  v1()--·âÉñÌ¨
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v2()--Ñş³Ø
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v3()--Î÷áª
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v4()--³¯¸è
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v5()--³ç³Ç
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v6()--ÓñĞé¹¬
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v7()--ò¿ÓÈÄ¹
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v8()--³ç³ÇÒ°Íâ
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(5,1709,3063)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v9()--±±º£
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(6,1733,3013)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v10()--ÑàÉ½
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(7,1766,3365)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v11()--À¥ÂØÉ½Â´
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(8,1853,2803)
			SetFightState(1)
			CostIBItem(j)
		end;
			CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v12()--Î÷À¥ÂØ
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(9,1706,3452)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v13()--Ê×ÑôÉ½
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(10,1436,3124)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v14()--ÓÎ»ê¹Ø
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(11,1810,3133)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v15()--Ãç½®
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(12,1720,3319)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
 end;

function  v16()--¾ŞÂ¹
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(13,1835,3002)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v17()--äü¹Ø
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(14,1575,3378)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v18()--ÃÏ½ò
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(15,1539,3403)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v19()--ÈıÉ½¹Ø
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(16,1633,3192)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v20()--áªÉ½
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(17,1747,3317)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v21()--ÄÁÒ°
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(18,1620,3150)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v22()--¾øÁúÁë
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(19,1645,3137)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  v37()--³ÂÌÁ¹Ø
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
			Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
		else
			NewWorld(65,1628,3008)
			SetFightState(1)
			CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;


function  v38()--¹ÅÕ½³¡
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
			
		elseif(GetPK()>=88)then
				Msg2Player("Ng­êi ch¬i tªn ch÷ ®á kh«ng thÓ chuyÓn ®Õn ViÔn Cæ")
			
		elseif(GetCamp()==0)then
				Msg2Player("T©n thñ kh«ng thÓ vµo ViÔn Cæ")
			
		else
				NewWorld(64,1600,3238)
				SetFightState(0)
				SetRevPos(64,216)
				Msg2Player("B¹n ®· thiÕt lËp ®iÓm trïng sinh t¹i ViÔn Cæ")
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function  no()
		CloseDialog()
end;

function  v58()--ÇàÍ­É½
	local j=FindAValidIBItem(8,159,2,0)
	if(j~=0)then
		if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then          --Éæ¼°ÈÎÎñ£¬ÓĞµÄ±äÉí×´Ì¬Ö»ÄÜ¿¿ÈÎÎñ½â³ı£¬¹ı³ÌÖĞ²»¿ÉÊ¹ÓÃ´«ËÍ
				Msg2Player("ë tr¹ng th¸i nµy kh«ng thÓ chuyÓn tiÕp")
			
		else
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
				SetFightState(0)
				CostIBItem(j)
		end;
		CloseDialog()
	else
		Talk(1,"no","Xin x¸c nhËn b¹n cã vËt phÈm nµy vµ vÉn cßn hiÖu lùc!")
	end;
end;

function pt_wild_page1()
 local w=GetWorldPos()
 if w==64 or w==66 or w==71 or w==72 then return main() end
 Say(10286,15,"Phong ThÇn ®µi/v1","Diªu Tr×/v2","T©y Kú/v3","TriÒu Ca/v4","Sïng thµnh ®¹i doanh/v5","Ngäc H­ cung/v6","Xi V­u Mé/v7","Sïng thµnh/v8","B¾c H¶i/v9","YÕn S¬n/v10","Ch©n nói C«n L«n/v11","T©y C«n L«n/v12","Thñ D­¬ng s¬n/v13","Trang sau/pt_wild_page2","Dong/pt_wild_close")
end

function pt_wild_page2()
 local w=GetWorldPos()
 if w==64 or w==66 or w==71 or w==72 then return main() end
 Say(10286,14,"Du Hån/v14","Miªu C­¬ng/v15","Cù Léc/v16","§ång quan/v17","M¹nh T©n/v18","Tam S¬n /v19","Kú S¬n/v20","Môc D·/v21","TuyÖt Long lÜnh/v22","TrÇn §­êng/v37","ChiÕn tr­êng ViÔn Cæ /v38","Thanh §ång s¬n/v58","Trang truoc/pt_wild_page1","Dong/pt_wild_close")
end

function pt_wild_close() CloseDialog() end

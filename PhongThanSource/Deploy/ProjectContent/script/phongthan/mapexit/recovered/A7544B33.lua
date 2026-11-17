--Trap äü¹ØtoÎ÷áª
--TrapID£º29
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn T©y Kú")
		NewWorld(20,1646,3286);
		SetFightState(0)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

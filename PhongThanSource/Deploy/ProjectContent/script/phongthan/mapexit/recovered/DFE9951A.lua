--Trap ³¯¸ètoÐùÔ¯¶´Ò»²ã
--TrapID£º50
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Hiªn Viªn ®éng tÇng 1")
		NewWorld(27,1612,3190);
		SetFightState(1)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

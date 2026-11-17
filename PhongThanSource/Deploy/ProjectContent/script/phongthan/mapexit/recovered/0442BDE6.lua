--Trap ³¯¸èto³ÂÌÁ¹Ø
--TrapID£º51
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn TrÇn §­êng")
		NewWorld(65,1514,3297);
		SetFightState(1)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

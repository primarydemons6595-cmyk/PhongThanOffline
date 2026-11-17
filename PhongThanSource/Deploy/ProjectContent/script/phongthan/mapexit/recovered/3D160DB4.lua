--Trap ³¯¸ètoÄÁÒ°
--TrapID£º49
--Update: ÈÙ½¯·É(2004-08-26)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Môc D·")
		NewWorld(18,1796,2914);
		SetFightState(1)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

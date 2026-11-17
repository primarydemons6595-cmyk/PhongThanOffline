--Trap Î÷áªtoáªÉ½
--TrapID£º45
--Update: ÈÙ½¯·É(2004-08-26)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Kú S¬n")
		NewWorld(17,1927,3035);
		SetFightState(1)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

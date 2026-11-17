--Trap Î÷áªto»ÄÄ®
--TrapID£º47
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Hoang m¹c")
		NewWorld(22,1459,3643);
		SetFightState(1)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

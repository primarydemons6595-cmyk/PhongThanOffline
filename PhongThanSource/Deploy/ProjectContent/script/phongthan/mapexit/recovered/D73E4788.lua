--Trap ÃÏ½òtoÑàÉ½
--TrapID£º30
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Yªn S¬n")
		NewWorld(7,1902,3651);
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

--Trap ÈýÉ½¹Øto³ç³ÇÒ°Íâ
--TrapID£º34
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Sïng thµnh")
		NewWorld(5,1562,3267);
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

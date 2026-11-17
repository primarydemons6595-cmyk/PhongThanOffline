--Trap Î÷áªtoÓñÈª±ù´¨
--TrapID£º46
--Update: chensong(2004-08-15)½¨Á¢

function main(sel)
	if (IsPlayerInsideWeapon(PlayerIndex)==0)then
		Msg2Player("§· ®Õn Ngäc TuyÒn B¨ng Xuyªn")
		NewWorld(32,2015,2855);
		SetFightState(1)
	else
		Msg2Player("Ngåi xe kh«ng thÓ ®i qua, xin xuèng xe!")
	end;
end;

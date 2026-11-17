-- Adapted loose binding of original script.pak entry 0x381C646E.
-- Authentic transition: PT 2 Chongcheng Camp -> PT 5 Chongcheng Outskirts.
function main(sel)
	Msg2Player("Entering Chongcheng Outskirts")
	NewWorld(5,1808,2943);
	SetFightState(1)
end;

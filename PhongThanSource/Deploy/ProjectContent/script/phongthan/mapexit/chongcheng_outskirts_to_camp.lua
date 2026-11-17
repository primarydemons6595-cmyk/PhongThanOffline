-- Adapted loose binding of original script.pak entry 0x5F310810.
-- Authentic transition: PT 5 Chongcheng Outskirts -> PT 2 Chongcheng Camp.
function main(sel)
	Msg2Player("Entering Chongcheng Camp")
	NewWorld(2,1544,3266);
	SetFightState(0)
end;

function main()
 local r=EventVttLoot(0)
 if r==1 then CloseDialog();return end
 local text="Ch­a thÓ nhÆt r­¬ng. H·y ®øng gÇn r­¬ng, bËt PK chiÕn ®Êu vµ më khãa hµnh trang."
 if r==-2 then text="Chê ®ñ 1 phót tõ lÇn nhÆt r­¬ng tr­íc."
 elseif r==-3 then text="R­¬ng ®ang cã ng­êi nhÆt." end
 Talk(1,"no",text)
end
function pt_vtt_loot_finish()
 local r=EventVttLoot(1)
 if r==1 then Msg2Player("NhËn 1 Th«ng Thiªn Ch©u.")
 elseif r==-4 then Msg2Player("Tói kh«ng ®ñ chç. R­¬ng vÉn cßn, h·y thö l¹i.") end
end
function no() CloseDialog() end

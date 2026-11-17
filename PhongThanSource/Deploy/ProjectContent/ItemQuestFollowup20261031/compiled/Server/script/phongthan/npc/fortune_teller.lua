function no() CloseDialog() end
function main()
 SayTask("ThÇy Bãi: mang mét trong b¶y quÎ ®Õn ®©y, ta sÏ ®æi cho ng­¬i MËt ®å da dª ®Ó t×m kho b¸u.",{{"T×m b¶o vËt","pt_treasure_options"},{"MËt ®å da dª","pt_treasure_about"},{"NhËn l¹i mËt ®å bŞ mÊt","pt_treasure_restore"}})
end
function pt_treasure_options()
 SayButtons("Chän quÎ muèn ®æi lÊy MËt ®å da dª:",8,{"QuÎ Kh«n/pt_treasure_1","QuÎ Kh¶m/pt_treasure_2","QuÎ Ly/pt_treasure_3","QuÎ CÊn/pt_treasure_4","QuÎ §oµi/pt_treasure_5","QuÎ Tèn/pt_treasure_6","QuÎ ChÊn/pt_treasure_7","KÕt thóc/no"})
end
function pt_treasure_accept(n)
 if TreasureItem(1,n)==1 then Talk(1,"no","§· ®æi quÎ lÊy MËt ®å da dª. Chuét ph¶i ®Ó ®äc manh mèi; cã thÓ ®Æt vµo « phİm nhanh 1-4.") else Talk(1,"no","Ch­a thÓ ®æi: ng­¬i cÇn cã quÎ ®· chän, hµnh trang cßn chç vµ ch­a cã nhiÖm vô t×m kho b¸u ®ang thùc hiÖn.") end
end
function pt_treasure_about() Talk(1,"no","MËt ®å da dª sÏ s¸ng dÇn khi ng­¬i ®Õn gÇn kho b¸u. §äc mËt ®å kh«ng lµm mÊt vËt phÈm; chØ më kho b¸u thµnh c«ng míi dïng hÕt mËt ®å.") end
function pt_treasure_restore() if TreasureItem(3,0)==1 then Talk(1,"no","§· cÊp l¹i MËt ®å da dª cho nhiÖm vô ®ang thùc hiÖn.") else Talk(1,"no","Kh«ng cã mËt ®å cÇn cÊp l¹i, vËt phÈm vÉn cßn trong tói/kho, hoÆc hµnh trang ch­a ®ñ chç.") end end
function pt_treasure_1() pt_treasure_accept(1) end
function pt_treasure_2() pt_treasure_accept(2) end
function pt_treasure_3() pt_treasure_accept(3) end
function pt_treasure_4() pt_treasure_accept(4) end
function pt_treasure_5() pt_treasure_accept(5) end
function pt_treasure_6() pt_treasure_accept(6) end
function pt_treasure_7() pt_treasure_accept(7) end

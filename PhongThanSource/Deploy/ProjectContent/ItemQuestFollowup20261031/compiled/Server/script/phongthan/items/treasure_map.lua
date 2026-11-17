function no() CloseDialog() end
PT_TREASURE_CLUES={[1016]="Th¹ch L©m Tiªu BÝch - ¶i Tam S¬n",[1065]="Vïng ®Êt Èm ­ít / Phong Yªn - ¶i TrÇn §­êng",[1008]="Mang Mang TuyÕt Nguyªn - ch©n nói C«n L«n",[1015]="Th¹ch KiÒu TiÓu KÝnh - M¹nh T©n",[1014]="Phong DiÖp Thô L©m - §ång Quan",[1011]="Lôc L©m Th©m Sø - ¶i Du Hån"}
function main(item)
 local ok,map,distance,last=TreasureItem(0,item)
 if ok~=1 then Talk(1,"no","MËt ®å ch­a cã manh mèi ®ang thùc hiÖn. H·y ®Õn ThÇy Bãi ë TriÒu Ca (212,187).");return end
 if distance<0 then Talk(1,"no","Manh mèi trªn mËt ®å: <color=green>"..(PT_TREASURE_CLUES[map] or tostring(map)).."<color>.");return end
 local light="¸nh s¸ng yÕu"
 if distance<=2500 then light="¸nh s¸ng mê ¶o" end
 if distance<=400 then light="¸nh s¸ng m¹nh" end
 if distance<=25 then SayButtons("MËt ®å ph¸t ra ¸nh s¸ng chãi chang. Kho b¸u ë rÊt gÇn!",2,{"Më kho b¸u/pt_treasure_dig","KÕt thóc/no"});return end
 local direction="B¹n ®ang tiÕn gÇn kho b¸u."
 if last>0 and distance>last then direction="B¹n ®ang ®i xa kho b¸u." end
 Talk(1,"no","MËt ®å ph¸t ra <color=yellow>"..light.."<color>. "..direction)
end
function pt_treasure_dig()
 if TreasureItem(2,0)==1 then Talk(1,"no","§· më kho b¸u vµ nhËn phÇn th­ëng.") else Talk(1,"no","Ch­a thÓ më kho b¸u: h·y ®Õn ®óng vÞ trÝ, gi÷ mËt ®å trong hµnh trang hoÆc « phÝm nhanh vµ chõa chç nhËn quµ. MËt ®å vÉn ®­îc gi÷ nguyªn.") end
end

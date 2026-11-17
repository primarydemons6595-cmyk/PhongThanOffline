PT_COMPASS_MAPS={[1001]="Phong ThÇn ®µi",[1002]="Sïng thµnh ®¹i doanh",[1003]="Ngäc H­ cung",[1004]="Xi V­u Mé",[1005]="Sïng thµnh",[1006]="B¾c H¶i",[1007]="YÕn S¬n",[1008]="Ch©n nói C«n L«n",[1009]="T©y C«n L«n",[1010]="Thñ D­¬ng s¬n",[1011]="Du Hån",[1012]="Miªu C­¬ng",[1013]="Cù Léc",[1014]="§ång Quan",[1015]="M¹nh T©n",[1016]="Tam S¬n ",[1017]="Kú S¬n",[1018]="Môc D·",[1019]="TuyÖt Long lÜnh",[1020]="T©y Kú",[1021]="TriÒu Ca",[1022]="Hoang m¹c",[1023]="Sa M¹c Thæ Thµnh",[1024]="Sa M¹c Phong Than",[1025]="Sa M¹c Lôc Ch©u",[1026]="Sa M¹c chÕt",[1027]="Hiªn Viªn ®éng tÇng 1 ",[1028]="Hiªn Viªn ®éng tÇng 2 ",[1029]="Hiªn Viªn ®éng tÇng 3 ",[1030]="Hiªn Viªn ®éng tÇng 4 ",[1031]="Hiªn Viªn ®éng tÇng 5 ",[1032]="Ngäc TuyÒn B¨ng Xuyªn",[1033]="B¨ng Xuyªn TuyÕt Cèc",[1034]="§¹i Phong B¨ng Xuyªn",[1035]="§¹i Th¹ch B¨ng Xuyªn",[1036]="B¨ng Xuyªn Chi Cùc",[1037]="§«ng H¶i Thñy Vùc",[1038]="§«ng H¶i Long Cung",[1039]="§«ng H¶i H¶i C©u",[1040]="§«ng H¶i Long Vùc",[1041]="§«ng H¶i Long Uyªn",[1042]="BÝch Du cung tÇng 1 ",[1043]="BÝch Du cung tÇng 2",[1044]="BÝch Du cung tÇng 3",[1045]="BÝch Du cung tÇng 4",[1046]="BÝch Du cung tÇng 5",[1047]="Khæn Tiªn cung tÇng 1",[1048]="Khæn Tiªn cung tÇng 2",[1049]="Khæn Tiªn cung tÇng 3",[1050]="Khæn Tiªn cung tÇng 4",[1051]="Khæn Tiªn cung tÇng 5",[1052]="Diªu Tr×",[1053]="§¹i H¶i",[1054]="Bång Lai",[1055]="§«ng Doanh",[1056]="Ph­¬ng Tr­îng",[1057]="Kho¸ng tr­êng",[1058]="Ò©Íõ¹È",[1059]="³ç³Ç¿ó³¡",[1060]="Thiªn Lao",[1061]="Ngäc H­ cung 10 n¨m tr­íc",[1062]="Ngäc H­ cung 10 n¨m sau",[1063]="TriÒu Ca 10 n¨m sau",[1064]="ChiÕn tr­êng ViÔn Cæ n¨m 2000 tr­íc",[1065]="TrÇn §­êng",[1066]="Tr­ Lung Thµnh Tr¹i",[1067]="V¹n Tiªn trËn (Thæ)",[1068]="V¹n Tiªn trËn (Thñy)",[1069]="V¹n Tiªn trËn (Háa)",[1070]="V¹n Tiªn trËn (Phong)",[1071]="ChiÕn tr­êng Th­¬ng Chu",[1072]="Khai Minh ®¶o",[1073]="Giai Méng",[1074]="Ngôc Ph¸p s¬n",[1075]="Bé L¹c Di Ph­¬ng",[1076]="B¶n TuyÒn Th¸nh §Þa",[1077]="L­u Ba s¬n",[1078]="Kh«ng Tang Linh S¬n",[1092]="ÌìÀÎ",[1093]="ÌìÀÎ",[1094]="ÌìÀÎ",[1095]="ÌìÀÎ",[1096]="ÌìÀÎ",[1097]="Diªm La ®iÖn",[1098]="Hoµng TuyÒn phñ",[1099]="Nam Kha quËn"}

function no() CloseDialog() end
function main(item)
 if TravelItem(0,item)~=12 then return end
 SayTask("ChØ Nam Ch©u cã thÓ ®­a ng­¬i ®Õn n¬i ®· ghi nhí bÊt kú! H·y chän chøc n¨ng!",{{"Ghi nhí","pt_compass_save"},{"ChuyÓn tiÕp","pt_compass_go"},{"Xãa","pt_compass_clear"}})
end
function pt_compass_menu(action)
 local choices={}
 for i=1,5 do
  local ok,map,x,y=CompassItem(0,i)
  local label="§Þa ®iÓm "..i
  if ok==1 then label=label..": <color=green>"..(PT_COMPASS_MAPS[map] or tostring(map)).."<color> ("..x..":"..y..")" end
  tinsert(choices,label.."/pt_compass_"..action..i)
 end
 tinsert(choices,"KÕt thóc/no")
 local prompt="H·y chän ®Þa ®iÓm cÇn ghi nhí:"
 if action=="go" then prompt="H·y chän ®Þa ®iÓm cÇn chuyÓn ®Õn:" end
 if action=="clear" then prompt="H·y chän ®Þa ®iÓm cÇn xãa:" end
 SayButtons(prompt,getn(choices),choices)
end
function pt_compass_save() pt_compass_menu("save") end
function pt_compass_go() pt_compass_menu("go") end
function pt_compass_clear() pt_compass_menu("clear") end
function pt_compass_apply(op,slot)
 local ok=CompassItem(op,slot)
 if ok~=1 then Msg2Player("Kh«ng thÓ thùc hiÖn t¹i ®©y. VÞ trÝ vµ l­ît sö dông ®­îc gi÷ nguyªn.");return end
 if op==1 then Msg2Player("§· ghi nhí vÞ trÝ. Kh«ng tiªu hao l­ît sö dông.") end
 if op==3 then Msg2Player("§· xãa vÞ trÝ. Kh«ng tiªu hao l­ît sö dông.") end
 CloseDialog()
end
function pt_compass_save1() local ok=CompassItem(0,1);if ok==1 then MsgBox("Ghi ®Ì vÞ trÝ ®· l­u?","pt_compass_confirm1","no") else pt_compass_apply(1,1) end end
function pt_compass_confirm1() pt_compass_apply(1,1) end
function pt_compass_go1() pt_compass_apply(2,1) end
function pt_compass_clear1() pt_compass_apply(3,1) end
function pt_compass_save2() local ok=CompassItem(0,2);if ok==1 then MsgBox("Ghi ®Ì vÞ trÝ ®· l­u?","pt_compass_confirm2","no") else pt_compass_apply(1,2) end end
function pt_compass_confirm2() pt_compass_apply(1,2) end
function pt_compass_go2() pt_compass_apply(2,2) end
function pt_compass_clear2() pt_compass_apply(3,2) end
function pt_compass_save3() local ok=CompassItem(0,3);if ok==1 then MsgBox("Ghi ®Ì vÞ trÝ ®· l­u?","pt_compass_confirm3","no") else pt_compass_apply(1,3) end end
function pt_compass_confirm3() pt_compass_apply(1,3) end
function pt_compass_go3() pt_compass_apply(2,3) end
function pt_compass_clear3() pt_compass_apply(3,3) end
function pt_compass_save4() local ok=CompassItem(0,4);if ok==1 then MsgBox("Ghi ®Ì vÞ trÝ ®· l­u?","pt_compass_confirm4","no") else pt_compass_apply(1,4) end end
function pt_compass_confirm4() pt_compass_apply(1,4) end
function pt_compass_go4() pt_compass_apply(2,4) end
function pt_compass_clear4() pt_compass_apply(3,4) end
function pt_compass_save5() local ok=CompassItem(0,5);if ok==1 then MsgBox("Ghi ®Ì vÞ trÝ ®· l­u?","pt_compass_confirm5","no") else pt_compass_apply(1,5) end end
function pt_compass_confirm5() pt_compass_apply(1,5) end
function pt_compass_go5() pt_compass_apply(2,5) end
function pt_compass_clear5() pt_compass_apply(3,5) end

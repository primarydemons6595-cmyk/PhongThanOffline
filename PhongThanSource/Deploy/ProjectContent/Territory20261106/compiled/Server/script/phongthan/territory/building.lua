function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","§· thùc hiÖn thµnh c«ng.")
 elseif result==2 then Talk(1,"pt_no","Giao dÞch ®ang chê l­u. Kh«ng thùc hiÖn l¹i; server sÏ tù hoµn tÊt.")
 else Talk(1,"pt_no","Kh«ng thùc hiÖn ®­îc. H·y kiÓm tra quyÒn, vËt phÈm mang theo, ng©n l­îng, nh©n khÝ vµ ®iÒu kiÖn cña thµnh.") end
end
function main()
 SayTask("Thµnh thÞ l·nh ®Þa. ChØ ng­êi cã quyÒn qu¶n lý ®­îc x©y/n©ng cÊp; gi¸ vµ giíi h¹n theo b¶ng gèc. Studio cã b¶n ®å vµ thuéc tÝnh chi tiÕt.",{
 {"Th«ng tin thµnh","pt_info";show=1},{"X©y c«ng tr×nh t¹i chç ®øng","pt_page0";show=1},{"N©ng cÊp thµnh","pt_upgrade";show=1},{"Cèng hiÕn t¹i th­¬ng khè","pt_donate";show=1},{"VÒ Phong ThÇn §µi","pt_exit";show=1}})
end
function pt_info() Talk(1,"pt_no","CÊp thµnh: "..TerritoryValue(1).." - Nh©n khÝ: "..TerritoryValue(0).." - Phån vinh: "..TerritoryValue(7).." - Cèng hiÕn cña b¹n: "..TerritoryValue(2)) end
function pt_upgrade() pt_result(TerritoryAction(2,0)) end
function pt_exit() pt_result(TerritoryAction(4,0)) end
function pt_donate() Talk(1,"pt_no","§Õn th­¬ng khè cña thµnh hoÆc qu¶n lý kho ë Phong ThÇn §µi ®Ó hiÕn tµi nguyªn.") end
function pt_page0() SayTask("Chän c«ng tr×nh. VÞ trÝ x©y lµ chç nh©n vËt ®ang ®øng; cÇn ®ñ ng©n quü vµ tµi nguyªn.",{{"Cæng thµnh","pt_build0";show=1},{"Th¸p canh","pt_build1";show=1},{"VËt tæ Th­¬ng Lang","pt_build2";show=1},{"Thao tr­êng","pt_build3";show=1},{"Phßng chiÕn xa","pt_build4";show=1},{"Tr¹i lÝnh","pt_build5";show=1},{"Trang tiÕp","pt_page1";show=1}}) end
function pt_page1() SayTask("Chän c«ng tr×nh. VÞ trÝ x©y lµ chç nh©n vËt ®ang ®øng; cÇn ®ñ ng©n quü vµ tµi nguyªn.",{{"Th­¬ng khè","pt_build6";show=1},{"T­íng ®µi","pt_build7";show=1},{"LÇu trèng","pt_build8";show=1},{"TÕ ®µn","pt_build9";show=1},{"Phßng luyÖn ®¬n","pt_build10";show=1},{"§iÖn Lu©n Håi","pt_build11";show=1},{"Trang tiÕp","pt_page2";show=1},{"Trang tr­íc","pt_page0";show=1}}) end
function pt_page2() SayTask("Chän c«ng tr×nh. VÞ trÝ x©y lµ chç nh©n vËt ®ang ®øng; cÇn ®ñ ng©n quü vµ tµi nguyªn.",{{"Phong ThÇn §iÖn","pt_build12";show=1},{"Lß luyÖn","pt_build13";show=1},{"HuyÒn ®µn","pt_build14";show=1},{"D­îc ®iÕm","pt_build15";show=1},{"§µi håi sinh","pt_build16";show=1},{"VËt tæ Thanh Ng­u","pt_build17";show=1},{"Trang tiÕp","pt_page3";show=1},{"Trang tr­íc","pt_page1";show=1}}) end
function pt_page3() SayTask("Chän c«ng tr×nh. VÞ trÝ x©y lµ chç nh©n vËt ®ang ®øng; cÇn ®ñ ng©n quü vµ tµi nguyªn.",{{"VËt tæ Phi Hïng","pt_build18";show=1},{"S©n luyÖn thó","pt_build19";show=1},{"HÝ viÖn","pt_build20";show=1},{"Cèi xay","pt_build21";show=1},{"C©y 1","pt_build22";show=1},{"C©y 2","pt_build23";show=1},{"Trang tiÕp","pt_page4";show=1},{"Trang tr­íc","pt_page2";show=1}}) end
function pt_page4() SayTask("Chän c«ng tr×nh. VÞ trÝ x©y lµ chç nh©n vËt ®ang ®øng; cÇn ®ñ ng©n quü vµ tµi nguyªn.",{{"C©y 3","pt_build24";show=1},{"C©y 4","pt_build25";show=1},{"C©y 5","pt_build26";show=1},{"Bån hoa 1","pt_build27";show=1},{"Bån hoa 2","pt_build28";show=1},{"Bån hoa 3","pt_build29";show=1},{"Trang tiÕp","pt_page5";show=1},{"Trang tr­íc","pt_page3";show=1}}) end
function pt_page5() SayTask("Chän c«ng tr×nh. VÞ trÝ x©y lµ chç nh©n vËt ®ang ®øng; cÇn ®ñ ng©n quü vµ tµi nguyªn.",{{"Bån hoa 4","pt_build30";show=1},{"Bån hoa 5","pt_build31";show=1},{"§ång T©m KÕt","pt_build32";show=1},{"Hoa §oµn CÈm Tó","pt_build33";show=1},{"Trang tr­íc","pt_page4";show=1}}) end
function pt_build0() pt_result(TerritoryAction(3,0)) end
function pt_build1() pt_result(TerritoryAction(3,1)) end
function pt_build2() pt_result(TerritoryAction(3,2)) end
function pt_build3() pt_result(TerritoryAction(3,3)) end
function pt_build4() pt_result(TerritoryAction(3,4)) end
function pt_build5() pt_result(TerritoryAction(3,5)) end
function pt_build6() pt_result(TerritoryAction(3,6)) end
function pt_build7() pt_result(TerritoryAction(3,7)) end
function pt_build8() pt_result(TerritoryAction(3,8)) end
function pt_build9() pt_result(TerritoryAction(3,9)) end
function pt_build10() pt_result(TerritoryAction(3,10)) end
function pt_build11() pt_result(TerritoryAction(3,11)) end
function pt_build12() pt_result(TerritoryAction(3,12)) end
function pt_build13() pt_result(TerritoryAction(3,13)) end
function pt_build14() pt_result(TerritoryAction(3,14)) end
function pt_build15() pt_result(TerritoryAction(3,15)) end
function pt_build16() pt_result(TerritoryAction(3,16)) end
function pt_build17() pt_result(TerritoryAction(3,17)) end
function pt_build18() pt_result(TerritoryAction(3,18)) end
function pt_build19() pt_result(TerritoryAction(3,19)) end
function pt_build20() pt_result(TerritoryAction(3,20)) end
function pt_build21() pt_result(TerritoryAction(3,21)) end
function pt_build22() pt_result(TerritoryAction(3,22)) end
function pt_build23() pt_result(TerritoryAction(3,23)) end
function pt_build24() pt_result(TerritoryAction(3,24)) end
function pt_build25() pt_result(TerritoryAction(3,25)) end
function pt_build26() pt_result(TerritoryAction(3,26)) end
function pt_build27() pt_result(TerritoryAction(3,27)) end
function pt_build28() pt_result(TerritoryAction(3,28)) end
function pt_build29() pt_result(TerritoryAction(3,29)) end
function pt_build30() pt_result(TerritoryAction(3,30)) end
function pt_build31() pt_result(TerritoryAction(3,31)) end
function pt_build32() pt_result(TerritoryAction(3,32)) end
function pt_build33() pt_result(TerritoryAction(3,33)) end

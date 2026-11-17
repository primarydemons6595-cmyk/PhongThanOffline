function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","§· thùc hiÖn thµnh c«ng.")
 elseif result==2 then Talk(1,"pt_no","Giao dÞch ®ang chê l­u. Kh«ng thùc hiÖn l¹i; server sÏ tù hoµn tÊt.")
 else Talk(1,"pt_no","Kh«ng thùc hiÖn ®­îc. H·y kiÓm tra quyÒn, vËt phÈm mang theo, ng©n l­îng, nh©n khÝ vµ ®iÒu kiÖn cña thµnh.") end
end
function main()
 SayTask("LËp thµnh cho l·nh ®Þa: cÇn bèn s¸ch lËp thµnh mang trong tói, danh väng vµ nh©n khÝ ®ñ ®iÒu kiÖn. Ng©n quü l·nh ®Þa tr¶ gi¸ ®Êt/®ång thau; nh©n vËt tr¶ lÖ phÝ. Chän vïng ®Êt:",{
 {"Hoµng Sa Thµnh","pt_land0";show=1},{"Hång DiÖp Thµnh","pt_land1";show=1},{"H¾c ¸m Thµnh","pt_land2";show=1},{"Thu Phong Thµnh","pt_land3";show=1},{"H¹ NhËt Thµnh","pt_land4";show=1}})
end
function pt_land0() SayTask("Chän vËt tæ cho thµnh:",{{"Th­¬ng Lang","pt_create0_0";show=1},{"Thanh Ng­u","pt_create0_1";show=1},{"Phi Hïng","pt_create0_2";show=1}}) end
function pt_create0_0() pt_result(TerritoryCreate(0,0)) end
function pt_create0_1() pt_result(TerritoryCreate(0,1)) end
function pt_create0_2() pt_result(TerritoryCreate(0,2)) end
function pt_land1() SayTask("Chän vËt tæ cho thµnh:",{{"Th­¬ng Lang","pt_create1_0";show=1},{"Thanh Ng­u","pt_create1_1";show=1},{"Phi Hïng","pt_create1_2";show=1}}) end
function pt_create1_0() pt_result(TerritoryCreate(1,0)) end
function pt_create1_1() pt_result(TerritoryCreate(1,1)) end
function pt_create1_2() pt_result(TerritoryCreate(1,2)) end
function pt_land2() SayTask("Chän vËt tæ cho thµnh:",{{"Th­¬ng Lang","pt_create2_0";show=1},{"Thanh Ng­u","pt_create2_1";show=1},{"Phi Hïng","pt_create2_2";show=1}}) end
function pt_create2_0() pt_result(TerritoryCreate(2,0)) end
function pt_create2_1() pt_result(TerritoryCreate(2,1)) end
function pt_create2_2() pt_result(TerritoryCreate(2,2)) end
function pt_land3() SayTask("Chän vËt tæ cho thµnh:",{{"Th­¬ng Lang","pt_create3_0";show=1},{"Thanh Ng­u","pt_create3_1";show=1},{"Phi Hïng","pt_create3_2";show=1}}) end
function pt_create3_0() pt_result(TerritoryCreate(3,0)) end
function pt_create3_1() pt_result(TerritoryCreate(3,1)) end
function pt_create3_2() pt_result(TerritoryCreate(3,2)) end
function pt_land4() SayTask("Chän vËt tæ cho thµnh:",{{"Th­¬ng Lang","pt_create4_0";show=1},{"Thanh Ng­u","pt_create4_1";show=1},{"Phi Hïng","pt_create4_2";show=1}}) end
function pt_create4_0() pt_result(TerritoryCreate(4,0)) end
function pt_create4_1() pt_result(TerritoryCreate(4,1)) end
function pt_create4_2() pt_result(TerritoryCreate(4,2)) end

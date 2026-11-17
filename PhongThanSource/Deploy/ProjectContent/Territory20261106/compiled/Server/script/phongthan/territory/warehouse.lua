function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","§· thùc hiÖn thµnh c«ng.")
 elseif result==2 then Talk(1,"pt_no","Giao dŞch ®ang chê l­u. Kh«ng thùc hiÖn l¹i; server sÏ tù hoµn tÊt.")
 else Talk(1,"pt_no","Kh«ng thùc hiÖn ®­îc. H·y kiÓm tra quyÒn, vËt phÈm mang theo, ng©n l­îng, nh©n khİ vµ ®iÒu kiÖn cña thµnh.") end
end
function main()
 if IsTongMember()~=1 then Talk(1,"pt_no","B¹n cÇn gia nhËp l·nh ®Şa tr­íc.");return end
 SayTask("Cèng hiÕn tµi nguyªn cho l·nh ®Şa. ChØ dïng vËt phÈm trong bèn tói mang theo, kh«ng dïng ®å ®· khãa. §ång thau khai kho¸ng vµ ®ång thau gèc ®Òu ®­îc nhËn. §iÓm mçi ngµy theo cÊu h×nh Studio.",{
 {"250 ®ång thau (1 ®iÓm)","pt_bronze1";show=1},{"2.500 ®ång thau (10 ®iÓm)","pt_bronze2";show=1},
 {"50.000 l­îng (kh«ng ®iÓm)","pt_money0";show=1},{"500.000 l­îng (1 ®iÓm)","pt_money1";show=1},{"5.000.000 l­îng (10 ®iÓm)","pt_money2";show=1}})
end
function pt_money0() pt_result(TerritoryDonate(0,0)) end
function pt_money1() pt_result(TerritoryDonate(0,1)) end
function pt_money2() pt_result(TerritoryDonate(0,2)) end
function pt_bronze1() pt_result(TerritoryDonate(1,1)) end
function pt_bronze2() pt_result(TerritoryDonate(1,2)) end

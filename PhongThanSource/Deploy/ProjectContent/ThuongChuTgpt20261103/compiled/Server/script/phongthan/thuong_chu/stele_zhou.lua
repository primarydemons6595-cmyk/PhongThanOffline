function main() Say("ChiÕn tr­êng Th­¬ng Chu - chän mét d¹ng trËn chung cho toµn phiªn",8,"B¸o danh/pt_tc_join","Vµo chiÕn tr­êng/pt_tc_enter","T×nh h×nh phiªn/pt_tc_status","Danh väng vµ c«ng tr¹ng/pt_tc_saved","Mua buff 2 ngµy/pt_tc_buff","Thuª trang bŞ/pt_tc_rent","H­íng dÉn/pt_tc_help","KÕt thóc/pt_tc_close") end
function pt_tc_join() if ThuongChu(1,2)==1 then Talk(1,"main","§· b¸o danh. HÕt 10 phót b¸o danh h·y chän Vµo chiÕn tr­êng.") else Talk(1,"main","Ch­a b¸o danh ®­îc. KiÓm tra cÊp yªu cÇu, giê b¸o danh, c©n b»ng phe vµ nhËn hÕt th­ëng trËn tr­íc.") end end
function pt_tc_enter() if ThuongChu(2)==0 then Talk(1,"main","CÇn b¸o danh ®óng phe vµ chê b¾t ®Çu thi ®Êu.") end end
function pt_tc_help() Talk(1,"main","B¸o danh 12h vµ 20h trong 10 phót, thi ®Êu 30 phót. İt nhÊt 5 ng­êi mçi phe. Mét d¹ng chiÕn tr­êng ®­îc chän chung cho c¶ phiªn. NhËn VŞ Quèc LËp C«ng ë V¨n Träng hoÆc Kh­¬ng Th­îng. TrËn Thèng So¸i: sau 10 phót xuÊt hiÖn boss, h¹ boss råi ®o¹t cê vµ mang vÒ nép; phe nép ®ñ 3 cê So¸i ®èi ph­¬ng th¾ng ngay.") end

function pt_tc_close() CloseDialog() end
function pt_tc_status()
 local names={"TruyÒn thèng","ViÔn Cæ","ViÔn Cæ Thèng So¸i"}
 local mode=ThuongChu(19);local name="Ch­a më";if mode>0 then name=names[mode] end
 Talk(1,"main","Phiªn nµy: "..name..". §iÓm Th­¬ng/Chu: "..ThuongChu(7,1).."/"..ThuongChu(7,2)..". Cê So¸i Th­¬ng/Chu: "..ThuongChu(26,1).."/"..ThuongChu(26,2)..".")
end
function pt_tc_saved() Talk(1,"main","Tİch lòy: "..ThuongChu(12).."; c«ng tr¹ng: "..ThuongChu(15).."; danh väng Th­¬ng: "..ThuongChu(14,1).."; danh väng Chu: "..ThuongChu(14,2)..".") end
function pt_tc_reward() local xp=ThuongChu(8);if xp>0 then Talk(1,"main","§· nhËn "..xp.." kinh nghiÖm.") else Talk(1,"main","Kh«ng cã th­ëng chê nhËn t¹i ®©y.") end end
function pt_tc_buff() if ThuongChu(25)==1 then Talk(1,"main","§· dïng 1200 ®iÓm tİch lòy: t¨ng 10% sinh lùc, néi lùc vµ xuÊt chiªu phï hîp hÖ ph¸i trong 2 ngµy.") else Talk(1,"main","CÇn 1200 ®iÓm tİch lòy vµ ch­a cã buff Th­¬ng Chu cßn h¹n.") end end
function pt_tc_rent() Say("Thuª 3 ngµy, trang bŞ nhiÖm vô khãa. Gi¸p +6: 20 T­íng Ên vµ 15000 c«ng tr¹ng. Vò khİ: 30 T­íng Ên vµ 20000 c«ng tr¹ng.",5,"Trang bŞ lôc 8x/pt_tc_armor80","Trang bŞ lôc 10x/pt_tc_armor100","Vò khİ 9x/pt_tc_weapon90","Vò khİ 10x/pt_tc_weapon100","Quay l¹i/main") end
function pt_tc_rented(ok) if ok==1 then Talk(1,"main","§· nhËn trang bŞ thuª, h¹n dïng 3 ngµy kÓ tõ lóc nhËn.") else Talk(1,"main","Ch­a thuª ®­îc. KiÓm tra T­íng Ên, c«ng tr¹ng, chç trèng vµ trang bŞ ®óng hÖ ph¸i.") end end
function pt_tc_armor80() pt_tc_rented(ThuongChu(22,80)) end
function pt_tc_armor100() pt_tc_rented(ThuongChu(22,100)) end
function pt_tc_weapon90() pt_tc_rented(ThuongChu(23,90)) end
function pt_tc_weapon100() pt_tc_rented(ThuongChu(23,100)) end

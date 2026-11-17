function main()
 if ThuongChu(10)==0 then Talk(1,"pt_tc_close","Ng­¬i ch­a b¸o danh phiªn nµy.");return end
 Say("VŞ Quèc LËp C«ng: "..ThuongChu(21).."/10 l­ît h«m nay. Mçi nhiÖm vô 4 v¹n; 3 l­ît ®Çu kh«ng tèn tiÒn ®ång, l­ît 4-10 thªm 3 tiÒn ®ång.",12,"H¹ 10 qu©n tiªn phong/pt_tc_killquest","C­íp 10 cê ®èi thñ/pt_tc_flagquest","H¹ 10 th¸m qu©n/pt_tc_scoutquest","Ph¸ 3 vËt tæ/pt_tc_totemquest","TiÕn ®é nhiÖm vô/pt_tc_progress","Tr¶ nhiÖm vô/pt_tc_finish","Hoµn thµnh nhanh - 5 tiÒn ®ång/pt_tc_quick","§æi nhiÖm vô - 1 tiÒn ®ång/pt_tc_change","Nép cê So¸i hoÆc cê ®èi thñ/pt_tc_deliver","T×nh h×nh chiÕn tr­êng/pt_tc_status","Rêi chiÕn tr­êng/pt_tc_leave","KÕt thóc/pt_tc_close")
end
function pt_tc_take(n) if ThuongChu(4,n)==1 then Talk(1,"main","§· nhËn nhiÖm vô. Tr¶ nhiÖm vô ®­îc 10 ®iÓm tİch lòy vµ 15 danh väng cña phe m×nh.") else Talk(1,"main","CÇn ë doanh tr¹i phe m×nh, ch­a cã nhiÖm vô, cßn l­ît ngµy, ®ñ b¹c vµ tiÒn ®ång.") end end
function pt_tc_killquest() pt_tc_take(1) end
function pt_tc_flagquest() pt_tc_take(2) end
function pt_tc_scoutquest() pt_tc_take(3) end
function pt_tc_totemquest() pt_tc_take(4) end
function pt_tc_finish() if ThuongChu(5)==1 then Talk(1,"main","§· tr¶ nhiÖm vô: +10 tİch lòy, +15 danh väng phe.") else Talk(1,"main","Ch­a ®ñ tiÕn ®é hoÆc ch­a vÒ doanh tr¹i ®óng phe.") end end
function pt_tc_quick() if ThuongChu(16)==1 then Talk(1,"main","§· dïng 5 tiÒn ®ång hoµn thµnh nhiÖm vô: +10 tİch lòy, +15 danh väng phe.") else Talk(1,"main","CÇn nhiÖm vô ®ang nhËn vµ 5 tiÒn ®ång, ®øng t¹i doanh tr¹i ®óng phe.") end end
function pt_tc_change() Say("§æi nhiÖm vô: 1 tiÒn ®ång, kh«ng t¨ng l­ît vµ kh«ng thu thªm b¹c.",5,"H¹ qu©n tiªn phong/pt_tc_change1","C­íp cê/pt_tc_change2","H¹ th¸m qu©n/pt_tc_change3","Ph¸ vËt tæ/pt_tc_change4","Quay l¹i/main") end
function pt_tc_changed(n) if ThuongChu(17,n)==1 then Talk(1,"main","§· ®æi nhiÖm vô, tiÕn ®é míi tİnh l¹i tõ ®Çu.") else Talk(1,"main","CÇn nhiÖm vô ®ang nhËn, chän nhiÖm vô kh¸c vµ cã 1 tiÒn ®ång.") end end
function pt_tc_change1() pt_tc_changed(1) end
function pt_tc_change2() pt_tc_changed(2) end
function pt_tc_change3() pt_tc_changed(3) end
function pt_tc_change4() pt_tc_changed(4) end
function pt_tc_progress() Talk(1,"main","Lo¹i nhiÖm vô: "..ThuongChu(11).." (1: tiªn phong, 2: cê, 3: th¸m qu©n, 4: vËt tæ); tiÕn ®é: "..ThuongChu(20)..". §ang mang cê: "..ThuongChu(27)..".") end
function pt_tc_deliver() if ThuongChu(18)==1 then Talk(1,"main","§· nép cê cho phe m×nh.") else Talk(1,"main","CÇn ®ang mang cê ®èi ph­¬ng vµ vÒ ®óng doanh tr¹i phe m×nh.") end end
function pt_tc_leave() ThuongChu(3) end

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

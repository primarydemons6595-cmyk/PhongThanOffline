function pt_books_close() CloseDialog() end
function pt_books_open()
 SayButtons("<color=yellow>S¸ch Ch­ HÇu<color><enter>M¶nh th­êng: "..TerritoryBookCount(0).."; m¶nh Tiªn Ma: "..TerritoryBookCount(1)..".<enter>Gãp m¶nh vµo l·nh ®Þa; l·nh chóa ®æi s¸ch ®Ó lËp thµnh. S¸ch (Khëi/Thõa/ChuyÓn/Hîp) lµ bèn nguyªn liÖu lËp thµnh t¹i Th­¬ng Thang. M¶nh th­êng cÇn cÊp 15; m¶nh Tiªn Ma cÇn cÊp Tiªn Ma 14. Mçi m¶nh céng 1 cèng hiÕn. M¶nh th­êng th­ëng EXP cÊp x 5.000 mét lÇn/ngµy.",5,"Gãp m¶nh th­êng/pt_books_donate","Gãp m¶nh Tiªn Ma/pt_books_realm","§æi s¸ch - m¶nh th­êng/pt_books_pool0","§æi s¸ch - m¶nh Tiªn Ma/pt_books_pool1","KÕt thóc/pt_books_close")
end
function pt_books_done(result) if result==1 then Talk(1,"pt_books_open","§· hoµn tÊt giao dÞch S¸ch Ch­ HÇu.") elseif result==2 then Talk(1,"pt_books_close","Giao dÞch ®ang chê l­u vµo nh©n vËt; server sÏ tù hoµn tÊt. Kh«ng thùc hiÖn l¹i.") else Talk(1,"pt_books_open","Kh«ng thùc hiÖn ®­îc. §øng gÇn ®óng NPC, kiÓm tra l·nh ®Þa, cÊp, quyÒn l·nh chóa, m¶nh mang theo ch­a khãa vµ « trèng.") end end
function pt_books_donate() pt_books_done(TerritoryBook(0,0)) end
function pt_books_realm() pt_books_done(TerritoryBook(1,0)) end
function pt_books_pool0() SayButtons("§æi s¸ch. Sè m¶nh/quyÓn mÆc ®Þnh 250, chØnh ®­îc trong Studio. Lý Thiªn V­¬ng gèc ®æi ChuyÓn/Hîp; Loose bæ sung Khëi/Thõa t¹i cïng dÞch vô ®Ó hoµn thµnh bé bèn s¸ch lËp thµnh.",5,"Khëi/pt_books_0_0","Thõa/pt_books_0_1","ChuyÓn/pt_books_0_2","Hîp/pt_books_0_3","KÕt thóc/pt_books_close") end
function pt_books_0_0() pt_books_done(TerritoryBook(0,58)) end
function pt_books_0_1() pt_books_done(TerritoryBook(0,59)) end
function pt_books_0_2() pt_books_done(TerritoryBook(0,60)) end
function pt_books_0_3() pt_books_done(TerritoryBook(0,61)) end
function pt_books_pool1() SayButtons("§æi s¸ch. Sè m¶nh/quyÓn mÆc ®Þnh 250, chØnh ®­îc trong Studio. Lý Thiªn V­¬ng gèc ®æi ChuyÓn/Hîp; Loose bæ sung Khëi/Thõa t¹i cïng dÞch vô ®Ó hoµn thµnh bé bèn s¸ch lËp thµnh.",5,"Khëi/pt_books_1_0","Thõa/pt_books_1_1","ChuyÓn/pt_books_1_2","Hîp/pt_books_1_3","KÕt thóc/pt_books_close") end
function pt_books_1_0() pt_books_done(TerritoryBook(1,58)) end
function pt_books_1_1() pt_books_done(TerritoryBook(1,59)) end
function pt_books_1_2() pt_books_done(TerritoryBook(1,60)) end
function pt_books_1_3() pt_books_done(TerritoryBook(1,61)) end

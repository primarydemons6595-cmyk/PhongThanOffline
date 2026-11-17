function main()
 if ThuongChu(10)==0 then Talk(1,"pt_tc_close","Ng­¬i ch­a tham gia trËn Th­¬ng Chu nµy.");return end
 Say("VŞ Quèc LËp C«ng",7,"H¹ qu©n Tiªn Phong/pt_tc_killquest","Tiªu diÖt Nguyªn So¸i/pt_tc_marshalquest","Tr¶ nhiÖm vô/pt_tc_finish","T×nh h×nh chiÕn tr­êng/pt_tc_status","Tİch lòy c¸ nh©n/pt_tc_points","Rêi chiÕn tr­êng/pt_tc_leave","KÕt thóc/pt_tc_close")
end
function pt_tc_killquest() if ThuongChu(4,1)==1 then Talk(1,"main","H·y tiªu diÖt 10 nh©n vËt phe ®èi ph­¬ng råi quay l¹i tr¶ nhiÖm vô.") else Talk(1,"main","Ch­a nhËn ®­îc: ®ang cã nhiÖm vô hoÆc ®· hoµn thµnh môc nµy.") end end
function pt_tc_marshalquest() if ThuongChu(4,2)==1 then Talk(1,"main","Tham gia tiªu diÖt mét Nguyªn So¸i råi quay l¹i tr¶ nhiÖm vô.") else Talk(1,"main","Ch­a nhËn ®­îc: ®ang cã nhiÖm vô hoÆc ®· hoµn thµnh môc nµy.") end end
function pt_tc_finish() if ThuongChu(5)==1 then Talk(1,"main","§· x¸c nhËn chiÕn c«ng. Hoµn thµnh c¶ hai môc sÏ nhËn GiÊy Chøng NhËn Th­¬ng Chu.") else Talk(1,"main","Ch­a ®ñ ®iÒu kiÖn tr¶ nhiÖm vô hoÆc tói kh«ng cßn chç nhËn th­ëng.") end end
function pt_tc_status() Talk(1,"main","§iÓm phe Th­¬ng: "..ThuongChu(7,1)..". §iÓm phe Chu: "..ThuongChu(7,2)..".") end
function pt_tc_points() Talk(1,"main","Tİch lòy trËn nµy: "..ThuongChu(6)..". Tèi ®a 1.000 ®iÓm mçi trËn.") end
function pt_tc_leave() ThuongChu(3) end
function pt_tc_close() CloseDialog() end

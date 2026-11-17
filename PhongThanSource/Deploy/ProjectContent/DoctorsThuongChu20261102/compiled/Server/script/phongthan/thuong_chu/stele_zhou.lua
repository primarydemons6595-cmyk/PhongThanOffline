-- Documented VNG Thuong Chu entry; native session owns admission and PK.
function main()
 Say("ChiÕn tr­êng Th­¬ng Chu",4,"B¸o danh phe 2/pt_tc_join","Vµo chiÕn tr­êng/pt_tc_enter","H­íng dÉn/pt_tc_help","KÕt thóc/pt_tc_close")
end
function pt_tc_join()
 if ThuongChu(1,2)==1 then Talk(1,"main","§· b¸o danh. Khi hÕt thêi gian chuÈn bŞ, quay l¹i ®©y ®Ó vµo trËn.")
 else Talk(1,"main","Ch­a b¸o danh ®­îc: cÇn cÊp 120, ®ang më b¸o danh, phe ch­a ®ñ 100 ng­êi vµ kh«ng h¬n phe kia qu¸ 10 ng­êi. H·y nhËn th­ëng trËn tr­íc nÕu cßn.") end
end
function pt_tc_enter()
 if ThuongChu(2)==0 then Talk(1,"main","Ch­a vµo ®­îc. CÇn b¸o danh ®óng phe vµ chê trËn chiÕn b¾t ®Çu.") end
end
function pt_tc_help() Talk(1,"main","Mçi phe tèi ®a 100 ng­êi, cÇn İt nhÊt 15 ng­êi ®Ó b¾t ®Çu. B¶o vÖ t­íng phe m×nh, h¹ Nguyªn So¸i hoÆc t­íng phe ®Şch ®Ó chiÕm ba mÆt trËn. NhËn nhiÖm vô t¹i V¨n Träng hoÆc Kh­¬ng Th­îng. KÕt thóc trËn, nhËn th­ëng t¹i Trô V­¬ng hoÆc Vâ V­¬ng.") end
function pt_tc_close() CloseDialog() end

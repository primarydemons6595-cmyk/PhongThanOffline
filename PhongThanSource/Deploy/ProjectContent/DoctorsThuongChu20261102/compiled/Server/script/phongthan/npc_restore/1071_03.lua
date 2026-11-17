function main() Say("TiÕp dÉn chiÕn tr­êng - mçi l­ît 1 v¹n l­îng",4,"MÆt trËn thø nhÊt/pt_tc_port1","MÆt trËn thø hai/pt_tc_port2","MÆt trËn thø ba/pt_tc_port3","KÕt thóc/pt_tc_close") end
function pt_tc_port1() pt_tc_port(1) end
function pt_tc_port2() pt_tc_port(2) end
function pt_tc_port3() pt_tc_port(3) end
function pt_tc_port(n) if ThuongChu(9,n)==0 then Talk(1,"main","Ch­a di chuyÓn ®­îc. CÇn ®ang tham chiÕn ®óng phe vµ cã 1 v¹n l­îng trong ng­êi.") end end
function pt_tc_close() CloseDialog() end

function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","Đã thực hiện thành công.")
 elseif result==2 then Talk(1,"pt_no","Giao dịch đang chờ lưu. Không thực hiện lại; server sẽ tự hoàn tất.")
 else Talk(1,"pt_no","Không thực hiện được. Hãy kiểm tra quyền, vật phẩm mang theo, ngân lượng, nhân khí và điều kiện của thành.") end
end
function main()
 SayTask("Thành thị lãnh địa. Chỉ người có quyền quản lý được xây/nâng cấp; giá và giới hạn theo bảng gốc. Studio có bản đồ và thuộc tính chi tiết.",{
 {"Thông tin thành","pt_info";show=1},{"Xây công trình tại chỗ đứng","pt_page0";show=1},{"Nâng cấp thành","pt_upgrade";show=1},{"Cống hiến tại thương khố","pt_donate";show=1},{"Về Phong Thần Đài","pt_exit";show=1}})
end
function pt_info() Talk(1,"pt_no","Cấp thành: "..TerritoryValue(1).." - Nhân khí: "..TerritoryValue(0).." - Phồn vinh: "..TerritoryValue(7).." - Cống hiến của bạn: "..TerritoryValue(2)) end
function pt_upgrade() pt_result(TerritoryAction(2,0)) end
function pt_exit() pt_result(TerritoryAction(4,0)) end
function pt_donate() Talk(1,"pt_no","Đến thương khố của thành hoặc quản lý kho ở Phong Thần Đài để hiến tài nguyên.") end
function pt_page0() SayTask("Chọn công trình. Vị trí xây là chỗ nhân vật đang đứng; cần đủ ngân quỹ và tài nguyên.",{{"Cổng thành","pt_build0";show=1},{"Tháp canh","pt_build1";show=1},{"Vật tổ Thương Lang","pt_build2";show=1},{"Thao trường","pt_build3";show=1},{"Phòng chiến xa","pt_build4";show=1},{"Trại lính","pt_build5";show=1},{"Trang tiếp","pt_page1";show=1}}) end
function pt_page1() SayTask("Chọn công trình. Vị trí xây là chỗ nhân vật đang đứng; cần đủ ngân quỹ và tài nguyên.",{{"Thương khố","pt_build6";show=1},{"Tướng đài","pt_build7";show=1},{"Lầu trống","pt_build8";show=1},{"Tế đàn","pt_build9";show=1},{"Phòng luyện đơn","pt_build10";show=1},{"Điện Luân Hồi","pt_build11";show=1},{"Trang tiếp","pt_page2";show=1},{"Trang trước","pt_page0";show=1}}) end
function pt_page2() SayTask("Chọn công trình. Vị trí xây là chỗ nhân vật đang đứng; cần đủ ngân quỹ và tài nguyên.",{{"Phong Thần Điện","pt_build12";show=1},{"Lò luyện","pt_build13";show=1},{"Huyền đàn","pt_build14";show=1},{"Dược điếm","pt_build15";show=1},{"Đài hồi sinh","pt_build16";show=1},{"Vật tổ Thanh Ngưu","pt_build17";show=1},{"Trang tiếp","pt_page3";show=1},{"Trang trước","pt_page1";show=1}}) end
function pt_page3() SayTask("Chọn công trình. Vị trí xây là chỗ nhân vật đang đứng; cần đủ ngân quỹ và tài nguyên.",{{"Vật tổ Phi Hùng","pt_build18";show=1},{"Sân luyện thú","pt_build19";show=1},{"Hí viện","pt_build20";show=1},{"Cối xay","pt_build21";show=1},{"Cây 1","pt_build22";show=1},{"Cây 2","pt_build23";show=1},{"Trang tiếp","pt_page4";show=1},{"Trang trước","pt_page2";show=1}}) end
function pt_page4() SayTask("Chọn công trình. Vị trí xây là chỗ nhân vật đang đứng; cần đủ ngân quỹ và tài nguyên.",{{"Cây 3","pt_build24";show=1},{"Cây 4","pt_build25";show=1},{"Cây 5","pt_build26";show=1},{"Bồn hoa 1","pt_build27";show=1},{"Bồn hoa 2","pt_build28";show=1},{"Bồn hoa 3","pt_build29";show=1},{"Trang tiếp","pt_page5";show=1},{"Trang trước","pt_page3";show=1}}) end
function pt_page5() SayTask("Chọn công trình. Vị trí xây là chỗ nhân vật đang đứng; cần đủ ngân quỹ và tài nguyên.",{{"Bồn hoa 4","pt_build30";show=1},{"Bồn hoa 5","pt_build31";show=1},{"Đồng Tâm Kết","pt_build32";show=1},{"Hoa Đoàn Cẩm Tú","pt_build33";show=1},{"Trang trước","pt_page4";show=1}}) end
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

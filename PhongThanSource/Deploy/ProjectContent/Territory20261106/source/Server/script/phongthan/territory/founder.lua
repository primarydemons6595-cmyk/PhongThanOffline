function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","Đã thực hiện thành công.")
 elseif result==2 then Talk(1,"pt_no","Giao dịch đang chờ lưu. Không thực hiện lại; server sẽ tự hoàn tất.")
 else Talk(1,"pt_no","Không thực hiện được. Hãy kiểm tra quyền, vật phẩm mang theo, ngân lượng, nhân khí và điều kiện của thành.") end
end
function main()
 SayTask("Lập thành cho lãnh địa: cần bốn sách lập thành mang trong túi, danh vọng và nhân khí đủ điều kiện. Ngân quỹ lãnh địa trả giá đất/đồng thau; nhân vật trả lệ phí. Chọn vùng đất:",{
 {"Hoàng Sa Thành","pt_land0";show=1},{"Hồng Diệp Thành","pt_land1";show=1},{"Hắc Ám Thành","pt_land2";show=1},{"Thu Phong Thành","pt_land3";show=1},{"Hạ Nhật Thành","pt_land4";show=1}})
end
function pt_land0() SayTask("Chọn vật tổ cho thành:",{{"Thương Lang","pt_create0_0";show=1},{"Thanh Ngưu","pt_create0_1";show=1},{"Phi Hùng","pt_create0_2";show=1}}) end
function pt_create0_0() pt_result(TerritoryCreate(0,0)) end
function pt_create0_1() pt_result(TerritoryCreate(0,1)) end
function pt_create0_2() pt_result(TerritoryCreate(0,2)) end
function pt_land1() SayTask("Chọn vật tổ cho thành:",{{"Thương Lang","pt_create1_0";show=1},{"Thanh Ngưu","pt_create1_1";show=1},{"Phi Hùng","pt_create1_2";show=1}}) end
function pt_create1_0() pt_result(TerritoryCreate(1,0)) end
function pt_create1_1() pt_result(TerritoryCreate(1,1)) end
function pt_create1_2() pt_result(TerritoryCreate(1,2)) end
function pt_land2() SayTask("Chọn vật tổ cho thành:",{{"Thương Lang","pt_create2_0";show=1},{"Thanh Ngưu","pt_create2_1";show=1},{"Phi Hùng","pt_create2_2";show=1}}) end
function pt_create2_0() pt_result(TerritoryCreate(2,0)) end
function pt_create2_1() pt_result(TerritoryCreate(2,1)) end
function pt_create2_2() pt_result(TerritoryCreate(2,2)) end
function pt_land3() SayTask("Chọn vật tổ cho thành:",{{"Thương Lang","pt_create3_0";show=1},{"Thanh Ngưu","pt_create3_1";show=1},{"Phi Hùng","pt_create3_2";show=1}}) end
function pt_create3_0() pt_result(TerritoryCreate(3,0)) end
function pt_create3_1() pt_result(TerritoryCreate(3,1)) end
function pt_create3_2() pt_result(TerritoryCreate(3,2)) end
function pt_land4() SayTask("Chọn vật tổ cho thành:",{{"Thương Lang","pt_create4_0";show=1},{"Thanh Ngưu","pt_create4_1";show=1},{"Phi Hùng","pt_create4_2";show=1}}) end
function pt_create4_0() pt_result(TerritoryCreate(4,0)) end
function pt_create4_1() pt_result(TerritoryCreate(4,1)) end
function pt_create4_2() pt_result(TerritoryCreate(4,2)) end

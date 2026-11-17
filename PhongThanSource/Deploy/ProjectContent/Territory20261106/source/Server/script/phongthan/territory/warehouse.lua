function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","Đã thực hiện thành công.")
 elseif result==2 then Talk(1,"pt_no","Giao dịch đang chờ lưu. Không thực hiện lại; server sẽ tự hoàn tất.")
 else Talk(1,"pt_no","Không thực hiện được. Hãy kiểm tra quyền, vật phẩm mang theo, ngân lượng, nhân khí và điều kiện của thành.") end
end
function main()
 if IsTongMember()~=1 then Talk(1,"pt_no","Bạn cần gia nhập lãnh địa trước.");return end
 SayTask("Cống hiến tài nguyên cho lãnh địa. Chỉ dùng vật phẩm trong bốn túi mang theo, không dùng đồ đã khóa. Đồng thau khai khoáng và đồng thau gốc đều được nhận. Điểm mỗi ngày theo cấu hình Studio.",{
 {"250 đồng thau (1 điểm)","pt_bronze1";show=1},{"2.500 đồng thau (10 điểm)","pt_bronze2";show=1},
 {"50.000 lượng (không điểm)","pt_money0";show=1},{"500.000 lượng (1 điểm)","pt_money1";show=1},{"5.000.000 lượng (10 điểm)","pt_money2";show=1}})
end
function pt_money0() pt_result(TerritoryDonate(0,0)) end
function pt_money1() pt_result(TerritoryDonate(0,1)) end
function pt_money2() pt_result(TerritoryDonate(0,2)) end
function pt_bronze1() pt_result(TerritoryDonate(1,1)) end
function pt_bronze2() pt_result(TerritoryDonate(1,2)) end

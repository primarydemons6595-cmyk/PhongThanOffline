function pt_books_close() CloseDialog() end
function pt_books_open()
 SayButtons("<color=yellow>Sách Chư Hầu<color><enter>Mảnh thường: "..TerritoryBookCount(0).."; mảnh Tiên Ma: "..TerritoryBookCount(1)..".<enter>Góp mảnh vào lãnh địa; lãnh chúa đổi sách để lập thành. Sách (Khởi/Thừa/Chuyển/Hợp) là bốn nguyên liệu lập thành tại Thương Thang. Mảnh thường cần cấp 15; mảnh Tiên Ma cần cấp Tiên Ma 14. Mỗi mảnh cộng 1 cống hiến. Mảnh thường thưởng EXP cấp x 5.000 một lần/ngày.",5,"Góp mảnh thường/pt_books_donate","Góp mảnh Tiên Ma/pt_books_realm","Đổi sách - mảnh thường/pt_books_pool0","Đổi sách - mảnh Tiên Ma/pt_books_pool1","Kết thúc/pt_books_close")
end
function pt_books_done(result) if result==1 then Talk(1,"pt_books_open","Đã hoàn tất giao dịch Sách Chư Hầu.") elseif result==2 then Talk(1,"pt_books_close","Giao dịch đang chờ lưu vào nhân vật; server sẽ tự hoàn tất. Không thực hiện lại.") else Talk(1,"pt_books_open","Không thực hiện được. Đứng gần đúng NPC, kiểm tra lãnh địa, cấp, quyền lãnh chúa, mảnh mang theo chưa khóa và ô trống.") end end
function pt_books_donate() pt_books_done(TerritoryBook(0,0)) end
function pt_books_realm() pt_books_done(TerritoryBook(1,0)) end
function pt_books_pool0() SayButtons("Đổi sách. Số mảnh/quyển mặc định 250, chỉnh được trong Studio. Lý Thiên Vương gốc đổi Chuyển/Hợp; Loose bổ sung Khởi/Thừa tại cùng dịch vụ để hoàn thành bộ bốn sách lập thành.",5,"Khởi/pt_books_0_0","Thừa/pt_books_0_1","Chuyển/pt_books_0_2","Hợp/pt_books_0_3","Kết thúc/pt_books_close") end
function pt_books_0_0() pt_books_done(TerritoryBook(0,58)) end
function pt_books_0_1() pt_books_done(TerritoryBook(0,59)) end
function pt_books_0_2() pt_books_done(TerritoryBook(0,60)) end
function pt_books_0_3() pt_books_done(TerritoryBook(0,61)) end
function pt_books_pool1() SayButtons("Đổi sách. Số mảnh/quyển mặc định 250, chỉnh được trong Studio. Lý Thiên Vương gốc đổi Chuyển/Hợp; Loose bổ sung Khởi/Thừa tại cùng dịch vụ để hoàn thành bộ bốn sách lập thành.",5,"Khởi/pt_books_1_0","Thừa/pt_books_1_1","Chuyển/pt_books_1_2","Hợp/pt_books_1_3","Kết thúc/pt_books_close") end
function pt_books_1_0() pt_books_done(TerritoryBook(1,58)) end
function pt_books_1_1() pt_books_done(TerritoryBook(1,59)) end
function pt_books_1_2() pt_books_done(TerritoryBook(1,60)) end
function pt_books_1_3() pt_books_done(TerritoryBook(1,61)) end

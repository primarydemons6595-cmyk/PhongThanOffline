# Màu thoại, đồ bot, Kho Rương và chat

Ngày 04/11/2026. Các thay đổi được biên dịch Release và kiểm tra bằng bộ native
`Tools/test_shop_bank_followups_20261104.ps1`. Biên bản triển khai và bản sao dữ liệu
nằm trong `analysis/tet_event_shop_20261103/`.

- Hộp lựa chọn NPC mã hóa thẻ màu trước khi chuyển cho bộ hiển thị native.
  Tên NPC dịch từ dữ liệu gốc giữ màu được tác giả cấu hình, gồm mẫu NPC 102
  Hoàn Cẩu tinh. Không đổi màu hàng loạt các mẫu khác cùng tên.
- Công việc bảo trì đồ bot có chế độ thống kê trước khi áp dụng, sao lưu nhân vật,
  biên nhận từng bot và nhật ký từng serial. Chỉ chọn đồ lục Vũ Sĩ cấp thấp
  (tier 1–3), bot khác phái không dùng được, chưa nâng cấp, không cần cho nhiệm vụ,
  không đang giao dịch, không đang mặc hoặc cầm tay. Nhận diện yêu cầu hệ phái
  trên cả thuộc tính native 37 và 39. Không tác động túi người chơi.
- Chặn nhặt/rao lại đồ không dùng được và không có giá trị bán. Đồ đang mặc sai
  phái hoặc hỏng không ngăn bot thay đồ hoạt động được.
- Sửa KPlayer::RepairItem: bot không có kết nối client vẫn được sửa đồ sau khi
  trả tiền thành công. Trước đó nhánh kiểm tra kết nối nằm sau Pay khiến bot có
  thể mất tiền mà không được sửa. Bot chủ động sửa đồ đang mặc còn độ bền <=50%.
  Đồ đã về 0 vẫn theo giới hạn bản gốc, không được phục hồi miễn phí.
- Thêm nhật ký biến động kho và thông tin độ bền, ô trang bị, vật phẩm trên tay.
  Khi thiếu đồ hoạt động được, bot tìm mua từ cửa hàng NPC thật bằng tiền sở hữu.
- F2 phân loại riêng lễ bao/túi quà và phù biến thân; 437 hàng có giá, loại 33 hàng
  không có giá khỏi danh mục. Giữ nguyên dòng master để không dịch ID.
- Bàn Cổ Khai Thiên được phép hiện trong bộ chọn kỹ năng nhanh theo loại Produce.
- Thuê/mua kho chấp nhận task hoàn thành lưu dạng số hoặc chuỗi ASCII "2".
  Khôi phục form mật khẩu và bàn phím ảo native, kiểm tra mật khẩu trên máy chủ.
- Phím ~ chuyển chat sang cột bên phải trong chế độ cửa sổ. Tăng bề rộng render
  260px, giữ nguyên kích thước thế giới và vị trí HUD gốc; không xóa lịch sử chat.

Kiểm tra tự động dùng codec Engine thật, hàm sửa đồ production, luồng bảo trì
production và hình học chat ở 40 độ phân giải. Cần người dùng kiểm tra hình thức
trực tiếp trong game; các kiểm tra này không thay thế nghiệm thu giao diện.
Hoàng Ngọc thường/cao cấp không thay đổi quy tắc theo yêu cầu tạm bỏ qua.

Đã triển khai: world PID 200136, 105 bản đồ, sự kiện revision 27 không báo lỗi.
Đã đối chiếu từng serial trong dữ liệu PTHC và xóa 275 món khỏi 49 bot; kiểm tra
lại sau khi lưu xác nhận các serial đã vắng mặt. Không còn món chờ trong danh sách
đã duyệt. Sao lưu nhân vật trước thao tác:
`analysis/tet_event_shop_20261103/shop-bank-content-before-20261103T224412767322Z`.
PhongNguyet3465 còn 129/200 tải trọng và đang đi train có mục tiêu; ChienThanPK
không còn kẹt vật phẩm trên tay, 148/200; NgocVu7291 đang đi mua đồ, 130/200.

Sửa tiếp lệch cảnh khi bật chat: Represent2::LookAt căn theo canvas vật lý,
trong khi tên/thanh máu và tọa độ cảnh dùng vùng chơi chưa gồm cột chat. Đồng bộ
tâm bộ vẽ qua phép đổi tọa độ native, cập nhật lại biên cảnh ngay khi đổi kích
thước và reset bộ vẽ, kể cả nhân vật đứng yên. Kiểm tra dùng nguyên thân hàm
LookAt và ViewPortCoordToSpaceCoord của Represent2 ở bảy chiều rộng, bật/tắt
cột chat và di chuyển camera; thân/tên và tọa độ chuột khớp nhau. Chỉ thay
CoreClient.dll sau khi người dùng thoát game, không khởi động lại server.

Kiểm tra computer-use tiếp theo xác nhận bấm đúng thân Sứ Giả Sự Kiện không mở
thoại, bấm lệch bên phải mới mở. Nguyên nhân thứ hai ở GameClient::HandleInput:
Wnd_GetScreenSize trả chiều rộng thế giới, nhưng MulDiv đổi tọa độ chuột cần
chiều rộng canvas vật lý có thêm cột chat. Đã cộng g_PtChatDockWidth vào thang
đầu vào, giữ Wnd_SetCursorPos dùng kích thước canvas đối xứng. Kiểm tra trích
nguyên khối xử lý chuột production: năm độ phân giải, hai tỷ lệ cửa sổ, bật/tắt
chat, điểm trong vùng cảnh và vùng chat, không thay WM_MOUSEWHEEL.

Đã thay Game.exe và nghiệm thu trực tiếp bằng computer-use trên nhân vật GameMa:
bấm đúng thân Sứ Giả mở thoại khi chat bên phải; đóng thoại bằng nút X đúng
vị trí; chuyển ~ về chat thường rồi bấm đúng thân Sứ Giả vẫn mở thoại; bật ~
lại và bấm đúng thân Sinh Hoạt Sư mở đúng menu Thu Thập/Khai Khoáng. Giữ game
mở sau khi kiểm tra. Biên bản `chat-input-deployment.json` đã đánh dấu
VERIFIED_IN_GAME.

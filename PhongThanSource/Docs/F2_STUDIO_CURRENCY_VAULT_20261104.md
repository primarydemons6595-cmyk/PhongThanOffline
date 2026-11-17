# F2, tiền tệ và rương — 04/11/2026

Đã nạp bản native và Studio. Biên bản triển khai:
`analysis/tet_event_shop_20261103/f2-vault-centre-deployment.json`.

- Game khởi động được căn giữa vùng làm việc lần đầu; các lần resize khi bấm `~`
  tiếp tục dùng `SWP_NOMOVE`. Đã mở bằng computer-use và quan sát cửa sổ ở
  (510,167), thay vì (0,0); giữ màn hình đăng nhập, không nhập tài khoản/mật khẩu.
- Điều kiện thuê/mở kho nhận script Thủ Khố gốc ở Diêu Trì/Tây Kỳ và object rương
  có script kho hợp lệ, cùng bản đồ và trong khoảng cách 384. Không nhận mọi object
  rương tùy ý. Đã kiểm tra cả chính sách native và VM Lua thật, trang 2–5.
- Trả đồ Bách Bảo về đúng nhóm; xét riêng từng item để chuyển phù/ngoại trang
  và lễ bao. Giữ tập hợp sản phẩm có giá và mọi giá/số lượng gốc. Bỏ chữ nhãn
  thêm đè lên ảnh tab gốc. Thứ tự thực: Hot, 1 Dược phẩm, 2 Chuyển, 3 Ghép,
  4 Hiệu quả, 5 Bách Bảo, 6 Biến thân, 7 Lễ bao.
- `CHỈNH SỬA SERVER → Sửa F2`: ảnh giao diện/nhãn/ảnh item lấy từ client; kéo
  item vào tab hoặc trước item khác để đổi thứ tự, tìm item trong danh mục đầy đủ,
  thêm item mới, đặt số lượng/giá/ô thanh toán, bỏ món, sửa nhãn Mới/Bán chạy/Đề cử.
  Khôi phục ba ảnh nhãn đúng hash từ PAK gốc. Native trước đây chỉ bật visibility
  nhưng không lấy ảnh `IconList`; đã nối ảnh và ba bit nhãn, giữ ABI trường 1 byte.
- Bản nháp F2 tự lưu sau mỗi thao tác đã cập nhật. `Lưu vào bản sửa` tạo patch
  cả Client và Server; dùng luồng áp dụng patch hiện có. Dữ liệu bị sửa bên ngoài
  được kiểm tra hash, không ghi đè im lặng.
- `Hai ô số dư`: bật/tắt giá trị và chọn Xu (QuestKey 47), bạc thường, Bạc khóa
  hoặc item chính xác. Mỗi sản phẩm chọn PaymentSlot 1/2; server đọc cấu hình riêng,
  không nhận loại tiền hay giá từ client. Hiện giữ toàn bộ món cũ ở ô 1/Xu.
  Ô 2 mặc định là Bạc khóa. Khung/icon nền hai ô vẫn thuộc ảnh UI gốc.
- `settings/phongthan/F2Display.ini` đồng bộ hai phía. Bảng ibshopgoods thêm cột
  PaymentSlot và chuẩn hóa đuôi dữ liệu cũ thiếu Genre, giữ ID, tuple, giá và số lượng.
  Tiền/item được đếm từ bốn túi F4, bỏ item khóa/đang mặc/kho. Giao dịch kiểm tra
  đủ chỗ và hoàn lại bạc/Bạc khóa khi cấp item thất bại; vật phẩm thanh toán dùng
  transaction quest hiện có, giới hạn trong bốn túi. Giá × số lượng chặn tràn số.
- Bạc khóa tái sử dụng task native 4847 và ô F4 đã có; không phải một ví mới.
  `QUẢN LÝ SERVER → Character Editor`: thêm trường `Bạc khóa (lượng)` để đặt số dư.
  10.000 lượng = 1 vạn. Lưu theo quy trình nhân vật hiện có (nhân vật phải đăng xuất
  khi áp dụng qua server). Không phát tiền cho nhân vật thật trong kiểm thử.

Kiểm tra: build native không lỗi; model/draft/patch/đọc lại F2, WPF render, append
item mới, chọn loại tiền/PaymentSlot, nhãn TCVN3; số dư 4 túi; trừ/hoàn 4 loại tiền;
export/reload bản ghi nhân vật có Bạc khóa, giữ nguyên từng record item và companion.
Computer-use xác nhận Studio thật đã có Sửa F2, ảnh Mới và hai số dư mặc định.
Chưa ghi nhận thao tác mua/thuê tiêu tiền trực tiếp trong game là đã kiểm thử.

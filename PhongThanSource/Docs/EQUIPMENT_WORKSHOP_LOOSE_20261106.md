# Ghép Trang Bị, F4 và thông tin Avatar — Loose 06/11/2026

Đã cài vào bản Loose chính lúc 23:56 ngày 06/11/2026 (giờ Việt Nam). Theo yêu cầu, **không build runtime SecureRelease, không tạo patch Client/Server Release**. Các tệp mới nằm trong `PhongThanRuntime-Loose`; không thay bản phát hành 1.02.

## Ghép Trang Bị trong Studio

Mở nhóm **Ghép Trang Bị → NPC / Công thức**. Có sáu tab: Xích Tùng Tử, Thái Thượng Lão Quân, Lôi Chấn Tử (Diêu Trì), Trấn Nguyên Đại Tiên, Tào Bảo và Thương nhân Tây Vực. Chọn công thức để xem ảnh vật phẩm, nguyên liệu, số lượng và thuộc tính; có thể sửa, nhân bản, tạm tắt hoặc thêm công thức.

- Công thức native được đọc từ bảng đang dùng; không đổi ID của công thức cũ. Các ô nguyên liệu, điều kiện sao/cấp gốc và phí từng lần được giữ riêng.
- Bổ sung 5.046 công thức chuyển mẫu Tinh Quân từ 216 họ trang bị có mẫu gốc tương ứng. Có 48 nhánh thiếu một số mẫu; chỉ mở các bước liền kề đã đối chiếu, không tự sinh thuộc tính cho bước thiếu.
- Mảnh dùng lần 4–6; nguyên dùng lần 7–9. Các bùa 10–12 là nhóm riêng. Tỷ lệ bằng nền trang bị/công thức cộng tổng phần trăm bùa, giới hạn 100%. Ví dụ nền 50% cộng ba bùa 10% là 80%; nền 70% cộng ba bùa 10% là 100%.
- Tỷ lệ nền Tinh Quân là cơ chế Loose có thể chỉnh trong Studio; không khẳng định đã tìm được thuật toán server gốc. Bước 1–3 mặc định 100%, các bước sau mặc định 50% theo nền của bảng nâng chung hiện có.
- Nhánh Thái Thượng bậc cao 9→10 thành công lên 10, thất bại về 6; phí lần 12 là 1.000 vạn lượng. Nhánh sơ/cao có hậu quả thất bại riêng; không dùng lẫn với nâng trang bị thường.
- Chuyển mẫu giữ serial, seed, phần chỉ số đã có, trạng thái khóa và dữ liệu lỗ/ngọc. Không cấp lại món mới để thay thế món đang nâng. Giao dịch lỗi tiền/chỗ trống/tháo vật phẩm được hoàn lại.
- Tùy chọn lấy nguyên liệu tự động chỉ dùng các túi mang theo; không lấy trang bị đang đeo, trong kho, bị khóa, hoặc trang bị nguyên liệu đã nâng/đục lỗ. Món chính vẫn phải được chọn trong ô ghép.
- Có trình sửa bùa, tỷ lệ nền, nút và hội thoại NPC. Menu tùy chỉnh giữ lối vào chức năng cũ; các NPC khác không bị tự ghi lại menu. Lần cài này bật menu Thái Thượng, giữ các menu khác.
- Có thể tạo mẫu trang bị/thú cưỡi mới từ đúng dòng/cấp nguồn. Mẫu mới được nối cuối bảng với ID mới; không giả định mọi bảng có từng khối 10 cấp. Mẫu mới kế thừa công dụng của mẫu nguồn; thêm công dụng hoàn toàn mới vẫn cần mã xử lý tương ứng.
- Trong tab Lôi Chấn Tử có **Lỗ / Ngọc / Hiệu ứng**, sửa 12 bảng gốc về điều kiện, nguyên liệu, phí, tỷ lệ, ảnh và tọa độ. Bảng dùng chung được cập nhật cả Client/Server; bảng tọa độ chỉ cập nhật Client.

Các nút “đưa vào patch” ở đây là hàng đợi thay đổi của Studio để áp dụng vào Loose qua Client/Server Sync; không có thao tác tạo patch phát hành Release trong đợt cài này. Sau khi áp dụng thay đổi bảng vật phẩm/công thức, mở lại game và GameServer để hai phía cùng nạp dữ liệu mới.

Ảnh giao diện thực tế do Studio render, dùng tài nguyên game:

- [Tinh Quân — Classic](../../analysis/upgrade_restore_20261106/installed-ui-verification/workshop-classic-1.png)
- [Tinh Quân — Liquid](../../analysis/upgrade_restore_20261106/installed-ui-verification/workshop-liquid-1.png)
- [Lôi Chấn Tử — Liquid](../../analysis/upgrade_restore_20261106/installed-ui-verification/workshop-liquid-2.png)
- [Tào Bảo — Classic](../../analysis/upgrade_restore_20261106/installed-ui-verification/workshop-classic-4.png)

## Lỗ/ngọc trên đồ đang đeo F4

Luồng vẽ ảnh trang bị lớn trước đây không gọi lớp lỗ/ngọc. Đã nối lớp này sau ảnh trang bị, theo dữ liệu lỗ thực của vật phẩm; không đục thêm lỗ hay sửa đồ nhân vật. Tọa độ lấy từ bảng gốc, giữ kích thước canvas và độ lệch từng frame SPR để tránh hiệu ứng rung vị trí.

Tài nguyên UI gốc ghi `物品12级特效` trước các lớp Helm1/Armor1/Belt1/Boots1 và dùng các sprite `tou.spr`, `shen.spr`, `yao.spr`. Lớp viền này riêng với hiệu ứng lỗ/ngọc. Loose giữ điều kiện nâng +12, đồng thời nhận diện mẫu Tinh Quân +12 đã có chỉ số sẵn trong bảng; không cộng thêm thuộc tính nâng lần nữa. Đã đối chiếu tài nguyên và kiểm thử mã, chưa nghiệm thu hình ảnh trên nhân vật thật sau cài.

## Thông tin dưới Avatar

- Tên server lấy từ lựa chọn server của phiên đăng nhập; bỏ ô nhập tên server trong Studio.
- Tình trạng mạng lấy từ phản hồi ping GameServer đang có, không tạo gói đo mới. Hiển thị đang đo/tốt/vừa/kém/mất kết nối. Mẫu quá 10 giây chuyển về chờ mẫu mới; chưa có mẫu không tự báo tốt. Độ trễ bao gồm thời gian server xử lý.
- Website và nội dung riêng vẫn tùy chỉnh được. Giữ các lựa chọn căn chữ, dịch ngang/dọc, màu và cỡ chữ. Lần cài này bật luân phiên tên server và tình trạng mạng, giữ các giá trị tùy chỉnh cũ.

[Ảnh trình sửa Avatar trong Studio](../../analysis/f4_avatar_20261106/avatar-studio-preview.png).

## Cấp nghề và nấu ăn

Trong Loose, cấp 0 là chưa học; lần học đầu là cấp 1. Câu Cá, Chế Biến Thức Ăn và Nấu Nướng là ba nghề độc lập. Sinh Hoạt Sư Triều Ca (217,194) dạy cả ba; lần học đầu cần nhân vật cấp 20 và 1.000 lượng mỗi nghề. Nấu Nướng cấp 1 có công thức cơ bản Tôm xào/Canh suông, không cần sách mở công thức. Cá/tôm thô cần chế biến thành nguyên liệu của công thức trước.

UI đã phân biệt “chưa học nghề” với cấp nghề đã học; không tự cấp thêm nghề hoặc thay dữ liệu nghề nhân vật.

## Kiểm tra và cài đặt

- CoreServer, CoreClient, Game và Studio dành cho Loose build thành công.
- 5.046 chuyển mẫu thành công/thất bại đã kiểm tra; kiểm thử giao dịch bao gồm tỷ lệ cộng, cô lập NPC, lấy nguyên liệu, giữ định danh, thiếu tiền/chỗ trống, hoàn tác, tụt cấp và mất đồ đúng nhánh.
- 12.631 kiểm tra model Studio đạt, gồm đối chiếu công thức, vòng đọc/ghi giữ byte gốc, menu không cộng lặp, tắt menu trả lại script gốc, tạo mẫu thú cưỡi và hash tệp chờ áp dụng.
- Kiểm tra nâng trang bị cũ, parser cấp/sao riêng, F4, canvas animation, tình trạng mạng và cấp nghề đạt. Đã kiểm tra ảnh sáu tab ở cả Classic/Liquid từ bản Studio sau cài.
- [Biên bản cài](../../analysis/upgrade_restore_20261106/deployment-Loose.json); [danh sách tệp](../../analysis/upgrade_restore_20261106/loose-install-plan.json). Có sao lưu trước khi thay; đối chiếu hash sau ghi; **87 tệp CharacterStore giữ nguyên**.

Chưa đăng nhập nhân vật để nghiệm thu giao dịch và hiệu ứng trong game sau cài. Các kiểm tra tự động không thay thế bước này; server/game không tự khởi động để tránh tác động phiên chơi và trạng thái nhân vật.

## Dọn bản clone cũ

Đã xóa `D:\PhongThan-Studio-LiquidGlass` theo yêu cầu trước đó (155.771 tệp, khoảng 8,9 GB). Trước khi xóa đã giữ và đối chiếu hash 20.845 tệp mã thử, cấu hình, bản nháp và dữ liệu nhân vật cũ trong bản lưu riêng tại `analysis/upgrade_restore_20261106/clone-notes-and-data-20261107-000036`. Đây là bản lưu dữ liệu đối chiếu, không phải bản Studio chạy song song. [Biên bản dọn clone](../../analysis/upgrade_restore_20261106/clone-cleanup.json).

## Sửa tiếp ngày 07/11/2026

Đã cài bản vá Studio nhận nhầm bản nháp Event khác giống bản đã lưu là chỉnh sửa đang chờ. Không xóa bản nháp, không tắt bảo vệ chỉnh sửa thật; chỉ thay DLL Studio Loose. [Nguyên nhân và kiểm chứng](STUDIO_FALSE_PENDING_DRAFT_20261107.md).

Đã sửa thêm lỗi Lua của menu Thái Thượng, bỏ tham chiếu script Tiêu sư Đồng Quan bị thiếu và sửa cách đánh giá mẫu ping cũ: [LUA_WORKSHOP_ESCORT_NETWORK_20261107.md](LUA_WORKSHOP_ESCORT_NETWORK_20261107.md).

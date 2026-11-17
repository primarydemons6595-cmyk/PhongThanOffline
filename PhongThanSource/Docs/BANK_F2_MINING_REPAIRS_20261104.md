# Kho, thuộc tính trang bị, F2 và khai khoáng — 04/11/2026

Đã biên dịch Release CoreServer, CoreClient và Game; đã thay bản chạy khi Game đóng, máy chủ lưu và thoát sạch (exit 0). Không sửa cấu hình sự kiện, bot hay tỷ lệ rơi trong đợt này. Không thêm SPR vào Server.

## Thay đổi

- Thuê kho nhận đúng NPC Thủ Khố Sùng Thành Doanh (template 151, script quản lý kho gốc). Tìm NPC thật trong phạm vi 384, kể cả khi NPC vừa bấm gần nhất là nồi hoặc NPC khác. Bỏ kiểm tra tư thế chiến đấu không phù hợp; vẫn kiểm tra sống, bản đồ, khoảng cách và các điều kiện giao dịch.
- Bốn nút dưới kho và nút thuê dùng sprite gốc. Hộp gửi/rút tiền Việt hóa của bản trước được giữ lại.
- Thông báo mở hành trang/thiếu Vải dầy hoặc Xu dùng hộp Thông tin nhỏ có nút đóng, thay thông báo trên màn hình.
- Xích Đồng đao (N.v) bản có dòng cố định 1/50/1/5 lấy trần nâng từ đúng mẫu cùng họ, cấp, hình và cấu trúc thuộc tính. Trần là 10/100/10/15. Giữ nguyên danh tính nhiệm vụ, hạn chế giao dịch và cường hóa. Tooltip so sánh sao tối đa theo cùng trần này.
- Bỏ tab Tất cả trong F2. Phân bổ đủ 470 sản phẩm vào 8 tab còn lại, giữ các phân loại cũ và bổ sung 287 món trước đây chưa thuộc tab nào; không đổi ID hay giá.
- Mồi câu (buff 898) lấy thời hạn gốc 3600 giây. Dữ liệu cũ đang có buff vô hạn được cấp lại hạn một giờ khi khôi phục; những hạn đã có không bị đặt lại mỗi lần đăng nhập.
- Thanh Đồng Sơn dùng 34 vị trí miệng núi lửa thật từ dữ liệu cảnh gốc. Hình gốc ghép cơ học hai nửa, giữ nguyên palette và pixel. Điểm có khoáng sáng, hết khoáng tối bằng chế độ màu sẵn có của engine và hồi sau 2400 tick (khoảng 133 giây). Bỏ các điểm khai khoáng đặt tạm ở trung tâm. Giữ điều kiện cuốc, kỹ năng và chi phí khai khoáng gốc.

## Kiểm tra

- Build ba target thành công.
- BankRentalTests: 29 kiểm tra chính sách thuê/mua và hạn sử dụng độc lập.
- SmithProbe dùng dữ liệu vật phẩm thật: nâng (N.v) tới đúng trần qua 38 lần, không thay đổi khi chỉ xem báo giá, giữ cường hóa 12 và base, khôi phục lại magic bằng setter sản xuất thành công. Đã đọc đường lưu/khôi phục P0: magic lưu nguyên giá trị; không có kiểm tra ép về khoảng cố định cũ. Đây chưa phải thử đăng xuất/đăng nhập bằng nhân vật thật.
- LifeRuntimeTests: 72 vị trí, 34 crater, thời gian hồi, nguyên liệu, đầy túi, tranh chấp thu hoạch và hủy hành động đều qua.
- Native SPR reader đọc đúng sprite đã xuất bản trên Client: 480×248, tâm 240/124, palette/RLE hợp lệ.
- Đối chiếu bản chạy: hash ba binary đúng receipt; 43 region, catalog object và sprite đúng bản kiểm tra; cả Client/Server đủ 470/470 sản phẩm.
- Test-GlobalIBShop kiểm tra dây nối và bảo vệ mua hàng thành công.

Kết quả triển khai và trạng thái sẵn sàng nằm trong `analysis/tet_event_shop_20261103/bank-f2-mining-deployment.json`. Chưa có kiểm thử chấp nhận bằng nhân vật trực tiếp trong game cho đợt này.

Máy chủ mới đã sẵn sàng: world PID 215296, Bishop PID 188552, đủ 105 bản đồ; CustomEvents revision 27, 3 NPC, Error rỗng. Đã xác nhận trạng thái mới theo đúng PID và revision hiện hành.

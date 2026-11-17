# Phá Quân và phạm vi nâng cấp Xích Tùng Tử — 06/11/2026

Đã cập nhật bản Loose chính và tạo patch Client/Server 1.02. Các dịch vụ đã tắt trước khi cài; lượt này không khởi động lại dịch vụ hoặc đăng nhập nhân vật để nghiệm thu trực quan.

## Lỗi Phá Quân chỉ có ba dòng

15 nhóm thuộc tính Phá Quân 10x, gồm năm loại trang bị × ba phái, để trống sáu ô thuộc tính thường trong bảng công thức. Chúng có công thức nâng từ Phá Quân bậc trước. Mã Loose trước đây xóa toàn bộ thuộc tính thường rồi chỉ tạo ba dòng mở rộng, làm mất phần kế thừa.

`PhongThanForgeInherited.h` tìm nhóm tiền nhiệm duy nhất qua tuple đầu vào/đầu ra của công thức, tiền tố và trường OrgEffect. Không suy ra quan hệ bằng phép trừ ID hoặc SetID. Các trường số của 30 dòng thuộc tính và 30 dòng công thức liên quan đã đối chiếu với PAK gốc đang có hiệu lực trong Client_Original. Ba PAK được liệt kê nhưng không có trên máy được ghi rõ trong biên bản; không khẳng định đã kiểm tra các PAK thiếu đó.

Phần sửa:

- Khôi phục năm dòng thuộc tính thường được bảo đảm của mỗi nhóm, cộng ba dòng mở rộng. Dòng thường thứ sáu, ở những nhóm có khai báo 1%, vẫn theo tỷ lệ trong dữ liệu.
- Giữ chuỗi RNG cũ của ba dòng mở rộng, nên sửa phần kế thừa không đổi các dòng này.
- Tạo mới và dựng lại từ seed/recipe dùng chung cách giải quyết nhóm kế thừa. Công lực đọc giới hạn từ cùng nhóm đó.
- Khi nạp hành trang, nhận diện đúng trạng thái cũ bị mất phần thường trước khi phục hồi. Giữ serial, seed, nhãn nâng cấp, các slot mở rộng và dữ liệu lỗ/ngọc khảm. Trạng thái không khớp mẫu lỗi cũ được giữ nguyên.
- Không đổi bảng PAK, không cấp lại đồ hay chỉnh trực tiếp save nhân vật. Đồ cũ được xét phục hồi trong luồng nạp nhân vật sau khi mở lại server và đăng nhập.

VNG cũng liệt kê năm thuộc tính thường cho Phá Quân Kháng Long cấp 100 của Dị Nhân trong [thông báo sửa thuộc tính Cam 1.19](https://phongthan.vinagame.com.vn/tin-tuc/can-biet/chinh-sua-cac-thuoc-tinh-trang-bi-cam-tu-ban-cap-nhat-1-19.html). Trị số triển khai được lấy từ bảng PAK, không lấy từ tên món hoặc bài hướng dẫn.

## Xích Tùng Tử hiện hỗ trợ những gì?

Bảng dưới đếm mẫu trong các bảng vật phẩm đang được Loose sử dụng. Các mẫu khác cấp hoặc biến thể có thể cùng tên. “Có” nghĩa là có tuple và nhánh nâng cấp phù hợp trong bộ hiện tại; chưa phải kiểm chứng giao dịch trực tiếp của từng instance trong game.

| Nhóm | Mẫu có đường nâng | Mẫu chưa có đường nâng tại Xích Tùng Tử |
|---|---:|---:|
| Vũ khí cận chiến | 1.621 | 1.790 |
| Áo giáp | 1.370 | 1.900 |
| Mũ | 1.370 | 1.900 |
| Đai | 1.370 | 1.900 |
| Giày | 1.370 | 1.900 |
| Phi phong | 1.370 | 1.900 |
| Pháp bảo | 410 | 1.030 |
| Thần ấn | 30 | 130 |
| Pháp khí | 0 | 380 |
| Ngọc bội | 0 | 51 |
| Thú cưỡi | 0 | 6.226 |
| Vũ khí tầm xa cũ | 0 | 30 |
| Nhẫn | 0 | 10 |
| Hộ uyển | 0 | 20 |

Ví dụ đang có: các họ vũ khí Trảm Tướng/Hóa Huyết/Đãng Ma; các bộ Huyền Vũ, Cự Đấu, Đấu Trận, Vũ Khúc, Lâm Binh… ở các tuple phù hợp; pháp bảo Toàn Tâm Đinh, Lạc Hồn Chung, Hồng Hồ Lô, Ngọc Hư Phù, Cọc Độn Long…; Thần ấn Nhân Hoàng Phá Quân ấn, Thái Thanh Vô Vi ấn, Đông Hoàng Vạn Linh ấn.

Ví dụ chưa có tại NPC này: nhiều mẫu Tinh Quân Tiên/Ma; Bạch Liên/Thanh Liên/Hồng Liên pháp khí; các ngọc bội Thanh Linh/Huyền Vũ/Phục Hổ; Thần Tiêu/Thiên Cang/Thông Thiên Phục Ma/Nguyên Thủy Phù Mệnh/Hồng Quân Khai Thiên/Phục Hy Bát Quái ấn. Dụng cụ thu thập cũng không có đường nâng sao ở đây. Pháp bảo Tiên/Ma còn có nhánh riêng tại Tào Bảo; danh sách thiếu tại Xích Tùng Tử không kết luận mọi hệ nâng cấp khác đều thiếu.

Đã chạy 40.739 lượt báo giá quy tắc trang bị thường và 350 lượt pháp bảo bằng mã native. Với nhánh trang bị thường, mỗi lượt cô lập một công thức trong bộ đọc để kiểm tra điều kiện của chính công thức đó; không thay giao dịch runtime hoặc thứ tự tìm công thức của bản chính.

Kết quả phân loại: 33.765 hồ sơ công thức có đường xử lý; 325 hồ sơ thiếu ô phí; 48 thiếu ID trong bảng delta; 6.951 hồ sơ bắt đầu từ +12 bị chặn bởi giới hạn +12 hiện hành. Chưa mở nâng +13–15 hoặc tự đặt phí/thuộc tính cho dữ liệu thiếu. Lượt kiểm tra này chỉ sửa mất thuộc tính Phá Quân; chưa bổ sung các nhánh NPC còn thiếu.

Danh sách từng món, tên/loại/row/tuple và ID công thức: [xich-tung-tu-items.csv](../../analysis/followup_quests_20261106/xich-tung-tu-items.csv). Bằng chứng máy đọc: [xich-tung-tu-audit.json](../../analysis/followup_quests_20261106/xich-tung-tu-audit.json).

## Kiểm tra và cài đặt

- 135 công thức Cam: tạo/dựng lại ổn định; 30 công thức dùng 15 nhóm kế thừa; 2.722 kiểm tra, gồm giữ dòng mở rộng, phục hồi lặp không cộng trùng, +1–12, lớp dữ liệu lỗ khảm và giữ trạng thái đã chỉnh khác mẫu lỗi.
- Công lực: 544 trường hợp trên 135 công thức.
- CoreServer, CoreClient, GameServer, GameClient và Studio đã build qua; Client/Server Release đều báo `RUNTIME_SOURCE_CURRENT`.
- Patch Client 1.02 và Server 1.02 đã kiểm tra hash và chữ ký ECDSA. Đây là patch tiếp sau 1.01; máy phát hành từ bản cũ hơn cần chuỗi cập nhật phù hợp.
- Biên bản cài Loose: `analysis/followup_quests_20261106/deployment-Final.json`; bản sao lưu `backup-Final-20261106-180540`. 87 file CharacterStore giữ nguyên hash trong bước cài đặt.

Chưa có nghiệm thu trực quan sau cài trên nhân vật thật. Cần mở lại server/client bằng Studio, xem lại món Phá Quân 10x cũ sau đăng nhập và thử một giao dịch nâng cấp đã có công thức.

## Bổ sung cho Loose lúc 23:56 ngày 06/11/2026

Đã cài thêm tab **Ghép Trang Bị**, 5.046 công thức Tinh Quân có mẫu đối chiếu, trình sửa công thức/bùa/phí/menu NPC, hiển thị lỗ/ngọc trên đồ đang đeo F4 và thông tin Avatar tự lấy từ phiên đăng nhập. Theo yêu cầu mới, đợt này **chỉ cập nhật Loose, không build hoặc tạo patch Release**; bản phát hành 1.02 ở trên không bị thay.

Chi tiết, phạm vi dữ liệu còn thiếu, ảnh Classic/Liquid và biên bản cài: [EQUIPMENT_WORKSHOP_LOOSE_20261106.md](EQUIPMENT_WORKSHOP_LOOSE_20261106.md). Chưa nghiệm thu trong game trên nhân vật thật sau cài.

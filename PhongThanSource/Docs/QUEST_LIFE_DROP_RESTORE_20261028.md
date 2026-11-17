# Phục hồi nhiệm vụ tân thủ, Đầu Lĩnh và các sửa liên quan — 28/10/2026

Đã phát hành gói QuestLifeDrop20261027 gồm 140 file; world PID 136448, 105 map, 4.053 NPC/quái. Registry nạp 9.240 script, failed=0. Receipt và đối chiếu độc lập tại `analysis/quest_life_drop_20261027/publication.json` và `post_publish_verification.json`. Có sao lưu trước khi thay; hash dữ liệu nhân vật trước/sau giữ nguyên, các dịch vụ tài khoản không khởi động lại.

## Nhiệm vụ tân thủ và Đầu Lĩnh

- Thêm lời mời Mao Lư khi nhân vật mới cấp 1 đăng nhập; đích theo ba ảnh tham chiếu: Tạp Hóa Thương / Từ Hàng Đạo Nhân / Thiếu Hạo. Đây là phần mở đầu, không ghi đè chủ tuyến Mao Lư cấp 25 của Dị Nhân.
- Phục hồi Tân Thủ Thí Luyện: Giáp Sĩ diệt 8 Kiếm Nhân, Đạo Sĩ 8 Tuyết Quái, Dị Nhân 8 Hỏa Diện. Giáp Sĩ nhận/trả tại Tạp Hóa Thương (207,198); Đạo Sĩ tại Thợ Đồng (220,199); Dị Nhân tại Thợ Đồng (191,202). Mục tiêu và phần thưởng tra trong dictionary/localization gốc: Giáp/Đạo 150 XP và mũ cấp đầu; Dị Nhân 200 XP và Lang Nha Yêu Đái. Trạng thái mới 4963/4964, không ghi đè task cũ; F11 có tiến độ và NPC có marker.
- Đầu Lĩnh: bỏ hai bản sao template 175 người qua đường, dùng template gốc 1097, Kind=3, nam Dị Nhân; không dùng 1103 là biến thể chiến đấu. Một NPC thường trực Cự Lộc (239,192), tọa độ đối chiếu dictionary gốc. Đổi tên theo ảnh người dùng. Giữ task 34 và callback giao dịch Cứu Tế; thêm hội thoại mặc định đúng ảnh kể cả không có bước nhận/trả. Kiểm tra world sau restart có đúng một mẫu 1097, không còn mẫu 175 ở map này.

## Các sửa cùng gói

- Tứ Tượng theo hướng dẫn VNG: đúng họ map lấy nguyên liệu; trạng thái Truy tìm Tam Nguyên 293 trong 1.800 giây; tự thêm nguyên liệu vào túi; tối đa 10 nguyên liệu mục tiêu; 5 lượt miễn phí/ngày. Vật phẩm gốc Tìm nguyên liệu (IB 293) cung cấp trạng thái để nhận thêm lượt, không gia hạn lại thời gian đã dùng. Giữ phần thưởng giao dịch hiện có. Tỷ lệ tìm nguyên liệu 1/10 là chính sách tái dựng của source, chưa xác minh là tỷ lệ bản gốc. Nguồn: https://pt.vnggames.com/tin-tuc/can-biet/huong-dan-chi-tiet-ve-nhiem-vu-tu-tuong.html
- Trừ Họa: vật thể hái dùng chung không bị ẩn toàn server sau một người nhặt; bitmask riêng nhân vật ngăn nhặt trùng. Tịnh Hóa Phù dùng trạng thái riêng 336 trong 30 giây, không áp nhầm skill chiến đấu cùng số.
- L mở cửa sổ Kỹ năng sống lấy bố cục và 13 SPR gốc, có 9 mục, nút đóng, Escape và đóng khi rời phiên. **Chưa phục hồi backend học nghề/chế tạo và lưu cấp nghề, tinh lực, thể lực**; các giá trị chưa có dùng `--`. Không khẳng định đã phục hồi toàn bộ kỹ năng sống.
- Gộp hai bảng VNG_WeaponPart của bản sửa Tích Lịch kiếm (N.v) đang chờ phát hành.
- Loot cấp 90: đối chiếu log 344 lượt hạ Thạch Thần/Lam Cốt có 146 vật phẩm, 100 trang bị, 13 lượt thêm dòng thuộc tính. 9 quái xanh có 11 vật phẩm. LevelGate=NONE, không có phạt chênh cấp. Chưa thấy lỗi chặn rơi; không tự tăng tỷ lệ đồ hiếm.

## Kiểm tra

Build Release CoreServer/CoreClient/GameClient qua. Lua 4 thật nạp 133 file cuối cùng; 4 fixture hành vi qua cho Mao Lư, Thí Luyện, Đầu Lĩnh, Tứ Tượng: sai phái/map, nhận/trả lặp, giao dịch thất bại, túi đầy, hết buff, quota/ngày, vật phẩm trả phí và chống đếm lặp theo nhân vật/entity. Native hái nguyên liệu qua 89 kiểm tra. Native tài nguyên Đầu Lĩnh qua 3 lớp/120 frame; SPR cửa sổ nghề qua 13 file/26 frame, không lỗi.

Chưa nghiệm thu click/đối thoại và hình ảnh bằng nhân vật thật sau lần phát hành này. Không sửa trực tiếp save nhân vật để giả lập hoàn thành nhiệm vụ.

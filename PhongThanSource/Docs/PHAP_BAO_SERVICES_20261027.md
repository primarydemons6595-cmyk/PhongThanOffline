# Pháp Bảo: Xích Tùng Tử và Tào Bảo — 27/10/2026

Đối chiếu [VNG: Hệ thống Pháp Bảo phong phú](https://pt.vnggames.com/tin-tuc/can-biet/he-thong-phap-bao-phong-phu.html) với script, bảng vật phẩm, công thức, phí và thuộc tính thăng cấp của PAK gốc đang có hiệu lực.

## Kết quả rà ban đầu

Xích Tùng Tử đã có script và cửa sổ ghép nhưng native chỉ xử lý một phần công thức vật liệu/vũ khí/trang bị; chưa có luồng tổng hợp Pháp Bảo. Bảng `fa_bao_sheng_ji_shu_zhi_biao.txt` tồn tại nhưng chưa được nạp vào phần tính thuộc tính nâng cấp.

Tào Bảo đã hiện ở Bất Chu Thiên Quan 244/236 (map runtime 1073, template 759), nhưng `script/phongthan/npc_restore/1073_06.lua` chỉ là hội thoại hướng dẫn/tra cứu. Script Tào Bảo Ngọc Hư Cung trong PAK là NPC luyện đan, không dùng để thay nhầm NPC Bất Chu.

## Phần bổ sung

- Native đọc 486 quy tắc Pháp Bảo hợp lệ: 88 quy tắc ghép/chuyển bậc và 398 quy tắc thăng cấp. Cả 486 dòng khớp từng byte với PAK gốc khi đối chiếu theo ID; không viết lại công thức theo tên vật phẩm.
- Đọc riêng `(số lần nâng)` và `[cấp mẫu]`, giữ yêu cầu nâng #9 trước khi chuyển bậc. Không cho các dấu ngoặc của Pháp Bảo mở rộng phạm vi ghép vũ khí khác.
- Nạp bảng tăng thuộc tính Pháp Bảo gốc, đủ các bước 1–12; dùng nhãn nâng cấp của bảng này trong tooltip. Nâng cấp giữ serial, seed và thuộc tính ban đầu, chỉ cộng đúng delta. Trạng thái nâng cấp dùng cơ chế lưu/wire hiện có, không đổi layout item.
- Ghép/chuyển bậc sinh đúng dòng amulet.txt và đúng phái yêu cầu của mẫu. Bảo toàn cờ khóa khi nguyên liệu có ràng buộc. Đồ Phổ MagicScript được đổi từ cách ghi công thức Lua `6.1.key` sang danh tính native `6.key.0`.
- Kiểm tra đủ nguyên liệu, khóa, giao dịch, khoảng cách và đúng NPC. Đặt được kết quả rồi mới trừ phí; lỗi đặt đồ, tạo đồ, tháo nguyên liệu hoặc thanh toán thì hoàn tác. Ghép có tỷ lệ thất bại dùng tỷ lệ nguồn và tiêu hao nguyên liệu/phí theo công thức; không dùng hậu quả mất trang bị của luồng vũ khí để xử lý thăng cấp Pháp Bảo.
- Xích Tùng Tử giữ các nhánh cũ và thêm mục Pháp Bảo. Tào Bảo có ghép/thăng cấp Tiên Ma, sử dụng cửa sổ thường với mode riêng 7; không rơi vào nhánh đục lỗ của Lôi Chấn Tử.
- Tào Bảo nhận nhóm 100/101 và các Pháp Bảo yêu cầu cấp Tiên Ma trong nhóm chung. Vì Thất Trần Trai Tiên/Ma, một số Trấn Hồn Tháp và Lục Hồn Phiên vẫn nằm ở nhóm 1, không được dùng số nhóm làm tiêu chí duy nhất. Xích Tùng Tử giữ phạm vi nhóm 1 đã có trong dữ liệu.
- Bảo điển hợp thành hiển thị 48 công thức nhóm chung tại Xích Tùng Tử và 66 công thức liên quan Tiên Ma tại Tào Bảo (26 công thức trùng nhóm chung, tổng duy nhất vẫn 88). Nguyên liệu dài được chia trang, phí hiển thị chính xác bằng lượng.

## Khác biệt và giới hạn nguồn

amulet.txt và bảng thuộc tính nâng cấp khớp toàn bộ PAK. Bảng tổng công thức/phí của Loose khác PAK ở các nhánh khác; lần này chỉ xác nhận các dòng Pháp Bảo liên quan, không thay toàn bộ bảng để tránh làm mất sửa trước đó.

Một số câu trong bài web có tên/chi tiết không nhất quán với bảng đang có: ví dụ công thức Thần Tôn Hình Thiên Ấn lặp vật liệu của bậc trước, trong khi dòng PAK chuyển bậc dùng vật liệu khác. Runtime giữ công thức, cấp mẫu, chi phí và thuộc tính trong PAK hiện hành; bảo điển hiển thị theo dữ liệu đó. Thất Thần Trai trong bài có tên Thất Trần Trai trong bảng vật phẩm. Không thêm công thức ghép cho các món bài chỉ ghi nhận từ giải đấu/đấu giá.

Ô phụ phí đầu tiên trống trên một dòng ghép đã tồn tại được đọc là 0 phụ phí; không lấy nhầm các cột phí nâng 13–15 làm phí tạo món. Ô phí nâng cấp thiếu hoặc dòng phí không tồn tại vẫn bị từ chối. Phạm vi nâng cấp được mở là 1–12 như hướng dẫn, chưa mở nâng 13–15.

## Kiểm chứng

- Native: 486 lượt báo giá, 88 ghép/chuyển bậc, 398 nâng cấp; thử hoàn tác thanh toán trên từng quy tắc, thiếu tiền, đầy túi, lỗi sinh đồ, lỗi tháo nguyên liệu, thất bại xác suất, giữ nguyên phần dư stack, giữ cờ khóa và giới hạn NPC/realm. Có kiểm tra các quy tắc Tiên Ma của nhóm 1 tại Tào Bảo.
- Lua 4 thực: menu Xích Tùng Tử, nhánh cũ, mode 7 của Tào Bảo, 114 mục công thức chia trang và chặn sai map.
- Bộ ghép trang bị cũ qua lại: nâng thường/tinh chế, hoàn nguyên, phí, xác suất, giữ ID/seed, hoàn tác và số lượng đã chọn.
- CoreServer/CoreClient/GameClient Release build qua. Chưa kiểm tra trực quan và thao tác ghép trên nhân vật Game Loose sau triển khai.

Nguồn và biên bản: analysis/fabao_audit_20261027. Hai gói FabaoServices20261027 và FabaoRealmScope20261027 đã áp dụng đầy đủ. World PID 130132; console xác nhận 105 map đã nạp, 4.044 NPC/quái, 0 ScriptError; 6 file triển khai cuối khớp SHA-256. Hồ sơ nhân vật không đổi. Tào Bảo vẫn đúng map 1073 tại tọa độ 244/236. Biên bản cuối: realm_publication/publication.json và realm_publication/post_publish_verification.json.

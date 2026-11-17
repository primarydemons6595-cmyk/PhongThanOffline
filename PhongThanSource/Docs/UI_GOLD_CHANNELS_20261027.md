# Sửa màu khung chi tiết và hội thoại NPC — 27/10/2026

Đã áp dụng UiGoldChannels20261027 vào Loose: 8 SPR và bản ghi hash completion. Không đổi DLL, script NPC, INI, index, vị trí hoặc kích thước UI. Server không khởi động lại, PID vẫn 114888.

Các SPR true-color của bộ khung này lưu màu BGR, trong khi bộ nạp Loose chuẩn hóa chúng theo RGB. Cả bộ xem trước cũ cũng mắc lỗi tương tự. Không thiếu bản vàng riêng: khung và tab gốc đã là màu vàng. Chuẩn hóa riêng các pixel BGR của tám tài nguyên đã chứng minh khớp PAK gốc sang thứ tự RGB mà renderer Loose đang đọc. Giữ nguyên header, bảng offset, số frame, RLE, alpha, chiều dài dữ liệu và mọi vị trí pixel. Chuyển hai lần trả về đúng byte nguồn ban đầu. Không thay bộ giải mã chung vì những SPR indexed và tài nguyên RGB hiện có cần giữ nguyên quy tắc của chúng.

Phạm vi:

- MainProperty của Thuộc Tính Chi Tiết, ID 101785AE.
- Ba tab Tấn Công / Phòng Thủ / Khác, mỗi tab đủ hai frame.
- Ba nền trang thuộc tính chứa nhãn gốc.
- `spr/ui4/fsbook/npc_dui_hua_kuang.spr`, khung THÔNG TIN dùng bởi UiInformation2.ini, đúng ảnh hội thoại người dùng báo.

Đã rà 27 tài nguyên liên quan trong các INI thuộc tính và NPC; giữ nguyên hash 19 tài nguyên indexed/đã đúng. Chỉ sửa tám tài nguyên true-color có byte nguồn khớp PAK gốc; không đổi tài nguyên chưa xác định nguồn.

Kiểm chứng bằng SprGetHeader của engine sản phẩm: 8 tài nguyên / 11 frame nạp được, palette và RLE hợp lệ; năm frame tiêu đề/tab thường có pixel vàng đúng. Các frame tab được chọn giữ màu sáng gốc. Chạy lại kiểm thử trên chính Client Loose sau áp dụng cũng qua. 9 file đúng hash; có backup, chỉ Client/completion thay đổi, không đụng dữ liệu nhân vật. Chưa kiểm tra trực quan trong một phiên nhân vật Game Loose.

Script tái lập: Tools/normalize_ui_truecolor_channels.py. Kiểm thử native: Tests/Native/UiGoldSpriteTests.cpp. Gói: Deploy/ProjectContent/UiGoldChannels20261027. Bằng chứng: analysis/ui_gold_channels_20261027/audit.json, native-test.txt, publication.json; ảnh dialog_gold.png và status_gold.png.

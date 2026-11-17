# Bộ Đầu quân Chu không cấp Trúc Giản và hồi sinh quá lâu

Ngày kiểm tra: 05/11/2026.

Nguyên nhân đã xác định trong dữ liệu đang cài: bản ghi có tên Bộ Đầu quân Chu tại Ngọc Hư Cung Tương Lai, tọa độ 206/204 (MPS 52864/104704), đã dùng template 428 thay cho 116. Bản ghi nằm trong `Server/settings/phongthan/npc_regions/1062/v_102/103_region_s.dat`; file region này được ưu tiên hơn file standalone `103_npc_s.dat` còn giữ ID 116.

Template 428 dùng hình ani050, script `npcdeath/redboss.lua`, ReviveFrame 129600 (2 giờ ở 18 FPS). Template 116 dùng script nhiệm vụ `phongthan/giapsi/death_116.lua`, ReviveFrame 10800 (10 phút). Vì vậy thay ID để đổi hình đã thay cả hành vi, chỉ số và lịch hồi sinh, dù tên hiển thị vẫn là Bộ Đầu quân Chu.

Đã sửa riêng template của đúng bản ghi này từ 428 về 116. Giữ nguyên toàn bộ byte còn lại, gồm tên, tọa độ, cấp và năm quái khác trong cùng vùng. Hình ảnh trở về tài nguyên của mẫu 116. Không thay dữ liệu nhân vật, bước nhiệm vụ hay tự cấp Trúc Giản. Khi sửa hình về sau cần tách thay tài nguyên hình ảnh khỏi thay ID mẫu NPC.

GameServer không chạy lúc sửa, nên thay đổi sẽ nạp khi khởi động world lần tiếp theo. Chưa kiểm tra hạ boss bằng nhân vật thật sau sửa.

GameMa đang lưu task 3 = 73, đúng bước lấy Trúc Giản. Script nhiệm vụ đang cài không quay tỷ lệ: nếu đúng người hạ, đúng map, đúng bước và thêm được đồ vào túi thì cấp 1 Trúc Giản và chuyển bước 74. Bộ kiểm tra Lua 4 chạy trực tiếp script đã qua 16 điều kiện, gồm bước/map/hệ/người hạ, lặp và đầy túi. Bộ log riêng đã thêm từ lượt trước, không thay quy tắc nhiệm vụ.

Sao lưu và biên nhận hash: `analysis/truc_gian_20261105/103_region_s.before_id_restore.dat`, `boss_identity_restore.json`. Test: `PhongThanSource/Tests/Native/GiapSiTrucGianLuaTests.cpp`.

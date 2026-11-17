# Trừ Họa: hái cây, biến mất và hồi lại — Loose, 29/10/2026

Đã bỏ điều kiện bitmask 4553 khóa mỗi vị trí cây vĩnh viễn cho từng lượt nhiệm vụ. Giờ có thể chọn bất kỳ cây đang hiện nào, kể cả cây đã hái trước đó sau khi nó hồi. Task 4553 chỉ còn là receipt giao dịch: đảo bit để compare-and-set nhận vật phẩm, không dùng làm điều kiện sở hữu cây. Không sửa dữ liệu nhân vật cũ.

Sau khi QuestExchange cấp vật phẩm thành công, gọi `KObj::SetState(OBJ_PROP_STATE_HIDE)` trên chính cây vừa hái. Cơ chế Prop hiện có gửi trạng thái ẩn đến client, lấy BornTime từ LifeTime và tự chuyển về DISPLAY khi hết thời gian. Hai record 345 Nguyên Sâm và 350 Sơn Xuyên Liễu trong `settings/obj/ObjData.txt` có Kind=Prop, LifeTime=180 tick, khoảng 10 giây ở GAME_FPS=18. Không tự đặt timer mới hoặc xóa/tạo object liên tục. Tài liệu này xác nhận cấu hình và cơ chế engine hiện có, không khẳng định đã tìm được toàn bộ Lua nhiệm vụ gốc cùng phiên bản.

Giữ thời gian hái ba giây, điều kiện nhiệm vụ/map/buff Tịnh Hóa Phù, ngắt do di chuyển/trúng đòn/chết/đổi map/phiên. Cây chỉ ẩn khi nhận đồ thành công; đầy túi không làm mất cây. Khi hai người cùng hái, người hoàn tất đầu tiên nhận được đồ và ẩn cây; phiên còn lại không phát thưởng trùng. Sau hồi, mọi nhân vật đủ điều kiện đều có thể hái lại.

Kiểm tra `Tests/Native/GinsengRespawnTests.cpp`: PASS 105 kiểm tra chạy handler thu thập sản xuất trong fixture, gồm thứ tự ngược, cùng cây sau hồi, hai người cạnh tranh, buff, đầy túi và ngắt phiên. Cơ chế đếm BornTime dùng trực tiếp engine Prop hiện có. Chưa nghiệm thu hái cây trong phiên nhân vật thật.

Build Dev CoreServer thành công. Chỉ thay `PhongThanRuntime-Loose/Server/CoreServer.dll`, không thay client, không áp runtime Release. World 125080 lưu/thoát sạch exit 0; chín file nhân vật không đổi trong lúc sao chép; world mới 157304. Gói/hash/backup: `Deploy/ProjectContent/HerbRespawn20261029`, `analysis/herb_respawn_20261029`.

Thay thế mô tả cũ “cây dùng chung không bị ẩn, bitmask riêng ngăn nhặt trùng” trong các tài liệu thu thập ngày 28/10.

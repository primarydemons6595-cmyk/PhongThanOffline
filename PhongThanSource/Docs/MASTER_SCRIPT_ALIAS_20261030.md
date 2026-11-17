# Sửa API IsMaster của script gốc — Loose

Lỗi được người dùng báo tại `npc_recovered_batch/1004_94a2d951.lua`: main gọi IsMaster ở cấp >=20 trước khi mở menu. Runtime chỉ đăng ký `IsMantleMaster` (ScriptFuns.cpp), lấy số đệ tử đã lưu và quan hệ sư đồ online. Không có global `IsMaster`, nên script dừng với nil-call.

Đã nối `IsMaster=IsMantleMaster` trước payload tại mọi script Loose đang gọi IsMaster(): 7 Server, 4 Client. Giữ nguyên nhiệm vụ và dịch vụ native, không trả hằng giả. `Tools/vo_su_ngoc_hu_guard.lua` cũng được cập nhật để các gói tạo sau có alias. Publisher MasterAlias nối sau LevelGift để không bị ghi đè bởi gói cũ.

Kiểm tra Lua4 thực tế nạp 11 file, gọi alias khi trạng thái master 0/1/0. Gọi đúng pt_original_main của Võ Sư Dị Nhân ở cấp 90 và task897/898 đang hoạt động mở menu thành công. Test trước đó đã mock IsMaster, nên không phát hiện API thiếu; fixture mới chỉ cung cấp API native IsMantleMaster.

Đã áp live Loose và reload 7 VM server, failed=0, world PID142980; không restart hay đổi DLL. Backup và hash ở `analysis/master_alias_20261030/publication.json`.

Các thông báo Packing world sync data failed (KSubWorld.cpp) và Packing sync data failed (KNpc.cpp/KPlayer.cpp) là lỗi trả về từ SendData/PackDataToClient. Chưa xác định kết nối hoặc buffer gây lỗi; không khẳng định alias đã xử lý hai thông báo mạng này. Chưa kiểm thử thao tác trong client với nhân vật thật.

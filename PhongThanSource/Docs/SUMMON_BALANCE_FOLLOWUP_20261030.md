# Đệ Dị Nhân: công lực, nhịp đánh, tầm đuổi và nút gốc

## Nút kiếm: giữ đúng Thường / Khóa

Đọc lại trực tiếp `ui/ui3/召唤兽顶部控制条.ini` trong `Client_Original`:
serverlist.pak, entry `017EF8BE`, SHA256
`e640c595d6d9ce5ab24dd4f2c9a0d44abea007f4263260e196598701f169a86b`.

- Thường: chủ hoặc thú bị tấn công thì thú chủ động phản công.
- Khóa: thú nhận mục tiêu mà chủ tấn công.

Nút không phải công tắc tự tìm quái/bật tắt chiến đấu. Source cuối cùng giữ
đúng hai chế độ trên, nạp cả OpenTip và CloseTip nguyên bản từ INI. Dùng
trạng thái khóa đã được server xác nhận để vẽ nút và chọn tooltip; bấm nút
gửi PT_STATUS_SUMMON_LOCK. SetLock đồng bộ ngay trạng thái về client.
Không có tính năng tự quét mục tiêu được publish trong đợt này.

## Cấp hiển thị và sức mạnh

Cấp chiến đấu vẫn theo chủ; cấp skill được gửi riêng trong owner snapshot,
có kiểm tra rank1..63, mode0/1 và định danh NPC. Thanh máu ngoài bản đồ,
HUD đệ và thông tin xem NPC dùng rank này để hiển thị10, không dùng cấp90.
Cache kiểm tra entity ID nên NPC tái sử dụng slot không nhận cấp/mode cũ.
Hai Core DLL được cập nhật cùng nhau cho owner snapshot mở rộng.

Theo yêu cầu tăng sức mạnh của bản Loose, nền công mới:

`templateDamage * (100 + ownerLevel * summonRank) / 100`

Giữ phân biệt template của chín loại thú, giữ các chiêu/đòn phụ gốc và không
thay chỉ số quái/boss khác. Có kiểm tra đầu vào và clamp tràn số. Đây là
quy tắc cân bằng được viết cho Loose, không phải công thức VNG đã xác minh.
HP/AR/phòng ngự tiếp tục lấy Lua level script theo cấp chủ.

Ví dụ chủ90, Phong Quyển Tàn Vân rank10, không buff:

| Chỉ số | Trước | Mới |
|---|---:|---:|
| HP | 1700 | 1700 |
| AR | 4750 | 4750 |
| Nền công | 90 | 900 |
| Descriptor vật lý chiêu145 | 225 | 2250 |
| Damage sau ExDefense của Cửu Anh thường | 137 | 2162 |
| Động tác đánh (template15 frame) | 14 frame | 6 frame |

Damage trên là kiểm tra công thức offline; damage từng đòn thực tế còn tùy
loại đòn phụ, trúng/trượt, buff, kháng và trạng thái mục tiêu. Log thực tế
trước sửa ghi đòn phụ146/148 rank1 chỉ mất56 HP Cửu Anh; không coi descriptor
đòn chính225 là sát thương của toàn bộ chuỗi đạn.

Tốc độ cộng `(rank-1)*10`, giới hạn100; rank10 cộng90 vào attack/cast speed.
Vẫn qua animation, cooldown và engine combat bình thường, không bắn tức thời.

## Phản công và giới hạn theo chủ

Nhận mục tiêu trong1024 pixel, tiếp tục đuổi trong giới hạn1280 từ chủ;
trước đây range theo Vision thường400 và recall384 khiến thú mất boss khi
chủ đứng xa. Khi đang giao chiến hợp lệ không bị recall ở384 nữa. Vượt leash,
chết/mất mục tiêu, đổi map hoặc mất chủ đều bỏ mục tiêu/quay về an toàn.
Ngoài tầm chiêu, thú chạy đuổi thay vì đi bộ. Thường đổi sang kẻ địch mới
đánh chủ/thú; Khóa chỉ nhận mục tiêu do chủ chỉ định. Vẫn kiểm tra quan hệ
PK/địch–đồng minh, không bật PK để đánh người khác. Không tự quét quái.

## Khắc hệ và kiểm tra

Giáp > Đạo > Dị > Giáp: chiều thuận200%, chiều ngược50%, cùng phái100%.
Đệ dùng hệ của chủ. Đệ có chủ và nhân vật cùng được áp hệ số giảm PvP;
không còn trường hợp pet nhận nguyên damage NPC khi người chơi nhận20%.
PvE với quái/boss không bị áp hệ số PvP/khắc phái.

Smoke native đạt cho chín chiêu, sở hữu, mana/cooldown/rollback, stale ID,
phản công cho chủ/thú, hai mode nút, đuổi900, không recall384 giữa combat,
hard leash1280, packet rank/mode và handler nút HUD trích từ production.
Stats test dùng bảng NPC/Skills/SummonSkill và Lua4 thật. Build Dev:
CoreServer/CoreClient/GameClient đều0 lỗi.

Mô phỏng một đệ, ba seed, đổi bên,1800 trận/cặp/mode,21600 trận tổng:
Giáp>Đạo1800/1800, Đạo>Dị1736/1800 (96,44%), Dị>Giáp1800/1800.
Tắt khắc hệ làm Đạo>Dị còn0/1800; bỏ đệ làm Dị>Giáp còn0/1800.
Đây vẫn là fixture người chơi chuẩn hóa, một projectile trúng vùng mỗi lượt,
AI/di chuyển/kháng chưa phải toàn bộ GameServer; không chứng nhận tỷ lệ thắng
PvP thật. Không dùng script ép kết quả thắng; helper damage production áp
cùng chính sách cho mọi cặp. Bộ3 thú cũ không dùng làm chuẩn cân bằng.

## Triển khai

Đã lưu/dừng GameServer sạch (exit0), thay duy nhất Loose Server/CoreServer.dll,
Client/CoreClient.dll và Client/Game.exe. Backup/hash tại
`analysis/summon_balance_followup_20261030/published.json` và `binaries-before`.
GameServer mới PID178548; readiness được cập nhật trong receipt sau khi tải.
Release không thay đổi. Nút auto-fight được đề xuất lúc hiểu sai đã rút khỏi
source trước publish; phần publish chỉ có Thường/Khóa đúng dữ liệu gốc.
Chưa nghiệm thu ảnh UI và combat trực tiếp sau bản cập nhật; không tự đăng nhập
nhân vật. Cần gọi lại thú để nhận chỉ số mới.

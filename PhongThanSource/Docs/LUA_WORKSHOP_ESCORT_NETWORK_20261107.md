# Lua Thái Thượng, script Đồng Quan và nhãn mạng — 07/11/2026

Chỉ sửa bản Loose. Không build runtime SecureRelease hoặc tạo patch phát hành.

## Menu Thái Thượng Lão Quân

Đã tái hiện lỗi `cannot access a variable in outer scope`, token `ptw_saved_main`, tại script `script/phongthan/npc_restore/1052_02.lua`. Trình tạo menu mới của Studio khai báo biến local rồi tham chiếu trong hàm khác bằng cú pháp closure không phù hợp với Lua cũ của game. Kiểm thử model C# trước đó không phát hiện được lỗi cú pháp này.

Đổi chỗ giữ hàm main gốc thành biến trong Lua state riêng của script; giữ nguyên lối gọi chức năng cũ, menu mới và callback mở chế độ 1002. Sửa cả script đã cài lẫn trình sinh menu trong Studio để lần lưu sau không tạo lại lỗi.

Kiểm tra bằng `Engine.dll` và `LuaLibDll.dll` của Loose: script trước sửa tái hiện lỗi; script sau sửa nạp thành công, gọi main mới, main gốc và callback 1002 đều qua. GameServer PID 41048 đã nhận lệnh safe reload đúng một script, trả `completed`, `processed=1`, `failed=0` trước lần reset server.

## Script Tiêu sư Đồng Quan bị thiếu

Chuỗi console `\script\È╦´┌\õ³╣Ï´┌╩ª.lua` là đường dẫn GBK `\script\运镖\潼关镖师.lua` hiển thị qua OEM 850, ID **C514C3D4**. Không phải tên tệp UTF-8 cần đổi font.

- Không có tệp Loose hoặc binding tương ứng. Không tìm được ID trong 22 PAK hiện có của Client_Original hoặc 22 PAK của nguồn Staging. Client_Original vẫn thiếu ba PAK được khai báo: `resupd.pak`, `resupd1.pak`, `Resource.pak`; không khẳng định đã tìm trong những tệp không có trên máy.
- Tìm được đúng một tham chiếu trong NPC section của Đồng Quan map 1014, region 90/98, template 152 tại MPS 46529/100805.
- Theo yêu cầu “không có thì bỏ”, xóa riêng trường script của NPC này qua override `settings/phongthan/npc_regions/1014/v_098/090_region_s.dat`. Giữ NPC hiển thị; không thay bằng chức năng hoặc phần thưởng khác.
- Giữ đủ năm NPC của region, nguyên tọa độ, tên, template, cấp và loại. Các section địa hình, vật cản, bẫy và vật thể giữ nguyên. Bản đồ gốc không bị viết đè. Đã kiểm tra cả giới hạn tọa độ/loại của bộ nạp native khi dùng override.

Các script skill thiếu khác trong báo cáo Tiên/Ma không bị xóa theo thay đổi này.

## Vì sao local hiện “Mạng: Kém”

Nhãn Avatar mới đã đánh giá sai độ cũ của mẫu: mẫu quá 10 giây bị xếp Kém. Trong source server, mỗi tick chỉ kiểm tra một ô kết nối (`m_nGameLoop % m_nMaxPlayer`). Console hiện cấu hình 1.200 ô; ở 18 FPS, một vòng khoảng 66,7 giây. Vì vậy mẫu có thể cũ quá 10 giây theo cách lấy mẫu bình thường, không chứng minh RTT cao hoặc kết nối local kém.

Đã sửa:

- Mẫu cũ chuyển sang **Chờ mẫu mới**, không dùng để kết luận tốt/kém hiện tại.
- Mẫu mới hiện kết quả kèm RTT tính bằng ms. Rê chuột xem RTT lần đo trước và tuổi mẫu.
- Chưa có mẫu vẫn chờ đo; socket mất kết nối vẫn hiện mất kết nối. Mẫu mới có độ trễ cao vẫn bị đánh giá kém, không gán mọi kết nối local thành tốt.
- Không đổi giao thức ping, chu kỳ ping, timeout, đồng bộ vị trí hoặc logic bot.

Số RTT này gồm lịch xử lý của client/server và hàng đợi, không chỉ thời gian truyền trên loopback. Performance trước reset vẫn quanh 18 FPS, nhưng có tick đỉnh khoảng 201 ms trong một cửa sổ đo; do đó sửa nhãn không phải bằng chứng đã hết giật. Bot vẫn chỉ được báo cáo theo phạm vi đã thống nhất.

## Cài đặt và kiểm tra

- Script/menu qua bộ Lua thật. Trình sinh Studio tạo lại cùng script sau sửa, không có thay đổi thừa.
- 12.627 kiểm tra model workshop và chín kiểm tra trạng thái bản nháp qua.
- Game Loose và Studio build thành công. Kiểm thử phân loại kết nối xác nhận mẫu mới tốt/vừa/kém, mẫu cũ 10–67 giây chuyển về chưa xác định; các kiểm tra F4 đi kèm vẫn qua.
- Đã cài tám tệp/binding/manifest có kế hoạch và SHA, có bản sao lưu. **87 tệp CharacterStore giữ nguyên trong lúc cài**, sau khi GameServer đã lưu và thoát sạch.
- Chỉ GameServer được reset; các dịch vụ Goddess, Account, Relay, Bishop và Event giữ PID cũ.

Biên bản và bằng chứng nằm tại `analysis/missing_escort_script_20261107`: `pak-audit.json`, `map-refs.json`, `region-candidate/region-plan.json`, `script-live-install.json`, `install-plan.json`, `deployment.json`, `world-stop.json`, `world-start.json`. Kết quả kiểm tra sau khởi động được ghi riêng khi server hoàn tất nạp map.

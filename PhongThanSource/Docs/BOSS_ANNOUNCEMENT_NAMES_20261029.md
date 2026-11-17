# Tên thông báo Boss Hoàng Kim — 29/10/2026

Đã cập nhật 12 boss theo lịch trong Loose từ `Boss <template>` thành `Boss Hoàng Kim - <tên loại boss>`: Hỗn Độn, Cùng Kỳ, Thao Thiết, Đào Ngột, Cửu Anh, Bàn Cổ, Đại Bằng, Dương Tiễn, Giao Long, Ly Long, Bất Nghĩa Hầu, Cầu Long.

- EventService/catalog.json dùng tên Unicode cho Studio/API.
- EventService/catalog.tsv dùng đúng byte TCVN3 dạng hex cho thông báo native trong game.
- script/servertimer.lua dùng cùng tiền tố cho lịch dự phòng, giữ cơ chế bỏ qua boss do EventService quản lý.

Chỉ đổi nhãn hiển thị và tiền tố. ID event, template, map, cờ boss, lịch, revision cấu hình, spawn và drop giữ nguyên. Hàm Announce native lấy nhãn này cho cảnh báo trước giờ xuất hiện, bắt đầu và kết thúc.

Danh mục native đã thay trước lần khởi tạo của world 100920; không khởi động lại world thêm lần nữa. Timer được LiveReload completed, failed=0. Dịch vụ Event được dừng sạch qua cửa sổ sau khi yêu cầu shutdown HTTP đã hoàn tất và khởi động lại thành PID 151436 để nạp tên mới vào API. Studio CLI restart trước đó timeout vì host HTTP dừng nhưng cửa sổ quản lý vẫn mở; không ép tắt dịch vụ hoặc world để khắc phục.

Kiểm tra Lua 4 thật: mọi dòng lịch boss, cảnh báo trước 5 phút/1 phút, phát một lần mỗi phút, không phát khi boss còn hiện diện. So sánh danh mục xác nhận chỉ Name/nhãn thay đổi. API dịch vụ mới trả đúng đủ 12 tên, kết nối world 100920. Không phát cảnh báo thử cho người chơi.

Gói: `Deploy/ProjectContent/BossAnnouncement20261029`, ba file. Source tạo danh mục và source timer đã được cập nhật để lần xuất bản sau không đưa lại tên số. Bằng chứng/backup: `analysis/boss_announcement_20261029`, gồm publication.json, timer-reload.json, api-verification.json và behavior.lua.

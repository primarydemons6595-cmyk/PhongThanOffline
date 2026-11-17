# Sửa Start Server của Studio Loose — 29/10/2026

Đã triển khai vào hai vị trí Studio của Loose: `ManagerStudio/Release` và `ManagerStudio/Wpf/PhongThanStudio.App/bin/Release/net10.0-windows`. `Release` ở đây là cấu hình build của Studio trong Loose; không tạo hoặc áp patch vào `PhongThanRelease`.

Nguyên nhân: `WindowsServerControlHost.Launch` chạy PowerShell với `Start-Process -RedirectStandardOutput/-RedirectStandardError`, rồi chờ launcher kết thúc và đọc hết stdout. Trên môi trường hiện tại, lời gọi không trả về cho tới khi tiến trình dịch vụ thoát. `ServerControlView.Execute` vẫn giữ trạng thái busy và khóa các nút; StartCore không thể đi tiếp tới Account/Relay/Bishop/World. Fixture trước sửa: sau hai giây Goddess vẫn sống nhưng Launch chưa hoàn tất; Launch trả về sau 15,7 giây, khi dịch vụ đã thoát.

`WindowsServiceProcess` thay launcher PowerShell bằng CreateProcessW, kế thừa riêng ba handle stdin NUL/stdout/stderr dạng file qua HANDLE_LIST. Trả PID ngay khi tạo tiến trình; dùng môi trường đã loại các biến magic-root như trước. Không có pipe chờ EOF và dịch vụ vẫn sống khi tiến trình gọi đã thoát. World giữ ConPTY và quy tắc lưu/dừng sạch hiện có. Giao diện bổ sung tên thao tác đang thực hiện; các nút được mở lại khi thao tác hoàn tất hoặc báo lỗi.

Kiểm tra:

- Fixture sau sửa: Launch hoàn tất trong khi dịch vụ còn sống. Tiến trình gọi thoát nhanh, dịch vụ vẫn sống, đọc được stdout/stderr trực tiếp từ file.
- 25 kiểm tra vòng đời/readiness/log và 194 kiểm tra Studio đạt.
- Studio build thành công, không warning/error.
- Bật riêng Goddess thật trong Loose trả về sau 0,5 giây; sau đó Bật toàn bộ khởi động đủ Goddess, Account, Relay, Bishop, GameServer và Event. Snapshot đúng PID GameServer 18176 báo 8.059 NPC/quái, giao diện render báo SẴN SÀNG, sáu đèn xanh và các nút còn dùng được.
- Đã đóng Studio bằng CloseMainWindow (không ép tắt hoặc bỏ chỉnh sửa), cập nhật và đối chiếu hash 48 file ở hai vị trí, mở lại Studio PID 150976. World không bị restart trong lúc cập nhật Studio.

Trong kiểm tra live còn phát hiện GameServer Loose thoát với 0xC0000139: CoreServer Dev từ lần sửa VTT trước cần ba export PtReleaseNextName/PtReleaseEnabled/PtReleaseMount mà Engine.dll cũ chưa có. Đã sao lưu và đồng bộ Engine.dll Dev hiện hành vào Server Loose. Đối chiếu export không mất hàm cũ, thêm 27 hàm. Đây là phần phụ thuộc đã bị bỏ sót ở lần triển khai CoreServer trước; không thay binary client hoặc bản phát hành.

Bằng chứng, log, ảnh render, backup Studio và Engine: `analysis/studio_server_start_20261029`. Cụm Loose được giữ chạy sau kiểm tra.

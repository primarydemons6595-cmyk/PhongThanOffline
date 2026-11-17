# Studio Build Client/Server/Event — 26/10/2026

Đã thêm checkbox **Bypass Execution Policy** vào tab **Build Client/Server/Event**. Mặc định bỏ tích. Khi tích, Studio truyền `-ExecutionPolicy Bypass` trước `-File` cho tiến trình PowerShell build; không gọi `Set-ExecutionPolicy` và không thay đổi chính sách Windows lâu dài.

Target **Event** gọi `Build/Build-PhongThanEvent.ps1` để build dự án `Sources/EventService/PhongThan.EventService.csproj` bằng .NET. Output nằm trong `PhongThanSource/Output/Server`. Các target C++ giữ pipeline VC6 hiện có.

Publish Event chỉ nhận bộ binary đã build thành công trong phiên Studio: EXE, DLL, deps.json, runtimeconfig.json và PhongThanStudio.Core.dll. Không ghi đè PhongThan.Event.cfg, access.token hoặc trạng thái EventService. Cần tắt riêng dịch vụ Event trước khi publish target này; không bắt dừng GameServer.

Kiểm chứng: 40 kiểm tra, gồm đối số bypass cho mọi target, ánh xạ đủ binary, loại trừ cấu hình/token, chặn target lạ và build Event thật qua NativeBuild. Studio build Release không warning/error; đã render và xem giao diện.

Đã cập nhật 8 DLL/PDB của Studio tại Release/net10.0-windows và Verified, có backup và xác minh SHA256. GameServer PID 48580 vẫn chạy. Chưa publish binary Event mới vào Server vì yêu cầu lần này là bổ sung chức năng build trong Studio.

Bằng chứng: `analysis/studio_build_event_20261026/tests.log`, `build_preview.png`, `publication.json`.

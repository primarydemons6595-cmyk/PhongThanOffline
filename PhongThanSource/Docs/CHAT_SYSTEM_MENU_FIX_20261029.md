# Giữ tin hệ thống và neo menu vào điểm bấm — Loose, 29/10/2026

## Lỗi được sửa

Bản trước chỉ chép tin từ `m_Sys.m_SysRoom` vào khung hệ thống bên trái. Thông báo server đi qua channel -1/GM vẫn chỉ có trong khung chat chung. Khi nút hình người ẩn khung chung, những thông báo này cũng biến mất khỏi màn hình.

`PhongThanSystemChatHistory.h` được dùng ngay sau bước mã hóa nội dung để giữ cả tin GM, CH_SYSTEM và tin hệ thống riêng. Phân loại theo kênh/nguồn, không dựa vào tên người gửi hoặc nội dung. Giữ nguyên byte mã hóa, độ dài và dữ liệu vật phẩm; bỏ lần chèn CH_SYSTEM trùng lặp. Khi chuyển sang hệ thống, hiện rõ khung và danh sách, khôi phục dòng lịch sử đang bị fade, cuộn về cuối; không tự fade khung này trong chế độ chỉ xem hệ thống. Chat người chơi vẫn nhận và giữ lịch sử để bật trở lại. Có log `CHAT_VISIBILITY` kèm số tin và hình học khung để chẩn đoán trên phiên chơi thật.

Menu cũ đi qua bộ định vị tooltip `ALW_GetWndPosition`: cứ click ở nửa dưới màn hình là lùi lên toàn bộ chiều cao menu, dù bên dưới còn đủ chỗ. Menu người chơi từ chat/Top/bang nay dùng cờ neo riêng: giữ tọa độ click nếu đủ chỗ, chỉ giới hạn vào mép màn hình khi cần. Các menu khác giữ cách định vị cũ. Có tên đích ở dòng đầu menu, dòng này không thực hiện thao tác.

Bảng Top cho chọn dòng bằng chuột trái, đổi màu cả dòng; chuột phải vào dòng cũng chọn đúng người và mở menu, không chỉ giới hạn vào ô tên. Tên đích được giữ từ dữ liệu trang xếp hạng; đổi trang làm sạch lựa chọn. Danh sách bang dùng con trỏ UI hiện tại thay vì vị trí chuột world có thể đã cũ, đồng thời chọn đúng dòng khi chuột phải.

## Kiểm tra và triển khai

- `Tests/Test-ChatSystemMenu.ps1`: PASS 1.734 kiểm tra helper sản xuất. Bao gồm phân loại nguồn GM/hệ thống riêng/CH_SYSTEM, không đưa chat người chơi vào khung hệ thống, không chèn CH_SYSTEM hai lần, giữ nội dung có byte 0 và metadata vật phẩm; tọa độ click nửa dưới màn hình còn chỗ và giới hạn ở 5 kích thước màn hình.
- Build Dev `GameClient`: thành công. Kiểm tra bản Game.exe mới với DLL hiện có: 226 liên kết nội bộ, không thiếu symbol.
- Chỉ thay `PhongThanRuntime-Loose/Client/Game.exe`; giữ nguyên hai Core DLL và GameServer.exe theo hash. Không khởi động lại world PID 125080. Gói/hash trước-sau: `Deploy/ProjectContent/ChatSystemMenuFix20261029`.
- Log, backup và kiểm tra sau triển khai: `analysis/chat_system_menu_fix_20261029`.

Kiểm thử fixture không thay thế kiểm tra hiển thị trong phiên nhân vật thật. Cần nghiệm thu nút hình người và menu chat/Top/bang sau khi đăng nhập bằng client mới.

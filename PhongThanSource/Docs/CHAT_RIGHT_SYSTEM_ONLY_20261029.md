# Tin hệ thống chỉ ở khung phải — Loose, 29/10/2026

Theo ảnh xác nhận của người dùng, nút hình người chỉ ẩn các khung chat bên trái; tin hệ thống giữ nguyên ở khung phải. Không chuyển hoặc sao chép nội dung hệ thống sang trái.

Đã bỏ lệnh gọi `PtKeepSystemChat` và bỏ việc hiện `CHANNELTAB_SYSTEM` bên trái khi tắt chat. Nút chỉ điều khiển các khung chat trái; khung `m_Sys` bên phải không bị di chuyển, ẩn hay sao chép. Khi tắt cũng không vẽ nền chat trái. Bật lại khôi phục lịch sử chat đã nhận. Giữ bản sửa vị trí menu, chọn dòng Top và tên đích.

Chỉ build/thay `PhongThanRuntime-Loose/Client/Game.exe`. Build Dev GameClient thành công; 226 liên kết DLL nội bộ của Game.exe không thiếu symbol. Hai Core DLL và GameServer.exe giữ nguyên hash; world PID 125080 không khởi động lại. Manifest và bản sao trước thay: `Deploy/ProjectContent/ChatRightSystemOnly20261029`, `analysis/chat_right_system_only_20261029`.

Tài liệu này thay thế hành vi “chép tin hệ thống sang khung trái” trong `CHAT_SYSTEM_MENU_FIX_20261029.md` và `REMOTE_PEOPLE_CHAT_LOOSE_20261029.md`. Helper/test lịch sử hệ thống cũ không còn được UI sản xuất sử dụng. Chưa nghiệm thu hiển thị trong phiên nhân vật thật.

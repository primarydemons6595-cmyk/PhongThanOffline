# Nút hình người chỉ ẩn nội dung — Loose, 29/10/2026

Điều chỉnh theo yêu cầu: giữ khung chat, nền, thanh cuộn và các nút tại chỗ; chỉ ẩn danh sách chữ/tin nhắn bên trong. Khi bật lại, hiện lịch sử đã lưu. Tắt khả năng bấm tên/vật phẩm của nội dung đang ẩn. Khung hệ thống bên phải độc lập, không di chuyển hoặc sao chép sang trái.

`SetPlayerChatVisible` giữ nguyên bố trí public/private và chỉ gọi Hide/Show trên message list con. PaintWindow vẫn vẽ nền khung dù nội dung đang ẩn. Bản này thay thế hành vi ẩn cả khung trong `CHAT_RIGHT_SYSTEM_ONLY_20261029.md`.

Build Dev GameClient thành công; 226 liên kết nội bộ không thiếu symbol. Chỉ áp `PhongThanRuntime-Loose/Client/Game.exe`; hai Core DLL và GameServer.exe giữ nguyên. World PID 125080 không khởi động lại. Manifest, backup và bằng chứng ở `Deploy/ProjectContent/ChatContentOnly20261029`, `analysis/chat_content_only_20261029`. Chưa nghiệm thu nút bằng nhân vật thật.

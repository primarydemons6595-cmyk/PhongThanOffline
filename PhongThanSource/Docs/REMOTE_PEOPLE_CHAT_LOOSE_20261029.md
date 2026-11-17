# Menu người chơi từ xa và chat mật — Loose, 29/10/2026

## Hành vi

- Chuột phải vào tên người chơi trong lịch sử chat, bảng xếp hạng người chơi và cột tên trong danh sách thành viên bang/quốc gia mở menu cùng kiểu menu cạnh nhân vật. Không cần giữ Ctrl; Alt vẫn mở biểu cảm. Danh sách xếp hạng quốc gia không được coi là danh sách tên nhân vật.
- Menu mở được với tên đang offline. Chỉ khi chọn thao tác, client gửi yêu cầu kiểm tra đến server. Server tra người chơi đang kết nối và đã vào map; offline/trong lúc đổi server không được coi là online. Phản hồi offline phân biệt xem trang bị và thao tác tổ đội. Không dựa vào trạng thái online cũ trong danh sách bang hoặc dòng chat.
- Menu từ xa không có Giao dịch và Oẳn tù tì. Server cũng từ chối hai mã thao tác này, không chỉ ẩn chúng ở UI. Menu cạnh nhân vật giữ nguyên.
- Xem trang bị, xin vào đội và mời đội được xử lý trên server với đích tra theo tên/ID. Luồng xin vào đội vẫn cần đội mở, đội trưởng chấp nhận và còn chỗ; luồng mời vẫn kiểm tra quyền đội trưởng và trạng thái nhận lời mời. Chấp nhận đơn từ xa chỉ tìm người đã có yêu cầu dành cho đúng ID đội trưởng, không cho nhận thành viên tùy ý.
- Theo sau vẫn yêu cầu nhân vật ở gần; nếu ở xa có thông báo rõ. Không thay đổi di chuyển hay dịch chuyển nhân vật.

## Chat mật và nút hình người

- Danh sách chọn kênh có thêm `Chat mật...`, nhập tên người nhận để mở cuộc trò chuyện. Chọn `Mật` trong menu người chơi cũng mở cuộc trò chuyện, chọn người nhận gần đây và đưa con trỏ vào ô nhập.
- Bổ sung định tuyến chat mật trực tiếp trong world Loose bằng packet chat/feedback gốc. Chỉ người nhận nhận tin; người gửi nhận phản hồi thành công hoặc offline. Danh tính người gửi lấy từ kết nối server, không lấy từ client. Giữ định dạng nội dung và dữ liệu đính kèm; giới hạn độ dài, kiểm tra packet và chống gửi dồn.
- Sửa thông báo không gửi được chat mật sang thông báo người nhận đã offline, thay chuỗi tên game cũ bị lỗi.
- Nút hình người ở góc trái chuyển toàn bộ nội dung chat người chơi sang nội dung hệ thống và ngược lại. Không dùng nút này để bật/tắt riêng bộ lọc chat mật. Vẫn nhận và lưu lịch sử trong khi ẩn, bật lại không xóa lịch sử. Chuột phải nút giữ menu cửa sổ chat mật riêng; tooltip giải thích cả hai thao tác.

## Xem đồ từ xa

Gói xem trang bị phiên bản 3 bổ sung thông tin chủ trang bị (tên, danh hiệu, bang, phối ngẫu, nghề, danh vọng, phúc duyên, PK, chuyển sinh, giới tính và cấp). Cửa sổ xem đồ không còn phụ thuộc NPC có mặt quanh người xem. Tiếp tục gửi đủ dữ liệu Cam, Tinh Quân, Tiên/Ma và Khảm của bản sửa trước. Packet 13 slot dài 15.691 byte; kiểm tra biên dịch bảo đảm dưới bộ đệm server 16 KiB trừ 32 byte dự phòng. Client/server phải cập nhật cùng nhau.

## Kiểm tra

- `Tests/Test-RemotePeopleChat.ps1`: 68 kiểm tra handler server và chat mật, 16 kiểm tra client. Bao gồm offline, kết nối chưa hoàn tất, đổi server, actor/map/target giả, thiếu kết thúc tên, phản hồi cũ, kiểm tra ID, xem đồ/đội khác map, gửi riêng không lọt người ngoài, phản hồi về người gửi, độ dài sai, rate limit và nội dung 255 byte. Dùng packet và handler sản xuất, thay thế kết nối/thế giới/tác động đội bằng fixture; không thao tác nhân vật thật.
- `Tests/Test-ViewedItem.ps1`: 109 kiểm tra dữ liệu trang bị và thông tin chủ trang bị, bao gồm không cộng nâng cấp hai lần.
- Protocol baseline: PASS 49 kiểm tra, 13 nhóm giao thức. Build Dev CoreServer/CoreClient/GameClient thành công. Kiểm tra trước thay: 554 liên kết DLL nội bộ, không thiếu symbol. Sau thay toàn client: 27 PE, 1.405 liên kết nội bộ, không thiếu symbol.

## Triển khai

Chỉ thay ba file trong Loose: `Client/Game.exe`, `Client/CoreClient.dll`, `Server/CoreServer.dll`. Gói và hash trước/sau ở `Deploy/ProjectContent/RemotePeopleChat20261029/manifest.json`. World 156052 lưu/thoát sạch exit 0; chín file nhân vật không thay đổi trong lúc sao chép. World mới PID 125080.

Sau thay: 3/3 file khớp manifest; world nạp đủ 105 map, snapshot mới và EventService xác nhận đúng PID với lease hợp lệ. Client PID 151972 mở cửa sổ `Phong Than Online`, phản hồi bình thường, nạp đúng CoreClient.dll mới và core init hoàn tất.

Bằng chứng, log và backup: `analysis/remote_people_chat_20261029`. Chưa nghiệm thu thao tác bằng hai tài khoản thật; kiểm tra hành vi server trong mục trên là kiểm tra fixture chạy handler thật.

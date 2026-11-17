# Cập nhật Loose: F3/F4, giao diện Tiên/Ma và bốn bot

Đã cài ngày 08/11/2026: `Client/Game.exe`, `Client/CoreClient.dll`, `Server/CoreServer.dll`, `Client/ui/realm_skills.ini` và manifest Client. Biên nhận: `analysis/followup_remaining_20261108/installation.json`. Có bản sao lưu đầy đủ tại đường dẫn ghi trong biên nhận. Không build/cập nhật runtime Release.

## Phần đã sửa

- F3 chỉ thay ảnh chân dung khi đường dẫn ảnh đổi; trước đây đặt lại ảnh và trạng thái ảnh mỗi khung hình. Khi bảng thuộc tính mở rộng đóng, không truy vấn/dàn lại các dòng ẩn. Kiểm tra hiện/ẩn trang vẫn chạy trong chu kỳ cập nhật 250 ms và khi chuyển trang.
- Đường vẽ trang bị F3/F4 dùng chung bộ nhớ đệm thông số SPR và kích thước/độ lệch từng frame. Lưu cả lần tìm ảnh thất bại, thử lại sau 5 giây; giới hạn 512 đường dẫn. Không lưu con trỏ frame hay bỏ kiểm tra dữ liệu của renderer. Vòng sáng, ngọc và chuyển frame vẫn chạy; không đổi chỉ số đồ.
- Chân dung Tiên/Ma dịch đúng chênh lệch giữa vòng ảnh gốc: +11,+4 pixel ở khung chuyển sinh thường; +9,+4 ở khung trang bị Tiên/Ma. Khung Nhân Giới giữ tọa độ cũ. Xem `analysis/followup_remaining_20261108/avatar-comparison.png`.
- Nút Tiên/Ma F5 chuyển vào vùng trống bên phải, theo từng phái. Điều kiện mở Tiên/Ma được giữ. Xem `analysis/followup_remaining_20261108/f5-tabs-comparison.png`. Hai ảnh là dựng từ SPR và tọa độ thật, không phải ảnh chụp phiên game đang chơi.
- Ba bot Quan.Gamer, Hải Bánh Cuốn, PhongNguyet3465 bị chặn khi khôi phục đồ dòng Vũ Sĩ 310. Bản cũ nhận nhầm theo tên giống bộ 311+, lưu `equip_set` và các thuộc tính kích hoạt bộ, dù dòng 310 gốc là đồ trắng. Bản sửa chỉ chuyển đổi khi toàn bộ mẫu thuộc tính cũ khớp chính xác bộ lỗi 55–59, đồng thời base/yêu cầu khớp bảng hiện tại. Xóa phần kích hoạt bộ gắn nhầm; giữ nguyên món đồ, serial, thuộc tính cơ bản, yêu cầu và các phần khác. Dữ liệu khác không khớp vẫn bị từ chối.
- xXSniperXx đầy cả túi, còn cầm món serial 78351. Bot có luồng phục hồi riêng ở server: chỉ khi túi không có chỗ, chuyển nguyên món đang cầm vào ô kho đã mở còn trống và lưu. Không đổi quyền mở kho của người chơi. Khi kho cũng đầy hoặc bot khóa/giao dịch, giữ nguyên món đang cầm. Bản chính sẽ áp dụng khi bot được nạp/chạy lại; không ghi đè file nhân vật chính bằng file thử nghiệm.

## Kiểm tra đã thực hiện

- Nạp đúng bốn bản sao PTHC bằng Core server thật, bật đúng luồng persistence đang dùng ở Loose. Trước sửa tái hiện đúng ba lỗi nạp; sau sửa cả bốn nạp/lưu được, sau đó nạp lại tiếp tục thành công.
- Đối chiếu đủ 109 món và serial trước/sau. Chỉ món 78351 đổi container tay → kho; năm món dòng 310 bỏ đúng mẫu kích hoạt bộ lỗi. Các record đồ còn lại giữ nguyên. Thử giả mạo thuộc tính thêm, giá trị bộ, base hoặc yêu cầu đều bị từ chối. Báo cáo: `analysis/followup_remaining_20261108/bot-conservation.json`, `probe-current.stdout.log`, `probe-reload.stdout.log`.
- Kiểm thử cache: 10.000 lượt dùng lại cùng ảnh còn 1 lần hỏi metadata và 4 lần hỏi frame cho animation 4 frame; có kiểm tra ảnh thiếu/phục hồi, giới hạn bộ nhớ và tràn đồng hồ. Đây là số lời gọi trong harness, không phải kết quả đo FPS trong game.
- Kiểm thử F4/socket: giữ thứ tự vòng/ngọc, frame offset, ảnh nền nguyên gốc; kiểm thử tooltip với 100 ánh xạ lỗ và 53 ảnh ngọc gốc đều qua.
- Clean rebuild hai Core Dev và build Game Dev qua; DLL đã cài nạp thành công. Gói thử có export chẩn đoán riêng được kiểm tra và loại khỏi bản cài. Không dùng DLL thử để triển khai.
- Lúc cài, 87 file CharacterStore, 77 file trong thư mục lưu bot, 41 binary Release, các bảng Skills/Missles và hai loose index giữ nguyên hash.

## Chưa xác nhận / còn thiếu

Chưa có phép đo FPS hoặc nghiệm thu F3/F4 trực tiếp trong một phiên chơi sau cài. Các thay đổi loại những công việc lặp đã xác định trong mã; chưa kết luận đã hết mọi nguyên nhân tụt FPS.

11 script chết/rơi đồ boss Tiên/Ma và công thức gốc của Huyết Chiến Sa Trường (1481), Độc Hành Thiên Hạ (1487) vẫn chưa có trong nguồn đã xác minh. Audit trước đó nằm ở `analysis/boss_skill_ui_20261107/boss-pak-audit.json`, `WEB_RESEARCH_20261107.md`. Tìm thêm trên mạng lần này chưa có nguồn công thức/script server xác thực. Chưa bật cơ chế thay thế. Đã hỏi riêng người dùng có cho phép cơ chế Loose cấu hình trong Studio cho đúng phần thiếu này hay không; chưa coi việc cho phép sáu nhóm Thần Kỹ trước đây là cho phép thay toàn bộ boss/skill Tiên Ma.

Các phần Thần Kỹ đã cài trước đó vẫn giữ nguyên: 15 nhóm, 51 biến thể sách, công thức thử nghiệm của sáu nhóm chỉnh trong Studio; chỉ sửa hồi chiêu Thần Kỹ, không đổi skill phái thường. Xem `THAN_KY_EXPERIMENT_20261107.md`.

Follow-up 08/11/2026: user-approved Loose trials for missing boss scripts and two passive formulas are now installed. See REALM_TRIAL_AND_F5_BUTTON_20261108.md and analysis/realm_trial_20261108/installation.json.

# Bot và mật tịch — 09/11/2026

Chỉ Dev Loose. Trạng thái cài phải đối chiếu `analysis/bot_autonomy_20261109/installation.json`. Candidate và DLL có `PtProbeAutonomy` là bản kiểm thử, không tự coi là đã cài.

Đã cài 28 tệp lúc 13:43 ngày 09/11/2026, có bản sao lưu `analysis/bot_autonomy_20261109/backup-20261109-134318`. Tại thời điểm cài, 138 tệp CharacterStore và 128 tệp nhân vật bot giữ nguyên; 41 binary runtime Release giữ nguyên. Khởi động lại GameServer/Event để nạp bộ mới; chỉ xác nhận sẵn sàng khi trạng thái native thuộc PID mới và còn hạn, không dựa trên tệp trạng thái phiên trước.

Kiểm tra lúc 13:56: GameServer PID 62340 đã kết nối Database/Relay/Gateway; Event PID 68720 nhận trạng thái đúng PID mới, lease hợp lệ, không có lỗi sự kiện. Có 200 bot trực tuyến trong cấu hình hiện tại. Hash 28 tệp cài vẫn đúng sau khởi động. Bằng chứng: `analysis/bot_autonomy_20261109/live-start-verification.json`.

Không coi khởi động thành công là hết mọi lỗi: mẫu này có 4 bot báo `inventory_bank_permission_or_password_required`; chúng không được vượt quyền kho hoặc xóa vật phẩm. Console còn báo thiếu một số script `skill/zhuansheng`, `skill/huanhun` và một dòng `Packing sync data failed...` sau kết nối. Đã lưu console trong `analysis/bot_autonomy_20261109/live-console.txt`; chưa xác định nguyên nhân của lần đóng gói thất bại hoặc sửa những tham chiếu script đó trong bộ cập nhật bot/UI/loot này. Không gán chúng cho thay đổi mới khi chưa có đối chiếu hoặc tái hiện.

## Mật tịch

Yêu cầu cuối: **đúng loại quái, mọi map, không khóa phái**. Thêm vào bảng loot riêng đang gắn với quái, không so chuỗi tên có thể đổi và không thêm lượt xổ độc lập.

| Template | Mật tịch detail | Bảng thường / xanh |
|---|---|---|
| 0 — Kiếm Nhân | 303 | jingren |
| 7 — Hoàn Cẩu | 304 | huangou |
| 2 — Hỏa Diện | 305 | dazhongren |
| 9 — Thảo Tiên | 306 | caoxian |
| 1 — Tuyết Quái | 307 | xueyao |
| 8 — Yếm Hỏa | 308 | yanhuo |

Mỗi bảng thêm một dòng `(Genre=6, Detail=1, Particular=<mã cuộn>, Level=1)`, qua adapter native ra đúng detail catalog. Tỷ lệ dùng RandRate dương phổ biến nhất trong các dòng trang bị/sách của cùng bảng quái. Giữ nguyên RandRange, Treasure, tiền và các dòng cũ. Quái thường/xanh có tỷ lệ khác nhau; hệ số sự kiện tiếp tục áp dụng qua cùng plan. Tỷ lệ/hash cụ thể: `scroll-drop-staging.json`.

Đây là bổ sung theo yêu cầu người dùng, không phải xác suất mật tịch VNG nguyên bản đã khôi phục. Script nhận nhiệm vụ đã có. Không tự sáng tác mức thưởng hoặc đổi chuỗi Giáo Huấn hiện hữu trong đợt thêm loot này.

## Bot

- Phục hồi có backoff, hạ dần ngưỡng tìm quái và lập lại đường khi không tiến triển; giữ nhân vật/nhiệm vụ/vật phẩm.
- Đầy túi: gộp chồng, bán dư an toàn, gửi kho đúng quyền/mật khẩu tại Thủ Khố; kho cũng đầy thì bán đồ dư có giá trị qua giao dịch thật. Giữ đồ đang dùng, khóa, nhiệm vụ, sách và vật phẩm đã hẹn giao dịch. Không có phương án an toàn thì chờ, không xóa đồ hiếm.
- Giá tham chiếu theo vật phẩm/nhóm, có biên giá. Mặc định dùng giá NPC/lực/phẩm chất: là giá tham chiếu Loose, không phải thị trường VNG. 1 Xu = 60 vạn = 600.000 lượng; khối Xu = 20 Xu. Dùng Xu thật và tiền lẻ, giữ tỷ giá lúc hẹn; không sinh tiền.
- Đội boss dùng quyền đội/sức chứa lãnh đạo thật. VTT/Thương Chu dùng vé, giờ, hội thoại, nhiệm vụ của server; không thưởng chỉ vì chọn kế hoạch. Nhận thưởng trận trước ở NPC để tránh khóa lần báo danh sau. Oẳn tù tì dùng giao dịch gốc.
- Tạo quốc gia vẫn cần cấp/danh vọng/lệ phí/vị trí, xin gia nhập cần duyệt. Lập thành vẫn cần sách, ngân quỹ, đồng thau, danh vọng, nhân khí. Bot không được cấp miễn phí tài nguyên.
- Cống hiến bot phải lưu atomic nhân vật bot; người thật giữ gate xác nhận lưu Goddess. Không chờ một xác nhận Goddess mà bot không có socket để nhận.
- Bổ sung ba NPC dịch vụ tại map 1001: Thương Thang (206,201), Thủ Khố Lãnh Địa (207,201), Sứ Giả Lãnh Địa (208,201). Đây là vị trí bố trí riêng cho Loose, không phải tọa độ gốc đã xác minh. Bot chờ đủ điều kiện và sách lập thành; chưa tự tìm mua sách còn thiếu để lập thành.
- NPC hội thoại dùng trạng thái IsAlive, không đòi máu dương: NPC gốc máu 0 vẫn hợp lệ.
- Tự vệ từ sát thương thật; gọi bạn từng cùng đội/đồng đội/người cùng quốc gia, có trần hỗ trợ và đi đường bình thường. PvP chủ động chỉ chọn bot hoặc người mở PK tự do; luật native quyết định được đánh hay không.

Studio → Bot → **Tự xử lý / giá mua bán** → **Bang hội, lãnh địa & PK/PvP**. Có revision chống ghi đè, nạp lại tối đa 5 giây; giá đã hẹn giữ tỷ giá cũ.

## UI và bằng chứng

Nút NPC cố định 95×28, 3 cột/2 hàng mỗi trang; nhãn dài xuống dòng. Tab Tiên-Ma căn theo INI active trong loose_index; Đạo Sĩ/Vũ Sĩ đã lên Tiên-Ma dùng thêm hàng tab tránh chồng. Bỏ mô tả kỹ thuật “Loose thử nghiệm/chỉnh Studio” khỏi tooltip game, giữ nguồn ở Studio/tài liệu.

Kiểm thử trong analysis: cấu hình managed, biên giá/đồng minh/PvP, render WPF Classic/Liquid hai kích thước, giao dịch native có bảo toàn tiền/đồ, đầy rương/đồ hiếm, lập quốc gia, tự vệ/hỗ trợ và sát thương skill thật. Chỉ coi ca có `SCENARIO_PASS` là đã chạy. Biên dịch được không chứng minh cả chu kỳ sự kiện/tải lớn đã thử đủ.

Chưa đo tải 500 bot chiến đấu liên tục hoặc FPS trực tiếp của phiên người dùng. Đây là kế hoạch hành vi có kiểm thử, không phải mô hình học máy tự huấn luyện.

# Hiệu ứng trang bị trong client gốc — 29/10/2026

Đây là kiểm tra chỉ đọc. Không thay luật hoặc màu hiệu ứng của Loose/Release.

Cập nhật tiếp theo theo yêu cầu người dùng: đã nối tài nguyên vào Loose; xem `EQUIPMENT_GLOW_LOOSE_20261029.md` để biết chính xác điều kiện triển khai và giới hạn. Phần bên dưới giữ phạm vi đối chiếu dữ liệu gốc.

Đọc trực tiếp theo thứ tự package.ini của Client_Original, giải mã và kiểm tra tất cả frame của 21 SPR trang bị được bảng trạng thái gốc tham chiếu. Không dùng sprite hiện tại trong Loose làm bằng chứng cho màu bản gốc. Đợt kiểm tra đầu chỉ chọn 13 mẫu và đã bỏ sót các state đồ Cam 44/45 và bộ Tiên/Ma 62–64. Công cụ hiện tự tìm các dòng trang bị trong toàn bảng, thay cho danh sách chọn tay.

Bảng `settings/npcres/zhuang_tai_tu_xing_dui_zhao_biao.txt`: vng00.pak, entry 3183, ID 9BE4F4F4, SHA256 cd73ca2d46daa0d9b2b0f10283ca3e1afc6ea1a3acd73603156768eb3a935b0f.

| State gốc | Nhãn trong dữ liệu gốc | Màu quan sát từ sprite |
|---|---|---|
| 26 | Hiệu ứng bộ 1, circle-perfect1 | Vàng |
| 27 | Hiệu ứng bộ 2, circle-perfect2 | Xanh tím rất nhạt/trắng |
| 28 | Hiệu ứng bộ 3, circle-perfect3 | Xanh ngọc/trắng |
| 44 | Hiệu ứng đồ Cam, circle-perfect4 | Vàng cam/đỏ |
| 45 | Hiệu ứng đồ Cam 2, circle-perfect5 | Tím hồng đậm |
| 62 | Hiệu ứng bộ Tiên/Ma Giáp Sĩ, 仙魔绿套甲 | Vàng/trắng |
| 63 | Hiệu ứng bộ Tiên/Ma Đạo Sĩ, 仙魔绿套道 | Hồng tím/trắng |
| 64 | Hiệu ứng bộ Tiên/Ma Dị Nhân, 仙魔绿套异 | Xanh lam/trắng |
| 35, 37 | Bộ 3 / 5 món +12, nữ | Xanh lục/ngọc, tâm trắng |
| 36, 38 | Bộ 3 / 5 món +12, nam | Vàng cam, tâm trắng |
| 72 | Bộ Tiên/Ma 3 món +12, nữ | Xanh lục/ngọc, tâm trắng |
| 73 | Bộ Tiên/Ma 3 món +12, nam | Vàng cam, tâm trắng |
| 39 | Vũ khí +12, biến thể 1 | Dải xanh ngọc và điểm đỏ cam |
| 40 | Vũ khí +12, biến thể 2 | Trắng |
| 41, 42 | Vũ khí +12, biến thể 3 / 4 | Trắng/xanh ngọc nhạt |
| 130 | Bộ liên server Giáp Sĩ | Vàng |
| 131 | Bộ liên server Đạo Sĩ | Xanh ngọc |
| 132 | Bộ liên server Dị Nhân | Đỏ cam |

Đây là nhãn và màu tài nguyên, chưa phải bằng chứng đầy đủ về điều kiện kích hoạt phía server. **Bản gốc có glow tím/hồng**, và bảng trạng thái gắn rõ một mẫu với đồ Cam, một mẫu với bộ Tiên/Ma Đạo Sĩ. Vì vậy kết quả đợt đầu không được dùng để kết luận đồ Cam không có màu riêng. Chưa đủ căn cứ để kết luận chỉ cần đeo một món là kích hoạt, hoặc xác định chính xác bộ Tinh Quân/Khảm nào chọn từng state. Hai state Tiên/Ma +12 72/73 phân biệt nam/nữ, trong khi ba state bộ Tiên/Ma 62–64 phân biệt phái; phải phân biệt hai nhóm hiệu ứng này.

## So với source Loose hiện tại

`Headers/PhongThanEquipmentWorldPresentation.h` và `PhongThanPlayerSnapshot.inl` hiện chọn vòng thường bằng số món cùng SetID/Group và phái, chọn hiệu ứng bộ +12 theo nam/nữ. Đây là cầu nối do dự án viết; phần 3 món mờ/5 món sáng có ghi rõ là quy ước đã chọn trong bản Loose, không phải luật server VNG đã chứng minh. Hiệu ứng vũ khí +12 đang dùng state 39 cho mọi phái theo lựa chọn Loose trước đó, khác việc client gốc có bốn tài nguyên 39–42. State 72/73 được cho phép trong protocol nhưng chưa được tự chọn cho bộ Tiên/Ma trong hàm BuildPhongThanPlayerSnapshot.

Vì vậy không thể dùng việc Loose hiện có vòng sáng để khẳng định đã khớp đầy đủ bốn nhóm đồ người dùng hỏi. Đợt này chỉ báo kết quả đối chiếu, không tự đổi các lựa chọn hiển thị đã có.

Bằng chứng: `analysis/original_equipment_glow_20261029/audit.json` (đường dẫn, PAK, hash từng SPR và màu đo được), `original-state-table.txt`, các SPR nguyên byte và `original-glow-sheet.png`. Công cụ chạy lại: `Tools/audit_original_equipment_glow.py`.

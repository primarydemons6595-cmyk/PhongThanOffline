# Nối Glow trang bị vào Loose — 29/10/2026

Gói `Deploy/ProjectContent/EquipmentGlow20261029`: 11 file, gồm CoreServer/CoreClient, bảy SPR gốc và hai bảng trạng thái client/server. Đã áp vào `PhongThanRuntime-Loose`, có backup trước khi thay. Cấu trúc packet/số slot hiệu ứng không đổi. Không triển khai vào PhongThanRelease.

## Hiệu ứng đã nối

| Nhóm đang đeo | Mốc của Loose | State / màu tài nguyên gốc |
|---|---|---|
| Bộ thường có SetID/Group | 3/4 món, 5 món | 26–28 theo phái, giữ tầng mờ/sáng hiện có |
| Cam | 3/4 món, 5 món | 44 vàng cam, 45 tím hồng |
| Tinh Quân nhân giới | Từ 3 món | 45 tím hồng |
| Bộ Tiên/Ma chưa đủ toàn bộ +12 | Từ 3 món | 62 Giáp Sĩ vàng, 63 Đạo Sĩ hồng tím, 64 Dị Nhân xanh |
| Bộ nhân giới toàn bộ +12 | 3/4 món | 35 nữ xanh lục/ngọc, 36 nam vàng cam |
| Bộ nhân giới toàn bộ +12 | Từ 5 món | 37 nữ xanh lục/ngọc, 38 nam vàng cam |
| Bộ Tiên/Ma toàn bộ +12 | Từ 3 món | 72 nữ xanh lục/ngọc, 73 nam vàng cam |

Hiệu ứng bộ +12 thay hiệu ứng bộ cơ bản như quy tắc đang có, không chồng cả hai vòng sáng. Vũ khí +12 tiếp tục dùng lựa chọn state 39 chung đã có trong Loose. Đồ Khảm giữ hiệu ứng của bộ/loại trang bị nền: lớp socket không làm mất danh tính Cam, Tinh Quân hoặc Tiên/Ma.

ID, sprite và màu đã đối chiếu trực tiếp client gốc; các nhãn 3/5 món +12 và Tiên/Ma 3 món +12 cũng ghi trực tiếp trong bảng trạng thái gốc. **Luật kích hoạt bổ sung là chính sách nối của Loose**, không tuyên bố đã phục hồi toàn bộ luật server VNG. Đặc biệt việc chọn 44/45 cho Cam và 45 cho Tinh Quân theo mốc trên là lựa chọn triển khai, không suy ra chỉ từ màu tên đồ.

## Chọn đúng bộ, đúng lớp trang bị

Server chỉ đọc các slot đang đeo trong bộ trang bị hoạt động. Khi có SetID/Group, chỉ cộng các món cùng bộ và cùng nhóm chất lượng/realm. Với Cam/Tinh Quân/Tiên Ma không có SetID trong bảng gốc, chỉ ghép các món cùng nhóm và cùng bậc; đồ trắng/xanh không được cộng vào các nhóm này. Dấu Tiên/Ma lấy từ requiregdlevel/requiregdflag thực tế của item, không suy từ tên hoặc từ việc nhân vật đã thăng tiên/nhập ma.

Kiểm tra tất cả bộ đang đeo và chọn bộ đủ món lớn nhất, ưu tiên nhóm cao hơn nếu bằng số món. Sửa lỗi cũ chỉ xét bộ của món hợp lệ đầu tiên nên một món lẻ có thể che cả bộ đủ năm món phía sau. Khi tháo đồ hoặc chuyển bộ hoạt động, snapshot tính lại các state từ đầu, không giữ Glow cũ.

## Kiểm tra

- Bộ kiểm thử native chạy với assert bật: 2/3/5 món, loại/bậc/bộ khác nhau, toàn bộ +12 và một món +11, nam/nữ, chọn bộ sau món lẻ, đảo thứ tự slot, state mới và ghép packet.
- Build Dev thành công. Hai DLL mới không thiếu hàm nhập từ các DLL đang có trong Loose; kiểm tra toàn client: 27 PE, 1.406 liên kết nội bộ, không thiếu symbol.
- Bộ nạp SPR native đọc từ chính Loose qua package/index: 7 file, 64 frame, 0 lỗi. Không thêm ID giả vào loose_index.
- Cả 11 file triển khai khớp hash gói. World 137688 lưu/thoát sạch exit 0; chín file nhân vật không thay đổi trong lúc sao chép. World mới PID 100920; registry 9.249 script, 0 lỗi nạp.
- Client mới mở thành công, cửa sổ Phong Than Online phản hồi và core init hoàn tất. Chưa nghiệm thu mặc/tháo từng bộ bằng nhân vật thật.
- Xác nhận cuối: world 100920 đã nạp đủ 105 map, snapshot khớp PID và cập nhật trong 1 giây; `final-verification.json` ghi 11/11 hash khớp. Dịch vụ Event hiện PID 151436 sau khi nạp lại danh mục tên boss trong đợt bổ sung tiếp theo.

Bằng chứng: `analysis/equipment_glow_loose_20261029`, gồm test/build log, sprite-loader.log, dependency-audit.json, publication.json, restart.json và backup. Nguồn gốc từng SPR: `provenance.json` trong gói. Báo cáo đối chiếu màu: `Docs/ORIGINAL_EQUIPMENT_GLOW_20261029.md`.

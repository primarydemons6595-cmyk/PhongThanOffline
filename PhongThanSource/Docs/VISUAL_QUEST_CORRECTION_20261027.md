# Sửa hiển thị trang bị, Vũ Sĩ và sách nhiệm vụ — 27/10/2026

Gói `Deploy/ProjectContent/VisualQuestCorrection20261027` gồm 49 file. Trạng thái triển khai phải đối chiếu `analysis/equipment_family_correction_20261027/publication.json`; build thành công chưa có nghĩa runtime đã cập nhật.

## Nguyên nhân và thay đổi

- Sách nhiệm vụ Mini/Tab: `KScenePlaceMapC::PaintCharacters` dùng `m_nMapId` nhưng đường tải bản đồ không gán thành viên đó. Cache server mang mã map thật nên mọi marker bị lọc. Painter nay nhận `SubWorld[0].m_SubWorldID` trực tiếp, giữ vàng cho chính tuyến và xanh cho daily/phụ có thể nhận, tọa độ toàn map và hạn cache 6 giây. Không phụ thuộc NPC đã stream vào màn hình.
- Vũ Sĩ: bảng loại nhân vật tham chiếu tên GBK nhưng gói loose chỉ có các bảng `nan_yu_shi_*`, `nv_yu_shi_*`, `yu_shi_*` và không có ID alias tương ứng. Log thật báo thiếu thân, đầu, vũ khí, ngựa và liên kết động tác. Gói bổ sung 38 bảng mang đúng tên native từ dữ liệu hiện hữu; thân dùng ysm/ysf, ngựa dùng ysx. Không thay bằng thân Giáp Sĩ.
- Bảng nữ Vũ Sĩ tier 1/2 có ô chạy trỏ `run0` không tồn tại; dùng `run1` cùng thân/đầu đã có. Các bảng ngựa Vũ Sĩ thiếu ô đánh tay không khi cưỡi; nối cùng animation tấn công đã có của đúng mount. Chi tiết tại `vusi-table-corrections.json`.
- Đứng cầm vũ khí: tám bảng liên kết động tác của bốn nghề, dùng chung nam/nữ, trước đây ánh xạ đứng bình thường tới ô không cầm vũ khí. Dòng có vũ khí nay chọn tư thế cầm vũ khí đã có, đồng bộ thân/đầu/vũ khí. Dòng tay không giữ nguyên. Không tạo SPR giả hoặc lấy vũ khí phái khác. Các bảng tay phụ không có model vẫn có thể hiện cảnh báo riêng trong trình xem từng layer; cần xét tay có model và kết quả ghép toàn nhân vật.
- Vũ khí tân thủ cấp 1: Đại đao, Pháp kiếm, Hỗn thế phủ (record key 91/101/111) có giá trị part 3, dẫn tới resource 2/bộ 2x. Sửa ba key về part 2/resource 1/bộ 1x ở cả Client và Server. Key 92 và các dòng khác giữ nguyên. Không đổi icon hay chỉ số vật phẩm.
- Tinh Quân/Phá Quân nhân giới: không dùng series 25..30 hoặc cảnh giới người mặc để chọn thân Tiên Ma. `KItemList::Equip` truyền phân loại trang bị thật từ `requiregdflag/requiregdlevel`. Đồ nhân giới 10x chọn row 5; chỉ đồ thuộc bank Tiên Ma mới chọn row 7/8 theo phía Tiên/Ma. Đây là sửa lỗi phân loại trong gói trước.

## Xác minh

- Build Release CoreServer, CoreClient và GameClient đều exit 0.
- `FactionEquipmentFixTests`: 29.430 tổ hợp; kiểm tra đồ nhân giới không đổi bộ khi chủ nhân có cảnh giới, đồ Tiên Ma và biên không hợp lệ. Login Vũ Sĩ giữ level/XP skill và F5.
- `VuSiResourcePipelineTests.cpp`: liên kết object production `KNpcResNode.obj` và `KPhongThanAppearance.obj`, dùng KTabFile và bộ đọc SPR thật, renderer chỉ cung cấp metadata và không mở cửa sổ. Qua 1.296 tổ hợp bốn nghề nam/nữ, chín row thân, đứng/chạy/đánh, ba row vũ khí và hai trạng thái cưỡi. Kiểm tra lớp ngựa riêng cho Vũ Sĩ; không chứng nhận mọi mount các phái khác. Kiểm tra ba key tân thủ resolve về resource 1 và key 92 còn resource 2.
- `QuestMapRenderTests.cpp`: cache packet thật cùng painter production, xác nhận màu, tọa độ, viewport Mini/Tab, NPC xa, cắt biên, chuyển map và hết hạn.
- Bộ tải SPR native đọc năm sách, 17 frame, không lỗi. Hồi quy Lua 4 năm callback daily và cache/nhấp nháy qua.
- Combat smoke 900 trận baseline qua; các lần tắt AOE/chặn đường bị phát hiện đúng. Mô hình AI và chỉ số của smoke vẫn là fixture, không phải tỷ lệ thắng PvP đo trên server thật.

Chưa nghiệm thu hình ảnh bằng client đăng nhập sau khi triển khai. Không dùng các kiểm tra headless để khẳng định đã quan sát HUD hay mọi hiệu ứng skill trong trận thật.

## Áp dụng

Publisher đối chiếu hash gói/đích, yêu cầu Game và Studio đóng, snapshot world mới và không có nhân vật online. Sao lưu file, dừng world bằng controller, đối chiếu toàn bộ `role_*.pthc`, thay đúng file trong manifest rồi khởi động lại. Receipt ghi hash, PID, NPC và kết quả dữ liệu nhân vật trước/sau. Không tự mở lại client trong khi cập nhật.

## K?t qu? tri?n khai

?? ?p d?ng ng?y 27/10/2026 l?c 20:26 (Asia/Saigon). Receipt x?c nh?n 49 file, world PID 109804, 4054 NPC v? d? li?u nh?n v?t kh?ng ??i. Hash runtime ??i chi?u l?i ?? 49/49 file. B? ki?m tra native 1.296 tr??ng h?p c?ng ch?y l?i tr?c ti?p tr?n Client runtime qua loose loader sau tri?n khai v? qua. Game ch?a ???c m? l?i ?? nghi?m thu h?nh ?nh trong phi?n ??ng nh?p th?t.

# Đồ Cam, công lực và Tinh Quân — 27/10/2026

Đã áp dụng gói OrangeHandbook20261027 vào Loose. World PID 125164 sau cập nhật; 4.044 NPC/quái, hồ sơ nhân vật và PID dịch vụ phụ không đổi. Chưa kiểm tra trực quan trong Game.

## Lỗi nhận đồ Cam

Danh mục cũ dò cấp theo dấu ngoặc tròn của nâng sao, trong khi công thức Cam dùng `[cấp mẫu]`. Vì vậy sinh 1.350 lựa chọn nhưng 1.215 lựa chọn không khớp công thức. Native từ chối đúng, nhưng Lua lại báo chung lỗi F4.

Giữ ID cũ để không đổi danh tính vật phẩm, ẩn 1.215 lựa chọn sai khỏi menu; còn 135 lựa chọn có mẫu đúng. Không sửa bảng gốc để ép cấp được món sai. Native trả mã riêng cho điều kiện sử dụng, mẫu chưa nạp, lỗi sinh vật phẩm, công thức không khớp, thiếu ô đặt vật phẩm và lỗi giao nhận. Thông báo F4 chỉ dùng khi kiểm tra kích thước ô thất bại.

## Thuộc tính và công lực

Bảng thuộc tính Cam và bảng mở rộng khớp từng byte với PAK gốc đang có hiệu lực. 135 công thức đang cấp cũng khớp từng byte khi đối chiếu theo ID. Một số tiền tố thấp như Linh Khí chỉ định ba dòng; giữ số dòng, xác suất và khoảng trị số được khai báo, không tự thêm dòng.

Công lực cũ so dòng Cam với giới hạn của mẫu trắng nên bỏ sót điểm. PhongThanForgePower.inl lấy giới hạn từ nhóm thuộc tính của công thức thực tế. Giữ điểm cơ bản, điểm nâng cấp và khảm, tránh cộng trùng. Chưa suy diễn thêm điểm cho thuộc tính mở rộng hoặc ý nghĩa OrgEffect khi chưa có bằng chứng đầy đủ.

## Tinh Quân

Thêm Trang Bị → Trang Bị Tinh Quân (10.200 mẫu) và Vũ Khí → Tinh Quân (1.600 mẫu). Cả 11.800 dòng khớp dữ liệu gốc. Đai/giày có một số dòng không khai báo phái: chỉ phân nhóm menu theo họ mẫu/cấp của áo tương ứng, không thêm yêu cầu phái vào vật phẩm. Thú cưỡi Tinh Quân đã có trong nhóm thú cưỡi, không nhân đôi.

## Kiểm chứng

- Lua 4 thực: 3.966 menu/callback, giới hạn nút và xem trước icon qua.
- Native công lực: 544 trường hợp trên 135 công thức, tỷ lệ thấp/tối đa, restore, vật phẩm đã khảm và loại thuộc tính sai qua.
- CoreServer/CoreClient Release build qua; gói 5 file đúng hash, có bản sao lưu.
- Bằng chứng: analysis/orange_handbook_20261027, đặc biệt eligible_original_proof.json, native-test.txt và publication.json.

Các bảng công thức/vũ khí tổng thể của Loose có khác PAK gốc ở những nhánh khác; đối chiếu ở đây kiểm tra riêng các công thức Cam và mẫu Tinh Quân thực sự được cấp, không khẳng định toàn bộ bảng đã được phục hồi.

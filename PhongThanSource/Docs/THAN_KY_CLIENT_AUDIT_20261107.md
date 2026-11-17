# Đối chiếu Thần Kỹ trong client gốc — 07/11/2026

Đã đối chiếu 15 nhóm tên trong [bài VNG](https://pt.vnggames.com/su-kien/than-ky/noi-dung-1443.html) với bảng Skills.txt và vật phẩm lấy trực tiếp theo thứ tự PAK của Client_Original. Cả 15 nhóm đều có bản ghi kỹ năng tương ứng và icon trong bộ PAK hiện có. Không dùng F5 làm danh mục đầy đủ kỹ năng.

| Nhóm | ID tìm được trong bảng gốc | Script cấp skill trong PAK hiện có |
|---|---|---|
| Huyền Ảnh Tàn Hoa | 1996–1999 | Chưa tìm thấy |
| Huyết Nguyệt Trảm | 1986–1989 | Chưa tìm thấy |
| Phật Nộ Hỏa Liên | 1990, 2000 | Chưa tìm thấy |
| Lôi Hỏa Tàn Phong | 1991–1992, bảng ghi **Toàn Phong** | Chưa tìm thấy |
| Truy Hồn Tán Hoa | 1993 | Chưa tìm thấy |
| Vạn Kiếm Quy Tông | 1994–1995 | Chưa tìm thấy |
| Hình Thiên Chiến Ý | 1510–1511, 1556 | Có cho 1556; hai biến thể cũ chưa tìm thấy script |
| Triệu Hồi | 1551 | Có |
| Khô Mộc Phùng Xuân | 1508–1509, 1547 | Có cho 1547; biến thể cũ chưa tìm thấy script |
| Đoản Binh Giao Tiếp | 1557 | Có |
| Họa Địa Vi Lao | 1504–1505, 1516–1517, 1560 | Có cho 1560; biến thể cũ chưa tìm thấy script |
| Xung Phong Hãm Trận | 1496, 1498–1499, 1518, 1552 | Có cho 1552; biến thể cũ chưa tìm thấy script |
| Nam Minh Ly Hỏa | 1513–1514, 1548–1550 | Có cho 1548–1550; biến thể cũ chưa tìm thấy script |
| Thiên Toàn Địa Chuyển | 1554–1555 | Có; 1951–1952 là nhánh “Lò Luyện”, không tự coi là skill nhân vật |
| Băng Hồn Tuyết Phách | 1553 | Có |

Các ID gồm bản chính, nhánh phụ và biến thể trùng tên; không phải mỗi ID là một kỹ năng riêng cần học. Tên trùng chưa chứng minh cùng cấp/vật phẩm sử dụng. Riêng “Triệu Hồi” là từ chung, các vật phẩm khớp tên còn phải lọc theo script và thuộc tính; số lượng tên khớp không được coi là số sách Thần Kỹ.

Đã giải nén nguyên byte 9 file công thức có sẵn để đọc và lưu hash, chỉ trong analysis. Sáu nhóm mới ID1986–2000 có bản ghi và hình ảnh nhưng chưa tìm thấy các script công thức được tham chiếu trong PAK hiện có. Chưa kiểm tra đủ toàn bộ đồ thị hiệu ứng/skill phụ, đường dùng sách, điều kiện học và giao thức đồng bộ để tuyên bố các nhóm đã dùng được trong Loose.

Tình trạng này chứng minh cần khôi phục theo từng nhóm, không được nói chung rằng client thiếu Thần Kỹ. Có bản ghi/icon không tự chứng minh đã có toàn bộ server logic; có script cấp skill cũng chưa chứng minh NPC đổi sách và đường sử dụng đã nối xong.

Bằng chứng: `analysis/boss_skill_ui_20261107/than-ky-original-audit.json`, `than-ky-script-proofs.json`, bản sao dữ liệu `original-than-ky-scripts/`, script kiểm toán `audit_than_ky.py`. Bảng/nguồn PAK và SHA256 có trong JSON. Ba PAK được khai báo nhưng không có ở Client_Original là giới hạn của lần kiểm tra, không kết luận chúng không tồn tại trên hệ thống VNG.

Lần này chỉ kiểm toán, chưa sửa runtime, chưa tự thêm kỹ năng vào F5, chưa đặt công thức mới và chưa build Release.

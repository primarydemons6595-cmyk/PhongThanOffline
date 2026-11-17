# Tiên/Ma và hiệu ứng kỹ năng — 06/11/2026

Đã đối chiếu dữ liệu Loose với các PAK gốc hiện có, sửa lỗi nạp dữ liệu và kiểm thử bằng mã native. Chưa kiểm tra trực tiếp toàn bộ động tác đánh trong game; không thể kết luận đã đầy đủ 100%.

## Hiệu ứng khi đánh, gồm Vũ Sĩ

- Client chỉ có 325 dòng đạn/hiệu ứng, trong khi bảng server và bản gốc có 588 ID. Bổ sung 264 dòng thiếu từ bản gốc, giữ các dòng cũ và bản sửa thời gian sống của đạn 133.
- Giới hạn ID cũ 550 bỏ qua các ID cao, gồm hiệu ứng đánh thường Vũ Sĩ 584. Giới hạn mới bao phủ ID 1–588 và giữ kiểm tra biên.
- Sửa đúng ba cột hình/khung/âm thanh bị lệch ở ID 538, 539, 540, 543, 549, 558, 559, 560, 566, 571. Không đổi sát thương, tỷ lệ hoặc công thức kỹ năng.
- Khôi phục `earthbomb-01.spr` bằng giải nén frame từ PAK gốc. Loader thật đã đọc được đủ 13 frame; không thay hình bằng tài nguyên tự vẽ.
- Kiểm tra tĩnh 112 kỹ năng gốc/sách và các nhánh tham chiếu, tổng cộng 260 tài nguyên. Các SPR tìm được đã qua bộ nạp native: 140 file, 1.145 frame.

Vẫn thiếu 38 tham chiếu tài nguyên trong bộ dữ liệu hiện có, gồm icon, script công thức và hiệu ứng. Danh sách: `analysis/movement_khulai_20261106/assets-unavailable.json`. Riêng hiệu ứng nổ Vũ Sĩ `\spr\skill\bomb\jv_sha_chu_fa.spr` chưa tìm được trong các PAK hiện có. Kiểm tra đồ thị tĩnh không bao phủ mọi thay đổi ID động qua Lua hoặc đường triệu hồi đặc biệt của Dị Nhân.

## Giao diện và Cẩm Nang Sự Kiện

Avatar/F3/F4/F5 chỉ hiện phần Tiên/Ma khi trạng thái thăng cấp hợp lệ: giai đoạn 13, hệ Tiên hoặc Ma, cấp 1–90. Trước đó ẩn hoàn toàn, gồm nút F4. F5 khôi phục đúng năm ô kỹ năng từng phái từ dữ liệu gốc; Đạo Sĩ có trang thứ năm. Trang F3 chỉ đọc số liệu thật và ghi rõ bộ trang bị đang hoạt động; không tự đổi trang bị khi xem.

Cẩm Nang Sự Kiện (item 5572) có chọn Tiên/Ma, cấp 1–90, danh vọng Tiên và Ma riêng, học tối đa nhóm năm kỹ năng Tiên/Ma. Server kiểm tra sách đang mang, trạng thái nhân vật, phạm vi giá trị và chuyển bộ trang bị an toàn trước khi đổi hệ/hạ cấp. Không mở cổng thế giới chung.

Script công thức gốc của Huyết Chiến Sa Trường (1481, Giáp Sĩ) và Độc Hành Thiên Hạ (1487, Dị Nhân) đang thiếu. Nút học tối đa kiểm tra trước và báo thiếu, giữ nguyên toàn bộ danh sách kỹ năng nếu thiếu script; không báo thành công giả. Nhóm Đạo Sĩ/Vũ Sĩ có đủ file script tham chiếu, nhưng chưa xác nhận mọi công dụng bằng một phiên chơi thực tế.

Chưa khôi phục đầy đủ cơ chế nhận kinh nghiệm Tiên/Ma tự nhiên hoặc thêm công thức chiến đấu mới. Giao diện và quản trị đọc/ghi trạng thái hiện có; không suy đoán công thức bị thiếu.

## Kiểm chứng và cập nhật

Chín nhóm kiểm thử trong `analysis/movement_khulai_20261106/checks.json` đều qua: đồng bộ bước đi, điều kiện hiện UI, thao tác Cẩm Nang, bảng hiệu ứng, bố cục kỹ năng, menu Lua, Khứ Lai Phù và nạp SPR. Dev CoreServer/CoreClient, Game.exe và toàn bộ Secure Release đã build qua. Các kiểm thử này không thay thế kiểm tra giao diện/cast trực tiếp trong game.

Biên nhận cài Loose và bản sao lưu: `analysis/movement_khulai_20261106/runtime-deployment.json`. Biên nhận khôi phục Khứ Lai Phù cho GameMa: `role-recovery.json` cùng thư mục. Chỉ khi trạng thái biên nhận là `APPLIED_*` mới coi đã cài/đã cấp vật phẩm. Patch có kiểm tra hash/chữ ký riêng; xem thư mục `PhongThanRelease/Update` và báo cáo xác minh.

Bản Loose đã cài và xác minh SHA; GameMa đã nhận đúng một Khứ Lai Phù trong Túi 3, giữ nguyên 135 vật phẩm cũ và tiến độ nhiệm vụ. Patch Client 1.03 (3 tài nguyên) và Server 1.03 (6 tài nguyên) đã tạo xong, hash và chữ ký ECDSA-SHA256 đều hợp lệ; biên nhận `analysis/movement_khulai_20261106/patch-verification.json`. Bản Release cài riêng tại `Update/PhongThanQH` chưa tự động áp patch. Kiểm tra sau cài bằng bộ nạp native trên Loose: F5 cả bốn phái qua, 140 SPR/1.145 frame đọc thành công; chưa thay thế kiểm tra cast trong game.

Làm rõ phạm vi “38 chưa tìm thấy”: đây là tham chiếu của bộ bảng Loose được kiểm tra tĩnh, không phải 38 lần lỗi nạp quan sát trên client gốc. Chưa chứng minh client gốc gọi tất cả các nhánh này. KMissleRes::Draw trong source còn chọn bộ hiệu ứng khác khi LowMissle bật; script cấp kỹ năng cũng có thể đổi tham chiếu động. Do đó không dùng số38 để kết luận client gốc thiếu38 hiệu ứng hoặc phải thay38 tài nguyên. Cần xác minh bảng thắng theo thứ tự PAK, công thức/nhánh thực thi và đường nạp thực tế của đúng skill. Bộ thực thi client gốc không mặc nhiên đồng nhất với source Loose. Lỗi không nạp được một hình cũng không tự chứng minh client sẽ crash; KSprite::Load trả FALSE khi mở tệp thất bại trong source hiện có. Đây là cơ chế source, không phải bản trace runtime của binary client gốc.

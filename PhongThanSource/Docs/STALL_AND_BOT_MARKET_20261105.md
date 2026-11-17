# Bày bán, môn bài và chợ bot — 05/11/2026

## Cách dùng

Trong F4, nút quản lý/định giá mở bảng sạp cả khi nhân vật đang bán. Bảng có nút **Bày bán / Dừng bày bán** theo trạng thái server. Đóng bảng hoặc nhấn Esc chỉ ẩn bảng; sạp vẫn hoạt động và mở lại được. Nút bày bán trên F4 cũng dừng được sạp đang mở. Dừng sạp cho nhân vật đứng dậy; mở lại không cần thoát game.

Yêu cầu mở và dừng được xử lý riêng: gửi lặp yêu cầu mở không đóng sạp; gửi lặp yêu cầu dừng không mở sạp trở lại. Dừng sạp không bị điều kiện khóa nhân vật, đồ đã bán hết, thiếu môn bài hay trạng thái bận của lần mở chặn lại. Các kiểm tra khóa của rương và giao dịch thường vẫn giữ nguyên.

## Quyền bày bán mặc định

- Sùng Thành doanh (1002), Ngọc Hư Cung (1003), Xi Vưu Mộ (1004): miễn môn bài.
- Phong Thần đài (1001), Tây Kỳ (1020), Triều Ca (1021), Diêu Trì (1052): cần môn bài đúng thành còn hạn.
- Thổ / Thủy / Hỏa / Phong trận (1067–1070): chỉ mở bằng môn bài đúng trận, theo mô tả gốc.
- Các map còn lại: cấm mặc định; quản trị có thể đổi trong Studio.

Server kiểm tra khi mở, trong lúc duy trì sạp và trước khi thanh toán. Hết quyền, chuyển sang map không hợp lệ hoặc cấu hình chuyển sang cấm thì sạp đóng. Nhân vật phải sống, xuống ngựa và ở trạng thái hòa bình. Môn bài thành không cấp quyền bày bán ngoài bãi.

## Đối chiếu F2

Đọc trực tiếp `Server/settings/ibshop/ibshopgoods.txt` và `Server/settings/item/001/ibitem.txt`, giải mã TCVN3. F2 có **13 loại môn bài bày bán**:

| Môn bài | Detail vật phẩm IB | Thời gian online |
|---|---:|---:|
| Triều Ca | 38 | 48 giờ |
| Tây Kỳ | 39 | 48 giờ |
| Diêu Trì | 40 | 48 giờ |
| Phong Thần đài | 41 | 48 giờ |
| Triều Ca | 125 | 144 giờ |
| Tây Kỳ | 126 | 144 giờ |
| Diêu Trì | 127 | 144 giờ |
| Phong Thần đài | 128 | 144 giờ |
| Thổ trận | 129 | 144 giờ |
| Thủy trận | 130 | 144 giờ |
| Hỏa trận | 131 | 144 giờ |
| Phong trận | 132 | 144 giờ |
| Môn bài thành thị (nhỏ) | 186 | 48 giờ |

Bảng vật phẩm còn có bốn bản môn bài trận 48 giờ (42–45), không có trong danh mục bán F2 hiện tại; cũng hỗ trợ đúng phạm vi. Môn bài thành thị dùng cho bốn thành và ba thôn, không dùng thay môn bài trận.

Sử dụng thành công tiêu hao một vật phẩm, tạo icon buff đúng tên/hình/thời gian. Rời mạng không trừ thời gian theo mô tả gốc. Thời gian đã dùng khi online vẫn được trừ. Không tiêu hao khi buff cùng loại còn hạn, hết ô buff, dữ liệu nhiệm vụ đã đầy, vật phẩm khóa/hết hạn hoặc thao tác không hợp lệ. Các buff khác vẫn giữ cách hết hạn của chúng.

## Studio

Vào **CHỈNH SỬA SERVER → Bày bán & lệnh bài**.

Chọn một hoặc nhiều map và đặt **Mở tự do**, **Khóa: cần lệnh bài** hoặc **Cấm bày bán**. Mở tự do bỏ yêu cầu môn bài; cấm thì môn bài cũng không mở được. Chọn **Lưu vào bản sửa**, sau đó áp dụng bằng **Bản sửa / Patch**. Server đọc lại quyền trong khoảng 5 giây. Nếu file mới không hợp lệ, giữ cấu hình hợp lệ trước đó.

Tab lệnh bài cho sửa/tạo tên, icon SPR có xem trước, mô tả, thời hạn 1–2.592.000 giây online, map được cấp quyền và bật/tắt sử dụng. Hỗ trợ 32 lệnh bài riêng bên cạnh bộ gốc. Lệnh bài riêng có vật phẩm IB riêng, không đổi tên một vật phẩm nền đang dùng cho chức năng khác. Sau khi áp dụng, mở lại client để nạp bảng vật phẩm và icon; có thể thêm vật phẩm mới vào F2 hoặc phần thưởng nhiệm vụ bằng danh mục Studio. ID riêng bắt đầu từ `8/62000/0`, hiển thị ở danh sách lệnh bài.

Lưu bản nháp tự động; tạo bản sửa kiểm tra phiên dữ liệu để tránh ghi đè bản đã đổi bên ngoài. Cấu hình và bảng vật phẩm được sinh cho cả Client và Server. Không tự đưa lệnh bài mới lên F2 và không tự cấp đồ cho nhân vật.

## Tích lũy cửa hàng

Mỗi giao dịch bày bán đã chuyển đồ và thanh toán thành công cộng một lượt bán và doanh thu cho người bán. Ngồi không, giao dịch thất bại hoặc gửi lặp không cộng. Cấp cửa hàng và tiến độ hiển thị trong bảng sạp, tồn tại sau khi đăng nhập lại.

Mô hình hiện tại có 10 cấp, các mốc tổng lượt bán để lên cấp lần lượt là 10 / 40 / 90 / 160 / 250 / 360 / 490 / 640 / 810. Đây là quy tắc triển khai cho lần sửa này; chưa xác nhận được bảng mốc nâng cấp gốc. Giữ số ô bán hiện có.

## Bot

- Khi tự động được bật, có ít nhất ba món trang bị dư đủ giá trị và không cần dùng, bot chọn một trong ba thôn còn được Studio mở tự do. Giữ đồ đang dùng, đồ nâng cấp có ích, đồ nhiệm vụ, đồ khóa và đồ đang hẹn giao dịch.
- Định giá bằng chính sách giá hiện có, tối đa 12 món mỗi phiên; không tạo đồ hoặc bạc mới. Mỗi phiên ngồi 1–2 giờ, kết thúc sớm nếu bán hết, cần hồi phục, nhận việc từ người chơi, có sự kiện phù hợp hoặc mất quyền bày bán. Sau phiên có khoảng nghỉ bày bán bốn giờ để tiếp tục chu kỳ khác.
- Tìm sạp thật đang mở, chọn đồ phù hợp với nghề/cấp/khả năng trang bị, sách học được hoặc thuốc đang thiếu. Ưu tiên thuốc và kỹ năng trước nâng trang bị, tính cả chi phí và đường đi. Ngân sách tối đa một phần ba bạc mang theo và không quá 5 triệu bạc; bỏ giá vượt đánh giá hợp lý.
- Trước mua kiểm tra lại danh tính người bán, vật phẩm, giá, quyền bán và ngân sách. Dùng chung thanh toán với người chơi; sau mua dùng luồng trang bị/học sách có sẵn.
- Đi qua điểm nối map thật; khi xa từ hai chặng hoặc đường bộ bị chặn thì thử phù/La Bàn đang sở hữu bằng hội thoại thật. Không dịch chuyển tự do, không cấp phù, không sửa tọa độ trực tiếp. Có giới hạn thời gian đi chợ để không kẹt vòng lặp.

Hàng rao bán được chọn từ các túi mang theo. Lần sửa này không thêm việc tự mở/rút rương có mật khẩu.

## Kiểm chứng và bản nạp

CoreServer, CoreClient và GameClient build không lỗi; Studio build không lỗi. Kiểm tra native dùng trực tiếp mã sản phẩm cho điều kiện map/môn bài, tiêu hao và hoàn tác, timer lưu/nạp, giao dịch và nâng cấp. Có 100 vòng mở → ẩn bảng → mở lại → dừng → mở lại, bao gồm gửi lặp và đóng khi khóa/bận. Kiểm tra controller bot cho thời hạn, ưu tiên và dịch chuyển; 107 kiểm tra dùng phù/La Bàn hiện có cũng qua.

Kiểm tra hồi quy khóa rương, thuê kho, khởi tạo bot, dọn đồ, sửa trang bị và mã truy vấn buff/khóa rương đã qua. Đối chiếu bảng vật phẩm xác nhận giữ nguyên các dòng ngoài nhóm lệnh bài; lệnh bài mới giữ template vật thể rơi có thật.

Chưa mở game bằng nhân vật thật để xác nhận giao diện nút mới và một phiên mua bán trọn vẹn; chưa quan sát một bot chạy đủ 1–2 giờ thực tế. Kết quả trên là kiểm tra native/WPF và build, không thay thế lượt thử trực tiếp đó. Không tiêu Xu, cấp đồ, sửa dữ liệu nhân vật hay tự khởi động world để kiểm tra.

Đã nạp Game.exe, CoreClient.dll, CoreServer.dll, hai file cấu hình mặc định và hai DLL Studio; đã đối chiếu SHA-256 với gói đã kiểm tra và giữ bản sao lưu trước nạp. Cửa sổ Studio hiện tại vẫn được giữ nguyên, không ép thoát hoặc bỏ nội dung đang soạn. Mở Studio lại để nạp giao diện mới và thấy tab bày bán.

Biên nhận và log: `analysis/stall_20261105/` ở thư mục gốc dự án. Đặc biệt: `native-deployment.json`, `studio-deployment.json` khi cập nhật Studio xong, `deployment-package.json`, `runtime-before/`, `native-tests.log`, `studio-checks.log`, `bank-bot-regression.log` và `checks/stall-editor.png`.

Ghi chú kỹ thuật: dùng task 5351–5353, ngoài vùng nhiệm vụ Studio, thuê rương, tín dụng nghề và đảo; mã truy vấn giao diện riêng `PT_GDI_STALL_STATUS`. Không đổi cấu trúc role hoặc kích thước gói tin.

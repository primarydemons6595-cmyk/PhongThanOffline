# Trấn Nguyên Đại Tiên — Diêu Trì, 25/10/2026

Đã phục dựng riêng NPC template 202 tại map 1052, tọa độ hiển thị (182,198). Script trong client gốc và bản đang chạy chỉ có lời chào; chức năng dưới đây được viết bổ sung dựa trên hướng dẫn chính thức và bảng dữ liệu của bộ hiện tại.

## Chức năng

- Thí luyện từ cấp 42: ba quỷ nhiệm vụ, ba loại quái tự nhiên, theo dõi F11; chỉ trả khi đủ sáu điều kiện. Thưởng 120.000 kinh nghiệm, nguyên liệu theo dữ liệu hiện có và quyền chọn hướng học chế tạo.
- Danh vọng riêng năm loại trang bị: quyên góp đúng vật phẩm đã chọn, tối đa năm lần/ngày, hỗ trợ vật phẩm tăng điểm; học công thức theo cấp danh vọng.
- Chế tạo trang bị cam theo 135 công thức cổ điển type 2/group 2–6: kiểm tra trang bị trắng, nguyên liệu, phí và đồ phổ; tạo tên, phẩm chất và thuộc tính thật, khôi phục nhất quán khi tải lại vật phẩm. Có điều kiện bộ năm món cho nhánh cấp 3.
- Linh Nguyên (vật phẩm 369) dùng cho danh vọng, hoàn toàn tách khỏi Linh Nguyện nuôi thú (418). Giữ nguyên quy tắc Linh Nguyện tối đa 30 và khóa sau nhận nuôi.
- Menu chính dùng nút; chọn vật phẩm/công thức dùng danh sách nguyên bản. Chỉ sửa hội thoại NPC này; dùng nút Kết thúc sẵn có. Thông tin dài chia trang, có bước xác nhận giao dịch.

Không đưa quái vùng khác về thành. Ba boss thí luyện sử dụng tọa độ đã kiểm tra ô đi được; các loại quái thường dùng quần thể đang có trên bản đồ.

## Phạm vi dữ liệu

Đây là bản phục dựng theo dữ liệu cổ điển đang cài, chưa phải xác nhận tương đương toàn bộ server VNG. Chưa bật 150 công thức chuyển hóa type 11. Chỉ ánh xạ 56 đồ phổ có định danh chắc chắn; công thức thiếu đồ phổ tương ứng không được thay bằng vật phẩm phỏng đoán. Đã sửa bốn dòng đồ phổ bị dính cột trong cả bảng client và server.

## Kiểm tra và triển khai

- Build Release: CoreServer, CoreClient, GameClient thành công.
- 126 kiểm tra native/Lua4: tiến trình thí luyện, giao dịch, chống trả thưởng lặp, giới hạn ngày, định danh vật phẩm, học công thức, chế tạo, điều kiện bộ cam, phân biệt Linh Nguyên/Linh Nguyện và phân loại nút/danh sách.
- 1.358 kiểm tra trên 135 công thức: tên, thuộc tính, seed và khôi phục vật phẩm; không dùng trường sao để lưu công thức.
- Thử publisher nội dung: client 3 file, server 3 file; hash đọc lại khớp.
- Đã thay chín file Loose và khởi động world PID 60436. Biên bản: `analysis/tran_nguyen_20261025/publication.json`; kết quả world/hash cuối: `live_verification.json` cùng thư mục.
- World đã kết nối Database, Relay, Gateway; stderr trống. Xác minh 334 file theo chuỗi biên nhận cập nhật, 3.999 NPC/quái, đủ sáu NPC hàng ngày mới, bảy NPC Sùng Thành, 99 vị trí nhiệm vụ cũ và vị trí Trấn Nguyên. Một khác biệt file vùng Diêu Trì từ biên nhận cũ được giữ nguyên, không ghi đè; các vị trí cần kiểm tra vẫn đủ.
- Dữ liệu nhân vật trên đĩa được sao lưu và giữ nguyên trong lúc thay file. Phiên world cũ trả exit code 2 do khóa lưu QuangHuy3 từ phiên web; không ghi thành “thoát sạch”. Người dùng xác nhận đã thoát phiên web. Biên bản giữ nguyên cảnh báo không xác nhận được lần lưu cuối của phiên này; publisher không phục hồi hay sửa dữ liệu nhân vật.
- Chưa kiểm tra trực tiếp hình ảnh hoặc đánh boss bằng game client; các kiểm tra script không thay cho bước đó.

Gói tái triển khai: `Deploy/ProjectContent/TranNguyen20261025`. Publisher nội dung được gọi sau gói nhiệm vụ hàng ngày để lần publish sau giữ bản sửa này.

## Nguồn đối chiếu

- https://pt.vnggames.com/su-kien/he-thong-tran-nguyen/thi-luyen-tran-nguyen.html
- https://pt.vnggames.com/su-kien/he-thong-tran-nguyen/diem-danh-vong-tran-nguyen-925.html
- https://pt.vnggames.com/tin-tuc/can-biet/huong-dan-cach-thuc-che-tao-trang-bi-cam.html
- https://pt.vnggames.com/tin-tuc/can-biet/huong-dan-ve-do-pho-trang-bi-cam-so-cap.html
- https://pt.vnggames.com/tin-tuc/can-biet/gioi-thieu-do-pho-trang-bi-cam-cao-cap.html
- https://pt.vnggames.com/tin-tuc/can-biet/gioi-thieu-ve-cac-trang-bi-cam-cap-3.html

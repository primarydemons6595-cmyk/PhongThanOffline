# Phù F2 và nhãn giá — 25/10/2026

Đã áp dụng CoreServer/CoreClient mới vào Loose, world **89440**. World trước 99964 lưu/dừng exit 0; file nhân vật giữ nguyên trong lúc thay bản. Sau khởi động đạt 350 hash và các kiểm tra thế giới của đợt trước. Chưa nghiệm thu trực tiếp trên giao diện game.

## Kết quả đối chiếu F2

Đọc `settings/ibshop/charmgoods.txt`, nối số dòng gốc một-based với `ibshopgoods.txt`, rồi đối chiếu tuple với bảng `item/001/ibitem.txt` đang chạy:

| Dòng goods | Vật phẩm | Tuple | Lượt của một món |
|---:|---|---|---:|
| 100 | Địa Linh | 8,119,2 | 10 |
| 101 | Liệu Nguyên | 8,120,2 | 10 |
| 102 | Truy Phong | 8,121,2 | 10 |
| 103 | Hải Hồn | 8,122,2 | 10 |
| 104 | Tật Điện | 8,123,2 | 10 |

Các mục biến thân/vật phẩm khác cũng nằm trong tab Phù, không vì cùng tab mà bị đổi thành phù dịch chuyển.

Đã xác nhận luồng dựng mẫu hàng F2 lấy `StackCount=1` của shop ghi đè số lượt 10 trong bảng IB. Đọc bản lưu hiện có cũng thấy các phù 119/120 còn count 1 và phù 121 count 2. Lỗi này không thể được phát hiện chỉ bằng kiểm thử gọi trực tiếp API dịch chuyển với một item giả lập đủ 10 lượt.

## Sửa đổi

- Dùng whitelist tuple chung `PhongThanTravelItemPolicy.h` giữa xử lý native, F2 và vật phẩm client. Các phù nhiều lượt là một vật phẩm chứa số lượt; không gộp thành chồng vật phẩm.
- Mẫu mua F2 giữ đủ định mức gốc, mua ba phù tạo ba món độc lập đủ lượt; giữ nguyên cách mua vật phẩm thông thường.
- Đưa bước nhận diện phù đã xác minh lên đầu đường dùng item, trước nhánh IB thuộc tính chung. Kiểm tra khóa di chuyển tương thích quy tắc cũ: khóa hoàn toàn chặn dùng; khóa chọn vật phẩm có vị trí dương không tự chặn mọi phù.
- Tooltip đọc số lượt hiện tại và hiển thị `Số lần sử dụng còn lại: N`; cập nhật theo snapshot item sau khi trừ lượt. Nhãn hỏng `Gi?c?` được thay bằng byte TCVN3 đúng của `Giá cả:`.
- Không tự nâng lại lượt của item đang có: count hiện tại có thể là phần đã dùng, và một số món từng được mua/gộp bằng luồng cũ. Phù mua mới giữ đúng định mức; món cũ hiển thị đúng count còn lưu, tránh tự ý xóa lịch sử sử dụng.
- `travel_item_diag.log` ghi kiểm tra quyền sở hữu/khóa khi mở phù để xác định nguyên nhân từ chối nếu còn một trạng thái đặc biệt trong game; không có thông tin mật tài khoản.

## Kiểm thử

Build Release CoreServer/CoreClient thành công. 348 kiểm tra native dịch chuyển vẫn đạt. Bộ mua hàng sử dụng giao dịch F2/QuestExchange thật kiểm tra 45 loại (25 loại bình và 20 loại phù), ba món đủ lượt, chỗ trống, thiếu Xu, lỗi tạo vật phẩm và hoàn trả; vật phẩm thường vẫn gộp đúng số lượng. Đây là kiểm thử code với biên engine cô lập, không thay thế một lượt chơi kiểm chứng trên client.

Gói và bằng chứng: `analysis/travel_f2_hotfix_20261025/{manifest.json,publication.json,clean_stop.json,live_verification.json}`; gói binary cố định `Deploy/ProjectContent/TravelF2Hotfix20261025`. Source đã sửa được dùng cho các lần build sau. Client_Original không thay đổi.

## Bổ sung: lỗi bấm phải không có menu

Sau phản hồi tiếp theo, log `travel_item_diag.log` xác nhận các lần bấm đã có `owned=1`, `allowed=1`. Các kiểm tra quyền sở hữu và khóa không phải nguyên nhân khiến menu trống trong những lần đó.

Đã tái hiện bằng chính Lua4/Engine của game: script production cũ chạy `main()` thành công nhưng không gọi `Say`, danh sách lựa chọn có độ dài 0. Biểu thức trộn nhiều `or` và `and` để lọc nhóm địa điểm không cho kết quả như dự kiến trong bộ Lua này. Đổi thành các nhánh `if` riêng cho nhóm cơ bản, nhóm nguyên tố và nhóm bí cảnh, giữ nguyên ID, đích đến và cơ chế trừ lượt.

Bộ kiểm thử mới chạy **script trước sửa** và yêu cầu tái hiện lỗi `NO MENU profile=1`; sau đó chạy script đã sửa và kiểm tra đúng số lựa chọn của cả chín nhóm có menu: 25, sáu nhóm 5, 20 và 10. Đây là kiểm thử đi qua hàm Lua `main()` và điểm gọi `Say`, bổ sung cho bộ kiểm thử native trước vốn chỉ gọi API dịch chuyển trực tiếp nên đã bỏ sót lỗi tạo danh sách.

Chỉ thay một script server, không thay client: world cũ 89440 lưu/dừng exit 0; world mới **99792**. Gói `TravelMenu20261025`, biên bản và bộ tái hiện lỗi tại `analysis/travel_menu_20261025`. Bản overlay chuẩn và nguồn tạo script đã cập nhật để không đưa lại biểu thức cũ ở lần publish sau. Người chơi cần đăng nhập lại sau khi server nạp script mới; không cần đóng cửa sổ game để thay binary trong lần này.

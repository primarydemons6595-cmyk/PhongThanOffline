# Thần Kỹ: mất sách sau kích hoạt — 09/11/2026

Đã cài riêng `PhongThanRuntime-Loose/Server/CoreServer.dll` lúc 14:24. Sao lưu: `analysis/than_ky_book_use_20261109/backup-20261109-142452`. GameServer cũ đã lưu và thoát sạch theo tín hiệu dừng chuẩn; không ép tắt. Không build hoặc cập nhật runtime Release, không thay client hoặc công thức kỹ năng.

Sau khởi động, GameServer PID 33712 kết nối Database/Relay/Gateway; Event PID 11360 nhận trạng thái đúng PID mới, lease hợp lệ. Mẫu sau nạp có 200 bot trực tuyến. Bằng chứng: `analysis/than_ky_book_use_20261109/live-start-verification.json`. Đây là kiểm tra khởi động và đồng bộ dịch vụ, chưa phải nghiệm thu thao tác F4 trong phiên game thật.

## Nguyên nhân đã tái hiện

`KItemList::NowEatItem` gọi Lua của sách. Lua kích hoạt quyền sử dụng Thần Kỹ và chỉ trừ một Linh Thạch. Tuy nhiên, sau khi Lua trả về, nhánh chung của vật phẩm `item_magicscript` tiếp tục `RemoveItem(nIdx, 1)`, làm mất chính quyển sách. Nhịp kiểm tra quyền sử dụng sau đó phát hiện sách không còn trong hành trang và thu hồi chiêu. Vì vậy thông báo kích hoạt xuất hiện nhưng danh sách chọn chiêu lại mất chiêu.

Kiểm thử cũ gọi trực tiếp hàm kích hoạt và đã bỏ sót bước tự trừ của hàm gọi bên ngoài. Bộ hồi quy mới đi qua gói yêu cầu dùng vật phẩm, hàm nhận gói, danh sách hành trang, Lua thật và gói đồng bộ chiêu thật.

## Phạm vi sửa

Nhận dạng đúng sách kích hoạt trong `ThanKyBooks.ini`, theo genre/detail/particular. Giữ sách ngoài nhánh tự tiêu hao chung, kể cả khi Lua từ chối kích hoạt do sai cấp, sai phái hoặc thiếu nguyên liệu. Không miễn tiêu hao cho toàn bộ vật phẩm có script. Script của các vật phẩm khác tiếp tục xử lý như trước.

Giữ serial, số lượng và vị trí sách. Kích hoạt lần đầu tốn một Linh Thạch; dùng lại chính quyển đã kích hoạt không trừ thêm. Một nhóm chỉ có một ID chiêu và thay cấp tại chỗ. Xóa hoặc chuyển sách khỏi túi mang theo vẫn thu hồi quyền theo thiết kế đã thống nhất.

## Kiểm chứng

- Bản dispatcher cũ tái hiện mất sách và thu hồi chiêu sau nhịp kiểm tra.
- Cả 51 sách cấu hình chạy qua đường dùng vật phẩm thật: giữ sách/vị trí/serial, đúng phí, bấm lại, thay cấp không trùng ID, gói danh sách chiêu đúng cấp, kiểm tra quyền sau một giây và phục hồi quyền từ task đã lưu.
- Sai cấp, sai phái, không có Linh Thạch: giữ sách. Cuộn magicscript thường vẫn tiêu hao. Chủ động bỏ sách: thu hồi đúng quyền chiêu.
- Build Dev sạch: 0 lỗi, 1 cảnh báo kích thước image có sẵn. DLL nạp được và chỉ giữ export sản phẩm; không cài export kiểm thử.
- Khi thay DLL: 235 tệp CharacterStore, 225 tệp nhân vật bot và 41 binary runtime Release giữ nguyên. Biên bản: `analysis/than_ky_book_use_20261109/installation.json`.

Chưa nghiệm thu hình ảnh F4 trên phiên chơi thật sau cài; không khẳng định riêng lỗi hiển thị đồ lục/hoàng kim đã hết chỉ dựa trên kiểm thử server. Chưa tự hoàn lại sách đã mất: ảnh hiện tại không xác định đủ tên/cấp sách; snapshot hiện tại cũng không chứng minh được số lượng sách trước khi bị trừ. Không phục hồi toàn bộ nhân vật về bản cũ hoặc đoán sách để thêm.

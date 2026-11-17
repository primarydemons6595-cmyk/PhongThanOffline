# Công thức và nồi sự kiện — 03/11/2026

Mỗi công thức trong Studio có các lựa chọn x1, x10, x100 và tên đơn vị có thể sửa. Số lượng nguyên liệu được quy đổi theo định lượng thành phẩm chuẩn. Nguyên liệu phải chia hết cho số thành phẩm chọn; Studio báo lỗi trước khi áp dụng nếu không chia được. Mỗi mẻ dùng một dụng cụ, không nhân số dụng cụ theo số bánh.

NPC và trạm chỉ có mục chọn công thức; cửa hàng vẫn xuất hiện ở NPC có chức năng bán hàng. Nồi đang nấu chỉ hiện lời nhắc khi bấm. Khi chín, hộp thoại chỉ có nút thu thành phẩm. Lời nhắc, thông báo chín và tên nút nằm trong các trường nội dung công thức hiện có của Studio. Công thức có thời gian nhưng không dùng trạm vẫn kiểm tra và nhận mẻ qua chính mục chọn công thức.

Tên nồi có tên nhân vật trong ngoặc. Chủ nồi được lưu dạng hex cùng dữ liệu nồi; nồi cũ có thể lấy lại tên từ khóa nhân vật trong tên file. Quyền thao tác vẫn dựa trên khóa tài khoản/nhân vật/thời điểm tạo vai trò, không dựa vào dòng tên hiển thị.

Thanh tiến trình dùng lại `PhongThanDrawGatherProgress` cùng hai SPR có sẵn:

- `spr/phongthan/gather/background.spr` — SHA256 `39a1fcb7859acb74b37df133940282657544ea26bb8667c7a4fc16d1c63a7737`.
- `spr/phongthan/gather/fill.spr` — SHA256 `56e148400ed26eb85743b4bb0935b7ef1dfc674c743a2b0e7eb5a4aec249860a`.

Không tạo ảnh hoặc kiểu thanh mới. Thanh bám vị trí nồi trong lượt hiển thị tên NPC. Máy chủ gửi thời điểm bắt đầu, chín và cháy bằng packet `0x30c6`; client nội suy số giây và không quyết định quyền nhận. Khi cháy hoặc nhận xong, máy chủ dọn nồi.

Snapshot cấu hình có dòng `V eventId recipeId batchMask` (bit 1/2/4 tương ứng x1/x10/x100). Lệnh Lua CustomEvent nhận thêm số lượng ở tham số thứ năm. Trạng thái nồi dùng thao tác 6 trả mã công thức, trạng thái và số giây còn lại. Lệnh cũ không truyền số lượng vẫn giữ định lượng chuẩn để tương thích script cũ.

Giao dịch cho phép tối đa 128 vật phẩm đầu ra khi dùng cơ chế tạo/chồng vật phẩm hiện có, để hỗ trợ 100 thành phẩm không chồng. Nếu thiếu ô hoặc tạo thất bại, giao dịch hoàn lại nguyên liệu. Đồ trong kho, vật phẩm và trạng thái nấu của nhân vật không bị xóa khi cập nhật script.

`Tools/RecompileCustomEvents` có chế độ xem trước. Chế độ áp dụng yêu cầu đóng Studio và dừng world; sao lưu cấu hình, script sở hữu, chỉ mục và manifest trước khi biên dịch lại. Nếu có bản nháp hoàn chỉnh dựa trên đúng phiên bản trước đó, công cụ giữ nội dung và cập nhật phiên bản nền vì lần biên dịch này không sửa nội dung do người dùng soạn.

Kiểm tra: native cooking/transaction, trạng thái hộp thoại Lua thực, hạn giờ client, tài nguyên thanh gốc, định lượng 100 và hoàn tác khi túi đầy. Hình giao diện Studio được lưu tại `analysis/client_requests_20261103/editor-checks/recipe-batches.png`.

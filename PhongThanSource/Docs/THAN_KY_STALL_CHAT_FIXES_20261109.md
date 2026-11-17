# Thần Kỹ, gian hàng và chat — 09/11/2026

Đã cài vào `PhongThanRuntime-Loose` lúc 17:38 ngày 09/11/2026. GameServer PID 59776 và Event PID 15788 đã xác nhận kết nối lúc 17:41, dữ liệu trạng thái mới đúng PID, lease hợp lệ, không có lỗi Event và 200 bot online. Một số bot vẫn báo chờ quyền/mật khẩu rương; số lượng thay đổi theo công việc đang thực hiện, không coi là đã xử lý xong. Không build/publish runtime Release. Chưa có xác nhận giao diện trên phiên chơi thật sau cập nhật.

## Các thay đổi

- Tin rao bán của bot dùng số vạn: `50000` → `5 vạn`, `15500` → `1,55 vạn`. Chỉ đổi cách hiển thị của 24 mẫu tin rao; không đổi giá tiền, giao dịch hoặc quy đổi xu.
- Bổ sung dòng Hồ Lô Đằng bị thiếu trong bảng NPC client thực sự đang dùng. Tài nguyên gourd được đối chiếu byte với PAK gốc, không thay bằng hình mới.
- Sửa tên/mô tả 51 sách Thần Kỹ: tên đỏ, có hệ phái, cấp và hướng dẫn sử dụng; bỏ chữ lỗi và thông tin kỹ thuật trong phần nội dung đã sửa. Tooltip còn sửa tên sách cũ đã được lưu/cache. Giữ bản vá không tiêu hao sách khi kích hoạt; không hoàn hay thêm sách vào nhân vật.
- Hồ Lô Đằng dùng danh sách chữ theo kỹ năng/hệ phái. Trang xác nhận hiển thị nguyên liệu và số lượng có màu. Giữ nguyên 29 định danh công thức và callback giao dịch.
- Sửa byte chữ “à” của Nhà Chiêm Tinh trong bảng tên hiển thị, không đổi khóa nhận diện script.
- Nút chọn NPC cố định 95 × 18, font 10, khoảng cách hàng 6; nhãn dài có thể trải hai dòng ra ngoài phần hình nút. Không tăng chiều cao nút theo chữ. Nội dung hội thoại/list vẫn giữ font 12.
- Hiện số lượng thật trong xác nhận mua gian hàng, chỉ đọc vì giao thức mua cả lô. Hiện tổng giá của cả lô, không nhân giá lô lần nữa với số lượng.
- Báo giá gian hàng dùng bạc, tránh dùng nhầm đơn vị tiền của shop NPC vừa mở trước đó. Cửa sổ gian hàng căn theo vị trí F4, kể cả tọa độ bắt đầu/kết thúc animation.
- Sửa trường hợp server cho xem hàng khi người mua còn chế độ chiến đấu nhưng lại từ chối mua. Chỉ chuyển về trạng thái đứng/an toàn khi người mua hợp lệ, ở gần và gian hàng hợp lệ; đồng bộ trạng thái đó về client. Giữ kiểm tra tiền, chỗ, quyền sở hữu, khoảng cách và chống mua trùng. Giao dịch thất bại có thông báo thay cho im lặng.
- Cả chat trái/phải giữ lịch sử khoảng 1,5 trang, cho cuộn đọc lại và không tự ẩn dòng theo bộ đếm cũ. Xóa vị trí/hiển thị link của khung trước khi vẽ khung mới để tránh link vật phẩm chồng khi cuộn.

## Kiểm tra

- Native CoreClient: tooltip 51 sách từ tên/intro cache cố tình sai, kiểm tra màu/phái/chữ lỗi; vật phẩm ngoài danh sách không bị thay. Kiểm tra tiền gian hàng sau shop NPC dùng xu và dòng tài nguyên NPC hoạt động.
- Lua 4 thực tế: 41 đường script sách, 4 trang danh sách NPC, 29 xác nhận/đổi và menu Cẩm Nang; callback/định danh công thức giữ nguyên.
- Native GameServer với gói xem/mua thực tế: tái hiện lỗi trước sửa; sau sửa mua cả lô đúng tiền/đồ, không mua trùng, thiếu tiền không mất đồ, đứng xa không tắt chiến đấu và không mua được. Kiểm tra cả 24 mẫu tin rao dùng `5 vạn`.
- Phần vẽ chat/control: 482 trường hợp cuộn với hai cỡ chữ, hai bố cục; link ngoài khung và link cũ được ẩn, không chồng chữ. Kiểm tra giá lẻ và kích thước nút cố định.
- Publisher: hai vai trò, chạy lặp không đổi kết quả, giữ sửa đổi mới hơn, từ chối đường dẫn Release. Kiểm tra cả tình huống publisher cũ khôi phục bảng trước đó: overlay cuối chỉ chấp nhận hash của nội dung tiền nhiệm đã kiểm chứng, giữ nguyên sửa đổi ngoài danh sách.
- Clean build Dev: Game.exe, CoreClient.dll, CoreServer.dll thành công. DLL tải được, có entrypoint thật và không còn export probe. Cảnh báo legacy vẫn có; đây không phải kết quả đo FPS trong phiên chơi.
- Cài đặt theo hash, sao lưu và kiểm tra đọc lại: 51 tệp; 235 tệp CharacterStore, 225 tệp bot và 41 binary Release không thay đổi bởi thao tác cài.

Biên nhận và log: `analysis/than_ky_presentation_20261109/installation.json`, `candidate-verification.json`, `publisher-verification.json`, `stall-fixed.stdout.log`, `client-native.stdout.log`, `chat-render-verification.log`, `lua-verification.log`.

Kiểm tra thêm bot mua ngựa/ghép đồ nằm ở [BOT_MOUNT_CRAFT_AUDIT_20261109.md](BOT_MOUNT_CRAFT_AUDIT_20261109.md). Chưa bổ sung AI kiếm công thức/nguyên liệu và ghép/thăng cấp trong đợt này.

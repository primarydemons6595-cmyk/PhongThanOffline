# Thanh công cụ và Chiêu Hồn Trận — 26/10/2026

## Nội dung bản sửa

- Thanh bên phải: giữ nền SPR gốc có alpha, dùng tọa độ từng control từ INI thay cho tự đặt `i*23`. Nút thu gọn về vị trí hông `Top=70`, nút ẩn client về `Top=199`. Khôi phục nút AutoKill đã bị bỏ sót. Khi thu gọn, dải còn 8 pixel và nền được cắt theo chiều rộng; mở lại khôi phục toàn bộ control.
- Ẩn client: thay lệnh `Switch(hidewindow)` không có đích bằng biểu tượng khay hệ thống; bấm biểu tượng hoặc Alt+B để khôi phục. Nếu không tạo được biểu tượng thì giữ cửa sổ hiện. Callback dùng subclass riêng của cửa sổ Game vì dispatcher Engine không chuyển tiếp `WM_APP`/`WM_HOTKEY` vào `HandleInput`. Không thay Engine dùng chung. Game đã có `SetMultiGame(TRUE)` nên vòng xử lý mạng vẫn chạy khi cửa sổ mất focus.
- Các icon cạnh minimap được tách hitbox theo ảnh SPR 24 pixel, cách 26 pixel; icon thế giới không còn chồng lên xếp hạng. Xếp hạng mở `KUiStrengthRank` và đường truy vấn Ladder sẵn có, thay vì bảng điểm chiến trường `KUiRankData`. Phục hồi frame xếp hạng Phong Thần, đặt danh mục tương thích ID ladder sẵn có; không tạo số liệu xếp hạng giả. Đây là adapter control lên frame gốc, không phải toàn bộ hệ thống bảng xếp hạng mới của VNG.
- Ẩn người chơi: nút bật/tắt cờ sẵn có; bổ sung tôn trọng cờ ở lớp thông tin cuối và lớp vẽ thế giới để tên/thanh máu/viền mục tiêu không còn xuất hiện sau khi ẩn thân. Chỉ áp dụng người chơi khác; không ẩn chính mình và NPC nhiệm vụ.
- Bản đồ thế giới: dùng lại SPR Phong Thần 800x600 từ PAK ưu tiên, căn giữa màn hình. Không cho `WorldSet.ini` ghi đè bằng ảnh JX và không áp tọa độ marker JX lên ảnh Phong Thần. Click hoặc phím đóng bản đồ vẫn dùng luồng hiện có. Chưa bổ sung click địa danh để tự di chuyển.
- Đấu giá: nút mở frame Phố Đấu giá gốc tại chỗ. **Source hiện không có module giao dịch/lưu ký đấu giá ở server**; bảng hiển thị dịch vụ chưa sẵn sàng. Không chuyển sang Kỳ Trân Các, không gửi lệnh giao dịch thường để giả lập mua/bán. Do đó phần mua/bán đấu giá còn thiếu; chỉ phần mở bảng được nối trong đợt này.
- Siêu Độ: 4 template 551–554 đã có đủ SPR skill001–004. Trước đây runtime ép `kind_partner` với phe khán giả; nhánh Paint có thể ẩn thân, nhánh thông tin không vẽ tên và cơ chế xóa thực thể im lặng áp dụng cho partner. Chuyển riêng trận sang `kind_dialoger`, phát snapshot sau khi đã đặt loại và tên. Nhờ đó dùng nhánh hiển thị và tồn tại của NPC server tĩnh, không áp cách sửa này lên tất cả đồng hành.
- Theo yêu cầu mới, phạm vi ghi nhận quanh tâm trận tăng từ 400 lên **600 MPS**, để quái ở sát mép vẫn được tính. Giữ 200 giây; vẫn cần đúng quái, bị hạ, đúng bản đồ, trận thật còn sống, đúng chủ sở hữu và phiên nhân vật. Buff/thời hạn lưu đơn lẻ không thay cho trận thật. Đổi map, hủy nhiệm vụ, hết hạn hoặc thoát phiên dọn trận.

## Đối chiếu và kiểm thử

Đọc tài nguyên bằng thứ tự `Client_Original/package.ini`; các proof ghi rõ PAK thắng ưu tiên và hash. Frame xếp hạng `F29B2E0F` đến từ `vng00.pak`; không dùng bản INI cũ đã trích trước đó làm nguồn cuối. SPR được giữ nguyên byte, không vẽ lại ảnh hoặc recolor.

- 58 kiểm tra native của engine Chiêu Hồn Trận: 4 mẫu, snapshot tên/loại cuối, còn tồn tại ở giây 199, hết giây 200, biên 600/601, biên đường chéo, đúng/sai quái/map/chủ, không mất cờ khi tạo thất bại, chống nhận trùng và tái sử dụng slot.
- 16 kiểm tra lifecycle khay hệ thống: tạo biểu tượng thất bại không ẩn, callback chuột/hotkey khôi phục, không subclass trùng, thoát dọn biểu tượng và callback. API Win32 được giả lập trong test; không thao tác cửa sổ game thật.
- 201 kiểm tra phụ thuộc/định dạng SPR và tọa độ control; đã xem bản dựng từ SPR gốc của thanh công cụ và ảnh bản đồ. Đây là bản dựng tài nguyên, chưa phải ảnh chụp nghiệm thu trong Game.
- GameClient, CoreServer và CoreClient Release build thành công. Gói `Deploy/ProjectContent/SidebarSoul20261026`, bằng chứng ở `analysis/sidebar_soul_20261025/`.

Đã phát hành sau khi người dùng xác nhận đóng Game/Studio Loose và thoát web: **52 file đúng hash**, gồm Game.exe, CoreClient.dll, CoreServer.dll và tài nguyên UI. World cũ 12468 lưu và dừng exit 0; world mới **105212** nạp đủ **105 map, 4.044 NPC/quái**, kết nối Database/Relay/Gateway thành công. Registry nạp 9.223 script, 0 lỗi; stderr world trống. CharacterStore không đổi trong bước thay file. Hash Goddess và cấu hình Event XP/đồ rơi được giữ nguyên.

Đã sao lưu trước thay; receipt ở `publication.json`, xác minh sau nạp ở `verification.json`. Chưa nghiệm thu trực tiếp thao tác nút/trận bằng nhân vật trên Game. Người dùng cần mở lại Game Loose để nạp binary và UI mới.

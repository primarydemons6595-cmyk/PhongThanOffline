# Hoàn tất nhiệm vụ, Avatar, chat và Ủy thác — 06/11/2026

Đã cài bộ mới vào Loose chính, có sao lưu. Game, GameServer và Studio đều đã tắt trước bước cài. Không tự khởi động lại các dịch vụ trong lượt hoàn tất này; trạng thái Ready/Event trên đĩa từ phiên trước không được coi là xác nhận hoạt động hiện tại.

## Nội dung

- Khôi phục alias/script được nạp cho Kế Tục tại thủ khố Ngọc Hư và các điểm nhận/trả chuỗi tân thủ Đạo sĩ. Bản sửa dùng helper riêng của Đạo sĩ, không đổi chuỗi Giáp sĩ hoặc Dị nhân. Các phần này đã cài ở đợt trước, được giữ trong bộ hiện tại.
- Căn HP/MP/EXP/EXP Tiên Ma theo đúng ba kiểu Avatar; ẩn thanh EXP trùng. Ảnh đối chiếu tại `analysis/followup_quests_20261106/avatar-before-after.png`.
- Mảnh sách Chư Hầu nối vào luồng kiểm tra/tiêu hao và kho sách bang hội native, có kiểm tra ghi lưu và chống nhận lặp; chính sách quản lý giữ trong Studio Thành thị / Bang hội.
- Thông tin dưới Avatar có tên server, câu trạng thái tự nhập, website hoặc nội dung tùy chọn. Studio có canh trái/giữa/phải và dịch **riêng chữ** theo X/Y. Trạng thái là nội dung người vận hành đặt, không phải phép đo server tự động. Ảnh editor tại `analysis/followup_quests_20261106/studio-avatar-offset.png`.
- Hai ô chat: bỏ resize lặp không đổi kích thước làm mất trạng thái ẩn dòng; bỏ lần cộng Y thừa khiến chữ trượt xuống vùng nút kênh. Hit-test cũng bỏ các dòng đã ẩn. Thời gian tự ẩn hiện có được giữ.

## Ủy thác đúng cửa sổ THOÁT có sẵn

Đã bỏ dòng Ủy thác thêm vào menu ESC và vô hiệu menu nhận thưởng Lua cũ. Chọn ba phương thức ngay trong cửa sổ THOÁT có sẵn; nhớ lựa chọn nếu bật checkbox. Chỉ thoát sau khi GameServer xác nhận cấu hình, có xử lý lỗi, timeout, hủy và gói xác nhận đến muộn. Client và Server dùng cùng giao thức Ủy thác v2.

Khi đăng nhập lại, server tự thanh toán sau khi nhân vật/hành trang đã nạp đầy đủ:

- Nhật Nguyệt Đơn còn hiệu lực: EXP cấp × 1.200 mỗi giờ, giới hạn bốn giờ/ngày.
- Lam Dương Đơn: mỗi giờ dùng một đơn để nhận mảnh hoặc viên Lam Thủy Tinh.
- Thái Cực Đơn: mỗi giờ dùng một đơn để tăng EXP kỹ năng chuột phải đã chọn, giới hạn cấp kỹ năng 10.
- Vật phẩm tự vào túi. Túi đầy thì chuyển thành thư qua hộp thư native hiện có; không thêm nút nhận thưởng Ủy thác.
- Thư tràn túi được giữ ẩn đến khi Goddess xác nhận ghi giao dịch. Có journal phục hồi trước/sau xác nhận lưu để tránh phát trùng hoặc trừ đơn mà mất quà.
- Thiếu đơn hoặc hộp thư đầy giữ phần giờ chưa thanh toán để tự thử lại, chưa trừ chi phí. Giờ không còn đủ điều kiện do đạt giới hạn EXP/cấp kỹ năng được dọn mà không trừ thêm đơn.

Hai giá trị chưa có công thức server gốc — EXP kỹ năng/giờ và tỷ lệ ra viên Lam Thủy Tinh — là cơ chế Loose được người dùng cho phép cấu hình trong Studio. Các mặc định hiện tại là 1.200 EXP kỹ năng/giờ và 1.000/10.000 cho viên nguyên; không gọi hai giá trị này là chuẩn server VNG.

## Kiểm tra

Đã qua: Lua 4 thật cho alias/chuỗi Đạo sĩ và Kế Tục; chuyển trạng thái Ủy thác bằng mã production; giao dịch QuestExchange; mailbox/journal với ghi đĩa thật trong fixture riêng; tám tổ hợp lựa chọn THOÁT cùng xác nhận/hủy/timeout; 2.500 khung chat và 810 trường hợp paint/hit-test; Studio render offscreen. Kiểm tra này không thay nghiệm thu đăng xuất/đăng nhập và hình ảnh trong client thật.

CoreServer/CoreClient/GameServer/GameClient/Studio đều build qua. Bản Release Client/Server đã kiểm tra dấu nguồn hiện hành. Patch Client 1.02 và Server 1.02 đã xác minh hash/chữ ký; cần cập nhật hai phía đồng bộ, cùng chuỗi patch trước đó.

Cài đặt cuối: `analysis/followup_quests_20261106/deployment-Final.json`. Bản sao lưu: `analysis/followup_quests_20261106/backup-Final-20261106-180540`. Không đổi trực tiếp hồ sơ nhân vật; 87 file CharacterStore giữ nguyên hash trong bước cài.

Phần bổ sung theo yêu cầu tiếp theo: [Phá Quân và kiểm tra Xích Tùng Tử](PHA_QUAN_XICH_TUNG_TU_20261106.md).

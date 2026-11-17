# BotAIgent: Local / OpenAI

Studio → Quản lý server → BotAIgent → AI Local / OpenAI API.

- Chọn AI Local, OpenAI API hoặc hội thoại quy tắc. Checkbox bật AI hội thoại là công tắc chung.
- Nhập khóa trong PasswordBox rồi lưu; để trống giữ khóa cũ. Khóa được mã hóa bằng Windows DPAPI theo tài khoản chạy Event, không trả về dashboard, không đưa vào client hoặc log.
- Model Online mặc định gpt-4.1-mini; cấu hình được tên model, lượt/phút, lượt/ngày. Hạn mức là số lần thử gửi, kể cả lỗi mạng; token chỉ cộng khi nhận usage thực tế. Không phải hạn mức tiền.
- Bot tự trò chuyện qua API chỉ khi bật tùy chọn riêng. Lỗi Online dùng hội thoại dự phòng và chờ 30 giây trước khi tự gọi lại. Phép thử thủ công không bị chặn bởi thời gian nghỉ này.
- Chọn bot, nhập câu hỏi, bấm So sánh Local / Online. Hai nhánh dùng dữ liệu trạng thái bot hiện tại, lịch sử rỗng để so sánh độc lập; hiện model, thời gian, câu trả lời hoặc lỗi. Không gửi chat hay thực hiện hành động game trong phép thử.
- Online dùng endpoint cố định https://api.openai.com/v1/chat/completions, strict JSON schema, store=false, tối đa 192 completion tokens và thời gian chờ 5 giây. Cùng bộ kiểm tra ngữ cảnh/TCVN3 và phân quyền hành động với Local. Đổi model cần tương thích các tham số này.

Kiểm tra: Event và Studio build 0 warning / 0 error; 73 kiểm tra managed đạt, bao gồm mock HTTP, DPAPI, không lộ khóa, quota, DNS và timeout.

Kiểm tra chạy thật 2026-10-29 khoảng 21:23–21:27 (+07): khóa đã được người dùng lưu; provider=openai, model=gpt-4.1-mini. So sánh một câu chào: Local trả lời sau 4319 ms (có khởi động model), Online hết 5030 ms không có phản hồi. Tra DNS api.openai.com lỗi server failure; thử DNS 1.1.1.1 cũng timeout; www.microsoft.com không phân giải được. Chưa xác minh API key hợp lệ, chưa có phản hồi/token usage Online và chưa đánh giá được chất lượng Online. Không đổi DNS/proxy/cấu hình mạng hệ điều hành.

Đã cập nhật Event và Studio; GameServer PID 166032 giữ nguyên phiên. Cấu hình/khóa người dùng được giữ nguyên.

## Cập nhật sửa kết nối khoảng 21:30 (+07)

Người dùng yêu cầu xử lý tiếp lỗi kết nối. Máy dùng DNS WARP 127.0.2.2/127.0.2.3; WARP MASQUE báo connected nhưng DNS và HTTPS bị treo. Truy vấn trực tiếp router trả được địa chỉ api.openai.com. Làm mới WARP vẫn lỗi; đổi giao thức consumer WARP từ MASQUE sang WireGuard, giữ WARP bật. Sau đổi, HTTPS tới endpoint models trả 401 khi không gửi khóa (đúng kỳ vọng), xác nhận kết nối lại được.

Phép thử qua chính Event Service với khóa đã lưu: Online gpt-4.1-mini trả HTTP 429, code credit_balance_exhausted trong 1010 ms; Local trả câu theo trạng thái bot trong 4222 ms có khởi động model. Không có câu trả lời Online vì số dư API đã hết. Khóa không bị đọc/in ra hoặc thay đổi. Chưa đánh giá được chất lượng Online; người dùng cần bổ sung API credits trong tài khoản OpenAI để thử tiếp. Không thực hiện thao tác thanh toán.

WARP vẫn báo network unstable sau đổi dù API đã phản hồi; đây là xác nhận kết nối ở thời điểm thử, không phải kiểm chứng độ ổn định lâu dài. Nếu cần hoàn nguyên giao thức dùng warp-cli tunnel protocol set MASQUE. Không đổi DNS/proxy/firewall hệ điều hành, không restart GameServer.

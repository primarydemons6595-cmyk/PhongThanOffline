# Thêm bot trên bản Release, không cần Studio

1. Mở `PhongThan.Event.exe` trong thư mục Server Release, chọn **BotAIgent**.
2. Bấm **Thêm bot**.
3. Chọn tạo tên ngẫu nhiên, nhập một tên, hoặc **Nhập tên từ TXT**.
4. Chọn phái/giới tính cụ thể hoặc **Chia đều 6 nhóm phái / giới tính**.
5. Bấm **Tạo bot**. Nếu hệ thống bot đang tắt, bấm **Chạy bot** tại tab chính.

TXT lưu dạng UTF-8 hoặc Unicode UTF-16, mỗi dòng một tên, ví dụ:

```text
ThanhLong99
Gió Lạnh Về
Hải Bánh Cuốn
LinhKun2k
Khách Vãng Lai
PhongGaming9x
```

Dòng trống được bỏ qua; khoảng trắng ngoài tên được cắt. Tên tiếng Việt được
giữ trong công cụ và chuyển TCVN3 khi gửi vào game. Chỉ nhận tên hợp lệ 3–24 byte
TCVN3, gồm chữ/số, dấu chấm, gạch dưới và khoảng trắng; không tự cắt tên hay đổi
tên trùng. File trùng tên, tên đã tồn tại hoặc vượt chỗ trống sẽ báo lỗi, không
tạo một phần danh sách. Nếu file đổi sau khi xem trước, phải chọn lại file.

Số lượng tự lấy theo TXT. Tổng giới hạn 128 hồ sơ gồm bot đã lưu trữ. Bot mới được
bật riêng, có chế độ tự động và tọa độ Tân Thủ Thôn theo phái. Tạo hồ sơ không tự
bật toàn bộ hệ thống bot nếu trước đó đang tắt. Khi GameServer chưa sẵn sàng,
hồ sơ được lưu và bot vào game sau khi server sẵn sàng.

Không cần thêm file vào client hoặc đưa Studio vào gói phát hành. Event dùng
assembly `PhongThan.Event.Runtime.dll` riêng và API có xác thực/kiểm tra revision.

Kiểm thử: `Tests/EventServiceHttp` chạy trên runtime tạm, kiểm tra tạo lô qua HTTP,
chia nhóm, tọa độ, tên Việt/TCVN3, UTF-8/UTF-16, tên trùng/Unicode tương đương,
giới hạn dung lượng, revision xung đột và tính nguyên khối của lô. Không tạo bot
thử trên server thật.

# PK và Khai Minh Đảo — 25/10/2026

Đã áp dụng lên Loose trước gói sửa nhiệm vụ tân thủ. CoreServer cuối của đợt PK có SHA256 `2ecaa3a779096a4f40a68e16ff9037ae3675a536a7343310b869e338d7944074`.

- Chết không trừ/rơi tiền. Bảng PK gốc có cột tiền bằng 0 nhưng loader JX cũ đọc nhầm cột tỷ lệ rơi vật phẩm thành tiền; đồng thời bỏ nhánh trừ tiền cố định khi bị NPC giết.
- Rơi ngẫu nhiên trang bị/vật phẩm chỉ áp dụng khi nạn nhân bật PK tự do, với các bảo vệ vật phẩm khóa/không rơi/kho/thú cưỡi và ngoại lệ hoạt động hiện có. Giữ các tỷ lệ từ bảng Phong Thần; không tự đặt lại tỷ lệ.
- PK vượt 6 được đưa đến Khai Minh Đảo. Cờ chịu phạt được giữ đến PK 0; xuống dưới 7 vẫn không thoát đảo. Kiểm tra cả dịch chuyển và nơi hồi sinh, không chỉ lời thoại thuyền phu.
- Thời gian giảm tội chỉ tính khi online trên đảo; giữ phần giây lẻ, không cộng thời gian offline. Lua dùng thang thời gian gốc, UI native dùng điểm PK qua phép chuyển đổi tương thích.
- Quan Âm Thủy đúng tuple 8/33/2 giảm 2 điểm mỗi lần, tiêu hao đúng vật phẩm đang sở hữu; PK 0 không tiêu hao. Mô tả client/server đã đổi cho đúng. Thuyền phu đảo chỉ cho ra khi PK 0.

Kiểm thử: 139 kiểm tra native, 6.001 giá trị của điều kiện thuyền phu bằng Lua4 thật; CoreServer/CoreClient build thành công. Chỉ phát hành CoreServer và ba file nội dung cần thiết, không thay CoreClient của bản sửa hồi sinh trước đó.

Bằng chứng ở `analysis/pk_prison_20261025/`, gồm provenance PAK gốc, tests, manifest cuối và hai receipt phát hành. World sau revision 2 là 87828, đã sẵn sàng; sau đó được thay bằng world 12468 của gói tân thủ, giữ nguyên DLL PK. Chưa thử PK bằng nhân vật thật trong giao diện game.

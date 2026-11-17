# Trang bị hiếm từ quái thường

Chỉ áp dụng cho engine Native Drop chính thức, sau khi tạo thuộc tính trang bị.
Không sửa các bảng rơi dùng chung vì boss cũng có thể dùng những bảng đó.

| Loại trang bị | Giữ lại so với tỷ lệ trước |
|---|---:|
| Trắng | 100% |
| Xanh (thuộc tính) | 50% |
| Hoàng kim | 10% |
| Lục (đồ bộ) | 2,5% |
| Cam | 1% |
| Tím | 1% |

Đây là hệ số nhân sau lần rơi gốc, không phải xác suất tuyệt đối mỗi lần giết
quái. Không thay số dòng thuộc tính, cấp vật phẩm hay bảng rơi của boss.

Điều kiện: quái thường, không có chủ, không có script tương tác/rơi riêng,
Special=0, Treasure và Treasure1 trong 0–1; không nằm trong danh sách
OriginalBossState; không dùng bảng rơi đang được Event gán riêng. Nếu chưa đọc
được danh sách boss thì giữ nguyên đồ rơi. Quái xanh/tinh anh giữ nguyên.
Tiền, nguyên liệu, thuốc và vật phẩm nhiệm vụ/sự kiện không phải trang bị giữ
nguyên; phần thưởng Lua và các hệ thống trao thưởng riêng không đi qua bộ lọc.

Triển khai: `Sources/Core/Src/NormalRareEquipmentPolicy.h` và
`LegacyDropRuntime.cpp`. Dùng luồng RNG riêng; không rút thêm số ngẫu nhiên của
bảng rơi gốc, boss hay hệ thống thuộc tính. Đồ bị loại được giải phóng trước khi
đưa ra mặt đất; ghi `NORMAL_RARE_EQUIPMENT` vào log với chất lượng, hệ số và kết quả.

Kiểm tra: `Tests/LegacyDropNativeTest/NormalRareEquipmentTests.cpp` duyệt đủ
10.000 giá trị mỗi loại trang bị và kiểm tra các trường hợp loại trừ.

# Xem trang bị người khác — Loose, 29/10/2026

Luồng xem trang bị cũ chỉ gửi dòng mẫu, seed, cấp nâng và tham số sinh đồ. Client dựng lại món từ bảng mẫu nên mất ScriptParam luyện Cam, tên Tinh Quân và các thuộc tính thực tế không có trong món nền. Đồ lục có thông tin bộ ngay trong mẫu nên thường vẫn hiển thị đủ.

## Thay đổi

- `PHONGTHAN_VIEW_ITEM_INFO` bổ sung tên thực tế, ScriptParam, thuộc tính cơ bản (20), điều kiện sử dụng (6), thuộc tính ma thuật (20), bộ/nhóm/số món kích hoạt, sức mạnh, phúc duyên, độ bền và hạn dùng. Socket gửi kèm định nghĩa bất biến để client có thể giải mã ngay, kể cả đồ Khảm trên nền Cam.
- Phiên bản riêng cho dữ liệu xem đồ là 2. Không đổi phiên bản dữ liệu giao dịch hoặc bản lưu nhân vật.
- Server lấy dữ liệu từ chính món đang đeo. Client chỉ áp vào đối tượng tạm của cửa sổ xem đồ, không tra cứu/cập nhật túi đồ bằng serial của người khác.
- Client kiểm tra toàn bộ các slot có đồ trước khi xóa cửa sổ cũ; từ chối phiên bản, tên không kết thúc, loại thuộc tính và socket sai. Khôi phục trạng thái trước, chép thuộc tính cuối cùng từ server sau để không cộng cấp nâng lần nữa.
- Gói đầy đủ 13 slot dài 15.403 byte, có kiểm tra tại thời điểm biên dịch để nằm dưới bộ đệm server 16 KiB trừ 32 byte dự phòng.

## Kiểm tra và triển khai

- `Tests/Test-ViewedItem.ps1`: 104 kiểm tra truyền/khôi phục đồ lục, Cam, Tinh Quân, Tiên/Ma, Khảm; toàn bộ thuộc tính, tên và trạng thái nâng; gửi lại không cộng hai lần; từ chối dữ liệu lỗi. Dùng layout KItem thật, helper truyền/nhận thật, phương thức nâng và registry socket thật; các setter cơ bản được cô lập, không thao tác tài khoản thật.
- `Tests/Test-PhongThanProtocolBaseline.ps1 -ProjectRoot <source>`: PASS 49 kiểm tra, 13 nhóm giao thức.
- Build Dev `CoreServer`, `CoreClient`: thành công. Kiểm tra trước khi thay: 328 liên kết DLL nội bộ, không thiếu symbol. Sau thay: toàn client 27 PE, 1.405 liên kết nội bộ, không thiếu symbol.
- Chỉ thay `PhongThanRuntime-Loose/Client/CoreClient.dll` và `PhongThanRuntime-Loose/Server/CoreServer.dll`. Bộ cài tương ứng ở `Deploy/ProjectContent/ViewedEquipment20261029`; manifest ghi hash trước/sau. Client và server cần dùng cùng bản vì kích thước packet đã thay đổi.
- World cũ PID 100920 lưu/thoát sạch (exit 0). Chín file nhân vật không thay đổi trong lúc sao chép. World mới PID 156052.
- Kiểm tra sau thay: 2/2 file khớp manifest; world mới nạp đủ 105 map, snapshot cập nhật và EventService xác nhận đúng PID. Client PID 145244 mở cửa sổ `Phong Than Online`, phản hồi bình thường, nạp đúng CoreClient.dll mới và core init hoàn tất.

Bằng chứng và bản sao lưu: `analysis/view_equipment_20261029`. Chưa nghiệm thu bằng hai tài khoản thật mở cửa sổ xem đồ trong game; kiểm tra Tiên/Ma ở đây là kiểm tra dữ liệu, không phải thử trang bị bằng nhân vật thật.

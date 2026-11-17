# Item Editor: tên tiếng Việt và danh sách schema

Hai lỗi giao diện: danh mục dùng `Legacy.Display` (CP936) để hiển thị tên item TCVN3; danh sách file lấy mọi TXT dưới settings/item nên đưa cả bảng nâng cấp/quy tắc/part vào Item Editor rồi báo schema không khớp. NPC không gặp lỗi tên vì có `NpcDisplayNames`.

## Bản sửa

- Tên item trong thẻ, tìm kiếm và cột tên nâng cao được giải mã TCVN3; bỏ marker # và thẻ màu ở nhãn hiển thị.
- Ô sửa tên dùng TCVN3, giữ marker trong giá trị gốc. Các cột đường dẫn SPR/giá trị khác dùng codec đang chọn; không chuyển mã cả file vì header/path và tên dùng bảng mã khác nhau.
- Hộp thuộc tính giữ nguyên chuỗi byte của ô không đổi, tránh encode lại đường dẫn khi chỉ mở và xác nhận.
- Item Editor lọc theo cùng schema mà TableDocument kiểm tra: tiêu đề tên/loại item/ảnh, tối thiểu 12 cột. Không nới điều kiện để cho phép ghi bảng quy tắc. Các file bị loại vẫn tồn tại và có thể xem ở Loose Editor.
- Danh mục hiện tại: 21 bảng phù hợp ở Client và 21 ở Server; loại 159 file phụ ở Client, 144 ở Server. Không xóa hay sửa các file đó.

Tên có ký tự `?` sẵn trong nguồn không thể phục hồi chỉ bằng đổi bảng mã; bản sửa giữ nguyên, không tự sáng tác tên.

## Kiểm tra

439 kiểm tra: mẫu TCVN3, header CP936 độc lập; tất cả bảng được liệt kê mở được, no-op giữ byte gốc; sửa tên trên bộ nhớ không đổi cột SPR; hoàn tác khôi phục byte gốc; mọi bảng phụ không hợp schema đều bị loại. Không ghi dữ liệu Client/Server trong lượt kiểm tra.

Kiểm tra WPF trên magicscript, questkey, skillbook và TownPortal: tên thẻ/ô sửa/tìm kiếm tiếng Việt đúng; serialize không thay đổi. Ảnh/log trong `analysis/studio_appearance_20261105/item-verification` và `item-ui-verification`.

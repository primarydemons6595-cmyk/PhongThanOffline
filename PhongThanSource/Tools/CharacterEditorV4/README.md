# Phong Thần Character Editor v4.1.1

Từ v4.1.1, bộ xuất nằm trong `CharacterStore/<tài khoản>/`: file nhân vật và thư mục `NativeDropPersistenceV1` ở cùng thư mục tài khoản. Cần bản server đã chuyển sang lưu theo tài khoản để chép trực tiếp cấu trúc này. Editor đọc được cả cấu trúc cũ và mới. Chức năng lưu trực tiếp vẫn yêu cầu GameServer đã dừng; không tự triển khai vào server đang chạy.

WinForms .NET 8, sửa từ source v3 người dùng cung cấp. Bản làm việc nằm trong `PT/PhongThanSource/Tools/CharacterEditorV4`; source v3 ban đầu không bị ghi đè.

## Sử dụng

- Chạy `PhongThanCharacterEditor.exe`, chọn file `.pthc`.
- F4 dùng ảnh nền, mannequin theo giới tính/hệ phái, vị trí trang bị từ `UiItem.ini`, và icon SPR từ bảng item client. Có 4 túi 5×7, 6 kho 6×10, danh sách đủ mọi container, tooltip và mức cường hóa đã giải mã.
- Kéo thả để chuyển ô, đeo/tháo; ô bị chiếm, sai loại trang bị hoặc vượt biên sẽ bị từ chối. Chuyển giữa các trang bằng bảng vị trí bên phải. Không tự đổi chỗ hoặc xóa món đang chiếm ô.
- Chọn thư mục client để dùng client khác. `Nạp PAK…` nhận nhiều file PACK; thứ tự chọn là thứ tự ưu tiên. `Ưu tiên ảnh/bảng từ PAK` cho phép đọc trực tiếp trong archive dù có file rời. Chuỗi `package.ini` được nạp tự động, trừ client ghi `LOOSE_ONLY`.
- SPR hỗ trợ indexed palette và Seaweed true-color RLE. PACK hỗ trợ raw, UCL NRV2B và SPR nén theo frame; chỉ đọc, không ghi/giải nén archive ra client. BZIP2/method lạ báo lỗi, không hiển thị ảnh giả. Layout F4 Loose được nhúng làm dự phòng khi client PAK không có `UiItem.ini` cùng đường dẫn.
- Sửa tiền, điểm, level; di chuyển hoặc xóa item đều tạo lại liên kết companion. Thuộc tính magic/base/requirements/quality/serial của các món còn lại được giữ nguyên byte.
- Cường hóa: chọn trang bị trên F4 hoặc danh sách, dùng `− / +` hoặc chọn mức 0–12 rồi `Áp dụng`. Bộ tính native thực hiện đúng delta của bảng server và thử tái tạo/nạp lại trước khi áp dụng vào bản đang chỉnh. Sau đó dùng Save/Export. Không trừ tiền/nguyên liệu, không quay xác suất như NPC; đây là chỉnh offline cho admin. Quy tắc đang có được giữ và khóa; muốn đổi chế độ phải hạ về +0 trước. Chỉ hỗ trợ loại đồ có recipe cường hóa gốc. Chỉ số đồ khác vẫn chỉ xem.
- Giữ thư mục `Native` (UpgradeNative.exe, Engine.dll, LuaLibDll.dll) cạnh exe editor. Bộ tính chạy nền, chỉ đọc bảng server; không điều khiển hay restart GameServer, không tự ghi CharacterStore.
- `Xuất bộ lưu…` tạo thư mục mới chứa `CharacterStore` và `NativeDropPersistenceV1`. Cần giữ cả hai thư mục.
- `Lưu file (offline)` có kiểm tra file nguồn thay đổi ngoài editor, backup có timestamp, atomic replace, và chặn ghi CharacterStore của Server khi GameServer đang chạy. Không sửa trực tiếp nhân vật còn online. Snapshot được ghi bền trước canonical state; snapshot cũ được giữ.
- File thiếu companion vẫn mở xem nhưng không cho Save/Export để tránh biến đồ có thuộc tính thành đồ tái tạo sai. Không tự xóa marker hoặc hạ revision.

## Build và kiểm tra

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Native/Build-Native.ps1
dotnet build PhongThanCharacterEditor.csproj -c Release
dotnet bin/Release/net8.0-windows/PhongThanCharacterEditor.dll --verify
dotnet bin/Release/net8.0-windows/PhongThanCharacterEditor.dll --upgrade-verify
```

Build native cần engine source `PT/PhongThanSource` và VC98 có trên máy phát triển. Chạy exe đã đóng gói không cần compiler. Không build đè DLL đang được app/test sử dụng; có thể truyền `-o` để build vào thư mục phát hành mới.

`--verify` cần chạy trong cây workspace PhongThan/PT có các dữ liệu kiểm chứng. Kết quả ghi vào `analysis/character_editor_v4_20261020`. Bộ kiểm tra xác minh lưu/đọc, xóa/ordinal, bảo toàn payload, backup từng lần, từ chối companion thiếu, ảnh client/PAK thật và bộ phục hồi. Không thay Main CharacterStore.

Editor tự tìm workspace từ thư mục chạy hoặc vị trí exe; hỗ trợ cả khi đặt source v4 trong thư mục `PhongThanCharacterEditor_v3_Source` nằm cạnh `PT`.

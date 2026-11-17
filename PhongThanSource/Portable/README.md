# Chuyển Phong Thần sang máy Windows khác

Repo gồm `Client_Original`, `PhongThanRuntime-Loose`, `PhongThanSource`. Phần bổ sung này chứa snapshot tài khoản/nhân vật và bộ thiết lập máy mới. Không thay đổi code hay cấu hình runtime gốc trên máy xuất bản snapshot.

## Cách dùng trên máy mới

1. Cài Git và Git LFS, đăng nhập GitHub bằng tài khoản có quyền truy cập repo riêng tư.
2. Nên clone vào đường dẫn ngắn, không dấu, ví dụ `D:\PhongThan`:

   ```powershell
   git lfs install
   git clone https://github.com/primarydemons6595-cmyk/PhongThan.git D:\PhongThan
   cd D:\PhongThan
   git lfs pull
   ```

3. Mở `PhongThanSource\Portable\SETUP.cmd`. Chấp nhận UAC khi Windows yêu cầu cài thành phần Microsoft. Nếu bộ cài yêu cầu restart, khởi động lại Windows rồi chạy SETUP lần nữa.
4. Mở `START-STUDIO.cmd` trong cùng thư mục. Dùng Studio để bật cụm server, chờ báo sẵn sàng, rồi mở `START-CLIENT.cmd`.

Không dùng nút Download ZIP trên GitHub thay cho Git LFS. Các file model và gói client lớn phải tải đủ; Setup kiểm tra SHA-256 trước khi khôi phục. Cần Windows x64, dung lượng trống cho repo, model được ghép lại và toolchain được giải nén.

## Dữ liệu được bổ sung

- `Backups/account.bak`: backup **COPY_ONLY mới của database account đang dùng trên SQL Server 2022 Express**, có checksum và RESTORE VERIFYONLY. Có 14 bản ghi Account_Info, 14 Account_Habitus, 4 ServerList tại thời điểm xuất; bảng, view và dữ liệu đều nằm trong `.bak`. Xem ngày UTC/checksum trong `account-manifest.json`.
- `State/runtime-state.zip`: CharacterStore của Loose, dữ liệu bang hội/legacy database, UserData client và bản CharacterStore riêng của `PhongThanRuntime-State`. Các thư mục này được giữ riêng theo đúng đường dẫn, không trộn dữ liệu Staging vào Loose. Có cả `brain.db` được sao lưu bằng SQLite backup API.
- `Models`: model Qwen3 4B đang dùng, chia thành ba phần LFS. Setup ghép lại đúng file gốc và kiểm tra SHA-256.
- `Prerequisites`: bộ cài Microsoft SQL LocalDB 2022, .NET Desktop Runtime 8/10, .NET SDK 10, Visual C++ x86/x64. Mỗi file có URL nguồn, SHA-256 và chữ ký Microsoft được kiểm tra khi cài.
- `Toolchain`: snapshot VC6 cùng DLL phụ thuộc, cache NuGet và các công cụ/tài liệu ở thư mục Tools/docs của workspace gốc.

## Cách khôi phục và chạy lại

Setup tạo **SQL LocalDB instance riêng theo đường dẫn clone**, dùng Windows authentication và database `account`. Không cần chuyển mật khẩu SQL `sa` của máy cũ. Database được restore chỉ khi chưa tồn tại; không dùng `WITH REPLACE`. Sau restore có DBCC CHECKDB và đối chiếu số dòng/checksum của ba bảng với snapshot, rồi kiểm tra kết nối đúng driver SQL 32-bit mà AccountServer sử dụng.

CharacterStore chỉ được áp dụng lần đầu khi chưa có dữ liệu người chơi. Chạy Setup lại không reset tài khoản hay tiến trình nhân vật đã chơi. Model đã tồn tại phải đúng checksum. MDF/LDF, toolchain giải nén, cache NuGet và thông tin thiết lập máy nằm trong `Local/` được Git bỏ qua. Dữ liệu nhân vật/model được khôi phục vào đúng thư mục runtime. `DataBase.ini` của máy mới được tạo từ template không chứa mật khẩu. Các launcher cập nhật lại named pipe sau mỗi lần khởi động LocalDB.

Nếu chuyển vị trí thư mục clone sau khi đã chơi, cần sao lưu dữ liệu mới trước; chạy Setup ở vị trí khác có thể tạo SQL instance mới từ snapshot trong repo. Snapshot này là dữ liệu tại thời điểm xuất, không tự đồng bộ tiến trình chơi sau đó giữa hai máy.

## Làm tiếp code

- `BUILD-STUDIO.cmd`: build Studio và các project .NET phụ thuộc; NuGet có thể cần Internet để tải targeting pack chưa có trên máy.
- `BUILD-NATIVE.cmd`: build native bằng toolchain VC6 đã chuyển theo. Các biến PATH/INCLUDE/LIB/PHONGTHAN_MSDEV chỉ áp dụng cho tiến trình launcher, không sửa môi trường toàn máy.
- Studio được mở bằng launcher sẽ nhận cùng môi trường build. Có thể cần bật lựa chọn ExecutionPolicy bypass cho tiến trình build trong Studio nếu chính sách PowerShell trên máy chặn script.

## Những thứ phụ thuộc máy/người dùng

- Model hiện dùng backend Vulkan. Cần driver GPU hỗ trợ Vulkan tương ứng; hiệu năng/RAM/VRAM tùy máy. Bộ thiết lập không thay driver GPU hay đổi chính sách AI đang dùng.
- Các khóa API Online AI và khóa ký phát hành được Windows DPAPI bảo vệ theo người dùng/máy. Chúng không thể dùng bằng cách copy file `.dpapi`; cần nhập/cấu hình lại nếu dùng các chức năng đó. Cấu hình hiện tại dùng AI local.
- PID/named pipe/lock/status cũ trong snapshot không có nghĩa là dịch vụ đang chạy. Các dịch vụ phải được khởi động lại trên máy mới; launcher làm mới kết nối SQL.

Đã thử khôi phục vào đường dẫn khác trên máy xuất, kiểm tra SQL 32-bit và build từ nguồn. Đây không phải biên bản chạy đầy đủ trên một máy Windows mới hoàn toàn; phần cài prerequisite/UAC và driver GPU cần được xác nhận trên máy đích.

# Phong Thần Offline — bản nhúng server vào client

Bản này biến Phong Thần online thành bản **offline chạy trong đúng 1 tiến trình
Game.exe**: không Bishop, không Goddess, không AccountServer, không MSSQL,
không mở cổng mạng nào. Client "chơi đến đâu xử lý đến đó": server chỉ được
bơm tick trong lúc client chạy, và map nào không có người thì ngủ
(MapSleep) nên gần như không tốn CPU.

## Cách hoạt động (tóm tắt)

- Toàn bộ traffic client ↔ GameServer vốn đi qua 2 interface COM `IClient`
  (client) và `IServer` (server). Bản offline thay tầng socket bằng **hàng đợi
  trong bộ nhớ** (`Sources/GameClient/Embedded/`), giữ nguyên lớp packet —
  handler của client và server không phải viết lại.
- Phần Bishop/Goddess được thay bằng một **gateway nhúng**: đăng nhập luôn
  thành công (tên tài khoản = tên thư mục lưu trữ), danh sách/tạo/xóa nhân
  vật đọc ghi trực tiếp qua `CPhongThanCharacterStore` — mỗi nhân vật là 1
  file `.pthc` trong `ServerData\CharacterStore\<tài khoản>\`, có backup tự
  động của store như bản online.
- Server được bơm tick ngay trong vòng lặp của client
  (`KNetConnectAgent::Breathe`), mọi thứ chạy trên main thread nên không có
  tranh chấp trạng thái Engine dùng chung.
- Chế độ offline chỉ bật khi `config.ini` của client có `[Offline] Enabled=1`
  (hoặc biến môi trường `PHONGTHAN_OFFLINE=1`). Tắt đi là client chạy như bản
  online bình thường — code online không bị đụng.

## Thay đổi so với bản online

- **Tối đa 30 nhân vật / tài khoản** (online: 3) — `PHONGTHAN_CHARACTER_LIMIT`
  và `MAX_PLAYER_PER_ACCOUNT` đã nâng lên 30 ở cả client lẫn server.
- Màn hình chọn nhân vật **phân trang 8 nhân vật/trang**: trang tự đi theo
  nhân vật đang chọn; dùng phím `←`/`→` để chọn, `PageUp`/`PageDown` để lật
  trang.
- **Nút Xóa nhân vật dùng được ngay trên client** (bản online bị khóa kèm
  thông báo "không thể xóa"; khóa đó đã gỡ, hộp thoại xác nhận + nhập mật
  khẩu cấp 2 vẫn giữ nguyên).
- **Nút "Luu Game" trong menu ESC** (hàng thứ 2): bấm là server lưu nhân vật
  ngay và báo "Da luu game." qua tin nhắn hệ thống.
- **Tự động lưu** mỗi khi chuyển map / chuyển cảnh, cộng thêm cơ chế lưu sẵn
  có của server (lưu định kỳ + lưu khi thoát game / về màn hình chọn nhân
  vật).
- Chat thế giới / bang hội qua Relay **tắt** trong bản offline v1 (Relay là
  tiến trình riêng, không nhúng). Chat gần và các hệ thống trong map vẫn
  chạy bình thường.

## Build (trên máy Windows có VC6 như khi build bản online)

Chỉ cần build thêm 3 target — GameClient giờ link thêm `CoreServer.lib` và
`common.lib` (cả hai lib đều được post-build copy sẵn vào `Sources\Lib\release\`):

```powershell
cd PhongThanSource
.\Build\Build-PhongThan.ps1 -Targets Common,CoreServer,GameClient
```

Script build tự chạy các test baseline sẵn có (protocol/item/...) sau khi
build Core — các thay đổi offline đã kiểm tra không phá các test này.

## Đóng gói bản chơi được

Sau khi build xong (Game.exe mới đã được copy vào thư mục Client của
`PhongThanRuntime-Loose`):

```powershell
.\Offline\DongGoi-PhongThanOffline.ps1
```

Script sẽ tạo thư mục `PhongThanOffline-Play` cạnh repo, gồm: toàn bộ
Client + thư mục `ServerData` (dữ liệu server, đã bật MapSleep) +
`config.ini` đã bật `[Offline]` + file `Choi-PhongThan-Offline.bat`.
Chạy file .bat đó là chơi — không cần chạy bất kỳ server nào.

Muốn chỉ định đường dẫn khác:

```powershell
.\Offline\DongGoi-PhongThanOffline.ps1 -RuntimeRoot D:\PhongThan\PhongThanRuntime-Loose -OutputDir D:\Game\PhongThanOffline
```

## Dữ liệu nhân vật

- Lưu tại `ServerData\CharacterStore\<tài khoản>\*.pthc` — mang cả thư mục
  `ServerData` đi là mang theo toàn bộ nhân vật.
- Tên "tài khoản" offline chính là tên thư mục đó; mặc định là tài khoản bạn
  gõ ở màn hình đăng nhập (bất kỳ tên nào cũng đăng nhập được, mật khẩu
  không kiểm tra ở bản offline).

## Chưa làm trong v1

- **Chế độ LAN 2 máy** (host là client luôn): tầng transport đã tách sẵn
  (chỉ cần thay hàng đợi bộ nhớ bằng socket thật cho kênh gateway/world),
  làm ở đợt sau.
- Nút "Luu Game" ở menu ESC là nút chữ (chưa có sprite riêng như các nút
  khác).

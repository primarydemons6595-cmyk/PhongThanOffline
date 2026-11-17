# Biểu tượng và màu tên cấp 200 trong client gốc

Đã xác nhận từ `Client_Original/game.dat` và chuỗi PAK gốc đang có.

- `ui/头顶图标.ini`, entry `4811AE69`, serverlist.pak, SHA256
  `c2521ea57f9efb74a6ae98ba19689ca76266f908115b2b6308f940c91c187dd3`:
  `[TopLevel] NameColor=255,243,123`, `NameBorderColor=222,40,41`,
  `NameSpr=\spr\ui4\头顶图标\皇冠200.spr`.
- Sprite entry `66133847`, update0_164-166.pak, SHA256
  `1c09b3454adf1e42e246dc22a4d5ee3448c768cac24fcd45894fae6dfb52bfa9`.
  Một frame 25x18, vương miện đỏ/vàng có chữ 200, khớp ảnh người dùng.
- Loader `0x71CD06–0x71CD8B` đọc màu chữ, viền và sprite vào các trường
  `+0x660`, `+0x664`, `+0x668` của đối tượng global `0xE6FF10`.
- Renderer `0x6DDBF5`: so sánh level `[ebx+0x1c]` với `0xC8` (200),
  bỏ nhánh vẽ icon nếu level nhỏ hơn 200. Nhánh vẽ dùng sprite global
  `0xE70578`, tên resource `0xE70598`.
- Màu tên: bảng nhảy tại `0x6DE8BC` cho camp 7 vào `0x6DD32D`.
  Nếu level >=200 dùng màu/viền TopLevel; dưới 200 dùng `0xFF55FF91`.
  Các camp/trạng thái khác có nhánh màu riêng, không khẳng định mọi trạng thái
  PK đều bị ghi đè bằng màu cấp 200. Có nhánh NewBirth riêng trước icon thường.

Màu trong ảnh là chữ vàng nhạt viền đỏ, không phải toàn bộ chữ đỏ.
## Triển khai Loose

Đã bổ sung nhánh TopLevel vào final scene name overlay của CoreClient:
vẽ sprite trước tên cho người chơi cấp >=200; màu chữ/viền TopLevel áp dụng
cho camp 7 theo binary gốc. NPC/thú triệu hồi và các camp khác giữ palette riêng.
Đọc màu và NameSpr từ INI gốc; vương miện nằm trước cả tiền tố chức vụ quốc gia
khi có, không đè chữ. Tên quốc gia dùng cùng màu chữ/viền tên nhân vật (cập nhật 06/11/2026 bên dưới); đội/cửa hàng và tiền tố chức vụ giữ kiểu chữ riêng.

`TopLevelNameTests.cpp` kiểm tra 199/200/201, player/NPC, 14 camp, vị trí sprite
25x18 trước tên và tiền tố. Kiểm thử đạt; CoreClient profile Dev build thành công
(0 lỗi). Sprite Loose khớp SHA256 với PAK gốc.

Đã sao lưu DLL cũ trong `analysis/level200_name_20261030/CoreClient.before.dll`
và publish duy nhất CoreClient.dll sang `PhongThanRuntime-Loose/Client`.
SHA256 mới: `AC4D93F028EE9177751C123DCD773B7CC3D2419BA75794920C599E49757937D3`.
Client mới PID 119888 báo LOOSE_ONLY_READY; GameServer PID 154544 tiếp tục chạy.
Không sửa Release. Chưa kiểm tra hình ảnh nhân vật sau đăng nhập bằng client mới.

## Sửa màu tên bang/quốc gia — 06/11/2026

Tên bang được vẽ trước nhánh chọn màu cấp 200, nên giữ màu camp và viền đen dù tên nhân vật bên dưới đã vàng nhạt/viền đỏ. Đã chuyển bước chọn palette lên trước cả hai lệnh vẽ và cho tên bang dùng cùng `dwColor`/`nameBorder`. Giữ điều kiện gốc cấp >=200/camp7, icon/vị trí, tiền tố chức vụ, tên đội/cửa hàng và logic gameplay.

Test có sẵn qua cho cấp199/200/201, người/NPC và 14 camp; CoreClient Dev build 0 lỗi/0 cảnh báo; SecureRelease build qua. Đã cập nhật riêng CoreClient.dll của Loose khi game đang đóng, có backup, kiểm tra import của Game/CoreClient và SHA đọc lại. Game.exe và CoreServer không đổi; không reset server. Chưa kiểm tra trực quan trong phiên game mới. Bằng chứng: `analysis/level200_clan_color_20261106/deployment.json` và các log build.

Bằng chứng và script kiểm tra nằm tại `analysis/level200_name_20261030/`
và `analysis/audit_level200_name.py`. Kết luận áp dụng bộ client/PAK đã kiểm tra.

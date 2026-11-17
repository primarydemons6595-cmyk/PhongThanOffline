# Studio, SQL và Bào Thương — 29/10/2026

Phạm vi: runtime `PhongThanRuntime-Loose`. Không thay Client_Original. Không xóa tài khoản/nhân vật thật trong khi triển khai.

## Xóa sạch tài khoản / nhân vật

Studio → Quản lý → Tài khoản / Nhân vật → tải tài khoản, tải nhân vật, chọn **Xóa sạch nhân vật** hoặc **Xóa sạch tài khoản**. Nếu đã xóa `.pthc` thủ công, nhập tên cũ để dọn dữ liệu phụ. Thoát client và dừng các dịch vụ game trước khi thực hiện để dữ liệu trong RAM không ghi lại file cũ.

Giao diện hiển thị danh sách file trước khi xóa; thao tác lưu bản sao trong `ManagerStudio/PhongThanStudio/Backups/Delete_*`. Dữ liệu SQL được lưu dưới dạng DPAPI `account.dpapi`, chỉ tài khoản Windows đã thực hiện thao tác giải mã được. Gồm file nhân vật, buff, hồ sơ, thư, magic companion hiện tại, dấu quản lý và thư mục dữ liệu cá nhân trong Client/UserData của nhân vật. `MissionMemory.dat`/`TaskTracking.ini` là nguyên nhân khiến sổ nhiệm vụ cũ còn xuất hiện dù file nhân vật mới.

Xóa toàn tài khoản dọn cả thư mục lưu của tài khoản, không đụng tài khoản khác. Tham chiếu bạn bè và phiếu nhận thưởng thế giới được dọn; dữ liệu quốc gia/đấu giá dùng schema/checksum thật. Quốc chủ hoặc nhân vật còn ký quỹ/đồ/tiền đấu giá chưa tất toán bị từ chối xóa để tránh làm mất tài sản bên khác. Nhật ký, lịch sử giao dịch và backup ngoài kho hoạt động được giữ phục vụ khôi phục. Client mới tự xóa riêng nhật ký/theo dõi cũ khi server xác nhận tạo nhân vật thành công, kể cả tạo lại cùng tên; không xóa khi đăng nhập nhân vật có sẵn. Studio không thể dọn cache từ xa trên client cũ chưa được cập nhật.

## SQL hiện tại

Đã loại 18 bảng web/CMS, bảng tài khoản bản sao; 21 cột không dùng; 39 module diagram/source-control và trigger cũ `drop_table` chỉ có tác dụng rollback DROP_TABLE. Giữ:

| Bảng / view | Nội dung |
|---|---|
| Account_Info | 10 cột đăng nhập, mật khẩu giao thức, trạng thái khóa/phiên và thời điểm |
| Account_Habitus | 8 cột thời lượng và nExtPoint mà AccountServer còn truy vấn |
| ServerList | 7 cột định danh và kết nối dịch vụ |
| View_AccountMoney | View tương thích các truy vấn đăng nhập/đăng xuất hiện tại |

14 tài khoản, 14 bản ghi Habitus, 4 bản ghi server được giữ nguyên. Không đổi ID, mật khẩu, số dư hoặc định dạng nhân vật. Full backup có checksum và RESTORE VERIFYONLY trước migration; đối chiếu số dòng/checksum các trường còn dùng trong cùng transaction; DBCC CHECKDB đạt sau commit. Công cụ lặp lại: `Deploy/Migrate-PhongThanAccountSchema.ps1`; mặc định chỉ tạo kế hoạch, `-Apply` mới sửa DB. Bằng chứng/bản sao live: `analysis/bot_aigent_20261029/sql_live/`.

## Bào Thương

Đủ 9 Thương Điếm: Sùng Thành Doanh, Ngọc Hư Cung, Xi Vưu Mộ, Triều Ca, Tây Kỳ, Sa Mạc Phong Thần, Đông Hải Hải Câu, Hiên Viên Động 3, Đại Phong Băng Xuyên. Bổ sung bốn điểm ngoài thành; giữ NPC khác và geometry/trap/object của các region. Giữ 27 hàng hóa và giá/trọng lượng của bảng gốc đang cài. Nhiệm vụ chỉ chọn hàng ở điểm khác, phù hợp cấp 40/50/60/70 cho các vùng mở rộng; không đổi số lượt/phí của bản đang dùng.

Nguồn đối chiếu: https://pt.vnggames.com/su-kien/huong-dan-tan-thu-cum-2008/bao-thuong.html. Triều Ca giữ tọa độ thực tế `(211,198)`; trang hướng dẫn ghi 211 ở phần đầu nhưng 221 trong bảng cuối. Các điểm mới được đặt trên ô đi được trong tọa độ hiển thị của nguồn.

Nhận nhiệm vụ rồi mở F11, chọn Bào Thương: **Đến Thương Điếm** / **Về giao hàng**. Có liên kết theo dõi ở cuối bảng tracker khi theo dõi Bào Thương. Nếu đã đủ hàng, liên kết tracker chọn nơi giao; nếu thiếu hàng, chọn shop bán đúng hàng. Các giá trị nhiệm vụ 333–335 và hàng hóa 110–119 được đồng bộ khi thay đổi và khi đăng nhập.

Đi xuyên map giữ đích NPC riêng, không dùng tọa độ trung tâm map làm đích. Chặng cổng có bán kính dừng 8 MPS để thực sự vào ô trap; nếu chặng dừng mà chưa chuyển map thì tiếp tục đi vào cổng. Chỉ kết thúc ở map đích và trong 128 MPS quanh NPC. Space hoặc thao tác đi tay hủy tìm đường. Link này cần Game.exe/CoreClient.dll mới; bot không cần client.

## BotAIgent — phần đã triển khai và giới hạn

Thay tab bản nháp “Bot giả người chơi” bằng BotAIgent. Studio có thêm/cấu hình, bật/tắt, lưu trữ bot và định vị trên Map Editor. Event Service có tab BotAIgent, console, lọc, dừng cuộn, xuất log và điều khiển hệ thống. Có thể mở thêm cửa sổ Event để theo dõi host đang chạy; đóng cửa sổ theo dõi không dừng host.

Bot dùng KPlayer thật trên server, không mở Game.exe, không dùng socket giả đăng nhập. Nạp các phần nhân vật bằng loader hiện có; lưu riêng `Server/BotAIgent/characters/<id>.pthc`; companion vẫn theo account bot trong CharacterStore. Cấu hình CAS có revision, lease 10 giây, tối đa 32 hồ sơ. Khi mất lease, bot được lưu rồi rời world; lỗi lưu giữ bot để tránh mất trạng thái. Xóa hiện là dừng/lưu trữ, giữ bản nhân vật để khôi phục.

Đã nối chế độ nghỉ/train cơ bản, gọi pickup theo kiểm tra sở hữu/khoảng cách của server, trả lời riêng bằng câu mẫu theo trạng thái thật, và hook ký ức người đánh chết bot vào file INI theo account/nhân vật/thế hệ. **Chưa hoàn thành bot tự làm toàn bộ nhiệm vụ ngày, party VTT/boss, tự chọn trang bị, thương lượng/chốt giao dịch, kinh tế/bày bán hoặc hội thoại AI local.** Bật AI local hiện bị từ chối rõ ràng; không giả trạng thái đã tích hợp model. Chat mẫu hiện hỗ trợ từ khóa tiếng Việt không dấu và phản hồi ngắn; chưa đạt hội thoại tự do như người thật. Những phần này cần các adapter và kiểm thử riêng.

## Bằng chứng

- 24 kiểm tra core: xóa, rollback, CAS, danh tính, bộ nhớ/quyết định mẫu, không làm sống lại dữ liệu cũ khi mất phản hồi commit SQL.
- 20 kiểm tra API HTTP: xác thực, tạo/lưu trữ bot, không báo online khi chưa có native feed.
- Migration đã chạy trên DB phục hồi riêng trước live; kiểm tra công cụ Studio đăng ký/xóa trên schema gọn, không còn dòng dư và có backup DPAPI.
- 1.854 kiểm tra Lua 4 cho 9 shop, điều kiện cấp, lượng hàng, vị trí, callback và thưởng lặp; 61 kiểm tra dữ liệu NPC/geometry/đường liên map.
- Kiểm tra live: 9/9 NPC đúng map/tọa độ, không trùng; bot thật spawn → stop/save → reload thành công; bot thử được lưu trữ, hệ thống bot tắt sau kiểm tra.
- Baseline item 19, protocol 49, registry kỹ năng đạt. Studio/Event build không lỗi/cảnh báo. Native build đạt; warning VC6 lớn về kích thước image/metadata được ghi trong log.
- Ảnh render WPF: `analysis/bot_aigent_20261029/{bots,account-deletion,event-bots}.png`.

Chưa thực hiện hành trình Bào Thương bằng nhân vật thật trong client hoặc kiểm tra trực quan giao diện F11 trong game. Kiểm tra graph/Lua/live NPC không thay thế lượt đi thực tế. File bằng chứng chính: `live_commerce_verification.json`, `live_bot_test.json`, `commerce_validation.json`, `publication.json`, `final_publication.json` trong cùng thư mục analysis.

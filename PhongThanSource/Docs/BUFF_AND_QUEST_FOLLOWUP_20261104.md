# Tiếp tục bộ nhiệm vụ và sáu lỗi — ghi chép cũ

> Cập nhật 05/11/2026: bộ sửa và nhiệm vụ đã được nạp; buff vật phẩm đã được kiểm tra trực tiếp trong game ở lượt trước. Các dòng “chưa nạp” bên dưới là lịch sử triển khai. Xem [báo cáo hiện tại](STUDIO_QUESTS_AND_FIXES_20261105.md).

## Kiểm chứng nhiệm vụ

- `StudioQuestChecks` kiểm tra schema, roundtrip, điều kiện phái, bước, NPC có sẵn không sửa region/script gốc; NPC mới thêm đúng một bản ghi và giữ bytes tên TCVN3. Xem `analysis/tet_event_shop_20261103/quest-checks-final.log`.
- Runtime production + transaction đã qua `StudioQuestNative.generated.cpp`; Lua sinh ra đã chạy qua VM thật (`studio-quest-lua-tests.log`). Không đưa fixture lên server.
- Preview dùng `spr/phongthan/dialog/dialog.spr` và `answer.spr` native; chi tiết bước đã hiện; reset cache theo workspace.
- Studio và runtime nhiệm vụ vẫn chưa được cài lên live. Wallet decimal ASCII đã được cài trước đó (receipt riêng).

## Người dùng báo tiếp về buff

Ngọc Tán, Lâm Tiên Lộ và Tiểu Thiên Hương Tục Mệnh Lộ có trừ item, tăng hiệu lực nhưng không có icon. Đã dùng computer-use chụp cửa sổ GameMa tại Diêu Trì: chỉ thấy một icon item trên cùng và ba icon skill dưới avatar.

Đã đọc sidecar nhân vật ở chế độ chỉ đọc: buff 16/19/22 và 330 còn khoảng 2.300 giây, nhiều buff khác còn chạy. Ảnh hầu như có đủ (audit 6.196/6.197 ở config lớn); không được kết luận ảnh tồn tại là UI đã hoạt động. `ConsumableCatalogTests.exe` dùng KTabFile thật đã nạp được 82 định nghĩa trên server live, gồm Ngọc Tán 14–27, Lâm Tiên 330/782 và Thiên Hương 334.

Các thay đổi đã biên dịch, **chưa nạp**, chưa chứng minh đã sửa hết lỗi icon trong game:

- Vitals không kèm snapshot buff không được xóa timer đã nhận.
- Bỏ gọi nhận snapshot hai lần trong cùng một packet.
- Nạp lại scheme icon khi scheme thanh điều khiển nạp lại.
- Lọc icon skill: không hiện nội tại/passive; timed active hoặc aura thực sự đang bật mới được hiện. Trạng thái không thời hạn được bật thật ghi “Đang bật”, không gán thời gian giả.
- Bổ sung probe theo thay đổi danh sách, không log mỗi frame: query result/size/active/detail/time trong `pt_consumable_ui_trace.log`.

Kiểm tra: `consumable-followup-tests.log` 400 server + 18 client checks; `consumable-catalog-test.log`; `buff-visibility-tests.log`; `buff-client-native-build.log` CoreClient/GameClient exit 0.

Đã hỏi người dùng thoát game để thay Game.exe/CoreClient.dll. Người dùng **chưa trả lời đã thoát**, chỉ hỏi tiếp về icon skill tự có và `--`; đã giải thích, sửa bộ lọc. Game PID 219124 còn mở tại lần kiểm tra cuối. Không ép tắt, không dùng item hoặc tiêu tiền trên nhân vật thật. Có thể cần bật tạm Client Diagnostics.ini (hiện Enabled=0) trước lần mở lại để kiểm tra probe, sao lưu và khôi phục thiết lập sau kiểm tra. Chính sách diagnostics được cache khi process chạy.

## Các thay đổi sáu lỗi đang chuẩn bị

- Thuê kho: bỏ gate gần treasury trong PTF4Process, LuaBankInfo và LuaBankPurchase; vẫn giữ actor sống, phiên đã nạp, khóa/trade/điều kiện/tiền/thời hạn. Chưa nạp. Test tích hợp rental cần cập nhật stub Name/ActionScript và thêm ca xa mọi treasury.
- UiInformation2: font 16, nội dung 22/40, 300×145, bấm tiếp từng 8 dòng trước xác nhận callback; sửa sao chép phần queue còn lại từ đúng offset; chưa test native render các trang dài. UiInformation text buffer 256→512.
- Hồi sinh: giữ UI gốc; nội dung bốn dòng gọn, tên lựa chọn xanh/cảnh báo đỏ/chi phí vàng, font16; cần kiểm tra ảnh native và đảm bảo text không vượt khung.
- Dialog `UiMsgSel`/Info1 còn chưa tăng font/căn toàn bộ, chưa xong mục 1.
- Tạp Hóa: đã thấy `RefreshPhongThanSetAttrib` tự áp catalog set vào vài hàng source SetID trống (part310/311, ví dụ Liệp Ưng/Kiêu Dũng). **Chưa sửa**: cần phân biệt hàng trắng đúng gốc; tránh thay đổi đồ boss/đồ đang có hoặc thêm marker tùy tiện.
- Thông báo PvP trong bang + bản địa: **chưa sửa**. Native public channels hiện Trade/Area/Nation/Friends/World; `CH_FACTION` là Quốc `\\O`, không phải Bang. Cần kiểm tra `CH_TONG`/route gốc trước khi nối, không gửi nhầm guild sang quốc. `PhongThanSendTongMessage` hiện gửi system đến thành viên, chưa phải channel Bang. Hook damage ở KNpc::CalcDamage (nDamage>0), death authoritative trong DoDeath/DeathPunish; cần chống spam, loại spar/arena.

## Live không thay đổi trong lượt này

Hashes client/server vẫn như receipt F2 trước. World người dùng đã khởi động lại bằng Studio: GameServer239980/Bishop238796 (console.json), không phải PID trong receipt F2 cũ. Source Output hiện có quest/UI WIP mới; không tự chép cả Output lên live. Chưa sửa event data, tiền, password, trang bị hoặc buff thời hạn của nhân vật thật.

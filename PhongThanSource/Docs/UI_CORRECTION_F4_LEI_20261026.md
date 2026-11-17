# Sửa lại bố cục minimap, ESC, bản đồ khu vực và F4 — 26/10/2026

## Nguyên nhân và thay đổi

Đợt trước đã sửa sai điểm neo cột icon trái minimap từ `x-20` thành `x-30`, tăng pitch lên 26 pixel và mở footprint từ 20×137 lên 30×182. Điều này làm cột tách khỏi khung minimap và dài xuống dưới. Bản sửa trả điểm neo về `x-20,y+7`, trả Left/Top của từng control về INI gốc, giữ footprint gốc 20×137. Không dùng một hàng tọa độ tự sinh cho tất cả nút.

Nút có nhãn số 1 mở menu Shield bốn dòng theo dữ liệu gốc: ẩn thú nuôi; ẩn người chơi trừ đội; ẩn tất cả người chơi; hủy ẩn. Nút này tách khỏi grid `HideSetting`. Cách dựng nút dùng SPR mũi tên ẩn gốc và chữ số xanh; không có một SPR đầy đủ mang tên ShieldOption trong dữ liệu đã tra. Menu giữ chữ đỏ/xanh và các ô màu như menu PK gốc. Các dòng chỉ điều khiển hiển thị phía client; không cấp buff ẩn thân, không đổi PK hoặc thao tác server lên người chơi khác.

Quy tắc hiển thị dùng chung cho thân, tên, thanh máu và viền chọn: tự thân không bị ẩn; NPC nhiệm vụ không bị ẩn; chế độ trừ đội so sánh hai team ID hợp lệ; dòng ẩn thú áp dụng Linh thú đã đồng bộ PetStyle và thú triệu hồi có owner registry. Hủy ẩn bỏ bộ lọc.

Icon kiểm tra chiến trường `BtnArenaQuery` được ẩn khỏi minimap thường. Đây là nút đã được nạp nhầm vào giao diện phổ thông, không phải con trỏ tìm đường. Bốn mũi tên gốc hiện khi chuột nằm trên vùng bản đồ hoặc chính nút mũi tên, ẩn khi ra ngoài. Nút click/giữ gửi bước cuộn tới `MapScroll`, dùng scale/giới hạn focus sẵn có; không đặt cờ hoặc di chuyển nhân vật. Giữ cơ chế đưa bản đồ trở về vị trí sau khoảng chờ sẵn có.

Hộp ESC trước đây tính trung tâm theo 360×186 nhưng các nút 240 pixel còn nằm ở Left=28, lệch khỏi tâm khung. Khung mới 296×156, bốn nút không co ảnh/chữ, cùng Left=28 và Top=20/47/74/101. Khung SPR gốc được vẽ thu gọn; nút và nội dung giữ nguyên kích thước. Tâm nút trùng tâm khung, khung căn theo toàn canvas hiện tại.

## Bản đồ thế giới

Đã nối 32 chấm xanh bằng SPR gốc. Click mở ảnh bản đồ khu vực tương ứng, đúng lựa chọn người dùng, không dịch chuyển/tìm đường. Có nút về bản đồ thế giới; ESC/chuột phải trong khu vực trở về bản đồ thế giới, ESC/chuột phải ở bản đồ thế giới đóng cửa sổ. Click nền giấy không còn tự đóng.

Ảnh khu vực lấy từ 32 `maps/<map>24.jpg` đang có hiệu lực trong Client_Original. Chuyển sang SPR true-color để dùng renderer UI hiện có; giữ nguyên toàn bộ pixel JPEG sau giải mã, không đổi màu hoặc resize file nguồn. Khi vẽ, ảnh chi tiết được fit giữ tỉ lệ trong khung. Không đổi scene/map của nhân vật, không thay minimap đang chạy. Dấu chấm được bố trí trên canvas Phong Thần 800×600 theo hình tham chiếu; đây là tọa độ UI phục dựng, không gọi là script click nguyên bản đã khôi phục.

## Đổi trang bị F4

Khôi phục INI gốc `ui/ui3/一键换装.ini`, ID `1A7D5540`, và các SPR của cửa sổ cùng icon `ChangeEquipSet` có sẵn trong UiItem.ini. Nút F4 mở cửa sổ 10 bộ, có chọn bộ/lưu/đổi/xóa, preview 11 ô theo giao diện gốc. Mỗi nhân vật lưu serial item riêng trong `EquipSets.ini` ở thư mục dữ liệu riêng của nhân vật, không lưu local item index. Ctrl+1…9/0 chọn và đổi bộ 1…10.

Đổi đồ dùng lệnh `GOI_SWITCH_OBJECT` và xác nhận trang bị từ server đang có; không sửa item/thuộc tính cục bộ. Trước khi bắt đầu, mọi món cần đổi phải đang đeo đúng ô hoặc còn trong một trong bốn túi mang theo. Thiếu món thì dừng trước khi gửi. Chỉ gửi một món mỗi lần, chờ snapshot xác nhận serial đúng ô rồi mới gửi món tiếp theo. Đổi nhân vật, đang giao dịch, đang cầm đồ hoặc quá hạn chờ 4 giây sẽ dừng; không tự gửi lặp, không báo đã thành công khi server chưa xác nhận. Món hết hạn/không đủ điều kiện/vướng ô túi vẫn chịu kiểm tra của server.

Đây là chuỗi giao dịch thay từng món, **không phải giao dịch nguyên bộ nguyên tử**. Nếu món sau bị server từ chối, các món trước đã được server chấp nhận vẫn giữ nguyên. Ô không được lưu trong bộ không bị tháo tự động. Trang bị ấn mới ngoài 11 ô giao diện gốc chưa được thêm vào preset.

## Lôi Chấn Tử tại Diêu Trì

Đã tìm được đúng tài nguyên `\script\瑶池\雷震子.lua`, ID `C821F1CD`. Theo thứ tự nạp trong `Client_Original/package.ini`, bản thắng ưu tiên là **Script.pak, 292 byte**, SHA256 `bcbecccfb1464386ea04c9db7934b8ade1ec77e6482ec2ae29117c353e52173d`. Kiểm tra mọi entry cùng ID trong bộ PAK chỉ thấy một bản này.

File chỉ có `main()` gọi lời chào về Chu Văn Vương/Vương Tử Thiên Ất và `no()` đóng thoại. Nó **không chứa bốn chức năng trong ảnh người dùng**. Không thay lời chào bằng bốn callback tự tạo rồi gọi đó là script gốc.

Sau khi kiểm tra PAK mới và thao tác client gốc, người dùng đã yêu cầu phục dựng nhánh theo quy tắc hiện có để test. Phạm vi implementation, ảnh tham khảo, kiểm thử và trạng thái áp dụng được ghi riêng tại [LEI_SOCKET_RESTORE_20261026.md](LEI_SOCKET_RESTORE_20261026.md). Phần phục dựng không làm thay đổi kết luận về nguồn Lua gốc ở trên.

Quét bổ sung 38.198 payload hiện có hiệu lực, loại Maps/Spr/Music/Sound/Font và giới hạn payload ≤250 KB; phát hiện 1.822 payload Lua, không có các chuỗi menu đã tra. Có 13 payload không giải mã được; kết quả này không chứng minh tuyệt đối rằng mọi biến thể server gốc đều không tồn tại. Phần bốn chức năng hiện chưa được phục hồi từ script gốc. Ảnh xác nhận **bốn nút**, không phải ba. Không sửa NPC Lôi Chấn Tử hoặc tự thêm script server trong gói UI này.

## Kiểm tra và phát hành

### Rà PAK cập nhật và thao tác client gốc bổ sung ngày 26/10

Quét bổ sung toàn bộ 110.175 entry trong 21 PAK hiện có theo package.ini, kể cả bản bị ghi đè, không áp giới hạn text 250 KB của lần quét trước. Nhận diện 1.949 payload Lua; các entry dạng frame SPR không được coi là script. Không lỗi giải mã entry method 0/1. `vng00.pak` và `update0_171-171.pak` không có payload Lua thay thế NPC này. Ba PAK được package.ini nhắc tới nhưng không có trên máy là `resupd.pak`, `resupd1.pak`, `Resource.pak`; kết luận chỉ áp dụng bộ client đang có.

Bảng ánh xạ trong vng00 còn chỉ đường dẫn mới `\script\yao_chi\lei_zhen_zi.lua` (ID `B89373F6`); kiểm tra mọi entry không tìm thấy payload của đường dẫn đó. Đường dẫn Trung văn ID `C821F1CD` vẫn chỉ có bản lời chào 292 byte từ Script.pak. Tuy nhiên vng00 có các bảng gốc `settings/item/da_kong_xiang_qian/` về giới hạn đục lỗ, yêu cầu nguyên liệu và quy tắc khảm, cùng mô tả Di Quang kính/Huyền Sắc Thủy Ngân nhắc đúng NPC. Đây là dữ liệu gốc tìm được, không phải file Lua điều khiển NPC.

Đã thao tác cửa sổ **Client_Original**, nhân vật ProKenDy tại Diêu Trì, chọn đúng Lôi Chấn Tử. Hội thoại thực tế có **bốn nút**; nhánh đầu Đục lỗ–Khảm nạm trang trí mở tiếp menu năm nút, rồi mở cửa sổ **Ghép** với tám vị trí vật phẩm quanh bát quái, các vị trí nguyên liệu, phí và nút Ghép/Ghép hết. Chỉ mở cửa sổ trống và đóng lại, không đặt vật phẩm hoặc thực hiện ghép. Đã bỏ bộ lọc ẩn người chơi tạm dùng để chọn NPC.

Kiểm tra metadata `.lua/.pak/.log` trước và sau các hội thoại không thấy file mới hoặc thay đổi. Điều này **không chứng minh giao thức server gốc tuyệt đối không gửi mã**, vì chưa phân tích lưu lượng hoặc bộ nhớ client. Kết quả hiện tại là đã xác nhận UI/nhánh hội thoại thật, **chưa lấy được Lua server gốc**. Không suy diễn các struct giao thức Loose thành bằng chứng giao thức chính thức; không thêm callback phục dựng vào NPC trong bước kiểm tra này.

Bằng chứng: `analysis/lei_latest_paks_20261026/audit.json`, `focused_evidence.json`, `mapped_script_check.json`, `client_before_npc_click.json`, `client_after_dialogue.json`. Không thay binary, Lua hoặc restart server trong đợt rà này.

- GameClient/CoreClient Release build thành công.
- 136 kiểm tra bằng các hàm click/hover/SelectMap trích từ source cuối và quy tắc ẩn hiển thị.
- 18 kiểm tra bằng implementation preset thật: INI theo nhân vật, serial sau đổi index, thiếu đồ, giao dịch/cầm đồ, đổi nhân vật, chờ ack, timeout, bộ thứ 10 và xóa riêng bộ. API server/UI giả lập; không thao tác nhân vật thật.
- Kiểm tra phụ thuộc SPR, vị trí icon so INI gốc, 32 ảnh khu vực có pixel trùng JPEG gốc; đã xem bản dựng từ tài nguyên của ESC/cột nút/bản đồ chọn. Chưa chụp nghiệm thu các thao tác trong cửa sổ game thật.

Đã publish gói client `Deploy/ProjectContent/UiCorrectionF4_20261026`, **48/48 file đúng hash**. Game Loose đã đóng khi kiểm tra tiến trình trước thay. Không đổi CoreServer, cấu hình Event, Lua nhiệm vụ, dữ liệu nhân vật hoặc khởi động lại server; world giữ **PID 105212**, snapshot có 4.044 NPC/quái. Trạng thái xác nhận ở `analysis/ui_correction_20261026/publication.json` và `verification.json`. Có backup trước thay; bước thử phát hành đầu bị lỗi đối số đường dẫn File.Replace, kiểm tra mọi hash cho thấy chưa file runtime nào đổi; sửa publisher rồi phát hành thành công.

Cần mở lại Game Loose để nạp binary và UI mới. Chưa xác nhận thao tác trực tiếp bằng nhân vật sau cập nhật; không coi ảnh dựng tài nguyên hoặc fixture native là ảnh chụp nghiệm thu trong game.

Bằng chứng cùng thư mục analysis: `original_proofs.json`, `resource_verification.json`, `ui_tests.log`, `equip_tests.log`, `loichantu_proof.json`, `loichantu_all_versions.json`, `loichantu_all_active_scan.json`, các ảnh preview và manifest/backup triển khai.

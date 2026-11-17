# Cẩm Nang Sự Kiện — công cụ lấy đồ thử nghiệm

Vật phẩm dùng lại: tuple Lua `(6,1,5572)`, Level 1. Giữ icon cuộn trúc gốc, sửa tên thành **Cẩm Nang Sự Kiện** và nối script mới `script/phongthan/test/event_handbook.lua`. Không tìm thấy payload script cẩm nang cũ trong chuỗi PAK đã kiểm tra; đây là công cụ test theo yêu cầu, không phải phục dựng hội thoại nhiệm vụ.

## Cách lấy và sử dụng

Trong **Túi tân thủ → MagicScript (ID)**, nhập **5572**, xác nhận nhận vật phẩm rồi nhấn chuột phải vào cẩm nang trong F4. Túi test chỉ cấp tự động cho các tài khoản trong cấu hình test hiện hữu. Không tự cấp cẩm nang vào nhân vật đang có.

- Nhận cấp 200; tăng qua luồng tăng cấp, sử dụng bảng tăng trưởng tương ứng, không xóa kỹ năng đã học.
- **Học tối đa kỹ năng theo hệ phái**: danh sách lấy từ 93 sách kỹ năng người chơi và `Skills.txt`, chỉ học đúng phái và trần cấp của server. Không cấp chiêu phụ/NPC theo một khoảng ID phỏng đoán. Kiểm tra dữ liệu và chỗ trống danh sách trước khi thay đổi; gọi lại không thêm trùng.
- Xu x100 dùng questkey ID **47**; Nguyên Bảo x1 dùng ID **46**.
- Tiền vạn: nhập 1–1000 vạn mỗi lần; kiểm tra tràn số và kết quả cộng tiền.
- Trang bị Trắng/Lục/Cam/Tiên Ma, chia phái, bộ phận và cấp yêu cầu; các mốc chục có nhãn 1x, 2x, 4x… theo dữ liệu thực tế.
- Vũ khí Trắng/Hoàng Kim; nguyên liệu nâng cấp/khảm/đục lỗ; nguyên liệu ép cam; Pháp Bảo/Ngọc Bội/Pháp Khí; nguyên liệu linh thú; thú cưỡi theo phái.
- Rê tên vật phẩm để xem icon ở bên phải. Danh sách có phân trang và cuộn; dùng nút Kết thúc gốc. Layout và giới hạn chuỗi mở rộng chỉ kích hoạt với key ảnh cẩm nang, không đổi kiểu hội thoại NPC khác.

## Danh mục và cấp vật phẩm

Danh mục có **16.255 lựa chọn mẫu**, không phải 16.255 tên độc nhất. Tên lặp có thể khác cấp/dòng bảng/recipe. Nhãn có tuple và bậc; server giữ whitelist ID → tuple/dòng mẫu, không nhận tuple tùy ý từ client.

Trang bị cấp đúng dòng template; đồ cam áp dụng recipe gốc, không chỉ đổi tên/màu. Cấp đồ qua giao dịch kho có rollback khi không đủ chỗ. Trang bị nhận từng món; nguyên liệu có lựa chọn 1/10/100, còn chịu giới hạn túi và stack thực tế.

Hai bảng gốc trước đó thiếu trong registry Loose đã được nối: `instrument.txt` (380 mẫu, detail 11) và `jadependant.txt` (51 mẫu, detail 12). Đọc thuộc tính/yêu cầu theo tên cột, không áp layout 70 cột lên bảng Pháp Khí 77 cột. Pháp Khí vào ô Instrument, Ngọc Bội vào ô JadePendant. Không dùng bảng nhẫn/vòng tay JX để thay hai loại này.

## Vũ Sĩ

Mã phái chính thức trong bộ dữ liệu là **3**, được đăng ký riêng thành `vusi` / Vũ Sĩ; không dùng tên Hệ Hỏa. Cho phép lưu, liệt kê và nạp nhân vật phái 3; giao diện tạo nhân vật vẫn chỉ có ba phái ban đầu.

Nút đổi chỉ hiện với Giáp Sĩ và được kiểm tra lại ở server. Phải tháo trang bị trước. Giữ cấp/chỉ số/nhiệm vụ/vật phẩm; bỏ kỹ năng Giáp Sĩ, thêm đánh thường Vũ Sĩ ID 1691 từ `playerskills.ini`, rồi dùng nút học tối đa cho phái mới. Bảng tăng trưởng thường 200 dòng và Tiên Ma 90 dòng lấy từ dữ liệu Vũ Sĩ gốc. Không tự chế profile newplayerini06/07.

Studio nhận diện rõ Vũ Sĩ (3), giữ nguyên byte phái khi sửa/lưu nhân vật, không ép ảnh nền sang Dị Nhân. Chức năng bù tiềm năng dùng mốc chuyển phái lưu tại engine task 4894: chỉ số nền Giáp Sĩ tới mốc chuyển và tăng trưởng Vũ Sĩ từ sau mốc đó. Nếu thiếu lịch sử, vẫn cho sửa/lưu thủ công nhưng không tự suy đoán để bù chỉ số. Pháp Khí/Ngọc Bội trong editor cũng đọc đúng bảng mới.

Cập nhật phái bằng bản tin native gắn MapId/EntityId (player event 117), không gửi lại self snapshot làm xóa hiển thị kho. Bản tin kiểm tra phạm vi phái, kích thước và chiều server→client. Đây là nhánh đổi phái phục vụ test; không bổ sung chuỗi nhiệm vụ Vũ Sĩ hay các dịch vụ tẩy điểm ngoài yêu cầu cẩm nang.

## Kiểm chứng

- Build Release: Game, CoreClient, CoreServer, GameServer, Goddess, Bishop, AccountServer, Relay.
- Lua 4 thực tế: 2.675 trang menu, callback/ảnh, thao tác max skill, nhập tiền.
- Max skill: cách ly bốn phái, trần cấp, lặp an toàn, thiếu chỗ/thiếu script không thay đổi một phần.
- CharacterStore thực tế: lưu/liệt kê/nạp phái 3; wire snapshot và cập nhật phái, từ chối phái ngoài phạm vi.
- Reader thuộc tính production: đủ 431 dòng Pháp Khí/Ngọc Bội, kiểm tra dịch cột yêu cầu.
- Loader SPR native: **676 file, 676 frame, 0 lỗi**. Icon preview và icon F4 mới có binding trong loose index; overlay giữ binding khi rebuild index.
- Studio: fixture riêng kiểm tra phái 3, từ chối đoán lịch sử còn thiếu, sửa tiền/lưu/nạp lại giữ nguyên phái và mốc chuyển; bù chỉ số lặp lại không cộng trùng. Không sửa nhân vật thật trong kiểm thử.

Gói: `Deploy/ProjectContent/EventHandbook20261026`. Biên nhận và bằng chứng: `analysis/event_handbook_20261026`. Chưa thay thế việc người dùng thử thao tác trực tiếp trong game.

## Hotfix d?ng l?i C?m Nang v? ? k? n?ng NPC tr?ng

?? ?p d?ng `EventHandbookReuse20261026`: CoreServer b? vi?c t? tr? MagicScript ri?ng ID 5572 sau khi m? menu. B?n tr??c b? s?t nh?nh tr? v?t ph?m trong `KItemList::NowEatItem`; C?m Nang b? x?a khi?n callback kh?ng c?n ?? ?i?u ki?n s? h?u n?n kh?ng nh?n ???c. C?n l?y l?i C?m Nang ?? b? tr? qua T?i t?n th?, MagicScript ID 5572; kh?ng s?a tr?c ti?p kho nh?n v?t.

L?i `standard.lua` / `nBegin` nil do engine g?i c?ng th?c cho c?c ? c?p k? n?ng r?ng, k? c? slot kh?ng c? k? n?ng. B?ng NPC c? 1.524 ? r?ng trong c?c m?u d?ng script n?y. ?? ??i chi?u script g?c `serverlist.pak`, entry 1227; kh?ng s?a script hay c?ng th?c g?c. Engine b? g?i Lua khi slot kh?ng c? k? n?ng ho?c tham s? r?ng, gi? fallback s? 0 cho ? r?ng.

Ki?m th? ch?y nh?nh script dispatch production: m? s?ch hai l?n, gi? quy?n s? h?u cho callback, kh?ng tr? khi script l?i; c?c v?t ph?m d?ng m?t l?n v? ngo?i l? c? gi? h?nh vi. Ki?m th? dispatch NPC v?i Lua 4 v? script g?c: b? ? tr?ng/slot kh?ng d?ng, gi? c?ng th?c c?p h?p l?. Sau restart: 105 map, 4.044 NPC/qu?i, database/relay/gateway k?t n?i; kh?ng c?n l?i nBegin trong console kh?i ??ng m?i. Hash DLL ??ng g?i; h? s? nh?n v?t kh?ng ??i. Bi?n nh?n: `analysis/event_handbook_reuse_20261026`. Ch?a ki?m th? nh?n ?? b?ng thao t?c tr?c ti?p trong Game.

## Hotfix ? nh?p ti?n v?n

?? s?a l?i g?i InputDialog: tham s? th? hai l? lo?i nh?p s? `2`, kh?ng ph?i gi?i h?n `1000`. Callback EBMoneyDone nh?n s? t? tham s? native, kh?ng ??c GetInput c?a lu?ng c?. Gi?i h?n 1?1000 v?n v?n ???c ki?m tra b?i server. Ki?m th? Lua 4 ?? 2.675 menu v? callback s? 37; reload ri?ng script tr?n world PID 123804 ho?n t?t 1/1, failed=0, kh?ng restart. B?ng ch?ng: analysis/event_handbook_money_20261026.

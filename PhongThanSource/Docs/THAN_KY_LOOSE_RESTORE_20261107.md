# Thần Kỹ — bản Loose, 07/11/2026

> Cập nhật sau: sáu nhóm còn thiếu bên dưới đã được nối bằng công thức **Loose thử nghiệm do người dùng cho phép**, có GUI trong Studio. Xem [bản cập nhật mới](THAN_KY_EXPERIMENT_20261107.md). Nội dung dưới đây ghi lại đợt phục hồi chín nhóm ban đầu.

Đã cài vào `PhongThanRuntime-Loose`. Không build hay cập nhật bộ runtime Release. Biên bản: `analysis/boss_skill_ui_20261107/installation.json`; sao lưu trước cài: `backup-20261107-174937` cùng thư mục.

## Cẩm Nang Sự Kiện và NPC

- Thêm **Thần Kỹ → Mảnh Thần Kỹ / Sách Thần Kỹ / Nguyên liệu**, tổng 71 mục. Giữ nguyên 28.361 số callback cũ, chỉ nối thêm 69 định danh còn thiếu. Có biểu tượng SPR gốc. Hai biến thể Linh Thạch cũ được ghi rõ để phân biệt với Linh Thạch nguyên liệu dùng đổi/kích hoạt.
- Các sách mới chưa có công thức chiến đấu được ghi **“chưa nối”**; dùng sách chỉ thông báo, không trừ sách hoặc Linh Thạch. Có vật phẩm trong danh mục không đồng nghĩa mọi chiêu của nó đã phục hồi.
- Thêm **Hồ Lô Đằng tại Ngọc Hư Cung (207,207), map 1003**, template 2711; dùng đúng tài nguyên hồ lô xanh gốc đã giải mã đối chiếu hình VNG. Mẫu NPC thân thiện và hội thoại là phần tích hợp Loose.
- NPC có 11 lựa chọn đổi sách thuộc chín nhóm đã nối; kiểm tra cấp 140, hệ phái, bản đồ, khoảng cách và đúng script NPC. Hình Thiên dùng 2/4/6 mảnh theo sách; các nhóm còn lại dùng 2 mảnh. Mọi công thức thêm 1 Đa Bảo Lệnh Bài và 1.000 Linh Thạch. Đã kiểm tra trực tiếp ô HTML `rowspan="11"`, không suy từ bảng văn bản bị làm phẳng.
- Đổi sách là giao dịch native: kiểm đủ đầu vào, tạo đầu ra, hoàn lại đúng vật phẩm/vị trí nếu lỗi. Nhận nguyên liệu từ bốn túi mang theo; không lấy đồ trong kho hoặc đang trang bị.

Nguồn vị trí, điều kiện, nguyên liệu và phí kích hoạt: [bài Thần Kỹ của VNG](https://pt.vnggames.com/su-kien/than-ky/noi-dung-1443.html).

## Thực thi đã nối

| Nhóm | Skill ID chính | Phần thực thi |
|---|---:|---|
| Khô Mộc Phùng Xuân | 1547 | Hồi máu theo mức cố định và phần máu thiếu, giới hạn đầy máu/nhịp hồi |
| Nam Minh Ly Hỏa | 1548 | Nhánh 1549/1550, hệ số sát thương người/quái và không kèm trạng thái xấu |
| Triệu Hồi | 1551 | Gọi thú đang thuộc quyền mình về; không sinh thêm thú |
| Xung Phong Hãm Trận | 1552 | Lướt có kiểm tra vật cản và lá chắn theo HP |
| Băng Hồn Tuyết Phách | 1553 | Tăng hồi phục động tác, trừ mana mỗi giây, tự dùng các buff đã học theo bảng gốc |
| Thiên Toàn Địa Chuyển | 1554 | Không trượt và đẩy lùi có kiểm tra vật cản |
| Hình Thiên Chiến Ý | 1556 | Chọn hình thú từ hai bảng gốc, thay kỹ năng đánh, HP theo trạng thái có thể đảo ngược; chặn gọi thú khi biến thân |
| Đoản Binh Giao Tiếp | 1557 | Thuộc tính tăng sát thương và sát thương thực theo điều kiện binh khí ngắn |
| Họa Địa Vi Lao | 1560 | Giữ chuỗi missile/event 1561–1563 và các thuộc tính đã có bộ xử lý |

Các con số lấy từ Lua/bảng gốc đã trích xuất, không viết lại các công thức Lua. Phần móc thực thi chiến đấu là mã Loose bổ sung, **không phải mã server VNG được tìm lại**. Ánh xạ ba sách Hình Thiên sang cấp 1/2/3 hiện là preset Loose, ghi rõ trong cấu hình; chưa có Lua dùng sách gốc để xác nhận ánh xạ này.

Theo yêu cầu người dùng: kích hoạt tốn 1 Linh Thạch; sách được giữ nguyên và chiêu chỉ tồn tại khi đúng quyển đã kích hoạt còn trong bốn túi. Nhận dạng theo serial, mã sách, cấp cấu hình và số lượng. Mất/đổi/nâng cấp sách thu hồi chiêu cũ; sách mới cần kích hoạt lại. Cùng nhóm dùng **một ID**, cập nhật cấp tại chỗ, không thêm hai lựa chọn Lv1/Lv2. Lưu dấu kích hoạt qua đăng xuất; không lưu chiêu thành học vĩnh viễn. Nhịp dọn danh sách tối đa một giây; lúc thi triển luôn kiểm lại điều kiện ngay.

Chọn chiêu qua bảng kỹ năng chuột trái/phải; không tự thêm Thần Kỹ vào các ô F5 cố định. Không sử dụng trong map Tiên/Ma theo bài hướng dẫn. Cấu hình sách: `Server/settings/phongthan/ThanKyBooks.ini`; các task 5000–5026 dành riêng native, nằm ngoài API task Lua.

## Kiểm chứng và phần còn thiếu

- 380 lần gọi tham số trên Lua gốc bằng Engine/Lua DLL thật: qua.
- Harness chạy **mã runtime đang build**, đọc kết quả Lua và bảng INI/tab thật: qua các đường hồi máu, hệ số, đẩy/lướt/lá chắn, gọi thú, tự buff, biến thân, 11 sách, thay cấp, thu hồi, đăng nhập lại và hoàn tiền. Thế giới/nhân vật/túi trong harness là adapter kiểm thử.
- Lua cũ thật: 50 script sách/mảnh, hai trang NPC, 11 xác nhận/đổi, phân trang Cẩm Nang và menu Tiên/Ma cũ: qua.
- Wrapper đổi sách và transaction native thật với túi kiểm thử: đủ 11 công thức, bốn túi, chống phát hai lần, hoàn tác khi lỗi, chặn sai cấp/phái/khoảng cách/script/kho: qua.
- CoreServer và CoreClient Dev build qua; sau cài, hai DLL nạp được và phân giải đủ phụ thuộc. Kiểm tra hash 101 tệp đã cài; 87 tệp CharacterStore và 41 binary Release giữ nguyên.
- **Chưa có phiên chiến đấu trong game để nghiệm thu**. Không diễn đạt các harness trên thành bằng chứng đã đánh thử đầy đủ trong game.

Sáu nhóm mới còn thiếu công thức gốc trong nguồn hiện có: Huyết Nguyệt Trảm, Huyền Ảnh Tàn Hoa, Phật Nộ Hỏa Liên, Lôi Hỏa Toàn/Tàn Phong, Truy Hồn Tán Hoa, Vạn Kiếm Quy Tông. Tài nguyên hình/bảng tên có thật nhưng không thay thế được thuật toán server. Không mở các công thức đổi/nâng cấp gây tốn nguyên liệu cho các chiêu này. Các script rơi đồ 11 boss và công việc bot/F3/F4/Avatar trước đó chưa được đánh dấu hoàn tất bởi bản cập nhật này.

Gói dữ liệu lưu tại `Deploy/ProjectContent/ThanKy20261107` kèm manifest/hash. Các công cụ kiểm thử, proof ảnh NPC, bản sao HTML và bộ cài có hoàn tác nằm trong `analysis/boss_skill_ui_20261107`.

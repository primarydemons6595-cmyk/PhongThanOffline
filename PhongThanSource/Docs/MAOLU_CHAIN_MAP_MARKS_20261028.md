# Chuỗi Mao Lư và trạng thái sách bản đồ — 28/10/2026

## Nội dung sửa

Bản QuestLifeDrop20261027 trước đó chỉ có lời mời mở đầu và kết thúc sau lần gặp đầu, không thưởng XP. Bản này thay bằng bốn lượt nhận thưởng, F11 theo dõi mục tiêu tiếp theo:

| Phái | Chặng 1 | Chặng 2 | Chặng 3 | Chặng 4 |
|---|---|---|---|---|
| Giáp Sĩ | Tạp Hóa (207,198) | Thủ Khố (202,198) | Lỗ Hùng (194,199) | Tạp Hóa (207,198) |
| Đạo Sĩ | Từ Hàng (206,196) | Tạp Hóa (216,201) | Thủ Khố (217,200) | Từ Hàng (206,196) |
| Dị Nhân | Thiếu Hạo (198,204) | Tạp Hóa (202,201) | Thủ Khố (197,201) | Thiếu Hạo (198,204) |

Tuyến Giáp Sĩ theo xác nhận trực tiếp của người dùng; tuyến Đạo/Dị theo mô tả người dùng và mục tiêu dictionary/localization gốc. Hội thoại hướng dẫn được viết lại bằng tiếng Việt; không tuyên bố từng câu là lời thoại gốc.

Nhân vật mới cấp 1 ở đúng tân thủ thôn tự hiện lời mời nhận nhiệm vụ khi login. Mỗi NPC có nút nhận thưởng; chỉ chuyển chặng và thưởng sau khi xác nhận. XP cơ bản theo bốn ngưỡng đầu của bảng runtime: 100/500/1100/1900. Với nhân vật cấp 1–4 có sẵn XP, thưởng ít nhất đủ phần còn thiếu để lên một cấp. Hành trình mới từ cấp 1 kết thúc ở cấp 5; nhân vật cũ từ cấp 5 trở lên chỉ nhận XP cơ bản của từng chặng, tránh thưởng một cấp cao tùy ý.

Dùng task 4960 phase 1..4 đang làm, 5 hoàn tất; version 4965. Nhân vật bị bản cũ kết thúc sớm phase 2 mà chưa nhận XP sẽ được đưa về chặng 1 khi login hoặc đối thoại; không sửa save trực tiếp. Không viết lại chủ tuyến Mao Lư cấp 25 Dị Nhân. Callback sách bản đồ chỉ đọc trạng thái; migration chạy ở login/đối thoại.

Bổ sung đúng hai NPC đang thiếu tại Ngọc Hư Cung: Tạp Hóa template 778 ở (216,201), Thủ Khố template 768 ở (217,200), dùng script gốc và lớp menu Mao Lư bên ngoài. Tọa độ có trong hyperlink dictionary gốc 49397–49399; không gán nhiệm vụ lên Sinh Hoạt Sư hoặc Tạp Thương Trương.

Sách Mini và Tab giữ kích thước 16×16, vẽ đủ 5 biến thể gốc: chưa nhận chính/phụ, đã nhận/đang làm, đủ điều kiện trả chính/phụ. Trước đó painter cố tình lọc chỉ variant 0/1 nên các trạng thái khác mất sách dù server đã gửi.

## Kiểm tra

CoreServer/CoreClient Release build qua. Lua 4 thật chạy 20 fixture/payload: ba phái, bốn chặng và lên cấp 1→5, F11, menu các NPC thật, sai phái/map, khóa/giao dịch, CAS thất bại, không thưởng lặp, quay lại sau login, migration bản lỗi cũ. XP/level trong fixture mô phỏng đúng nhánh DirectAddExp/LevelUp đã đọc trong source, không tạo nhân vật thử trên tài khoản người dùng.

QuestMapRenderTests chạy painter và cache mạng production: đủ 5 biến thể, Mini/Tab, tọa độ NPC xa, clipping, map hiện hành, cache hết hạn; vẫn 16×16. Hai NPC Ngọc Hư dùng loader SPR native thực: mỗi mẫu 40 frame đứng, không thiếu ảnh.

Gói `Deploy/ProjectContent/MaoLuChain20261028/manifest.json` gồm 12 file: 10 Lua, CoreServer.dll, CoreClient.dll. Bằng chứng ở `analysis/maolu_chain_20261028/`.

## Triển khai

Lượt thử đầu dừng trước file đầu tiên vì hash nhân vật GameMa thay đổi lúc world lưu/thoát. Toàn bộ 11 target khi đó còn đúng bản cũ; không rollback tiến trình nhân vật. Có `interrupted-publication-audit.json`; log lượt đầu trong `attempt01/`. Sau khi người dùng đóng Game, publisher chờ world cũ nạp xong để lấy snapshot mới rồi thực hiện sao lưu/cập nhật. Trạng thái cuối xem `publication.json` và `post_publish_verification.json`; không coi build/test là đã phát hành.

**Đã áp dụng:** 12/12 file đúng hash; world PID 128588, 4053 NPC/quái, 105 map; 9.242 script nạp thành công, failed=0. Hai dịch vụ Ngọc Hư 778/768 có đúng một bản tại tọa độ yêu cầu. 8 file nhân vật giữ nguyên hash trước/sau, dịch vụ tài khoản giữ PID. Chưa nghiệm thu thao tác bằng nhân vật thật sau phát hành.

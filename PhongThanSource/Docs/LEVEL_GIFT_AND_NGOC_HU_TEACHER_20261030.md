# Thẻ quà thăng cấp và Võ Sư Ngọc Hư Cung — Loose

## Phần thưởng

Thay 100 Xu trong Quà Tân Thủ ở Lễ Quan bằng **Thẻ Quà: Thăng Cấp Nhận Thưởng**, khóa mới `8.9700.2`. Giữ các trang bị, thú cưỡi và 100+100 bình thuốc của quà ban đầu. Không thu hồi Xu đã phát trước đây. Nhân vật đã nhận xong quà cũ có thể chọn lại Nhận quà tân thủ ở Lễ Quan để nhận riêng thẻ, một lần; không phát lại trang bị hoặc Xu.

Mỗi mốc nhận độc lập, không cần nhận theo thứ tự. Nhân vật cấp 90 vẫn có thể nhận các mốc thấp hơn chưa nhận. Thẻ không tự tiêu hao khi mở, hủy hội thoại, nhận một mốc hoặc nhận đủ tất cả mốc. Sau cùng hiển thị **Bạn Đã Nhận Hết Phần Quà, Có thể Bán Shop vật phẩm này.** Không tự xóa thẻ. Không nên bán thẻ khi chưa nhận đủ quà; không có cơ chế tự cấp lại thẻ đã bán.

| Mốc | Quà |
|---|---|
| 30 | Phi phong + mũ Lục 4x theo phái, khóa; 1 Đồng Nhân; 5 Thanh Lộ; 5 Chân Khí; 1 Nhật Nguyệt Đơn |
| 40 | Yêu đái + giày Lục 4x theo phái, khóa; 5 Đồng Nhân; 10 Lam Bảo Thạch; 1 Nhật Nguyệt Đơn; 1 Hồng Không Thư; 1 Di Ngoại Phù Siêu Cấp |
| 50 | Áo Lục 4x theo phái, khóa; 1 La Hán Hiệu Giác; 2 Mảnh Sách Chư Hầu; 1 Nhật Nguyệt Đơn; 1 Hồng Không Thư |
| 60 | Siêu Phàm Lân / Siêu Phượng / Siêu Linh theo phái, khóa giao dịch; 4 Mảnh Sách Chư Hầu; 1 Thái Cực Đơn; 1 Hoàng Không Thư; 10 Lam Bảo Thạch |
| 70 | Nón + phi phong Lục 8x theo phái, khóa; 4 Mảnh Sách Chư Hầu; 1 Mảnh Vải; 1 Hoàng Không Thư; 10 Hồng Bảo Thạch |
| 80 | Nón + phi phong Lục 8x theo phái, khóa; 4 Mảnh Sách Chư Hầu; 1 Thái Cực Đơn; 1 Thanh Long Lệnh; 2 Mảnh Vải |
| 90 | Việt Lân / Trác Long / Trác Lôi theo phái, khóa giao dịch; 1 vũ khí nhiệm vụ cấp 80 theo phái; Giáp Sĩ ngẫu nhiên ngắn/dài |

Hai Túi Thuộc Tính đã được bỏ theo trả lời của người dùng. Trang bị ở 70 và 80 được phát riêng tại cả hai mốc theo danh sách yêu cầu, không gộp. Không tự bổ sung khóa cho vũ khí cấp 80 vì yêu cầu chỉ nêu khóa cho trang bị lục và thú cưỡi.

## Định danh và tiến độ

- Trang bị lục: ParticularType `3+phái`, level template 4/8, series 2/7/12 hoặc 4/9/14. SetID gốc 2/4 tạo đúng phẩm chất lục; không chỉ đổi màu tên. Đây là bộ có áo cấp 40/80; cấp yêu cầu của mũ và phi phong lấy nguyên bảng gốc.
- Thú cưỡi: ParticularType 24/25/26, level template 6 ở cấp 60 và 8 ở cấp 90; giá trị yêu cầu cấp 60/90 đã đối chiếu bảng.
- Vũ khí nhiệm vụ: ParticularType 27/28 Giáp Sĩ, 29 Đạo Sĩ, 30 Dị Nhân; level template 8, yêu cầu cấp 80. Lựa chọn Giáp Sĩ được lưu trước khi phát để túi đầy/thử lại không đổi kết quả.
- Nhật Nguyệt Đơn dùng IB 164/0, loại gốc 30 ngày; Mảnh Sách Chư Hầu dùng IB 193/5, loại thường, không dùng bản Tiên Ma 1422/5. Mảnh Vải dùng IB 269/2 mở hành trang, không dùng nguyên liệu trùng tên 5165.
- Receipt thẻ 4704, phái đã chốt 4705, vũ khí Giáp Sĩ 4706; tiến độ số vật phẩm đã phát ở 4710..4716. Đã quét các script phongthan hiện có để tránh chiếm task đang dùng.
- Phát từng đơn vị theo API thật, chỉ tăng tiến độ sau khi AddNormalItem thành công. Đầy túi giữ phần đã nhận, lần sau chỉ phát phần thiếu. Kiểm tra sở hữu thẻ trong F4, cấp, phái, giao dịch và khóa ở callback. Lua/game loop thực thi đồng bộ; dùng cơ chế lưu task/vật phẩm hiện tại, không tạo cơ sở dữ liệu phụ.
- Dùng IB script để tránh nhánh tự trừ MagicScript trong KItemList::NowEatItem. Dòng thẻ mới tương thích cả cột bán shop của bảng lẫn cách KBPT_IBItem hiện tại đọc cờ cũ. Không sửa engine hoặc các vật phẩm IB khác. Thẻ có thể giao dịch theo template; tiến độ vẫn thuộc nhân vật, không thuộc bản thẻ.

## Võ Sư

Nguồn gốc: `Client_Original`, `Script.pak`, entry 107, ID `176EFDAC`, SHA256 `2a6937d6ee5ca9926c6d57d45f44be460ddcd9f05df616813faf5bf5785a9cc0`.

Giữ nguyên payload gốc ở đầu file, gồm nhiệm vụ sư đồ/Trừ Yêu và gửi thư. Bổ sung đúng đường dẫn gốc `script/玉虚宫/教师.lua` dưới cách lưu tên ANSI của Loose và đường dẫn NPC đã phục hồi `script/phongthan/npc_recovered_batch/1003_176efdac.lua`. Thêm kiểm tra map 1003, NPC template 154, entity ID, chủ sở hữu và cự ly khi mở/callback mua sách; sách chỉ mở cho Đạo Sĩ, không đang khóa/giao dịch.

Giữ nguyên `Sale(9)`: goods 452..459 gồm 8 sách Đạo Sĩ gốc; goods 502 là Cẩm Nang. Không đổi giá hay danh mục. Đã đối chiếu cả Client/Server và giá trong bảng skillbook. Không tuyên bố mọi nhiệm vụ sư đồ đã được nghiệm thu chỉ từ việc kiểm tra cửa hàng.

## Kiểm tra và phát hành

- Lua 4: 5.039 kiểm tra vòng đời thẻ cho ba phái và mọi mốc; thử đầy túi ở từng vị trí phát, nhận trùng, nhận ngược thứ tự, giữ thẻ và lưu lựa chọn vũ khí.
- Tích hợp Lễ Quan: 14 kiểm tra bổ sung (tổng 5.053 trong fixture), nhận dở/nhận tiếp, nhân vật đã nhận quà cũ và không gọi LeQuanStarterXu.
- Võ Sư: 16 kiểm tra qua script gốc + guard, đúng phái/map/NPC/khoảng cách, khóa/giao dịch và giữ hàm nhiệm vụ gốc.
- 112 lượt đối chiếu tuple/name trong các bảng Client/Server.
- Native KBPT_IBItem thực tế kiểm tra khóa thẻ 9700/2, kích thước, cờ bán shop, không rơi và đường dẫn script; native KPakFile đọc được script mới qua chỉ mục Loose.

Gói `Deploy/ProjectContent/LevelGift20261030`, chỉ Loose Client/Server, không DLL và không Runtime Release. `Tools/build_level_gift.py` tạo gói; `Tools/test_level_gift.py` chạy kiểm tra. Publisher offline `Deploy/Publish-LevelGiftContent.ps1` được gọi sau chuỗi nội dung Loose hiện có. Hai chỉ mục được cập nhật thêm đúng tên script, giữ các ID không liên quan.

Bằng chứng phát hành, backup, PID/độ mới snapshot và hash nằm trong `analysis/level_gift_20261030/`. Chưa nghiệm thu chuột/giao diện và nhận quà bằng nhân vật thật trong client. Trạng thái đang chạy chỉ được xác nhận khi final-verification.json đạt.

Đã xác nhận bản cuối: world PID 164688, 105 map, 9.843 NPC/quái, 9.255 script nạp và failed=0; snapshot mới dưới 2 giây khi kiểm tra. Võ Sư template 154 hiện diện đúng điểm (55936,103680) tại map 1003. 11 file live khớp gói. Lần dừng trước bản cuối exit code 0; 9 file nhân vật giữ nguyên trong lúc sao chép. Client đã mở lại, PID 164524, cửa sổ Phong Than Online. Không đăng nhập/tự nhận thưởng thay người dùng.

# Nhiệm vụ chung và giao diện Loose — 24/10/2026

> Đã có [bản sửa tiếp theo](SUNG_THANH_VTT_UI_FIX_20261024.md): Giáo Huấn dùng quái sẵn có, bỏ 12 quái thêm sai, khôi phục menu danh sách và nút vàng gốc, sửa thông báo/khung theo dõi và thêm NPC/VTT. Các số liệu triển khai ở biên bản mới thay cho bản này.

> Bản mới nhất: [sửa HUD, menu NPC, shop và HP/AI boss](QUEST_UI_POLISH_20261024.md).

Bản cập nhật tiếp nối [kiểm tra Giáp Sĩ](GIAP_SI_QUEST_AUDIT_20261024.md). Các số liệu triển khai và kiểm thử dưới đây thay cho số liệu của bản trước.

Nguồn đối chiếu: [Tây Kỳ](https://pt.vnggames.com/cam-nang/nhiem-vu/tay-ky.html), [Triều Ca](https://pt.vnggames.com/cam-nang/nhiem-vu/trieu-ca.html), [Giáp Sĩ](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-giap-si.html).

## Tám nhóm nhiệm vụ chung cho ba phái

Tứ Tượng xuất hiện ở cả hai trang và được tính một lần. Script kiểm tra hệ phái, cấp, tiến độ, vật phẩm và hình dạng biến thân; nút nhận/trả chỉ xuất hiện khi đủ điều kiện tương ứng.

| Nhóm | Cấp | Nội dung và phần thưởng đã nối |
|---|---:|---|
| Trùng Mộc | 19 | Đổi 3 Liễu Mộc, thú cưỡi trắng và Bá Lạc Nhãn đúng bậc thành thú cưỡi xanh. |
| Yên Phúc | 23 | Tống Dị Nhân → thầy bói. Nhánh nói thật nhận 5.000 lượng; nhánh mua quẻ tốn 300 rồi mất 1 danh vọng. Sửa điều kiện cấp 13 và giá 500 của script cũ. |
| Nhân Chi Tương Tử / Phu Thê | 27 | Dương Tiễn cấp 3 phù biến thân; gặp Nhậm Đại Ca với hai hình dạng theo bất kỳ thứ tự nào, sau đó gặp Đại Tẩu trong hình dạng yêu đương. Thưởng 5.000 kinh nghiệm và tăng vĩnh viễn 20 sức lực, có chốt chống nhận lại. |
| Tứ Tượng Đồng Quy | 31 | Giao 10 nguyên liệu được chỉ định; thưởng 1.200 lượng, 5 danh vọng và cơ hội trang bị xanh cấp 40 hoặc pháp bảo cấp 30. Có thể lặp. |
| Thiên Ngoại Phi Tiên | 39 | Thương nhân → thầy bói (1.000 lượng, nhận Bá Lạc Nhãn 1) → Cao Minh → bốn Trư Tinh ở Hoang Mạc → Băng Tinh → mở đổi thú cưỡi. |
| Minh Châu Ám Đầu | 43 | Nhánh từ chối cho con bạc vay tiền → tiệm cầm đồ → Võ sư Phong Thần Đài → Cù Lưu Tôn → ba loại máu cá Đông Hải, 3 Sơn Thạch và 10 Đồng Thau. Thưởng 5.000 lượng, 25% cơ hội nhận Hồng Bảo Thạch; có thể làm lại. Nhánh phụ cho vay trừ 100 lượng và không đẩy tiến độ nhiệm vụ; không đủ tiền sẽ không trừ. |
| Họa Địa Vi Lao | 57 | Tiểu Bảo → dược sư → Cự Thạch → Võ Cát → Xích Tinh Tử; giao Liễu Mộc, Côn Lôn Kính và hai Bất Diệt Đèn. Cao Giác bán đèn 5.000 lượng/chiếc. Thưởng 500 kinh nghiệm, 1–3 Hồng Thủy Tinh và một đèn thừa; có thể lặp. Số tinh thể theo phần nội dung hướng dẫn, vốn khác tiêu đề ghi 3. |
| Thiên Cương Tinh Quy Vị | 59 | Đủ 36 sao, cờ triệu hồi, gọi/quét quái và trả từng sao. Mỗi sao trong 35 sao đầu cho 1 danh vọng; lượt cuối cho 40.000 kinh nghiệm, 108 danh vọng và Bá Lạc Nhãn 9. Chọn nhánh Bá Lạc Nhãn trong hai lựa chọn của hướng dẫn, chưa làm lựa chọn thú cưỡi cấp 50. |

Thú cưỡi xanh dùng 16 bản ghi được bổ sung từ bảng thú trắng hiện có: giữ nguyên trị số, chuyển một thuộc tính nền sang ô thuộc tính xanh cố định, bổ sung ánh xạ hình ảnh. Đây là phương án tái dựng phù hợp hệ thống hiện có, chưa khôi phục bảng thuộc tính xanh ngẫu nhiên nguyên bản. Xác suất trang bị Tứ Tượng giữ mức gần script cũ (10% pháp bảo, 5% đồ xanh), chưa tái dựng biến thiên xác suất đồ xanh theo cấp của bản cũ.

## Hội thoại, chỉ dẫn và F11

- Chỉ dẫn cập nhật ngay khi nhận, đổi bước, nhặt vật phẩm và hạ quái; nạp lại từ tiến độ đã lưu khi đăng nhập. Hiện mục tiêu, bản đồ, tọa độ và số lượng; tiêu đề vàng, mục tiêu cyan, tiến độ xanh lá.
- F11 giữ bố cục danh sách nhiệm vụ bên trái, giới thiệu và các bước bên phải; nội dung dài có thanh cuộn. Có nút theo dõi, hủy theo dõi và xóa theo dõi. HUD ưu tiên nhiệm vụ vừa thay đổi.
- Bảo Điển được nối lại vào nút Help trên cùng thanh công cụ của Nhiệm vụ F11, dùng các trang trợ giúp và tài nguyên giao diện đã có.
- Lựa chọn NPC dùng nút. Nhãn dài được giữ đủ trong lời thoại cuộn, chọn từng trang bằng nút ngắn; không cắt bằng dấu ba chấm. Nút kết thúc màu vàng nằm riêng ở đáy hộp thoại, không chiếm ô lựa chọn nhiệm vụ.
- Võ sư Sùng Thành Doanh được sửa phép kiểm tra bản đồ và lời gọi kiểm tra sư phụ; giữ các dịch vụ võ công, sư đồ cũ.
- Giáo Huấn Giáp Sĩ chuyển mục tiêu sang Yến Sơn gần khu xuất phát: mỗi lượt diệt 40 Giáp Cốt; lượt cuối từ cấp 29 diệt 40 Thiên Hạo. Tiến độ đã nhận trước cập nhật được chuyển bản đồ mà giữ số đếm. Các yêu cầu cấp và giới hạn lượt/ngày được giữ.

## Vị trí và giới hạn đối chiếu

Gói hiện có 111 vị trí NPC/quái/điểm nhiệm vụ bổ sung, gồm 67 vị trí của bản trước và 44 vị trí bổ sung đợt này. Các tọa độ cụ thể của Trư Tinh, Cự Thạch, Thi Thú và ba cá Đông Hải được đưa vào bản đồ tương ứng; thêm Cao Giác tại Xi Vưu Mộ (205,197).

Hướng dẫn Thiên Cương chỉ nêu khu vực: sáu trụ gọi sao và các điểm gọi quái quanh trụ là vị trí do dự án chọn. Các điểm kiếm nguyên liệu Tứ Tượng tại Mục Dã và quái Giáo Huấn tại Yến Sơn cũng là vị trí bổ sung theo khu vực, không phải tọa độ máy chủ VNG đã khôi phục.

Điểm tìm Tàn Nhẫn ở Bắc Hải được điều chỉnh từ (187,210) thành (190,206), vì ô cũ nằm ngoài biên bản đồ hiện có. Hội thoại, chỉ dẫn và điều kiện khoảng cách cùng dùng vị trí mới. Các NPC có sẵn và dữ liệu địa hình được giữ nguyên. Chi tiết nằm trong [manifest](../Deploy/ProjectContent/GiapSiQuests/manifest.json).

## Kiểm thử và triển khai

- Build Release thành công: GameClient, CoreClient, CoreServer.
- 37 ca hành trình, 2.138 kiểm tra điều kiện; 140 script Lua 4 nạp thành công. Bao gồm ba hệ phái, 36 sao, kiểm tra cấp/bản đồ/biến thân, hai nhánh con bạc, chống nhận thưởng lặp và hoàn tác giao dịch khi túi không đủ chỗ. Mock giao dịch cũng từ chối chuyển sang chính trạng thái cũ như native; nhánh cho vay dùng phép trừ tiền riêng.
- Kiểm tra bằng bộ đọc bảng native: 16 thú xanh, 6 vật phẩm phép, 29 phần thưởng trang bị và các vật phẩm nhiệm vụ được sửa. Kiểm tra riêng mã chữ TCVN3, lọc nút kết thúc và dữ liệu nhật ký đạt.
- Bộ kiểm tra OverheadUi cũ không biên dịch do mock thiếu phương thức giao diện ảnh; không tính là đã đạt. Thay đổi trình bày được kiểm tra bằng bộ kiểm tra riêng.
- Đã xuất bản 239 file nội dung cùng bản sao bảng client cần thiết và 3 binary: tổng 243 file. Dừng server sạch, lưu nhân vật, sao lưu trước khi thay file rồi khởi động lại; so sánh dữ liệu nhân vật không thấy thay đổi do triển khai.
- World cuối PID 82940 đã kết nối Database, Relay và Gateway. Snapshot có 3.998 NPC/quái; kiểm tra đạt 111/111 vị trí bổ sung theo tọa độ gốc, quái chiến đấu có sinh lực dương, 243/243 file khớp hash gói cuối. Đây là kiểm tra world và nội dung triển khai, chưa phải lượt chơi nhiệm vụ trực tiếp.

Biên nhận và bằng chứng: [xuất bản cuối](../../analysis/giapsi_20261024/publication_revision4.json), [khởi động server cuối](../../analysis/giapsi_20261024/world_revision4.json), [kiểm tra vị trí trực tiếp](../../analysis/giapsi_20261024/live_verification.json). Bản sao lưu cuối: `PhongThanRuntime-Loose/ManagerStudio/PhongThanStudio/Backups/GiapSiQuests_20261024_1792820774989`.

Chưa kiểm tra trực tiếp hình ảnh và thao tác trong game: công cụ computer-use lỗi khởi tạo trước khi điều khiển cửa sổ. Kiểm thử hành trình dùng Lua thực với API người chơi giả lập, không thay thế một lượt chơi trực tiếp trên server. Không thao tác bản Client_Original.


Cập nhật VTT mới nhất: [Hai cửa Phong, 250 quái và va chạm cổng/rào](VTT_COLLISION_250_20261024.md).

C?p nh?t m?i: [Nhi?m v? h?ng ng?y, n?t h?i tho?i v? ?i?u ki?n Linh Nguy?n t?i ?a 30](DAILY_QUESTS_LINH_NGUYEN_20261025.md).

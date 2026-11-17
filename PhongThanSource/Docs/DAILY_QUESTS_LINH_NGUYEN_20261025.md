# Nhiệm vụ hằng ngày và Linh Nguyện — 25/10/2026

Đã xuất bản riêng bản sửa nút hội thoại (`publication_dialog.json`) và gói 47 file nhiệm vụ/native (`publication.json`, sau ?? `publication_revision2.json` v? `publication_revision3.json`) trong `analysis/daily_quests_20261024`. Không thay đổi Client_Original. Không áp dụng quy tắc nút/danh sách mới cho toàn bộ NPC.

## Điều kiện Linh Nguyện

- Dùng bộ đếm IBBuff **418** có sẵn, không tạo vật phẩm giả trong túi.
- Chỉ nhận khi đọc được dữ liệu linh thú và người chơi **chưa có linh thú, chưa từng được ghi nhận nhận nuôi**. Thú triệu hồi Dị Nhân không phải linh thú.
- Tối đa **30**. Các lượt hoàn thành khi đã đủ 30 không được xếp hàng để nhận bù về sau.
- Nhận nuôi thành công ghi cờ vĩnh viễn **task 4363** và xóa thưởng đang chờ. Người đã có linh thú từ bản trước được đánh dấu khi hệ thống kiểm tra. Nghỉ, giấu hoặc không còn sở hữu thú sau đó không mở lại quyền nhận.
- Thám Quân: chỉ lượt **trả thành công** được đếm; đủ 10 lượt trong ngày mới cộng 1. Nhận, hủy, hết hạn, thiếu tiền hoặc gọi lại callback không cộng lượt thành công.
- Mỗi lần trả hợp lệ của 9 nhiệm vụ còn lại cộng 1 nếu còn đủ điều kiện. Phần thưởng thường vẫn được xử lý khi không đủ điều kiện nhận Linh Nguyện.
- Kiểm tra nằm ở native, gồm cả API thưởng IBBuff cũ. Hoàn tác giao dịch nhận nuôi thất bại có nhánh hoàn tiền/bộ đếm riêng; nhánh đó không phải API thưởng.

## Script đã có và phần bổ sung

| Nhiệm vụ | Kết quả rà soát và xử lý |
|---|---|
| Bào Thương | Giữ 14 script thương nhân đang dùng; chỉ thưởng sau khi hàng giao thành công và nhiệm vụ đã đóng. |
| Thám Quân | Giữ 3 script hiện có; đếm riêng 10 lượt trả thành công, không dùng bộ đếm số lần nhận của script cũ. |
| Đạo Cụ | Giữ 2 script Tân Miễn/Hàm Chi; nối thưởng vào nhánh trả thành công. |
| Thất Quải | Giữ chuỗi gốc; chỉ trả khi đủ 7 quẻ. Nối đúng NPC Thầy Tướng Số. |
| Thiên Thụ | Giữ chuỗi gốc và thưởng theo độ trưởng thành; bổ sung NPC Bách Giám còn thiếu, chỉ cộng Linh Nguyện khi thực sự nộp cây. |
| Nhập Hồn | Giữ biến thể nhập trang bị có trong PAK gốc của bộ này. Mỗi giao dịch đã tiêu hao đủ nguyên liệu và Liễu Mộc là một lượt hoàn thành, không phụ thuộc có ra trang bị xanh. |
| Siêu Độ | Bổ sung nhánh chơi được, NPC Ân Hồng, phướn, mục tiêu quái sẵn có và chọn kinh nghiệm/ngân lượng; không kéo quái ở nơi khác về tân thủ. |
| Tứ Linh | Bổ sung 5 tầng theo từng hệ, mục tiêu/tọa độ, quái nhiệm vụ trong bản đồ khảo nghiệm, thưởng kinh nghiệm từng tầng và trả cuối. |
| Tống Tửu | Bổ sung NPC đầu/cuối, xe thật, lên/xuống xe, đổi bản đồ, yêu cầu xuống xe đánh quái, giao xe và chọn kinh nghiệm/mảnh Phong Ma Bích. |
| Thiên Cương Hồn | Bổ sung Ân Giao, nguyên liệu nhận, mục tiêu quái sẵn có, 5 Ảnh và boss đúng chủ nhiệm vụ, nhận Hồn rồi trả thưởng. |

Ân Hồng dùng tọa độ **190,199 của client đang cài**, thay vì tạo thêm một bản ở tọa độ của hướng dẫn web khác phiên bản. Tổng cộng 6 vị trí NPC được bổ sung trên ô đi được của bản đồ gốc; các bản ghi NPC khác trong vùng được giữ lại.

## Hội thoại và giao diện

Nút hội thoại dùng lát cắt của chính ảnh nút gốc để phần nền thực sự phủ hết vùng chữ. Chữ đầy đủ, xuống dòng theo từ và bố trí theo độ dài; không chỉ tăng vùng bấm trong khi ảnh nút vẫn ngắn. Không thay renderer ảnh dùng chung. Nút Kết thúc có sẵn được giữ nguyên.

Các NPC nhiệm vụ được mở rộng riêng tại script của NPC; dịch chuyển/danh sách và những hội thoại khác giữ API hiện có. Các nhánh đã có giữ nguyên nội dung trước đoạn nối thưởng. Không thêm lời quảng cáo Linh Nguyện vào mọi hội thoại. Nhật ký mới có tên thay vì ID.

## Kiểm tra và giới hạn bằng chứng

- Build Release CoreServer thành công; GameClient sửa hội thoại đã build và xuất bản riêng.
- 33.298 kiểm tra vẽ nút: phủ vùng ảnh, biên nguồn, chữ nằm trong nút và nút nằm trên chân hộp thoại.
- 362 kiểm tra native thưởng: đủ 10 Thám Quân, đổi ngày, chống nhận lặp, giới hạn 30, dữ liệu thú không đọc được, người có thú, cờ nhận nuôi vĩnh viễn và hủy hàng chờ không hợp lệ. Dùng chính mã eligibility/điều chỉnh bộ đếm sản xuất.
- 144 kiểm tra giao dịch/xe: giao dịch cùng pha, tiền, kinh nghiệm, túi đầy, hoàn tác, xe thật, chuyển map và trả lại không nhân thưởng.
- Lua 4 thật nạp các script đã sửa; 82 kiểm tra hành trình cho các nhánh mới và 6 nhóm hoàn thành có sẵn. Kiểm thử backend linh thú có kiểm tra cờ nhận nuôi, giao dịch thất bại và lưu/khôi phục dữ liệu.
- Đây là phục dựng theo dữ liệu và phiên bản đang cài. Bốn nhánh mới là code phục dựng, không phải script server VNG được khôi phục nguyên vẹn. Nhập Hồn kiểu phụ bản đời mới và đổi 36 Thiên Cương Hồn lấy trang bị không nằm trong gói này.
- Chưa thực hiện lượt chơi/quan sát trực tiếp trong client. Kiểm thử script dùng API trạng thái giả lập; kiểm thử native dùng mã sản xuất với môi trường kiểm thử. Kết quả này không thay thế kiểm tra hình ảnh và thao tác trực tiếp.

Nguồn đối chiếu: PAK Client_Original qua `original_evidence.json`, tọa độ/tên quái client và snapshot world; [hướng dẫn nhiệm vụ VNG](https://pt.vnggames.com/su-kien/huong-dan-tan-thu-cum-2008/nhiem-vu-dac-sac.html). Yêu cầu Thám Quân 10 lượt và Bào Thương nhận Linh Nguyện tuân theo yêu cầu của người dùng.

T?ng T?u ch? hi?n th?ng b?o nh?n Linh Nguy?n khi b? ??m th?c s? t?ng; nh?n th??ng th??ng ? m?c 30 ho?c sau khi nh?n nu?i kh?ng hi?n th?ng b?o sai.

Kh?i ph?c ri?ng 12 v? tr? nhi?m v? Tam S?n trong 7 file v?ng ?u ti?n: file v?ng r?ng tr??c ?? che c?c b?n ghi Npc_S b? sung. Gh?p ??ng b?n ghi ?? xu?t b?n tr??c, gi? nguy?n c?c ph?n h?ng r?o/v?t th? v? m?i NPC ?ang ch?n trong v?ng.

## World sau tri?n khai cu?i

World PID **94600** ?? k?t n?i Database, Relay v? Gateway; stderr tr?ng. Snapshot c? **3.999 NPC/qu?i**, ?? **6 NPC h?ng ng?y m?i**, **7 NPC S?ng Th?nh** v? **99/99 v? tr? nhi?m v? c?**, bao g?m 12 v? tr? Tam S?n v?a kh?i ph?c. **54/54 file g?i cu?i** v? Game.exe s?a n?t kh?p hash; c?c file nh?n v?t/t?i kho?n ?? sao l?u gi? nguy?n.

B?ng ch?ng: `analysis/daily_quests_20261024/live_verification.json`, `publication_dialog.json`, `publication.json`, `publication_revision2.json`, `publication_revision3.json`. So s?nh c?c bi?n nh?n c? c?n m?t kh?c bi?t ? file NPC Dao Tr? 1052/v_094/110_region_s.dat, n?m ngo?i g?i s?a n?y; gi? nguy?n tr?ng th?i hi?n c?, kh?ng t? ghi ?? b?ng b?n c?. C?c v? tr? c?n gi? n?u tr?n v?n c? m?t.

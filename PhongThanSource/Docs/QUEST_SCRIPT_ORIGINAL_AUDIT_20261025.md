# Đối chiếu nhiệm vụ với script gốc — 25/10/2026

Kết luận: các nhiệm vụ được thêm trước đây **chưa thể gọi là toàn bộ script gốc**. Có nhánh giữ script PAK và nối thưởng, có nhánh dùng lớp tiến trình viết lại theo hướng dẫn, có nhánh chỉ tìm được dữ liệu/lời thoại. Lần rà soát này đã xác minh 70 payload trong PAK tại 75 đường dẫn ứng viên, ghi lại nguồn/hash/nội dung và sửa các lỗi xác định được dưới đây. Năm đường dẫn không tìm thấy không được coi là bằng chứng nhiệm vụ không tồn tại ở đường dẫn khác.

## Các lỗi đã sửa và đã triển khai vào Loose

| Nhánh | Lỗi đã xác nhận | Bản sửa |
|---|---|---|
| Trừ Họa: Hạp Đằng | Chỉ lưu mốc 10 giây, không cấp ấn ký hiển thị. | Dùng buff có sẵn **335 — Hoàn Cẩu Ấn Ký, 10 giây**. Hạ Hoàn Cẩu tại Mạnh Tân mới nhận; hạ Giáp Cốt trong hạn và còn buff mới nhận Hạp Đằng. |
| Trừ Họa: Nguyên Sâm | Dùng NPC 399 `building031`, hiện ô ruộng; bấm là nhận ngay. | Dùng **object 345**, SPR `obj/box/yuansheng.spr`, đúng 10 vị trí Tam Sơn. Thu hái có thanh tiến trình 3 giây; di chuyển, trúng đòn, chết, đổi bản đồ hoặc thoát phiên sẽ ngắt. |
| Trừ Họa: quái canh cây | Hai Nguyên Tham Tinh có tầm nhìn 300 MPS, không tới hàng cây cách khoảng 512 MPS. | Tăng riêng tầm nhìn của hai quái canh tại khu vực lên 800, giữ nguyên template 608, cấp, sát thương và sinh lực. Đòn gây sát thương ngắt thao tác hái. |
| Trừ Họa: Sơn Xuyên Liễu | Dùng NPC cây thay object và chỉ có mốc thời gian vô hình của vật phẩm 9201 tự thêm. | Dùng **object 350**, SPR `obj/box/shanchuanliu.spr`, năm vị trí Đồng Quan. Dùng **Tĩnh Hóa Phù 277**, giữ script sử dụng sẵn có `净化符.lua` cấp **buff 336, 30 giây**. Cần còn buff trong lúc thu thập. |
| Tĩnh Hóa Phù | Dòng 277 trong bảng bị dính Genre 6 vào tên, thiếu một cột; đã tạo vật phẩm thay thế 9201. | Sửa đúng một dòng 277 ở cả client/server. Ngưu Sát cấp vật phẩm 277. Vật phẩm 9201 đã có vẫn được đổi 1:1 sang 277 bằng giao dịch có hoàn tác; không xóa vật phẩm nhân vật trên đĩa. |
| Họa Địa Vi Lao | Bản thêm cho 500 kinh nghiệm và 1–3 Hồng Thủy Tinh. | Theo callback `renwu2()` của Xích Tinh Tử trong PAK: **5.000 kinh nghiệm**, **3/4/5 Hồng Thủy Tinh** với tỷ lệ **50%/40%/10%**. Giữ điều kiện nhận/trả; người dùng đã xác nhận nhận được sau khi trạng thái cập nhật. |
| Thiên Cương Tinh Quy Vị | Thưởng 40.000 kinh nghiệm/Bá Lạc Nhãn; cả 36 tinh dùng mẫu 123 cấp 60 và không đặt tên từng tinh. | Theo PAK Dương Tiễn: **500.000 kinh nghiệm**, vũ khí theo phái, 108 danh vọng ở lượt cuối. Mỗi tinh thứ sáu trong nhóm dùng mẫu **124 cấp 80**, các tinh còn lại 123 cấp 60; dùng tên trong sáu script tháp gốc. |
| Siêu Độ | Giới hạn 6 lượt và thu Tử Kim Hồ Lô từ lượt thứ hai. | **Năm lượt thường/ngày**; mỗi lượt thêm phải tiêu hao một vật phẩm Tử Kim Hồ Lô thật. Việc hủy vẫn tính lượt đã nhận. Giữ bản sửa Chiêu Hồn Trận trước đó. |
| Tống Tửu | Native giới hạn 4 lượt/ngày, trái lời thoại 14664. | Chín lượt/ngày; đã kiểm tra lượt thứ năm, thứ chín và từ chối lượt thứ mười, giữ nguyên vật phẩm khi bị từ chối. |
| Đổi Thiên Cương Hồn | Thu 50 Đoản Kiếm tại Ân Giao, thay vì Đồng Thau tại Xích Tùng Tử. | Gỡ giao dịch sai ở Ân Giao. Xích Tùng Tử đổi **3 Phách + 50 Đồng Thau → 1 Hồn** bằng giao dịch native, kiểm tra đúng NPC, khoảng cách, khóa/túi và hoàn tác khi thất bại. Không cộng Linh Nguyện cho giao dịch nguyên liệu. |
| Các nhánh daily mới | Dùng `GetWorldPos` có thể đã được lớp tương thích script cũ đổi về ID nội dung 20/21, trong khi nhánh mới cần 1020/1021. | Thêm API riêng `DailyWorldPos` trả bản đồ runtime; chỉ đổi các module daily mới sang API này. Không đổi API của mọi NPC. |
| Trấn Nguyên | Trùng biến 4550/4551/4554 với Trừ Họa/Giáo Huấn, 4570–4572 với nhiệm vụ chung/Phu Thê, 4600+ với Vạn Tiên Trận. | Tách toàn bộ sang **4746–4761**, nén 120 đồ phổ thành bốn từ bit. Không sao chép các biến cũ bị trùng, không xóa tiến độ Trừ Họa/Phu Thê/VTT. |

Thanh tiến trình 3 giây là phần triển khai bổ sung để đáp ứng thao tác thu hái; chưa tìm được callback thu hái đầy đủ tương ứng để khẳng định thời gian này trùng server gốc. Hình cây, object, vật phẩm và thời gian hai buff lấy từ dữ liệu đang có. Script Tĩnh Hóa Phù sẵn có được giữ nguyên.

Khi ngắt thu hái, chỉ hủy phiên thu hái này; không gọi nhầm script hủy thanh tiến trình của sự kiện khác. Túi đầy hoặc phát sinh lỗi không tăng mask thu thập, không nhận lặp một cây. Các bản ghi NPC khác và section địa hình trong file vùng giữ nguyên byte.

## Phạm vi đối chiếu Tân Thủ Giáp Sĩ, Tây Kỳ, Triều Ca

| Nhóm | Nguồn và kết luận |
|---|---|
| Hộp Gấm | Có các script Tô Hộ/Thủ Khố/Sùng Ứng Loan/Triều Lôi/Triều Điền trong PAK. Phần gốc được giữ, có lớp giao dịch và giao diện bổ sung; không coi lớp bổ sung là mã server VNG. |
| Tân Thức, Dũng Đao, Kiếm Ái | Có script PAK liên quan. Bản chạy dùng bảng bước/lớp điều kiện viết thêm; đã kiểm tra các hành trình và giao dịch hiện tại, chưa hoàn nguyên toàn bộ callback về nguyên bản. |
| Trừ Họa, Giáo Huấn | Script Đại Phu Mạnh Tân/Tạp Hóa tìm được trong PAK chủ yếu có cửa hàng và nhiệm vụ khác. Các thành phần gốc của Trừ Họa nêu trên đã được đối chiếu riêng. Chưa tìm được toàn bộ chuỗi Lua cùng phiên bản để xác nhận tất cả mức thưởng/điều kiện của lớp tiến trình tự thêm. |
| Trung Thành, Trầm Hương, Khuyên Hàng, Thần Long, Bình An, Thần Dụ Kính, Tam Sách, Cổ Kim, Thiên Duyên, Thông Thiên | Đã trích script ứng viên liên quan và bảng 62 bước đang chạy. Đây vẫn là lớp phục dựng theo hướng dẫn, **chưa xác nhận tương đương toàn bộ script PAK**; các hành trình kiểm thử không chứng minh các phần thưởng của mọi phiên bản giống nhau. Không đổi hàng loạt các chuỗi này khi chưa xác định chính xác callback/phiên bản tương ứng. |
| Họa Địa Vi Lao | Đã đối chiếu callback trả cuối thực thi trong PAK và sửa phần thưởng như trên. Không thay điều kiện chỉ vì người chơi đã có Liễu Mộc. |
| Thiên Cương Tinh Quy Vị | Có đủ sáu script tháp, các script tinh và Dương Tiễn. Đã sửa phần thưởng, tên và mẫu/cấp tinh; giữ sáu bản đồ đúng nguồn gốc. Tiến trình hiện tại vẫn dùng thứ tự tuần tự của lớp thêm, chưa đổi sang bộ quay tinh toàn cục/cờ từng tinh của script cũ. |
| Trùng Mộc, Thiên Ngoại Phi Tiên, Nhân Chi Tương Tử, Yên Phúc, Minh Châu, Tứ Tượng Đồng Quy | Các nguồn ứng viên và bảng 32 hành động chung được lưu để đối chiếu. Đã kiểm tra các nhánh vật phẩm, tiền, biến thân, lặp và nhận thưởng của bản đang chạy. Chưa gán nhãn toàn bộ lớp viết lại là “đúng nguyên bản”. |

Các giới hạn này được giữ rõ để không lặp lại việc lấy kết quả kiểm thử code tự viết làm bằng chứng đã khôi phục đúng mọi script gốc. Với daily/Trấn Nguyên, các nhánh còn phục dựng tiếp tục được phân biệt trong các biên bản riêng.

## Kiểm tra và xuất bản

- 70 payload PAK được lưu kèm hash/đường dẫn, bảng 62 bước Giáp Sĩ và 32 hành động chung được trích riêng.
- 37 hành trình Lua4, 2.148 điều kiện; 144 payload Lua4 thực tế của gói đã nạp thành công.
- 83 kiểm tra engine thu thập: 10 Nguyên Sâm, năm Sơn Xuyên Liễu, thời gian, buff, sát thương/di chuyển, serial, đổi map, khóa, túi đầy và chống nhận lặp.
- 612 kiểm tra Trấn Nguyên, gồm 120 đồ phổ không đè nhau và 196 biến cũ không bị thay đổi; 15 kiểm tra giao dịch đổi Hồn; 163 kiểm tra áp tải/Tống Tửu; 105 kiểm tra Lua daily. Kiểm tra thưởng Linh Nguyện 366 và forge 1.358 vẫn đạt.
- Build CoreServer Release thành công. Đã thay 155 file, sau đó sửa thêm hai file vùng ưu tiên còn giữ bốn NPC Nguyên Sâm giả. Gói tái triển khai đầy đủ gồm **157 file**.
- World cuối **99624** kết nối Database/Relay/Gateway, stderr trống. Đạt **338 hash**, **84 vị trí NPC nhiệm vụ + 15 object thu thập = 99 vị trí**, sáu NPC hàng ngày, bảy NPC Sùng Thành, sáu tháp thật và vị trí Trấn Nguyên. Không còn 15 NPC cây giả tại các điểm đã đổi.
- Hai lần dừng world đều exit 0; file nhân vật không đổi trong lúc thay nội dung. Đọc kiểm tra bốn nhân vật trước triển khai không thấy vùng mới 4746–4761 bị sử dụng. Không sửa file nhân vật để đoán/chuyển các biến từng bị trùng.
- Chưa thực hiện một lượt chơi và quan sát trực tiếp bằng game client. Kiểm tra native/Lua/world và xem SPR không thay thế nghiệm thu gameplay.

Bằng chứng chính: `analysis/original_quest_audit_20261025/inventory.json`, `character_task_audit.json`, `manifest.json`, `revision2.json`, `publication.json`, `publication_revision2.json`, `live_verification.json`. Gói chuẩn: `Deploy/ProjectContent/OriginalQuestAudit20261025`; được áp dụng cuối chuỗi publisher nội dung để lần triển khai sau không đưa lại bản cũ.

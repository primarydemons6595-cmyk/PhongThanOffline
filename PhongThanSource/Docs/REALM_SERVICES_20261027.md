# Trang bị Tiên Ma, mở cổng và nhiệm vụ dẫn hồn

Trạng thái: **đã áp dụng 18 file vào Loose**, world PID **110168** nạp lại thành công. Biên nhận `analysis/realm_equipment_20261027/publication.json` xác nhận hash, dữ liệu nhân vật giữ nguyên và các dịch vụ hỗ trợ giữ nguyên PID.

Snapshot sau khởi động xác nhận hai Thuyền Phu tại 1065 (230,174), 1055 (223,197); Tam Long ở 1054/1055/1056 (200,207). Vân Tiêu gốc tại Diêu Trì (195,203) vẫn có trong world. Không bỏ qua các giai đoạn mở cổng để kiểm thử. Chưa có lượt chơi thực tế hoàn chỉnh bằng tài khoản người dùng.

## Quy tắc đã chốt

- F4 có hai bộ riêng. Tab được chọn là bộ đang dùng, đóng góp thuộc tính và ngoại hình. Bộ còn lại được lưu nguyên vật phẩm, serial và thuộc tính.
- Chỉ mở bộ Tiên Ma sau khi hoàn thành Thăng Tiên/Nhập Ma; kiểm tra phía Tiên/Ma và cấp Tiên Ma của từng trang bị tại server.
- Đồ Tiên Ma đang đeo trái điều kiện từ bản trước được trả về ô trống F4 sau khi nạp hết hành trang. Nếu đầy, giữ nguyên sở hữu trong vùng chờ; tự trả khi có chỗ.
- Mở cổng toàn server: đủ Tam Long trong từng ngày của 3 ngày liên tiếp → 1.500 lượt Danh vọng Nhân Giới hoàn thành → người đã đăng ký diệt đủ Tam Long trong một ngày. Mốc ngày theo UTC+7. Không mở sẵn để test.
- Tu Tiên Đạo/Nhập Ma Pháp Môn: 4 lượt dùng chung mỗi ngày, phí lần lượt 0/1/2/3 Xu (questkey 47). Mỗi lượt thưởng **2 điểm**, tối đa **8 điểm/ngày**. Không tự đặt thêm thưởng XP khi chưa có số liệu.
- Mỗi trận cần **5 hồn**: tiêu diệt 5 quái khác nhau trong bán kính 600 MPS quanh trận; dùng vật phẩm dẫn hồn, đưa về Đại Phu rồi phục mệnh. Trận ban đầu và lượt dẫn hồn đều có thời hạn 5 phút. Bỏ qua xác bị gọi lặp; mất trận, chết, đổi map hoặc kết nối lại phải dựng trận lại.

## NPC và vật phẩm

| NPC | Vị trí | Chức năng |
|---|---|---|
| Vân Tiêu | Diêu Trì 195,203 | Dụ Giao Long, nhận Râu Thần Long; giữ lời thoại cũ |
| Thuyền Phu | Trần Đường 230,174 | Đi Đông Doanh, phí 200 vạn lượng |
| Thuyền Phu | Đông Doanh 223,197 | Giao 10 Chấn Thiên Tiễn để dụ Giao Long; đến Phương Trượng/Vân Long khi cấp 120+ và có Râu Thần Long |
| Linh Quang đạo nhân | Diêu Trì 196,203 | Xuất hiện sau ba ngày Tam Long; đăng ký mở cổng từ cấp 121 |
| Cổng Bất Chu Thiên Quan | Diêu Trì 200,196 | Xuất hiện khi toàn server đã mở cổng |

Râu Giao Long trong yêu cầu tương ứng vật phẩm gốc **Râu Thần Long, material 180**; Chấn Thiên Tiễn là **material 177**. Râu là điều kiện lên đảo, không tự tiêu hao mỗi lần đi. Lối quay lại Trần Đường không thu thêm phí. Kiểm tra ô đặt chân hợp lệ trước khi thu tiền; hoàn phí nếu đổi map thất bại.

Các pháp khí gốc: magicscript 410/411 gọi trận, 412/413 dẫn hồn. Dùng đúng vật phẩm đang mang, không chặn các vật phẩm khác. Số điểm hai nhánh được lưu tại task 4767/4768; tiến độ lượt và biên nhận tách theo từng nhân vật.

## Đối chiếu dữ liệu gốc

Đã tra PAK theo thứ tự thắng trong `Client_Original/package.ini`. Script Xích Tinh Tử tìm thấy là bản Vi Lao cũ; script Vân Tiêu chỉ có lời chào. Không tìm được Lua server đầy đủ cho chuỗi mới trong bộ đang có. Phần chuỗi nhiệm vụ được phục dựng theo hướng dẫn VNG và thông số người dùng xác nhận, không coi là Lua server gốc khôi phục nguyên bản.

- [Mở cổng Tiên Ma](https://phongthan.vinagame.com.vn/tin-tuc/tin-tuc/nhiem-vu-mo-cong-tien-ma-gioi.html)
- [Danh vọng Nhân Giới](https://phongthan.zing.vn/tin-tuc/tin-tuc/nhiem-vu-danh-vong-nhan-gioi.html)
- [Cửu Lai](https://phongthan.zing.vn/tin-tuc/tin-tuc/nhiem-vu-cuu-lai.html)
- [Phong Hồi Lộ Chuyển](https://phongthan.zing.vn/tin-tuc/tin-tuc/nhiem-vu-phong-hoi-lo-chuyen.html)
- [Thăng Tiên Nhập Ma](https://phongthan.zing.vn/tin-tuc/tin-tuc/nhiem-vu-thang-tien-nhap-ma.html)

Giữ nguyên phần script trước marker của 7 NPC, chỉ thêm nhánh riêng và callback gọi lại menu cũ. 4 hub mới có script/index riêng. Không đổi cơ chế Say/list/button toàn client.

Model trận gốc **755**, Tiên Hồn **756**, Ma Phách **782**. Kiểm tra bằng loader SPR sản xuất: 34/34, 9/9, 9/9 khung đều có pixel, RLE hợp lệ. Tên trận/hồn được gửi cùng NPC, tái đồng bộ cùng entity mỗi 3 giây. Đây là kiểm thử tài nguyên và luồng native; chưa coi là quan sát trực tiếp một lượt chơi hoàn chỉnh.

Ngoại hình Linh Quang dùng model gốc 757; Thuyền Phu dùng model ngư ông 1453. Chưa có bằng chứng đó là đúng model NPC tương ứng ở server VNG. Vân Tiêu giữ placement/model 165 gốc, không tạo trùng NPC.

## Kiểm thử và lưu trữ

- `RealmEquipmentTests`: 21 tổ hợp phía/mask, 26 lỗi chèn có rollback, hai bộ 13 ô, giữ serial, đầy túi và trả đồ cũ an toàn.
- `RealmQuestTests`: 16 đường phái/giới tính/phía, điều kiện chủ tuyến riêng phái, NPC/map/khoảng cách, phần thưởng và chống nhận lặp.
- `RealmWorldTests`, `RealmWorldStoreTests`: ba ngày liên tiếp, ngắt ngày, mốc 1.500 lượt, nhóm đăng ký, lưu bền qua restart, biên nhận từng nhân vật và lỗi ghi/kiểm tra checksum.
- `RealmDailyTests`: hai nhánh, phí 0/1/2/3, 2 điểm/lượt, 5 xác khác nhau, vòng gọi hồn, dẫn/trả, hết giờ/chết/đổi map/kết nối lại, tái đồng bộ ổn định.
- `RealmIslandTests`: phí/hoàn phí, vị trí đặt chân, cấp 120, 10 tiễn một lần, đúng Giao Long của người nhận, thưởng đầy túi không mất tiến độ.
- `RealmQuestDialogTests`: 11 script thực thi bằng Lua 4, nút riêng NPC, xem trước không thay đổi nhiệm vụ và callback cũ.
- `RealmNpcResources`: 141 khung hình của trận/hồn/cổng/NPC.
- `RealmItemCatalogTests`: dùng loader bảng vật phẩm sản xuất, xác nhận 410/411/412/413/431 và material 177/180 giải đúng khóa.
- `RealmStoreRoundTrip`: dùng CharacterStore sản xuất lưu/nạp đủ 27 record gồm hai bộ 13 ô và vùng chờ, giữ toàn bộ byte vật phẩm và metadata trên wire.

Tiến độ toàn server lưu nguyên tử ở `Server/CharacterStore/realm_world.dat`, có checksum và biên nhận từng account/role. File lỗi sẽ khóa tiến trình thay vì tự xóa/mở cổng. Trang bị dùng vị trí 20 cho bộ không hoạt động, 21 cho đồ chờ trả; không đổi các mã vị trí cũ. Character Editor hiện giữ nguyên các record này khi lưu, chưa có giao diện chỉnh hai vùng riêng.

## Phạm vi cần đối chiếu tiếp

Điều kiện chủ tuyến dùng task 1/2/3 theo Đạo Sĩ/Dị Nhân/Giáp Sĩ, mốc hoàn tất Thông Thiên 90; Vũ Sĩ kế thừa task 3. Bộ Loose đã có chuỗi Thông Thiên Giáp Sĩ, nhưng script cũ Đạo Sĩ/Dị Nhân tìm thấy chỉ tới mốc 83; bản này không tự cấp mốc 90 cho hai phái. Cần rà/bổ sung phần chủ tuyến 92 còn thiếu để hai phái đi trọn chuỗi tự nhiên.

Ba đảo trước đó không có quái thường. Bản này bổ sung Tam Long và Giao Long bị dụ, chưa phục hồi quần thể quái/nguồn rơi Chấn Thiên Tiễn nguyên bản của đảo. Có thể lấy material 177 từ Cẩm Nang để test luồng dụ.

Nguồn tạo nội dung: `Tools/build_realm_services.py`. Gói triển khai và hash: `Deploy/ProjectContent/RealmServices20261027/manifest.json`.

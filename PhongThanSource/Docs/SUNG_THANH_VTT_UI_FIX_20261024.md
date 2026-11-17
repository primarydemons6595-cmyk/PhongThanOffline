# Sùng Thành, Vạn Tiên Trận và sửa giao diện nhiệm vụ — 24/10/2026

Đã triển khai vào `PhongThanRuntime-Loose`. Bản này thay các kết luận cũ về Giáo Huấn ở [biên bản nhiệm vụ chung](SHARED_QUESTS_UI_AUDIT_20261024.md).

## NPC Sùng Thành Doanh

| NPC | Tọa độ | Nguồn vị trí |
|---|---|---|
| Tô Đắc Kỷ | 200,202 | Bố trí bổ sung gần Tô Hộ |
| Triệu Bính | 216,200 | Bố trí bổ sung gần Triều Điền |
| Mai Vũ | 209,195 | Liên kết tọa độ trong client gốc |
| Tôn Tử Vũ | 210,194 | Liên kết tọa độ trong client gốc |
| Kim Quỳ | 210,196 | Liên kết tọa độ trong client gốc |
| Hoàng Nguyên Tế | 194,201 | Bố trí bổ sung gần Lỗ Hùng |
| Trần Kỳ | 201,204 | Liên kết tọa độ trong client gốc |

Trịnh Luân đã có tại (200,203), được giữ nguyên. Theo xác nhận của người dùng, “Nhị Tướng” là cặp Trịnh Luân và Trần Kỳ, không phải tạo một NPC mang tên Nhị Tướng.

Cả bảy NPC dùng Lua gốc tìm được trong PAK, thêm kiểm tra bản đồ ở hàm tương tác. Triệu Bính giữ dịch vụ luyện đơn. Ngoại hình dùng lại các template VNG hiện có, là lựa chọn tái dựng; chưa xác minh đó là ngoại hình NPC nguyên bản. Không tuyên bố đã bổ sung tính năng Hanh Cáp Nhị Tướng năm 2020 hay phần thưởng mới ngoài Lua tìm được.

## Ba Vạn Tiên Trận Thổ, Thủy, Hỏa

Vị trí được quy đổi từ ảnh toàn bản đồ do người dùng khoanh, dựa trên biên WOR và lưới va chạm hiện có. Điểm đứng được chọn trong ô đi được. Bản Phong giữ luồng trước.

- Xanh lá: chặn các lối thoát vùng an toàn quanh điểm vào/Thủ Khố như ảnh. Có 5 đoạn ở Thổ, 11 đoạn ở Thủy và 14 đoạn ở Hỏa. Dùng tài nguyên hàng rào `拒马` có sẵn. Mỗi đoạn có 30.000 sinh lực (thông số tái dựng), không nhận sát thương trước giờ chiến đấu; khi bắt đầu trận vẫn còn nguyên, người chơi phải phá. Chỉ đoạn bị phá mới bỏ va chạm, không tự sinh lại trong cùng lượt.
- Chặn phía server khi đi/chạy, bắt đầu nhảy, từng bước nhảy và di chuyển kiểu lướt; kiểm tra cả cắt góc đường chéo. Đổi loại vật cản cũ cho phép nhảy qua sang loại chặn thông thường. Không chỉ đặt sprite để che lối.
- Đỏ: bốn Hộ Vệ được đặt lại theo bốn vòng khoanh mỗi ảnh. Quái thường không sinh trong vùng an toàn hoặc phòng boss; giữ 150 quái thường/trận.
- Xanh dương: cổng thành template 369 và vùng va chạm phủ kín lối cửa. Giữ điều kiện hạ đủ bốn Hộ Vệ và nộp bốn Lệnh Bài; khi mở, cổng biến mất và người chơi đi qua lối đó. Ba trận này không còn nút dịch chuyển xuyên tường vào phòng boss.
- Vàng: Thông Thiên xuất hiện tại vòng khoanh sau khi đủ điều kiện. Đồng bộ tọa độ trong mission, guard, boss và bộ giải vị trí native.

Kiểm tra liên thông lưới xác nhận cổng đóng ngăn đường tới điểm boss; khi bỏ cổng thì đi tới được. Không sửa địa hình map trong gói cuối. Các file map sinh ra trong lần thử ban đầu đã bị loại khỏi gói triển khai. Lịch giờ và cấu hình thưởng Event Service không đổi.

## Sửa các lỗi giao diện và Giáo Huấn được báo lại

- Bản ghi rỗng dùng để dọn nhật ký không phát thông báo hoàn thành. Các bản ghi tiến độ dạng snapshot cũng không phát thông báo nhận/hoàn thành mỗi lần đồng bộ. Đây là nguyên nhân ba dòng thông báo dù chỉ bấm NPC hoặc đang đánh quái.
- Chỉ `SayButtons` yêu cầu dạng nút. `Say` và `SayTask` thông thường giữ danh sách, nên dịch chuyển và dịch vụ không bị ép thành nút. Các chuỗi nhiệm vụ bổ sung dùng `SayButtons` rõ ràng.
- Footer dùng đúng sprite vàng có sẵn `spr/ui4/npc对话条/结束对话.spr`, kích thước 113×29, có sẵn chữ Kết thúc trong ảnh. Đã giải mã để kiểm tra tài nguyên; không vẽ lại nút và không chồng thêm nhãn chữ.
- Nhật ký tránh câu chỉ đường lặp NPC; HUD bỏ dòng tiêu đề trùng trong nội dung snapshot. Tên nhiệm vụ VTT 4001–4004 được ánh xạ sang tên Thổ/Thủy/Hỏa/Phong, không hiện số ID làm tên. Snapshot VTT cũng được gộp để không tăng lịch sử liên tục.
- HUD là một khung trong suốt ở vị trí trái, bắt đầu từ y=240 để chừa vùng UI thú. Chiều cao tối đa 360 và thu theo chiều cao màn hình; hiển thị toàn bộ nhiệm vụ theo dõi qua vùng cuộn, không chỉ giữ ba mục. Thanh cuộn chỉ hiện khi con trỏ nằm trong khung và nội dung vượt chiều cao. Chưa nghiệm thu bố cục bằng ảnh chụp client chạy thật ở mọi độ phân giải.
- Giáo Huấn Giáp Sĩ dùng quái tự nhiên tại Yến Sơn: các lượt thường đếm Kiếm Nhân hoặc Xạ Nhân, lượt cuối đếm Hoàn Cẩu. Gợi ý tọa độ lấy từ thực thể có sẵn: (204,224), (227,224), (235,226). Giữ 40 mục tiêu, số lượt và phần thưởng hiện có. Tiến độ đã nhận với mục tiêu cũ được đổi mục tiêu mà giữ số đếm.
- Đã loại đúng 12 Giáp Cốt/Thiên Hạo do bản trước thêm sai tại Yến Sơn; không thêm quái mới để phục vụ Giáo Huấn. Tham khảo phân bố quái tại [FS Thời Đại](https://fsthoidai.com/1692156017-Cac_loai_quai_vat_trong_FS_Thoi_Dai.html), đối chiếu thêm template và thực thể có sẵn trong world Loose.

## Kiểm chứng

- GameClient, CoreServer, CoreClient build Release thành công.
- Kiểm tra native hàng rào/cổng: 116 điều kiện đạt, gồm khóa trước giờ, phá từng đoạn, không sinh lại, nộp Lệnh Bài, mở cổng không dịch chuyển, dọn trận và khởi tạo lượt mới.
- Vòng đời bằng Lua 4 thực: 344 điều kiện đạt, gồm 150 quái/trận, bốn Hộ Vệ, khóa boss, chống gọi chết lặp, sinh boss/thùng thưởng, Đại Phu và timer.
- Nhiệm vụ Giáp Sĩ và nhiệm vụ chung: 37 ca, 2.147 kiểm tra điều kiện đạt; 142 script phát hành nạp thành công. Kiểm tra Giáo Huấn gồm đếm xen kẽ Kiếm Nhân/Xạ Nhân, giới hạn lượt, mục tiêu cuối và chuyển tiến độ cũ.
- Bộ kiểm tra trình bày đạt: tách nút kết thúc, giữ định danh nhật ký, không báo hoàn thành cho bản ghi rỗng/snapshot và phân biệt danh sách với nút.
- World PID 98060: Database, Relay, Gateway kết nối; 275 file đúng hash; 99 điểm nhiệm vụ giữ lại và 7 NPC mới được xác nhận. Tổng 3.993 NPC/quái trong trạng thái nền, phù hợp 3.998 trước đó −12 +7. Quái tự nhiên template 0/3/7 vẫn có; template 13/16 thêm sai đã hết khỏi Yến Sơn.
- Đã mở thử đăng ký ba trận trên world thật: mỗi trận 150 quái, số hàng rào 5/11/14 và một cổng thành. Hàng rào có 30.000/30.000 sinh lực. Đã đóng cả ba lượt thử, xác nhận quái/rào/cổng được dọn hết. Revision cấu hình Event Service vẫn 8.
- Xuất bản thử vào thư mục kiểm tra riêng đạt 272/272 file nội dung, khớp gói đang chạy. Sửa luồng publish nhận tên INI có chữ số (như UiHelper2) và khởi tạo đường dẫn mặc định ngoài danh sách tham số để tương thích PowerShell 5.
- Server trước triển khai thoát sạch với mã 0; so sánh dữ liệu nhân vật sau lưu không thấy thay đổi do thao tác thay file. Không thao tác bản Client_Original.

Chưa có lượt chơi trực tiếp để đánh hàng rào và boss hoặc nghiệm thu giao diện bằng mắt trong client: công cụ điều khiển cửa sổ của phiên làm việc lỗi khởi tạo. Các kiểm tra world ở trên xác nhận thực thể và sinh/dọn lượt đăng ký; kiểm tra sát thương, đường đi và bước nhiệm vụ dùng native/Lua với API mô phỏng. Không coi đó là bằng chứng đã chơi hết ba trận.

Gói: [manifest](../Deploy/ProjectContent/SungVtt20261024/manifest.json). Bằng chứng: [xuất bản](../../analysis/vtt_layout_20261024/publication.json), [world](../../analysis/vtt_layout_20261024/live_verification.json), [đăng ký thử](../../analysis/vtt_layout_20261024/events_registration.json), [dọn lượt thử](../../analysis/vtt_layout_20261024/events_closed.json). Backup: `PhongThanRuntime-Loose/ManagerStudio/PhongThanStudio/Backups/SungVtt_20261024_1792834556516`.

Nguồn duy trì: `Tools/giapsi` và `Tools/build_giapsi_quests.cjs` cho nhiệm vụ; `PhongThanVttArena.inl`, `PhongThanVttLayout.h` và gói SungVtt cho bố trí trận/NPC. Luồng publish Giáp Sĩ gọi thêm `Publish-SungVttContent.ps1` để áp 30 override trận/NPC sau cùng; các nội dung nhiệm vụ/UI vẫn lấy gói Giáp Sĩ hiện tại, binary lấy build hiện tại. Script dựng và kiểm tra của đợt này nằm trong `analysis/vtt_layout_20261024`.

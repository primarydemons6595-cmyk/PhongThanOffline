# Đồng bộ nhiệm vụ, sách NPC, phù di chuyển và vũ khí khởi đầu — 25/10/2026

Đã triển khai lên **PhongThanRuntime-Loose**. World mới **99964**, Bishop **105700**; world cũ 99624 dừng và lưu thành công, exit 0. 160 file được kiểm tra hash khi thay; đối chiếu sau khởi động đạt 350 file của các đợt sửa liên quan. Không sửa Client_Original hoặc file nhân vật trong lúc triển khai. Chưa nghiệm thu hình ảnh bằng một lượt chơi trực tiếp trên client.

## Nhiệm vụ và biểu tượng sách

- `SetTask` của Lua đồng bộ giá trị ngay sau thay đổi. Các thay đổi task gốc và task qua giao dịch native làm mất hiệu lực cache trạng thái sách; nhận/trả nhiệm vụ không phải chờ hết chu kỳ cache cũ.
- Nhật ký được so sánh theo nhân vật, phiên kết nối, ID, bước, hành động và nội dung. Chỉ gửi lại dòng đã thay đổi; đổi nhân vật/đăng nhập lại được gửi lại đầy đủ. Trước đây việc chạy lại cả nhật ký gây nhiều lần ghi file đồng bộ trên luồng UI client.
- Sách Giáp Sĩ kiểm tra vật phẩm và tiến độ Giáo Huấn, phân biệt nhận, đang làm, đủ điều kiện trả. Bổ sung phép chiếu cho Tây Kỳ/Triều Ca, Siêu Độ, Thiên Cương Hồn, Tứ Linh, Tống Tửu, Trấn Nguyên và Vạn Tiên Trận.
- NPC giao rượu chỉ hiện sách khi có chuyến cần giao/nhận thưởng. Truy vấn sách VTT dùng nhánh native chỉ đọc, kiểm tra đúng map/NPC, tình trạng lượt trận và hộ vệ; không dùng phép nhận/trả nhiệm vụ để thử trạng thái.
- Không thay cách phân loại nút/danh sách của toàn bộ NPC hoặc dựng lại nút kết thúc.

## Đối chiếu phù với client gốc

Đọc chuỗi nạp thực tế trong `Client_Original/package.ini`, gồm `vng00.pak`, `vng.pak`, update 171 rồi các gói thấp hơn. Dữ liệu được chọn theo ưu tiên nạp, không dựa vào tên file đã giải nén hoặc chỉ lấy `settings.pak` cũ. Bảng IB/magic liên quan đang được `vng00.pak` ghi đè. Đây là bản update có sẵn trong Client_Original tại workspace, không phải khẳng định đã tải bản mới trên Internet.

| Vật phẩm | ID IB | Lượt trong bảng đang có hiệu lực |
|---|---|---:|
| Di Ngoại Phù | 35 | 10 |
| Địa Linh, Liệu Nguyên, Truy Phong, Hải Hồn, Tật Điện, Vạn Tịch | 119–124 | 10 mỗi món |
| Di Ngoại Phù siêu cấp | 159 | 50 |
| Bào Thương Hồi Thành Phù | 258, 737 | 10 |
| Hồi Thành Phù siêu cấp / bản nhỏ | 291 / 733 | 100 / 20 |
| Như Ý Di Ngoại Phù | 567 | 30 |
| x10 Dã Ngoại Truyền Tống Phù | 9248 | 100 |
| Truyền Tống Nguyên Tố x5, Bí Cảnh Thần Tiên x5 | 9249, 9250 | 50 mỗi món |
| Bào Thương Hồi Thành Phù x5 | 9252 | 50 |
| Nguyên Linh Địa / Bí Cảnh Thần Tiên | 9580, 9581 | 10 mỗi món |
| Sách Nguyên Linh Địa / Sách Bí Cảnh Thần Tiên | 9582 / 9583 | 40 / 20 |

Tên có “x5” hoặc “x10” là tên gói; không đổi số lượt thành 5/10 trái với cột số lần dùng. Bốn dòng 9580–9583 đã có ở client Loose nhưng thiếu ở server; đã bổ sung đúng bốn dòng từ bảng gốc mới vào server, giữ nguyên các dòng khác.

Phần thực thi của các phù trên được ràng buộc với **serial của đúng món vừa bấm**. Mở menu không trừ lượt; chọn hủy không trừ; chuyển map thất bại không trừ. Sau khi chuyển thành công mới trừ một lượt, giữ nguyên hạn dùng và không lấy nhầm một bản khác trong túi. Có kiểm tra chủ sở hữu, túi mang theo, khóa/giao dịch, đổi phiên, đổi map, hết hạn menu và trạng thái cấm di chuyển. Hồi thành dùng điểm hồi sinh của nhân vật. Phù Bào Thương chỉ hoạt động khi có task Bào Thương và trở về khu vực thương nhân tương ứng.

Giữ giao diện lựa chọn địa điểm dạng danh sách. 55 điểm đến lấy từ bảy script di chuyển có sẵn, chọn điểm đầu tiên trong những điểm đáp ngẫu nhiên gốc; không tuyên bố thuật toán chọn ngẫu nhiên đã được khôi phục nguyên bản. Các đường dẫn script thiếu của nhóm được bổ sung adapter dùng chung. Chí Tôn Thần Hành Lệnh (magic 264) và Thần Hành Phù (magic 5658) dùng menu di chuyển, chỉ tiêu hao khi thành công; bỏ adapter chặn cũ của 5658.

**Giới hạn bằng chứng:** chưa tìm được script gốc cho Chỉ Nam Châu Như Ý (IB 391) cùng dữ liệu đích tự động tương ứng. Mô tả mới chỉ nói đến nơi cần đến, không xác định đích. Món này được ghi nhận còn thiếu chức năng, chưa gán tạm tọa độ. Buff 738 là buff, các bao quà truyền tống là vật phẩm mở quà; không được đổi thành phù di chuyển trực tiếp.

## Trang bị khi tạo nhân vật

| Phái | Vũ khí, cấp 1 | Thuộc tính xanh lam |
|---|---|---|
| Giáp Sĩ | Đại Đao, row 90 | Xuất chiêu vũ khí +10%; STCB +10 điểm |
| Dị Nhân | Hỗn Thế Phủ, row 110 | Xuất chiêu vũ khí +10%; xuất chiêu bùa pháp +10%; STCB +10 điểm |
| Đạo Sĩ | Pháp Kiếm, row 100 | Xuất chiêu bùa pháp +10%; STCB +10 điểm |

Goddess tạo sẵn một item tại ô vũ khí đang đeo trong bản ghi nhân vật mới, không phát lại mỗi lần đăng nhập. ID nghề nội bộ là 0 Giáp Sĩ, 1 Đạo Sĩ, 2 Dị Nhân. Dùng các thuộc tính thật 115, 116, 121; màu xanh lam được xác định bằng thuộc tính trang bị, không phải chỉ tô tên. Định danh riêng của trang bị khởi đầu dùng trường tham số item có sẵn, không chiếm task nhiệm vụ. Các nhân vật cũ và vũ khí của họ không bị đổi.

## Kiểm thử và bằng chứng

- Build Release: CoreServer, CoreClient, Goddess thành công; CoreServer được build lại sau thay đổi cuối.
- Lua4 thực tế: 155 payload nạp thành công, 40 hành trình với 2.170 điều kiện, gồm sách thiếu/đủ vật phẩm và ba phái. Adapter Chí Tôn thêm cuối dùng cùng nội dung Include đã kiểm tra của các adapter khác.
- Phù native: 348 kiểm tra, đủ 55 điểm đến, 10 lượt liên tiếp, hết lượt, dùng lại callback, bản sao/serial khác, mất sở hữu, hết hạn, khóa, giao dịch, map lỗi và menu hết hạn.
- Vũ khí: 1.384 kiểm tra bằng phương thức KItem thật, gồm ba vũ khí cấp 1 và 135 công thức trang bị đã có; kiểm tra màu xanh lam, ID/giá trị thuộc tính, tái dựng từ tham số lưu và không ảnh hưởng công thức cam.
- CharacterStore thật + cache nhật ký: 32 kiểm tra trong thư mục thử độc lập; ba nghề tạo/lưu/nạp giữ nguyên byte item và ô đang đeo. Không tạo nhân vật thử trong dữ liệu người chơi thật.
- VTT: 201 kiểm tra bằng code nhiệm vụ/payout hiện tại; nhánh sách chỉ đọc hoạt động ngoài cự ly tương tác nhưng không cho nhận/trả từ xa.
- Sau triển khai: 350 hash, 3.984 NPC/quái, 84 vị trí NPC nhiệm vụ và 15 object thu thập, sáu NPC hằng ngày, bảy NPC Sùng Thành, sáu tháp thật và Trấn Nguyên đều đạt đối chiếu. Các bảo vệ Linh Nguyện 30 và cờ đã nhận nuôi 4363 giữ nguyên.

Bằng chứng: `analysis/quest_refresh_travel_starter_20261025/` gồm `travel_original.json`, `travel_destinations.json`, `manifest.json`, `publication.json`, `clean_stop.json`, `live_verification.json` và các bộ kiểm thử. Gói chuẩn `Deploy/ProjectContent/QuestRefreshTravelStarter20261025` được áp dụng sau gói audit cũ để tránh lần publish sau đưa lại script cũ.

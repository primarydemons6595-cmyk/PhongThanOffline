# Nhiệm vụ tân thủ, Thủ khố và khảo sát lag — 25/10/2026

## Các thay đổi đã đưa vào Loose

- Minh Châu ám đầu: `PTSJournal` không tạo nhật ký cho task 41 ở trạng thái 0. Đồng bộ trạng thái chưa nhận gửi bản ghi xóa mục 3024 để dọn mục ảo đã lưu ở client; không sửa tiến độ nhân vật đã nhận. Nhánh cho con bạc vay tiền không tự biến thành nhận nhiệm vụ chính.
- Vị trí tạo mới: Giáp Sĩ tại Sùng Thành 201/199, Đạo Sĩ tại Ngọc Hư 217/201, Dị Nhân tại Xi Vưu 196/203. Goddess đặt cả vị trí vào game và hồi sinh ban đầu khi tạo bản ghi mới. Không ghi lại vị trí của nhân vật cũ. Đã đối chiếu ô cản client/server; vùng thưa của Xi Vưu dùng quy tắc khởi tạo ô trống có sẵn trong loader server.
- Kế Tục: hiện cả hai mục tiêu, kể cả cấp 1; mỗi lần chỉ nhận một nhiệm vụ. Giữ giới hạn ngày, số quái theo cấp, tiến độ đang làm và quà thuốc cấp thấp. Dị Nhân nhận Mặt Quỷ (3/12) hoặc Hỏa Vũ (3/8); Đạo Sĩ nhận Băng Cơ (3/13) hoặc Ngọc Cốt (3/9). Sửa cả các bản sao helper trong đường xử lý quái chết, không chỉ menu Thủ khố.
- Thu Thập: đọc lại ba script từ PAK đang được `Client_Original/package.ini` ưu tiên. Hai NPC thực tế dùng alias `1003_acdf0300.lua` và `1004_7cc3eb20.lua`, không dùng bản cùng tên ở thư mục gốc. Cả hai đã có Thu Thập, yêu cầu **cấp 6** để nhận, đủ 10 nguyên liệu để hiện nút trả; đây là điều kiện script gốc. Giữ nguyên điều kiện đó, bổ sung phép chiếu sách nhiệm vụ cho trạng thái có thể nhận/trả Thu Thập. Đã chạy cả hai nhánh nguyên liệu của từng phái qua Lua4 thật.
- Theo dõi ngoài màn hình: mọi ID nhiệm vụ hợp lệ vừa được thêm đều mặc định bật theo dõi. Người chơi tắt trong F11 thì các cập nhật tiến độ và gói tin trùng không tự bật lại. Nhận lượt mới sau khi kết thúc sẽ bật theo dõi lại. Dùng ID thực đã giải mã từ bản ghi, kể cả header kiểu modulo cũ.

Không đổi quy tắc nút/danh sách của các NPC khác trong đợt này.

## Kiểm thử và phát hành

- GameClient: build thành công, 0 lỗi/0 cảnh báo. Goddess: build thành công, 0 lỗi/8 cảnh báo.
- 117 payload Lua4 nạp thành công; bộ hành trình Giáp Sĩ/Tây Kỳ/Triều Ca và nhật ký: 41 trường hợp, 2.200 kiểm tra.
- Thủ khố/Kế Tục: 24 trường hợp, 1.011 kiểm tra, ba phái, cả hai lựa chọn và nguyên liệu, không nhận đồng thời, giới hạn ngày, không trả sớm/nhận thưởng lặp, Thu Thập cấp 5/6 và thiếu/đủ vật phẩm.
- 44 kiểm tra bằng hàm `WakeUp` trích từ source hiện tại và lớp lưu nhật ký thật, cùng helper tọa độ tạo nhân vật.
- Đã thay 119 file theo hash. World cũ 87828 lưu và dừng exit 0; world mới 12468. Có backup trước thay và bản sao CharacterStore sau khi lưu. Không thay nội dung CharacterStore trong bước triển khai.
- Sau khởi động: đủ 105 map, 4.044 NPC/quái, 9.223 script nạp và 0 script lỗi; kết nối Database/Relay/Gateway thành công, stderr world trống, 119 hash khớp.
- CoreServer/CoreClient và cấu hình Event được giữ nguyên hash trong đợt sửa nhiệm vụ này; bản PK/Khai Minh đã áp dụng trước đó được giữ lại.

Chưa chạy nghiệm thu thao tác bằng nhân vật trong cửa sổ Game; các kiểm thử trên chứng minh nhánh code/script và dữ liệu, không thay cho kiểm tra hình ảnh thực tế.

Gói áp dụng sau các gói nhiệm vụ cũ: `Deploy/ProjectContent/StarterQuests20261025`. Bằng chứng nằm ở `analysis/starter_quests_20261025/`: `original_proofs.json`, `spawn.json`, `journeys.log`, `warehouse.log`, `tracking.log`, `manifest.json`, `publication.json`, `clean_stop.json`, `verification.json`.

## Lag/quái dịch chuyển/xác: chỉ khảo sát, chưa sửa

### Những gì đo được

Đo world 87828 khi không có người chơi: bộ đếm CPU của process khoảng 3, private working set 2.762.006.528 byte, I/O ghi khoảng 1.436.109 byte/giây tại thời điểm lấy mẫu. Một mẫu không đủ kết luận nghẽn CPU/ổ đĩa hoặc thiếu bộ nhớ. Chưa đo độ trễ từng tick world trong trận đánh.

`studio_live_world.enabled` đang bật. Source ghi snapshot khoảng 596 KB mỗi 1,5 giây, trên luồng world, dùng flush/rename với `MOVEFILE_WRITE_THROUGH`; đây là tải I/O có thật. Cần đo thời gian ghi dưới tải trước khi kết luận nó gây giật.

File `p1_blue_magic.jsonl` khoảng 4 GiB, trace hồi sinh client khoảng 128 MB. Trong mẫu 15 giây, các log này và log NPC sync không tăng. Chúng là dữ liệu cũ, **không thể quy kết là nguồn ghi liên tục hiện tại** chỉ vì kích thước lớn. Không xóa, cắt hoặc tắt log trong đợt này.

Các trace khung hình client cũ chủ yếu có trung vị 18–20 ms; một số có gián đoạn tối đa 1.689 ms. Trace 98916 có tối đa 94 ms, không có khung >100 ms. Đây là trace khung hình/người chơi, không có lịch sử vị trí từng quái đủ để ghép chính xác với lần người dùng thấy tốc biến.

### Các nhánh code cần xem xét

1. `KProtocolProcess::SyncNpcMin` gán trực tiếp vị trí NPC khi chuyển region; `KNpc::AutoFixXY` cũng gán thẳng tọa độ đã đồng bộ khi quái đang đi/chạy chuyển sang đứng/đánh/thi triển. Không có nội suy trong những nhánh đó. Đây là cơ chế có thể tạo hình ảnh nhảy vị trí khi vị trí dự đoán của client lệch server, dù kết nối localhost. Chưa đo được độ lệch thực tế của đúng con quái được báo.
2. Cần phân biệt actor quái chết và object xác. Client `KNpc::OnDeath` đi qua animation, tạo object xác nếu template là Body rồi xếp actor vào hàng xóa. `KObj::Activate` phía client giảm lifetime của Body và xóa khi về 0. **Bộ đếm nằm trong Activate, không phải Draw**. Chưa có bằng chứng đủ để kết luận xác sót là object không hết lifetime hay actor chưa bị xóa.
3. Server `KNpc::Revive` có thể trả về sớm nếu không giải được điểm spawn hợp lệ, trước khi hồi sinh. Đây là nhánh cần kiểm tra ở ca xác tồn tại; chưa tái hiện nhánh thất bại ấy với quái thật.
4. Trong mẫu 15 giây không có NPC normal đang chết ở snapshot, nên không bắt được ca xác tồn tại. Snapshot không chứa Doing/animation/object xác và chỉ cập nhật mỗi 1,5 giây; không dùng nó để khẳng định hệ thống dọn xác bình thường.

Đính chính thông báo tiến độ trước đó: nhánh lifetime xác thuộc `Activate`, không phải lượt vẽ. Không có thay đổi movement, AI, đồng bộ quái, hồi sinh quái, dọn xác hoặc cờ chẩn đoán trong gói này.

Bằng chứng: `performance.json`, `lag_sample.json`, `client_frame_summary.json`, `collision_inspection.json`. Khảo sát collision ô gốc chỉ là kiểm tra dữ liệu tĩnh, không mô phỏng đường đi hoặc hàng rào động.

Đối chiếu 3.754 ô gốc quái đang nạp không tìm thấy trường hợp ô client bị chặn nhưng server đọc được là trống; ba ô thuộc vùng thưa thiếu Region_S loose. Kết quả này không kiểm tra toàn bộ tuyến di chuyển và không loại trừ lệch va chạm khi quái chạy qua ô khác.

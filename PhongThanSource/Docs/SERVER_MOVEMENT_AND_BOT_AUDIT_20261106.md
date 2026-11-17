# Điều tra giật lùi vị trí, bộ nhớ server và bot — 06/11/2026

## Kết luận đã có bằng chứng

Cập nhật sau điều tra: đã sửa cách đối chiếu bước dự đoán với snapshot server; server vẫn quyết định tọa độ, ngưỡng sai lệch và hiệu chỉnh thật được giữ nguyên. Các mục mô tả Sequence=0 bên dưới là cơ chế trước bản sửa. Mẫu hiệu năng bên dưới là mẫu cũ PID 40356 đã lưu, không phải đo mới trong phiên đang chạy của người dùng. Hiện chưa có bản ghi đồng thời client/server đúng thời điểm người dùng bị giật.

1. Độ trễ loopback thấp không đồng nghĩa xử lý game tức thời. GameServer đặt 18 FPS, ngân sách khoảng 55,56 ms/nhịp. MainLoop gọi công việc lưu/đăng xuất/chuyển server trước Core Breathe; các công việc đồng bộ và một nhịp vượt ngân sách đều có thể trì hoãn xử lý lệnh.
2. `PhongThanPositionConsumer.inl` bỏ qua RECONCILE của chính nhân vật khi khoảng cách ≤128. Vượt ngưỡng này, hàm thay trực tiếp tọa độ logic/region, không hiệu chỉnh dần. Gói STAND của bản thân được bỏ qua đến khoảng cách 256, vượt ngưỡng thì đặt lại vị trí và lệnh đứng. TELEPORT được áp dụng ngay; gói khác map bị bỏ qua.
3. `PositionConsumerTests.generated.cpp` dùng nguyên hàm production đã xác nhận: 128 không dịch chuyển, 129 dịch về vị trí server; STAND 256 không dịch, 257 dịch; TELEPORT và chặn sai map đúng như mã. Đây là tái hiện cơ chế, không phải tái hiện đầy đủ tình huống chơi thực tế.
4. `KProtocol.cpp` gửi lệnh đi/chạy với Header.Sequence=0. Gói RECONCILE trong KNpc.cpp cũng dùng Sequence=0. ENTITY_POSITION hiện không có tick mô phỏng hoặc số lệnh di chuyển đã xử lý. Client chưa thể xác định snapshot đó ứng với mốc nào trong chuỗi dự đoán rồi phát lại các lệnh sau mốc đó.
5. Client đã tự điều khiển bước đi; WORLD_ENTITY_MOVE thường bỏ qua chính nhân vật qua ConformIdx. Vì vậy đề xuất “chỉ cập nhật bên client” đã có một phần trong cơ chế dự đoán hiện tại. Vấn đề nằm ở cách đối chiếu dự đoán với vị trí có thẩm quyền, không phải chỉ bật cho client tự đi.

Không thể dùng các kết luận trên để khẳng định duy nhất nguyên nhân giật: khác tốc độ, đụng vật cản, đổi region, trễ nhận/xử lý gói và khựng khung hình vẫn cần loại trừ.

## Mẫu hiệu năng đã lưu

Nguồn `PhongThanRuntime-Loose/Server/server_performance.json`, PID 40356, cửa sổ lấy mẫu 10.015 ms, 180 nhịp. Chỉ là một cửa sổ cũ, không đại diện mọi map hoặc mọi số bot.

| Hạng mục | Trung bình | Lớn nhất |
|---|---:|---:|
| Toàn nhịp | 23,593 ms | 136,653 ms |
| World | 4,913 ms | 15,145 ms |
| Bot | 14,878 ms | 91,960 ms |
| Một lần lưu bot | 30,327 ms | 56,812 ms |
| Một lần tìm đường | 2,031 ms | 39,105 ms |

16/180 nhịp vượt ngân sách. Bot chiếm khoảng 63% tổng thời gian nhịp trong mẫu; không cộng thẳng thời gian route/save vào bot vì các nhóm có thể lồng nhau. 18 FPS trung bình vẫn có thể chứa những nhịp giật ngắn.

Đo sizeof bằng đúng header native server (VC6): KNpc 7.512 byte ×48.000 =360.576.000 byte; KPlayer 180.744 ×1.200 =216.892.800 byte; KItem 1.316 ×200.000 =263.200.000 byte. Tổng khoảng 801,7 MiB bộ nhớ cấu trúc cố định, chưa tính dữ liệu con, map và cấp phát động. Map ngủ không giải phóng các mảng này. Không dùng working-set trimming để giả vờ giảm RAM.

## Phương án cần kiểm chứng trước khi sửa di chuyển

1. Thu đồng thời trace client và thống kê server, cùng tuyến đường/map/tốc độ. `movement_trace.flag` là cơ chế chẩn đoán sẵn có, cần đặt trước khi mở client; log đệm và giới hạn 3.600 khung. Kiểm tra bản Release có cho chẩn đoán trước khi yêu cầu người dùng chạy. Không tự đóng game để bật cờ.
2. Ghi mỗi lần hiệu chỉnh: lý do/mode, vị trí logic, vị trí hiển thị, tọa độ server, độ lệch, tuổi snapshot, tốc độ, vật cản, region và thời gian tick. Hiện trace chưa có toàn bộ mốc thời gian server/ACK nên phải thiết kế phần đo bổ sung có giới hạn trước.
3. So sánh có kiểm soát: tắt bot/bật bot; map vắng/đông; đứng nghỉ và chạy cùng một đường; số liệu tick trước và sau; giữ tắt ngủ map trong phép đo gốc để không trộn biến số. Không thay các cấu hình chơi của người dùng nếu chưa thống nhất lượt kiểm thử.
4. Nếu trễ tick trùng thời điểm kéo ngược, ưu tiên giảm đỉnh công việc trong world thread. Nếu tick ổn nhưng độ lệch tăng đều theo quãng đường, đối chiếu tốc độ/bước thời gian. Nếu luôn ở cùng ô, kiểm tra vật cản/cổng map.
5. Nếu cần cải tiến giao thức: thêm phiên di chuyển và số thứ tự lệnh, server xác nhận lệnh đã áp dụng; client đối chiếu snapshot theo mốc đó rồi phát lại lệnh chưa được xác nhận. Phải phân biệt lệnh đặt đích đi với input theo tick; không thể “phát lại” tùy ý khi giao thức hiện chỉ gửi điểm đích.
6. Tách hiệu chỉnh logic và hiệu ứng hình ảnh: sai lệch nhỏ đã được kiểm chứng có thể làm mượt offset hiển thị trong thời gian hữu hạn; giữ vị trí logic do server xác nhận. Dịch chuyển bắt buộc, chết/hồi sinh/đổi map vẫn áp dụng ngay. Đây là phương án thiết kế, chưa triển khai.
7. Kiểm thử ngắt kết nối, gói cũ, thay map, cưỡi thú, bị đẩy/làm choáng, tốc độ buff, vật cản và giới hạn chống gian lận. Không chỉ kiểm tra “chạy trông mượt hơn”.

## Bot: chỉ phân tích, không sửa logic

Bot là KPlayer thật, không phải một client riêng. Bộ quyết định đã chia lô `(count+17)/18`, di chuyển rà mỗi 100 ms và đã có budget tìm đường. Các tính năng này nên được giữ.

Hướng tối ưu theo thứ tự ưu tiên, cần profile trước khi triển khai:

- Giảm đỉnh lưu đồng bộ trên world thread. Bot tự lưu mỗi 30 giây, có lệch nhịp ban đầu nhưng một lần lưu vẫn có thể vượt ngân sách. Có thể lên lịch theo ngân sách hoặc chuyển phần I/O của snapshot bất biến ra worker; phải giữ thứ tự version và bảo đảm đăng xuất/đổi vật phẩm không ghi đè dữ liệu mới bằng snapshot cũ.
- RefreshTrainingIndex đang quét NPC đang dùng và dựng lại hai danh mục mỗi giây. Có thể cập nhật tăng dần theo spawn/chết/đổi map, kèm cơ chế đối chiếu định kỳ để tránh mục tiêu ma. Không bỏ khả năng chọn quái hoặc đi map khác.
- BotIndexDrops quét tới 20.000 slot object mỗi giây. Có thể dùng danh sách object còn sống/theo map và cập nhật khi rơi/nhặt/hết hạn. Phải giữ quyền sở hữu đồ và kiểm tra lại trước nhặt.
- Lập kế hoạch đích dùng index theo level, nhưng vẫn có thể duyệt nhiều ứng viên và tính đường. Ưu tiên cache liên kết map, giới hạn tổng công việc mỗi nhịp và phân bổ tiếp tục ở nhịp sau; không bỏ vĩnh viễn hành động hoặc giảm chức năng.
- Giữ diagnostics chiến đấu tắt khi không đo; hiện CombatSnapshots=0. Nếu bật, nó quét NPC và ghi file nên chỉ bật có thời hạn.

Chưa sửa bất cứ file PhongThanBot*.inl nào trong đợt này. Chế độ ngủ map coi bot là người chơi để giữ map bot đang hoạt động; còn làm ấm danh mục map khi có bot online nhằm bảo toàn bộ lập kế hoạch hiện tại. Vì vậy chưa hứa bot online sẽ giảm RAM mạnh nhờ nạp map theo nhu cầu.

## Trạng thái các sửa chữa riêng đã được yêu cầu trước đó

Các thay đổi này tách khỏi đề xuất đồng bộ vị trí:

- Tư thế bày bán đổi ngay trước snapshot; client nhận trạng thái bày bán của bản thân. Mở ngồi, đóng đứng. Không thay luồng RECONCILE/đi bộ/chạy.
- Quặng khai khoáng gộp vào chồng tương thích có sẵn, giữ khóa/tham số/hạn dùng, giới hạn chồng và hoàn tác khi thất bại. Chỉ đường phát quặng bật tùy chọn merge; giao dịch nhiệm vụ cũ mặc định không đổi.
- `2/10000` vẫn cộng may mắn theo cấu hình đang có `LuckBonusPerPoint=10`; chưa tự đổi lựa chọn này khi người dùng chưa trả lời. Studio mới hiển thị tỷ lệ thực và có nút đặt cộng may mắn về 0. Server đọc MiningPolicy lúc khởi động, không phải lúc nhấn Giữ thay đổi.
- Ngủ map là thử nghiệm, tắt mặc định. Map chưa được truy cập trì hoãn nội dung; map trống đủ thời gian nén lossless vật cản/bẫy, giữ NPC/đồ để không respawn hoặc mất trạng thái khi tỉnh. Người/bot, nhiệm vụ, NPC chờ hồi sinh, đồ rơi và đạn giữ map thức. Truy vấn script cũng có thể đánh thức map; chưa phải mô hình chỉ có con người mới được nạp map. Lần đầu đọc map là đồng bộ, có thể khựng và cần đo thực tế trước khi bật rộng.

Các fixture native và điều khiển WPF đã qua; chưa đo A/B RAM/tick trong game cho chế độ ngủ. Không dùng kết quả fixture để tuyên bố đã hết giật.


## Bản sửa sau điều tra

Lệnh đi/chạy có số thứ tự và số bước mô phỏng. Client lưu tối đa 256 mẫu trong 1.500 ms, đối chiếu vị trí server với đúng bước lịch sử thay vì vị trí client đã đi tiếp. Chỉ bỏ qua snapshot đi/chạy khi cùng lệnh/bước/map/nhân vật và sai lệch lịch sử không quá 128. Sai đường/tốc độ thật, lệnh dừng, dịch chuyển, chết, đổi map, lịch sử hết hạn và peer cũ vẫn dùng hiệu chỉnh có thẩm quyền. Server không nhận tọa độ/tốc độ/tick do client khai báo. Không mô phỏng phát lại tùy ý.

Kiểm thử production đã qua các trường hợp gói trễ, đổi đích, sai đường, giới hạn 128/129 và 256/257, gói cũ, clock wrap và vòng đệm đầy. Trace tùy chọn ghi thêm command/step được xác nhận và quyết định bỏ qua. Chưa có bản đo gameplay A/B đồng thời nên không kết luận đã hết mọi giật hoặc giảm RAM. Bot vẫn chỉ báo cáo, không sửa logic.

Các bổ sung Tiên/Ma, hiệu ứng kỹ năng và giới hạn tài nguyên: [báo cáo riêng](REALM_SKILL_EFFECTS_20261106.md). Studio đã được cập nhật và kiểm tra SHA; biên nhận tại analysis/server_sleep_20261106/studio-deployment.json.

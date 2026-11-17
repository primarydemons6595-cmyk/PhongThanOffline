# Tách điều phối bot khỏi GameServer

Trạng thái: thiết kế sau khi đọc mã hiện tại; chưa triển khai hoặc bật service bot mới. Chỉ áp dụng cho Loose khi triển khai thử. Việc tách tiến trình không tự bảo đảm hết lag.

## Hiện trạng xác minh trong mã

- `Sources/EventService/Program.cs`: Event đã chạy ngoài GameServer. Nó giữ cấu hình/lịch bot, phát lease và chạy worker hội thoại.
- `Sources/EventRuntime/BotAIgent.cs`: Studio/Event xuất `control.tsv`, đọc `status.json`. Đây là điều khiển bật/tắt và lịch, chưa phải bộ lập kế hoạch di chuyển/chiến đấu độc lập.
- `Sources/Core/Src/PhongThanBotAIgent.inl`: GameServer vẫn gọi quyết định bot trên world thread, kèm tìm đường, chọn mục tiêu, nhiệm vụ, đồ đạc, giao dịch và lưu.
- `PhongThanBotTraining.inl`, `PhongThanBotProgression.inl`, `PhongThanBotPvp.inl`: quyết định gọi trực tiếp KPlayer/KNpc và Lua trong world. Không thể chuyển nguyên các hàm này sang luồng/tiến trình khác rồi tiếp tục đọc/ghi các đối tượng đó.

## Ranh giới cần tách

| Bot Service riêng | GameServer giữ quyền thực thi |
| --- | --- |
| Chọn mục tiêu, chọn NPC/công việc, lập kế hoạch dài hạn | Kiểm tra nhân vật/NPC còn tồn tại, map và quyền tương tác |
| Tính đường trên bản đồ vật cản chỉ đọc | Kiểm tra đoạn đường theo vật cản hiện tại; thực hiện lệnh đi/chạy |
| Kế hoạch tổ đội, săn boss, vé VTT, tích nguyên liệu | Luật tổ đội, nhiệm vụ, vào trận và vật phẩm thực sự sở hữu |
| Đánh giá đồ, giá, ý định mua/bán/ghép | Kiểm tra và trừ tiền/nguyên liệu; giao dịch và ghép đúng một lần |
| Lịch, trí nhớ, hội thoại, tổng hợp thống kê bot | Sát thương, hồi chiêu, rơi đồ, kho đồ, tài khoản và lưu nhân vật |

Service nhận bản chụp trạng thái, không đọc con trỏ bộ nhớ của GameServer. Nó gửi ý định; GameServer có thể chấp nhận, từ chối hoặc báo cần lập kế hoạch lại. Không cho service tự đặt tọa độ, sinh tiền/vật phẩm, hoàn thành nhiệm vụ hoặc bỏ qua luật chiến đấu.

200 bot vẫn là 200 nhân vật cần mô phỏng và gửi trạng thái cho người chơi. Tách service giảm phần lập kế hoạch trên world thread, không loại bỏ chi phí mô phỏng này. Chạy cùng máy cũng phải giới hạn số worker để service không tranh hết CPU với world và client. Có thể chuyển service sang máy khác sau khi giao thức đã ổn định.

## Gói độc lập có thể chuyển sang server khác

Đóng gói `BotService` gồm chương trình, bộ lập kế hoạch, cấu hình, dữ liệu kiến thức và bộ kiểm thử. Mỗi server cần một `BotAdapter` tương thích, cung cấp trạng thái và nhận lệnh. Không thể chép service sang một server bất kỳ rồi tự hoạt động nếu server đó chưa có adapter/API phù hợp.

Giao thức cần version và capability: nghề/phái, map, nhiệm vụ, NPC, vật phẩm, ghép đồ, lãnh địa, PK, VTT. NPC/công thức mới do Studio tạo phải phát sinh catalog có phiên bản; service làm mới catalog theo phiên bản đó, thay vì nhúng cứng tên NPC vào chương trình.

Không nạp CoreServer.dll vào service để tạo một world thứ hai: cách đó nhân đôi RAM, có nguy cơ chạy script/lưu dữ liệu ngoài world thật và vẫn không giải quyết đúng ranh giới quyền thực thi.

## Trao đổi dữ liệu không chặn world

1. World tạo snapshot/delta bất biến với ngân sách công việc giới hạn. Chỉ gửi vùng quan tâm của bot; catalog tĩnh chia sẻ theo hash/phiên bản, không quét và xuất toàn bộ NPC/vật cản mỗi nhịp.
2. Ban đầu dùng IPC cục bộ, hàng đợi có giới hạn. Tác vụ I/O nằm ngoài world thread. Không chờ HTTP, đĩa, model AI hoặc service trả lời trong nhịp world.
3. Mỗi ý định mang `WorldEpoch`, `BotId`, `ActorSerial`, `Sequence`, `SnapshotTick`, `Deadline`, `MapId`, `MapGeneration` và serial mục tiêu. Server loại lệnh quá hạn, phiên cũ, sai nhân vật/map hoặc mục tiêu đã tái sử dụng slot.
4. Lệnh đi mới có thể thay lệnh đi cũ chưa thực hiện của cùng bot. Lệnh đổi đồ/tiền/ghép phải có mã giao dịch và kết quả lưu đủ để xử lý gửi lại đúng một lần; không dùng quy tắc bỏ lệnh đi cho giao dịch.
5. Server cấp token mục tiêu/hội thoại; service chọn trong những hành động được cấp. Không nhận tên hàm Lua tùy ý để gọi.
6. Worker chỉ lập kế hoạch từ dữ liệu bất biến. Quyết định cho mỗi bot có thứ tự, các bot chia đều lượt; không để nhóm đầu danh sách chiếm hết lượt.
7. Service chết hoặc mất kết nối: server không chờ. Hủy ý định quá hạn, bảo toàn giao dịch đang thực hiện, cho bot dừng an toàn hoặc chuyển về chế độ nội bộ đã chọn. Không hồi sinh bot hay ghi đè nhân vật từ snapshot cũ.

## Lộ trình kiểm chứng

- Giữ chế độ nội bộ làm mặc định; thêm chế độ service ngoài sau khi adapter hoàn chỉnh.
- Chạy đối chiếu chỉ đọc: service tính quyết định, chưa điều khiển bot; so kết quả với các tình huống nhiệm vụ, đầy túi, hết máu, mất mục tiêu, thiếu vé và đổi map.
- Chuyển từng phần: tìm đường/chọn mục tiêu trước; nhiệm vụ, kinh tế, tổ đội và PK sau. Một nhóm bot chỉ có một bộ điều phối đang sở hữu quyền ra lệnh.
- Đo cùng tình huống ở 50/100/200 bot, sau đó mới 500 nếu đủ tài nguyên: tick p50/p95/p99/max, số tick quá 55,56 ms, tuổi lệnh, độ trễ quyết định, CPU/RAM từng tiến trình và độ trễ client.
- Tải thử phải có di chuyển, đánh quái, đổi map, lưu nhân vật và giao dịch; không chỉ bot đứng yên. Thử tắt service, mất kết nối, gửi lại lệnh, NPC chết/tái sinh và nâng phiên bản catalog.
- Không công bố sức chứa online từ số bot cấu hình hoặc từ một lần đo ngắn.

## Đợt sửa đang thực hiện

Bản sửa ngày 09–10/11 vẫn giữ điều phối native; giới hạn công việc theo nhịp, chia nhỏ bước lập kế hoạch, tăng lượt tìm đường có ngân sách, sửa giới hạn 128 slot trong lượt gặp NPC, bỏ ép ghi đĩa cho các bản chiếu trạng thái có thể tái tạo. Lưu nhân vật/quyền sở hữu vẫn giữ độ bền dữ liệu. Đây là bước giảm nghẽn hiện tại, không được ghi nhận thành đã tách Bot Service.

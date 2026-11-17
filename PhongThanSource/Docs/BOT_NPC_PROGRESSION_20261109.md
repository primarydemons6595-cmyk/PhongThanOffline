# Bot đọc NPC, nhiệm vụ và công thức — 09/11/2026

Phạm vi: bản Loose. Đã cài 84 tệp lúc 19:10 ngày 09/11/2026, có sao lưu và biên nhận `analysis/bot_progression_20261109/installation.json`. Không build hoặc cập nhật runtime Release.

Sau khi khởi động lại, GameServer PID 64996 và Event PID 61544 kết nối bình thường; mẫu lúc 19:17 ghi nhận 200 bot online, 331 NPC trong danh mục và 30 NPC có hợp đồng mục tiêu Lua. Đây là xác nhận hoạt động và danh mục hiện tại, chưa phải kiểm chứng dài hạn từng bot tự hoàn tất mọi chuỗi ghép đồ. Mẫu vẫn có ba bot chưa hoàn tất nhiệm vụ mở rương và năm bot báo chưa tìm được quái phù hợp có thể tiếp cận; không coi các trạng thái này là đã được giải quyết hết.

## Cách dùng trong Studio hiện tại

1. Tạo hoặc đặt NPC bằng Maps Editor, gắn nhiệm vụ/cửa hàng được quản lý theo dữ liệu có cấu trúc của Studio. Điều kiện cấp, phái, mục tiêu, phần thưởng, điểm và giới hạn ngày vẫn là điều kiện thật của server.
2. Với ghép/nâng cấp, dùng mục Ghép Trang Bị để khai báo trạm/NPC, món đầu vào, nguyên liệu, món đầu ra, giá và tỷ lệ.
3. Áp dụng nội dung vào Server Loose bằng chức năng hiện có của từng mục. Chỉ lưu bản nháp trong Studio chưa làm thay đổi dữ liệu server mà bot đang đọc. Việc nạp NPC/script vào thế giới vẫn tuân theo luồng áp dụng của Studio.
4. Bot đọc revision nhiệm vụ/cửa hàng và làm mới danh mục NPC/công thức theo chu kỳ 30 giây khi vòng lập kế hoạch chạy. Thời điểm từng bot chọn nội dung mới còn tùy công việc đang làm và ngân sách xử lý chung; không bảo đảm cả 200 bot đổi việc trong 30 giây. Không cần tạo thêm một bản cấu hình NPC riêng cho bot đối với các mẫu đã hỗ trợ.

Chỉ đặt một NPC với script tùy ý chưa đủ. Cơ chế mới ngoài các mẫu nhiệm vụ/cửa hàng/công thức đã hỗ trợ cần adapter khai báo điều kiện, mục tiêu và thao tác hợp lệ; sau đó các NPC dùng lại cùng cơ chế có thể dùng chung adapter. Danh mục không cho phép bot tự đoán menu hoặc vượt điều kiện, mật khẩu và chi phí.

## Nhận diện nội dung hiện có và nội dung tạo mới

Bot dùng dữ liệu thực thi của server làm danh mục, không suy diễn chức năng chỉ từ tên NPC hoặc lời thoại.

| Nguồn | Bot đọc và thực hiện | Cập nhật |
| --- | --- | --- |
| NPC đang hiện diện | Entity, template, map, vị trí, script, hợp đồng mục tiêu Lua nếu có | Quét dùng chung mỗi 30 giây; bỏ NPC đã mất/đổi entity |
| Nhiệm vụ tạo bằng Studio | Điều kiện cấp/phái, bước, mục tiêu quái, vật phẩm, điểm, giới hạn ngày và NPC nhận/trả | Đọc revision server hiện có; không cần build lại bot khi thêm nhiệm vụ theo mẫu hỗ trợ |
| Cửa hàng/đổi điểm tạo bằng Studio | Giá, loại điểm, vật phẩm, điều kiện; chọn trang bị phù hợp và giao dịch đủ điểm | Theo revision; không cộng điểm trực tiếp cho bot |
| Ghép Trang Bị của Studio | Trạm, script, nguyên liệu, source/target row, chi phí, tỷ lệ | Server kiểm tra thay đổi tệp mỗi 2 giây; danh mục bot làm mới trong chu kỳ 30 giây |
| Bảng nâng cấp gốc | Tuple, mức sao, chi phí, quy tắc nâng | Dùng native quote và giao dịch hiện hành; bot mặc định nhắm ba mức đầu, không tự bỏ qua quy tắc |
| Script cũ có `PTBotGoal` | Mục tiêu/điều kiện chỉ đọc, nguyên liệu và callback được NPC thực sự cấp | Hợp đồng dùng chung; script cơ chế hoàn toàn mới cần khai báo adapter một lần |
| Script không có dữ liệu/adapter | Chỉ nhận diện sự hiện diện | Không tự bấm đoán hoặc đánh dấu đã hiểu đầy đủ |

Tạo NPC mới bằng Maps Editor đơn thuần chưa tự sinh ra ý nghĩa cho script tùy ý. Khi gắn NPC vào nhiệm vụ/cửa hàng/công thức có cấu trúc của Studio, bot nhận từ cùng dữ liệu đó. Không cần khai báo lại một NPC riêng trong mã bot cho các mẫu đã hỗ trợ.

`Server/BotAIgent/npc-knowledge.json` ghi danh mục NPC và thời điểm/PID. Khi bật chẩn đoán của runtime, `activity.log` ghi chọn mục tiêu, nhiệm vụ phụ tìm nguyên liệu, giao dịch và lý do chờ; cấu hình tắt chẩn đoán hiện có vẫn được giữ. Danh mục gồm các NPC đã nạp vào thế giới, không giả vờ có NPC của map chưa được nạp.

## Các luồng đã nối

- Mở rương: ưu tiên nhiệm vụ Thủ Khố, nhận qua menu, tìm đủ vật liệu rồi trả qua callback. Tách `bank_unlock_quest_required` khỏi `bank_saved_password_required`. Không đặt mật khẩu, không gán task hoàn thành để lách điều kiện.
- Nhà Chiêm Tinh: từ cấp 20, dùng lượt miễn phí hoặc tiếp tục lượt đang dở; xác nhận qua token roulette gốc. Không tự tiêu vật phẩm quay thêm chỉ vì đang có trong túi.
- Nhiệm vụ chung: đọc bảng `PTS_ACTIONS` và phần thưởng các bước tiếp theo để biết chuỗi nào cho nguyên liệu/công thức đang cần. Dùng callback do hội thoại hiện tại cấp, giữ nguyên điều kiện và phần thưởng.
- Thú cưỡi xanh: mục tiêu Trùng Mộc tại Tây Kỳ, kiểm tra thú trắng, Liễu Mộc và Bá Lạc Nhãn; chọn công thức đem lại lợi ích cho bot.
- Phi Tuyết tại Thợ Đồng Tây Kỳ: đọc trực tiếp 30 công thức native theo ba phái, giữ nguyên thú nền, cấp, tiền và nguyên liệu. Đây là NPC/thú cưỡi người dùng đã xác nhận muốn bot nhắm tới.
- Trấn Nguyên: thí luyện, mục tiêu quái của thí luyện, chọn nhóm trang bị, tích danh vọng qua đồ dư phù hợp, học đồ phổ đủ điều kiện và ghép bằng giao dịch gốc. Không hiến món đồ đang đeo hoặc tốt hơn món đang dùng. Chưa tự mở thủ tục tầng ba đòi nộp trọn bộ đồ cam.
- Công thức Studio và nâng cấp: dự trữ nguyên liệu, tìm nguồn rơi trong bảng drop thật, mua ở shop đăng ký, tìm chuỗi nhiệm vụ cho món thiếu, rút nguyên liệu đã sở hữu từ rương đã mở; tới đúng NPC, đưa vật phẩm vào ô ghép bằng thao tác kho, gọi native quote rồi compose; đưa phần còn lại về túi.

Bot giữ mục tiêu cha trong khi làm nhiệm vụ phụ kiếm công thức/nguyên liệu, tối đa bốn tầng phụ thuộc, chặn vòng lặp và có thời gian thử lại. Không bán/đem mặc lại nguyên liệu đang giữ cho kế hoạch. Chi phí ghép mặc định cần nằm trong 25% tiền đang mang; thiếu tiền thì tiếp tục hoạt động kiếm tiền, không được cấp tiền tự động.

Danh mục không bảo đảm mọi nguyên liệu đều có nguồn kiếm trong bản hiện tại. Nếu nguồn rơi/shop/nhiệm vụ không có, bot giữ đồ và chuyển việc/chờ thử lại, không tạo vật phẩm. Các bước thu thập có tương tác riêng và seal phức tạp chưa có adapter sẽ không tự nhận là đã xử lý được.

## Kiểm tra

- Legacy Lua 4: 81 script NPC/hợp đồng biên dịch và thực thi phần khai báo; 10.000 truy vấn đọc mục tiêu không làm tăng bảng phần thưởng, không ghi task/tiền/đồ.
- Native server độc lập: nhận/thu thập/trả nhiệm vụ mở rương qua hội thoại thật; Phi Tuyết cả ba phái trừ đúng tiền/đồ và chặn đổi trùng; đọc hơn 35.000 quy tắc; thêm NPC/nhiệm vụ theo cấu trúc Studio khi server đã chạy, đánh quái thật để hoàn thành, nhận đúng điểm và chặn quá lượt ngày.
- Native compose: bot chuyển đầu vào tới ô ghép, giao dịch cập nhật đúng template, giữ serial món chính, trừ đúng chi phí, thu hồi phần còn lại và dọn ô ghép/tay.
- Danh mục công thức cập nhật sau khi sửa chi phí trong cấu hình mà không build lại native.
- Lập kế hoạch dùng ngân sách chung, giãn ít nhất 125 ms giữa các lượt chọn mục tiêu mới; điều kiện mục tiêu đang làm được làm mới mỗi 1,5 giây. Đây là kiểm thử chức năng trong fixture, không thay thế theo dõi tải/FPS dài hạn với 200 bot đang chơi.

Các bản sửa trước về Thần Kỹ, chat, gian hàng và tin rao số vạn phải giữ nguyên. Không đổi dữ liệu nhân vật để mô phỏng thành tích; vật phẩm và tiền được cấp trong test chỉ thuộc nhân vật giả ở fixture.

# Đứng sau khi chọn nhân vật, READY rồi trở về NẠP MAP

## Bằng chứng

World đã có lúc ghi trạng thái hợp lệ rồi ngừng cập nhật. Các mẫu cũ có một nhịp xử lý mất 240.360 ms và 274.096 ms. Cả trạng thái world, thống kê tick và xác nhận Event cùng bị cũ. Đây không chỉ là lỗi chữ hiển thị trên Studio.

Lấy mẫu stack native bắt được chuỗi gọi thực tế: `PhongThanLifeTick → PtLife::Spawn → KSubWorldSet::SearchWorld → KSubWorld::EnsureContent → KRegion::LoadObject → fopen(…Region_S.dat)`. Tạo nguyên liệu thụ động đã đánh thức hàng loạt map lạnh ngay trong vòng xử lý chính. Relay có ngưỡng loại kết nối đăng ký sau 90 giây không nhận dữ liệu. Các lần bị chặn lâu có kết nối world→Relay ở CLOSE_WAIT, rồi world thoát. Tắt tùy chọn sau khi đường kết nối đã hỏng không tự nối lại Relay; cần một lần khởi động world sạch để kiểm tra chế độ tắt.

## Thay đổi hẹp

Thêm `PhongThanGatherSpawnWorld` cho việc tạo nguyên liệu/điểm mỏ. Khi ngủ map bật, nó chỉ trả về map đã tải và đang thức, không dùng `SearchWorld` để đánh thức map đang lạnh/ngủ. Khi người chơi dịch chuyển hoặc thao tác world chủ động nạp map, tick tạo nguyên liệu tiếp theo sẽ tạo các điểm tương ứng. Khi ngủ map tắt, hành vi tạo nguyên liệu trên các map hiện có được giữ nguyên. Không thay xác suất, số lượng, dự trữ mỏ, dữ liệu nhân vật hoặc logic bot.

Kiểm thử native dùng nguyên hàm tra map và thân `PtLife::Spawn`: map lạnh/ngủ không bị đánh thức, map thức và map vừa được kích hoạt tạo đúng điểm, không tạo trùng, loại trừ marker mỏ và map không tồn tại, giữ đường OFF. Dev CoreServer và Secure Release build qua. Cài riêng CoreServer.dll vào Loose sau khi world dừng bằng yêu cầu lưu/thoát bình thường; không ép tắt tiến trình. Có biên nhận SHA và bản sao lưu.

## Xác minh sau cài

Khởi động sạch PID 57224, giữ `MapSleep.ini Enabled=0` theo lựa chọn hiện tại. Sau khi tải xong toàn bộ map, world và Event ghi đúng PID, thời gian dữ liệu mới cách thời điểm kiểm tra khoảng 1–2 giây. Giám sát liên tục 101,96 giây đã qua ngưỡng mất kết nối Relay; nhịp mẫu cuối 18,071 FPS. Kết nối Database 5001, Relay 5003 và Bishop 5632 đều ESTABLISHED. Không đổi nhãn Studio để giả báo READY.

Các dòng `GateWay lost`/`DataBase lost` trong lần PID32304 được dừng là một phần của lần dừng bình thường có chủ đích để thay DLL. Không dùng riêng hai dòng này để kết luận lỗi tài khoản hoặc hỏng database.

Biên nhận: `analysis/login_hang_20261106/deployment.json`, `health-after.json`, `health-samples.jsonl`. Patch Server 1.04 đã tạo, 0 tài nguyên thay đổi, hash/chữ ký ECDSA-SHA256 hợp lệ (`patch-verification.json`); Client 1.03 giữ nguyên.

Chưa xác nhận người chơi vào được map sau bản sửa cho đến khi có phản hồi hoặc quan sát trạng thái vào game thành công. Chưa có kiểm thử gameplay đầy đủ cho lần khởi động lạnh với ngủ map bật hoặc mọi thao tác bật/tắt giữa phiên. Không kết luận đã sửa toàn bộ nguyên nhân lag hay mọi lỗi đăng nhập chỉ từ kiểm thử trên.

Xác nhận cuối từ người dùng ngày 06/11/2026 (Asia/Saigon): sau khi yêu cầu mở lại client và thử chọn nhân vật, người dùng trả lời “Đã vào được game”. Như vậy bước vào game sau bản vá đã được người dùng kiểm tra thành công. Giữ chế độ ngủ map tắt theo cấu hình hiện tại. Xác nhận này không thay cho kiểm thử mọi tài khoản hoặc khởi động lạnh/chuyển chế độ khi ngủ map bật.

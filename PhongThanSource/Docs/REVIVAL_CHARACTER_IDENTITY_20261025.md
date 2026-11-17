# Kiểm tra riêng luồng chết và hồi sinh

Ngày 25/10/2026. Tiếp nối bản sửa chọn nhân vật khi tự kết nối lại trong `RECONNECT_CHARACTER_IDENTITY_20261025.md`.

## Phạm vi đã kiểm tra

- `KPlayer::Revive`: hồi sinh tại chỗ/về điểm hồi sinh, gọi `KNpc::ChangeWorld`, phát trạng thái hồi sinh và đồng bộ HUD. Hàm này không đọc danh sách nhân vật và không chọn theo `RecommendedIndex`.
- Chuyển sang map đã mở giữ cùng NPC/entity; đường chuyển server dùng state và session route của nhân vật, không lấy nhân vật đầu tài khoản.
- Client nhận death/revive theo map và entity. Gói xóa entity từ mạng đã chặn xóa local actor bằng `ConformIdx`.
- Nhánh client kết thúc animation chết và xử lý hàng đợi `GWM_NPC_DEL`.

## Hai khe hở đã tái hiện trong kiểm thử

1. Nhánh dọn xác client kiểm tra `m_Kind != kind_player`, không kiểm tra slot có đang là nhân vật người chơi điều khiển hay không. Khi metadata loại NPC của local actor là loại thường, nhánh này có thể đưa chính slot local vào hàng đợi xóa.
2. `KSubWorld::ProcessMsg(GWM_NPC_DEL)` phía client không có guard tương tự đường nhận gói xóa từ mạng. Hàm thật xóa local actor, trả slot về pool, trong khi `Player[CLIENT_PLAYER_INDEX].m_nIndex` còn trỏ tới slot đó. Tái sử dụng slot làm dữ liệu actor đổi nhưng trạng thái Player/nhiệm vụ vẫn giữ cũ.

Kiểm thử bản trước sửa tái hiện bằng hàm xử lý hàng đợi thật và đoạn dọn xác lấy nguyên từ `KNpc::OnDeath`. Việc mô phỏng tái sử dụng slot minh họa cơ chế trộn actor/nhiệm vụ; không phải bằng chứng nhân vật offline còn lại đã thực sự được spawn trong lần lỗi của người dùng.

## Thay đổi và kết quả

- Nhánh dọn xác thêm điều kiện `!IsPlayer()`, bảo vệ local actor kể cả khi metadata loại NPC chưa đúng.
- Hàng đợi xóa kiểm tra biên index và `ConformIdx` **trước khi** đụng region, giảm ref hoặc giải phóng slot. NPC thường vẫn được dọn như cũ.
- CoreClient Release build thành công: **0 lỗi, 2 warning**.
- Bản trước: **3** kiểm tra tái hiện khe hở đạt. Bản sửa: **4.006** kiểm tra nhánh client đạt, gồm 2.000 vòng dọn xác/xóa trễ và các index không hợp lệ.
- **3.052** kiểm tra hàm `KPlayer::Revive`/`KNpc::SendCommand` thật đạt: 1.000 vòng luân phiên hai nhân vật với hồi sinh tại chỗ/về điểm hồi sinh, giữ slot/entity/tên/nghề và các giá trị skill/task đánh dấu; kiểm tra chuyển map thất bại và chặn thao tác khi đã chết. Hạ tầng map/network của bài kiểm thử là stub, không phải hai nhân vật thật đang đăng nhập.
- Log đã có: **18** cặp `revive_before`/`revive_after` phía server không đổi actor, player ID, entity, tên, template hay hệ. Log client có 22 death event, 36 revive event và **0** sự kiện `local_actor_remove`. Vì vậy **chưa đủ bằng chứng kết luận hai khe hở trên là nguyên nhân duy nhất của lỗi đã được báo**.

Đã sao lưu và thay riêng **Client/CoreClient.dll**. Giữ nguyên Game.exe chứa bản sửa reconnect; không sửa server/CharacterStore và không restart server. PID world trong lúc triển khai vẫn **109960**. Hash/biên nhận, nguồn trước sửa và bài kiểm thử nằm trong `analysis/revival_identity_20261025/`.

Chưa chạy một lượt chết/hồi sinh với nhân vật thật sau khi triển khai. Người dùng không cần cố tình chết nhiều lần để thay cho việc kiểm tra mã; trace nhận diện cũ vẫn còn để đối chiếu nếu lỗi phát sinh trong lúc chơi bình thường.

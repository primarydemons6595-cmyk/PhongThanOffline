# Kết nối lại chọn nhầm nhân vật và giữ nhiệm vụ của phiên trước

Ngày 25/10/2026. Đã áp dụng riêng `PhongThanRuntime-Loose/Client/Game.exe`.

## Nguyên nhân đã xác định

`KLogin::ProcessRoleListResponse` tự chọn theo `RecommendedIndex` khi tự đăng nhập lại. Goddess hiện trả chỉ số 0 cho danh sách nhân vật. Vì vậy nhân vật đang chơi ở vị trí khác có thể bị thay bằng nhân vật đầu danh sách. Khi nhân vật đang chơi cũng ở vị trí 0 thì không có biểu hiện lỗi, giải thích tính không liên tục.

Log client thực tế ghi nhận cùng PID 77220 và 105756 chuyển từ **GameMa** sang **DaoGia** qua map snapshot và self snapshot mới. Đây là đổi nhân vật thật trong luồng đồng bộ, không chỉ đổi ảnh trang bị. Bằng chứng trích gọn ở `analysis/reconnect_identity_20261025/observed_role_transitions.json`.

Nhiệm vụ dùng bộ nhớ tĩnh `KTaskDataFile` và cửa sổ `KUiTaskNote` có thể tồn tại qua kết nối lại. Thư mục riêng theo tài khoản/nhân vật trước đây chỉ được đổi ở cuối quá trình đồng bộ; bản ghi nhiệm vụ có thể đã đến trước thời điểm này. Vì thế giao diện có thể giữ nhiệm vụ của nhân vật trước khi các thuộc tính khác đã thuộc nhân vật mới.

## Thay đổi

- Chọn lại theo **tên nhân vật vừa chọn**; không dựa vào vị trí danh sách. Danh sách đổi thứ tự vẫn chọn đúng.
- Nếu đã có tên cần kết nối lại nhưng tên đó không xuất hiện, dữ liệu tên không hợp lệ hoặc bị trùng, kết nối tự động thất bại thay vì chuyển sang nhân vật khác. Người dùng có thể hủy tự kết nối và chọn lại thủ công. Lần đăng nhập tự động đầu tiên chưa có tên nhớ vẫn dùng gợi ý hợp lệ.
- Khi nhận chấp thuận vào world, lưu cấu hình/cửa sổ nhiệm vụ trong thư mục cũ, đóng cửa sổ nhiệm vụ và giải phóng bộ nhớ bản ghi, rồi gắn thư mục riêng cho nhân vật đã chọn **trước khi kết nối tới game server**. Các bản ghi mới được ghi đúng nhân vật.
- Thêm log `Client/client_reconnect_role_diag.log`: PID/tick, tên cần chọn, chỉ số gợi ý và tên thực sự chọn; không ghi mật khẩu.

## Kiểm chứng và triển khai

- Build GameClient Release: **0 lỗi**; còn 150 warning của phần mã legacy được biên dịch, không có warning tại helper chọn nhân vật mới.
- **1.020 kiểm tra đạt**, gồm 1.000 lượt đảo thứ tự danh sách, tái hiện cách chọn nhầm trước sửa, thiếu/trùng tên, dữ liệu tên không kết thúc, và kiểm tra lưu/nạp hai file nhiệm vụ bằng **UiTaskDataFile.cpp thực**. Cập nhật nhiệm vụ của B không làm đổi nhiệm vụ đã lưu của A.
- Đã sao lưu và thay Game.exe nguyên tử, kiểm tra SHA-256 sau thay. Biên nhận: `analysis/reconnect_identity_20261025/publication.json`.
- Server vẫn giữ PID **109960** trong lúc cập nhật; không restart server và không sửa CharacterStore, script nhiệm vụ hay cấu hình event.

**Giới hạn xác nhận:** đã xác định và sửa nhánh tự kết nối lại chọn sai nhân vật. Chưa thử mất kết nối/hồi sinh bằng hai nhân vật thật sau khi thay binary. Các cặp trace hồi sinh thuần túy đã có trong log server vẫn giữ tên/entity trước và sau; chưa có bằng chứng để kết luận mọi trường hợp hồi sinh không mất kết nối đều do cùng nguyên nhân. Giữ trace nhận diện hiện có để đối chiếu nếu trường hợp đó còn tái diễn.

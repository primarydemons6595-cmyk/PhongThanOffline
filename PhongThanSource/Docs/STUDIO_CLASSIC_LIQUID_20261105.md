# Studio chính: Classic và Liquid

Ngày 05/11/2026. Một ứng dụng tại `PhongThanRuntime-Loose/ManagerStudio/Release/PhongThanStudio.exe`, dùng Client/Server chính.

## Sử dụng

Mở Studio theo cách hiện tại. Chọn **Giao diện → Classic / Liquid** ở góc trên phải. Classic là lựa chọn mặc định ban đầu; lựa chọn sau đó được lưu. Liquid có nút **Màu / Nền** để chọn màu và ảnh.

Đổi theme giữ nguyên các màn hình chức năng, tab đang chọn và nội dung đang soạn. Cả hai dùng cùng mã xử lý, cùng bộ DLL và cùng dữ liệu; thêm chức năng vào Studio chính sẽ có ở cả hai theme trong cùng lần build. Không có bước đồng bộ sang một bản Studio chức năng khác.

## Giao diện Liquid

Nền và thanh bên trong suốt theo hình nền Studio. Nút có một đường biên ngoài và một lớp lấy mẫu nền; bỏ viền mảnh lồng trong, lớp trắng mờ và mảng phản sáng lệch. Khối chọn thanh bên dùng một đường bao Bézier liền, các điểm nối có tiếp tuyến liên tục; đầu và đuôi đi khác nhịp để mô phỏng nước chuyển giữa các mục lên/xuống. Không còn ghép các hộp chữ nhật tạo bậc ở cổ nước.

Classic giữ bố cục và resource gốc. Thay đổi hiển thị của Liquid được ghi nhớ để khôi phục khi đổi lại Classic. Phần Liquid lỗi khi khởi tạo sẽ trả về Classic và ghi thông báo ở thanh trạng thái.

## Kiểm tra và bản sao lưu

- Build Release không lỗi/cảnh báo.
- Đổi Classic → Liquid → Classic ba vòng; giữ cùng đối tượng ô soạn thảo, nội dung nháp và tab chức năng.
- Ảnh render Classic trước/sau có SHA-256 giống nhau.
- Kiểm tra không xuất hiện lớp tab điều hướng trùng sau khi đổi resource màu.
- Tái hiện template chứa transform đã Freeze, gửi 20 MouseMove; lớp phản chiếu dùng transform riêng và không còn ném lỗi.
- Kiểm tra nút rê/nhấn/nảy/trở về trạng thái ổn định và đường nước lên/xuống.
- Danh sách 18 tab chỉnh sửa và 18 tab quản lý lấy từ chính mã Studio hiện tại, gồm Bày bán & lệnh bài.

Ảnh, log, biên nhận: `analysis/studio_appearance_20261105/`. Binary trước khi thay nằm trong `backup-release`; ba file nguồn gốc nằm trong `baseline`. Mã giao diện ở `PhongThanStudio.App/Appearance`; các file chức năng gốc chỉ thêm hai điểm nối khởi tạo/startup. Không sửa định dạng dữ liệu, sinh Lua, patch hay cơ chế server trong thay đổi này.

Các kiểm tra trên là kiểm tra WPF/biên dịch, không khẳng định đã thao tác trọn vẹn mọi chức năng game. Quy trình đóng khi còn thay đổi chưa lưu của Studio vẫn giữ nguyên.

## Bổ sung: cửa sổ phụ và độ tương phản

Hộp Phần thưởng, bộ chọn vật phẩm và NPC có dictionary Classic cục bộ. Bản sửa bổ sung lớp resource Liquid ngay trong từng cửa sổ phụ, áp dụng lại sau khi dựng nội dung và dùng hình nền đã chọn kèm lớp kính tối để chữ/số vẫn đọc được trên ảnh sáng. Các ô bị vô hiệu hóa cũng giữ độ rõ của giá trị. Classic khôi phục dictionary và nền cũ khi đổi lại.

Đã render hộp phần thưởng thật ở cả Chung/Giáp Sĩ/Đạo Sĩ/Dị Nhân, kiểm tra giá trị kinh nghiệm 50.000 không đổi; render bộ chọn item/NPC và trường hợp ảnh nền trắng. Việc kiểm tra chỉ dựng/đọc giao diện, không bấm lưu hay tạo patch. Ảnh và log: `analysis/studio_appearance_20261105/dialog-verification/`.

Hàm dựng hộp phần thưởng được tách thành factory nội bộ để kiểm tra đúng màn hình sử dụng thực tế; hành vi lưu phần thưởng vẫn là cùng callback trước đó.

## Bổ sung: cây NPC, ô ngày, mật khẩu và Character Editor

Liquid bổ sung template riêng cho TreeView/TreeViewItem, DatePicker/DatePickerTextBox và PasswordBox. Các lớp trắng mặc định không còn phủ lên chữ trắng; ô mật khẩu vẫn là PasswordBox, không chuyển thành TextBox. DatePicker giữ các phần tương tác PART_TextBox/PART_Button/PART_Popup và giá trị ngày. Mặt nút có phản sáng theo chiều dày cùng vùng nền lấy mẫu lệch riêng ở mép; không chỉ dùng nét viền đơn sắc.

Character Editor là WinForms trong WindowsFormsHost, không hỗ trợ trong suốt xuyên qua WPF. Bản sửa vẽ nền từ ảnh Liquid ngay trên bề mặt WinForms, dùng bảng màu tối cho ô native, vẽ tab/nút để không còn nền trắng hệ thống. Phần canvas trang bị giữ nguyên. Khi quay về Classic, khôi phục màu, ảnh nền, kiểu tab và nút ban đầu. Đây là đồng bộ giao diện native, chưa phải shader khúc xạ vật lý đầy đủ như ảnh tham chiếu.

Kiểm tra ở `control-release-verification`: dựng cây NPC, ô ngày/mật khẩu; giữ giá trị sau theme; dựng Character Editor không mở/sửa/lưu nhân vật; kiểm tra khôi phục Classic. Bản binary trước sửa ở `pre-control-fix-release`.

Bộ thử cũ ở `D:/PhongThan-Studio-LiquidGlass` giữ làm bản đối chiếu. Các file mở nhanh cũ được chuyển sang ứng dụng chính, theme Liquid.

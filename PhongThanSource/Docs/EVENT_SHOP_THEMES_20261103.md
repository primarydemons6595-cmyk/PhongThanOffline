# Cửa hàng sự kiện: giao diện riêng theo sự kiện

Studio: **Sự kiện → Event khác → chọn sự kiện → 2 · Cửa hàng → Giao diện cửa hàng**. Chọn mẫu, xem trước, rồi **Lưu & áp dụng**. Mỗi sự kiện giữ lựa chọn độc lập; bản nháp cũng lưu lựa chọn. Không suy đoán chủ đề từ tên sự kiện.

Mẫu: Mặc định (`classic`), Tết Nguyên Đán (`tet`), Giáng Sinh (`christmas`), Trung Thu (`midautumn`), Halloween (`halloween`). Sự kiện cũ chưa có trường lựa chọn giữ mẫu Tết của lần triển khai trước; sự kiện mới mặc định mẫu trung tính.

Giáng Sinh dùng viền tuyết trắng, đèn tròn, sao, ông già Noel, tuần lộc và người tuyết; không dùng đèn lồng. Các nút Giáng Sinh cũng có viền trắng. Những mẫu còn lại có khung và bộ nút phù hợp chủ đề, với trạng thái thường/rê chuột/nhấn.

## Hiển thị và giao dịch

- Khung native 640×560; giữa khung có alpha bằng 0. Game vẽ lớp nền tối bằng `RU_T_SHADOW`, độ trong suốt nghịch đảo 18 trong thang 0–32 (tương đương độ đậm khoảng 111/255 ở Represent3). Không truyền giá trị RGBA 112 vào primitive này vì có thể gây nhiễu ở Represent2. Ảnh vật phẩm và chữ được vẽ riêng, không giảm độ rõ theo nền.
- Sáu mặt hàng mỗi trang, tối đa 19 mặt hàng theo giới hạn cấu hình hiện có. Icon và tooltip lấy từ dữ liệu vật phẩm thật của client; server không cần đọc SPR.
- 10.000 lượng = 1 vạn; 100.000 lượng = 10 vạn; 15.000 lượng = 1,5 vạn. Giá trị trừ tiền không thay đổi. Xu vẫn là vật phẩm QuestKey 4,47,0; đổi đồ vẫn dùng đúng ID và số lượng cấu hình.
- Máy chủ kiểm tra NPC thật, cùng bản đồ, khoảng cách, trạng thái sự kiện và khóa/giao dịch trước khi mua. Client chỉ gửi ID hàng, token và số thứ tự; không quyết định giá. Yêu cầu lặp cùng số thứ tự không trừ tiền lại.
- Đóng cửa hàng hoặc rời phạm vi sẽ ngừng sử dụng phiên mua. Cửa hàng NPC thông thường giữ luồng cũ.

## Dữ liệu và tài nguyên

Model Studio thêm `ShopTheme`. Snapshot thêm dòng `S\t[eventId]\t1\t[themeId]`, ID từ 0 đến 4, giữ tương thích cấu hình cũ không có dòng S. Gói cửa hàng native mang ID giao diện từ máy chủ; client chỉ chọn đường dẫn trong danh sách cố định.

Ảnh nguồn, nội dung prompt và SPR nằm tại `Deploy/ProjectContent/TetEventShop20261103/`. Xem `art-source/prompts.md` để biết mô tả tạo ảnh và chỉnh sửa Giáng Sinh. Công cụ built-in image_gen tạo/chỉnh ảnh; trình nhập chỉ cắt, đổi kích thước và mã hóa SPR native. Có 20 SPR, tổng 1.242.218 byte, chỉ ở client. Mỗi file được thêm vào loose index và manifest override để đóng gói Release.

Importer: `Tools/import_shop_themes_20261103.py`; installer giữ nguyên các alias cũ trong chỉ mục: `Tools/install_tet_shop_assets_20261103.py`.

## Kiểm tra

- CoreServer, CoreClient và GameClient biên dịch Release/Dev thành công. Cảnh báo kích thước image của CoreServer là cảnh báo linker hiện có; không coi đây là chứng nhận SecureRelease.
- Native: Schedule 28, ShopSnapshot 16, Pots 20, Actions 171 kiểm tra, gồm mua bằng bạc/Xu/đồ, túi đầy, phát lại yêu cầu, sai bản đồ, quyền NPC, khóa túi và định dạng tiền.
- Native production SPR loader: 20 tài nguyên, 50 frame; tâm khung trong suốt và tâm nút đủ đậm.
- Studio: cả năm lựa chọn qua vòng JSON/snapshot; từ chối mẫu ngoài danh sách. Kiểm tra mở cửa sổ xem trước bằng SPR thật, chụp ảnh từng mẫu, không sửa hay lưu sự kiện đang dùng.
- Ảnh xem trước đã render: `analysis/tet_event_shop_20261103/theme-previews/`.

Chưa thực hiện mua hàng bằng tài khoản người chơi trong game ở bước kiểm tra tự động; không dùng kết quả đọc SPR hoặc xem trước Studio thay cho kiểm tra đó.

## Sửa lỗi từ ảnh trong game và tự dọn trạm chưa dùng

- Nền nhiễu: sửa giá trị alpha cho đúng hợp đồng `RU_T_SHADOW` của cả Represent2/3.
- Chữ nút nằm sát mép: đặt `LabelYOffset` theo chiều cao nút và cỡ chữ. Hover đứng yên ở frame sáng, nhấn dùng frame lõm; không chạy hoạt ảnh lặp.
- Thiếu tiêu đề: cỡ chữ 20 không có trong `FontList` của skin; dùng font 18 đã được nạp và căn giữa dọc/ngang trong bảng tên.
- Studio, tab **4 · Nội dung & màu**: thêm **Tự dọn nồi / trạm chưa bắt đầu sau (giây)**, mặc định 300, giới hạn 30–86.400. Lựa chọn này được lưu vào bản nháp và cấu hình từng sự kiện, xuất dòng `D` trong snapshot.
- Nồi chưa bắt đầu có thời điểm đặt `Placed` lưu bền vững. Máy chủ dọn NPC và bản ghi khi hết hạn; khởi động lại không đặt lại thời hạn. Bản ghi cũ thiếu thời điểm đặt được cấp một khoảng chờ kể từ lần đọc sau nâng cấp rồi lưu lại. Nồi đang nấu vẫn dùng thời gian chín/cháy của công thức.
- Kiểm tra mới: PotTests 40, ShopSnapshotTests 22; PresentationTests xác nhận alpha 18, font 18 thực sự có trong cấu hình game, độ lệch dọc của chữ và 1.000 lần hover không đổi frame.

## Triển khai

Các bản sao trước thay đổi, nhật ký kiểm tra và biên nhận triển khai ở `analysis/tet_event_shop_20261103/`. Lưu cấu hình, công thức, phần thưởng và ID vật phẩm của người dùng; chỉ tạo phiên script mới và bổ sung trường giao diện. Không cài SPR sang server.

## Tooltip F4 và xác nhận số lượng

- Khi rê chuột vào ảnh mặt hàng, client dựng vật phẩm xem trước theo đúng tuple, đọc mô tả bằng `KGameObjDesc` đầy đủ và gọi `GetTooltipImageName`. Dùng đúng các phần title/thuộc tính/mô tả, ảnh lớn hoặc icon và các cờ hiển thị của tooltip F4. Vật phẩm xem trước được dọn ngay sau khi sao chép dữ liệu; không thêm vào hành trang.
- Sửa đường đọc cũ dùng buffer 2.048 byte trong khi `KItem::GetDesc` có hợp đồng đầu ra 4.096 byte. Không lấy một chuỗi ngắn thay cho toàn bộ mô tả native.
- Bấm Mua chỉ mở hộp xác nhận, chưa gửi giao dịch. Có số lượng, +/−, tổng giá, Xác nhận và Hủy bỏ. Escape đóng hộp xác nhận trước; hủy không gửi lệnh mua.
- Hàng bán theo gói vẫn giữ đúng đơn giá mỗi gói. Ví dụ mặt hàng cấu hình x10 thì số lượng chọn là 10/20/30…, giá nhân theo số gói. UI báo khi nhập số không chia hết cho gói.
- Máy chủ chỉ nhận ID hàng và số gói, tự tính lại số lượng nhận và giá từ cấu hình. Giới hạn 1.000 gói, 10.000 vật phẩm và tổng tiền/vật phẩm đổi không quá 1 tỷ. Túi không đủ hoặc thiếu tài sản thì hoàn tác toàn bộ, không mua một phần. Thay cấu hình sự kiện sẽ vô hiệu phiên mua cũ để tránh xác nhận theo giá đã đổi.
- Hộp mua dùng SPR `confirm.spr` 400×320 tạo bằng đổi kích thước cơ học từ khung của từng chủ đề. Cùng bộ nút chủ đề, không dùng cửa sổ mua mặc định của NPC thường.
- Có 25 SPR / 55 frame, chỉ tại client. Production loader xác nhận khung lớn và hộp xác nhận có tâm trong suốt, nút đủ đậm.
- CustomEventActionTests tăng lên 188 kiểm tra, thêm mua nhiều bằng bạc/Xu/đồ, thiếu tiền, túi đầy, phát lại yêu cầu nhiều món, số lượng không hợp lệ và cấu hình thay đổi khi đang mua.

Kiểm tra tự động không thay thế việc kiểm tra tooltip và hộp số lượng bằng tài khoản đang chơi trong game.

## Các ô thông tin mặt hàng

Mỗi mặt hàng có khung thẻ, ô ảnh, ô tên và ô giá riêng. Nền các ô dùng độ trong suốt nghịch đảo 23–27 trong thang native 0–32, giữ nhìn xuyên cảnh bên dưới. Viền mảnh đồng bộ màu Tết, tuyết trắng Giáng Sinh, xanh Trung Thu, tím Halloween hoặc nâu vàng mặc định. Tên dài xuống tối đa hai dòng; nút Mua đặt dưới ô giá. Hình vật phẩm và chữ giữ độ rõ bình thường. Chỉ vẽ thẻ cho mặt hàng có thật trên trang hiện tại.

Thay đổi bố cục này chỉ cần Game.exe mới; không cần khởi động lại server hoặc đóng Studio.

Cập nhật theo ảnh phản hồi: bỏ các viền lồng quanh ảnh/tên/giá và viền bao từng thẻ. Mỗi mặt hàng chỉ còn một nền tối trong suốt (alpha native 24); tên và giá phân biệt bằng khoảng cách và màu chữ. Chỉ vẽ đường ngang mảnh, màu trầm khi còn mặt hàng ở hàng dưới cùng cột. Hai mặt hàng trên một hàng không có đường ngăn phụ. Tăng khoảng đệm ngang giữa ảnh và chữ.

## Hộp mua nhỏ riêng — thay thế bản 400×320

Theo yêu cầu thu gọn như ảnh mẫu, hộp xác nhận hiện dùng khung mới **320×190**, không còn dùng khung cửa hàng thu nhỏ. Có năm bộ ảnh được tạo mới độc lập, với thanh tiêu đề và viền mảnh, ít trang trí; tâm khung trong suốt. Nút xác nhận/hủy mới 110×26, nút +/− mới 32×24, mỗi nút có ba trạng thái. Luồng xác nhận số lượng và kiểm tra giao dịch giữ nguyên.

Tài nguyên mới: `confirm-small.spr`, `confirm-button.spr`, `confirm-step.spr` trong thư mục từng chủ đề. Ảnh nguồn và prompt: `Deploy/ProjectContent/TetEventShop20261103/art-source/compact/`. Trình nhập: `Tools/import_compact_shop_dialog_20261103.py` chỉ cắt alpha, đổi kích thước và mã hóa SPR; không dùng lại ảnh khung cửa hàng lớn. `Tools/compact_shop_confirmation_20261103.py` ghi nhận thay đổi bố cục native.

Đã kiểm tra bằng bộ đọc SPR production: tổng 40 tài nguyên / 90 frame, gồm cả tài nguyên cũ được giữ lại nhưng không còn được hộp mua tham chiếu. GameClient biên dịch thành công. Chỉ thay client, máy chủ giữ tiến trình hiện tại; Studio có thể tiếp tục mở. Khi bổ sung index/manifest, trình cài đặt giữ khóa chỉnh sửa sự kiện để không ghi đè lần lưu của Studio.

## Bố cục hiện tại: viền thẻ vàng, hộp mua 300×140

Theo phản hồi tiếp theo, mỗi thẻ mặt hàng có **một viền vàng sáng mảnh bao ngoài**, nền tối trong suốt giữ nguyên; không có viền lồng quanh ảnh, tên hoặc giá. Bỏ đường ngăn bổ sung vì viền thẻ đã phân tách các mặt hàng.

Hộp mua thu tiếp xuống **300×140**. Tên món, số lượng, tổng giá và hai nút được đặt sát hơn. Thông báo số lượng sai dùng chung vùng tổng giá, nên không phải chừa một hàng trống riêng. Khung `confirm-tight.spr` được đổi kích thước cơ học từ bộ khung popup nhỏ riêng đã tạo, không từ khung cửa hàng lớn. Tài nguyên native hiện có 45 file / 95 frame, đã qua bộ đọc production. Luồng mua và tooltip giữ nguyên. Chỉ cần thay Game.exe, không khởi động lại server.

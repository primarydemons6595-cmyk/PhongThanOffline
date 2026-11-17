# Sửa bố cục giao dịch, sửa đồ và PK bot — 10/11/2026

Bản sửa này thay thế phần bố cục 10×4 ghi trong báo cáo trước. Lần sửa trước đã lấy kích thước bộ nhớ giao dịch của core làm kích thước UI; đó là quyết định sai. Hai cửa sổ phải dùng hợp đồng riêng theo ảnh nền gốc.

## Bố cục và chuyển vật phẩm

- Oẳn tù tì: **5 cột × 1 hàng**, tối đa 5 đơn vị ô mỗi bên. Dùng riêng vị trí ô, tiền, nút chọn và kết quả từ INI gốc.
- Giao dịch: **5 cột × 3 hàng**, tối đa 15 đơn vị ô mỗi bên. Tiền của mình nằm dưới hàng thứ ba.
- Bỏ lớp phủ lưới có alpha ngoài phạm vi 0–32 của bộ vẽ cũ, nguyên nhân hình nhiễu màu. Giữ ảnh nền gốc.
- Mở ở cạnh trái, căn chiều cao theo F4 và giới hạn màn hình. Xóa độ lệch hoạt ảnh cùng cờ bám cạnh phải/dưới để `Show`, `Paint` hoặc đổi độ phân giải không đẩy cửa sổ ra sau F4.
- Đối chiếu trực tiếp SPR: khung tiền giao dịch nằm tại x=41..93, y=155..170 và y=314..329. Vùng chữ được đặt bên trong khung; vùng nhập không tràn sang chỗ trống.
- Core vẫn giữ cấu trúc lưu nội bộ cũ để tránh thay ABI. Kiểm tra đặt đồ trên máy chủ giới hạn x/y và kích thước theo loại phiên; vị trí ngoài 5×1 hoặc 5×3 bị từ chối. Bot tìm chỗ bằng cùng giới hạn. Chuyển đồ thất bại trả về ô cũ, không bỏ vật phẩm trên tay.
- Giữ chuyển đồ từ cả bốn túi, chuột phải rút về túi đang chọn, khóa/xác nhận giao dịch và hoàn trả khi hủy.

## Sửa trang bị trong hành trang

Người dùng xác nhận lỗi có ở nút **Sửa trang bị**, với cả đồ độ bền 0 và đồ chỉ hao vài điểm. Các chỗ chặn đã sửa:

- Client và server nhận độ bền 0; vẫn từ chối đồ không có độ bền hoặc đã đầy. Đồ hao 1 điểm được xét như đồ hao nhiều điểm.
- Báo giá sửa luôn dùng bạc mang theo, không lấy nhầm đơn vị tiền từ cửa hàng trước đó.
- Công thức số nguyên cũ có thể làm giá sửa món rẻ/hao ít về 0, trong khi server từ chối giá 0. Giá sửa hợp lệ dương có mức tối thiểu **1 bạc**; phép tính dùng trung gian rộng và chặn tràn số.
- Máy chủ kiểm tra sở hữu, vị trí đang mặc hoặc trong bốn túi, khóa/giao dịch/vật phẩm trên tay, độ bền hiện tại và đủ bạc trước khi thu tiền. Không thu lại với đồ đã đầy.
- Bot dùng cùng đường sửa có trả bạc, gồm đồ hỏng hẳn.

## Vì sao bot tự bật PK tự do

`BotAutonomyPolicy` của core và `BotAutonomyProfile` của Event trước đây đều mặc định bật `ProactivePvp`. Không có `BotAIgent/autonomy.ini` trong phiên kiểm tra, nên mặc định có hiệu lực. Danh sách bản đồ trống được coi là mọi bản đồ. `BotPvpTick` chọn mục tiêu rồi gửi yêu cầu chuyển sang camp 8 (PK tự do). Khi trận kết thúc, việc trả camp qua đường đổi PK của người chơi có thể bị thời gian chờ chặn.

Hiện mặc định chủ động PvP **tắt** ở cả core và Event/Studio. Nếu bật lại phải có bản đồ được chọn rõ. Tự vệ/hỗ trợ đồng minh không tự cấp quyền chuyển PK tự do. Việc trả trạng thái PK tạm thời dùng đường nội bộ dành riêng cho bot, không chờ cooldown của thao tác người chơi và không vượt khóa phe/sự kiện/vận lương. Camp 8 cũ khi chính sách đã tắt được trả về trung lập ngoài lệnh chiến đấu do người chơi chỉ định và ngoài vùng/khóa sự kiện. Không xóa điểm PK. JSON trạng thái bổ sung `PkCamp`, `PkState` để đối chiếu thực tế.

## Kiểm chứng và triển khai

- CoreServer thật trong fixture cô lập: đủ 5/15 ô, từ chối ô thứ 6/16 và các tọa độ ngoài vùng, hoàn trả đồ/ô/tay chính xác; chuyển cả bốn túi và hủy; sửa đồ hỏng 0, hao 1 điểm và món giá thấp thu đúng bạc, không thu lặp.
- Game UI thật trong fixture: `Open/Show/Paint` ở 1024/1536/1920, đổi kích thước khi cửa sổ đang mở, tọa độ neo trái và tiền trong khung; bấm chuột vào matrix hành trang → hộp xác nhận sửa hiển thị → nút xác nhận gửi đúng vật phẩm/túi. Các kiểm thử tooltip, hội thoại và tin chiến đấu trước đó vẫn qua.
- CoreClient thật: đồ hỏng/hao ít ở cả bốn túi, giá bằng bạc và tối thiểu 1 bạc, tên quái, 99 sprite cùng các kiểm thử skill trước đó.
- CoreServer thật: mặc định tắt, danh sách bản đồ rỗng không kích hoạt chủ động PvP, trả camp cũ không chờ cooldown, giữ khóa sự kiện/nhân vật người thật/lệnh trực tiếp, tự vệ không tự bật PK tự do.
- Event Runtime: mặc định tắt, tự vệ giữ nguyên, cấu hình bật PvP kèm bản đồ vẫn hợp lệ. Assembly chạy kiểm thử trùng mã băm assembly cài.

Chỉ cài vào `PhongThanRuntime-Loose`, gồm Game.exe, CoreClient.dll, CoreServer.dll và Event Runtime DLL ở Server/ManagerStudio. Không build hoặc cập nhật bộ `PhongThanRelease` thật. Biên nhận cùng mã băm và bản sao lưu: `corrected-deployment-installation.json`, `corrected-deployment-final-installation.json` trong `analysis/server_chat_crash_20261109`.

Ngũ Chuyển Càn Khôn, Tụ Hoa Hóa Thương và phần mở/cộng thuộc tính Sách Phong Thần vẫn chưa hoàn tất. Bản sửa này chưa chứng minh tải 1.200 người thật ổn định.


## Sau c?i

GameServer 31324 / Event 78000 ?? s?n s?ng, lease h?p l?, ?? 200 bot. C? 200 c? `PvpMode=0`; camp g?m {7: 27, 0: 173}; kh?ng c? camp 8. Event API x?c nh?n `ProactivePvp=false`, `SelfDefence=true`. Kh?ng th?y d?ng l?i Packing/sync trong console phi?n m?i. N?m t?p c?i kh?p m? b?m.

Ba c?a s? ?o sau kh?i t?o c? 542 nh?p, 16 nh?p v??t ng?n s?ch; trung b?nh 23.600 ms, c?c ??i 74.484 ms. ??y l? m?u 200 bot, kh?ng ph?i ph?p th? 1.200 ng??i. K?t qu? trong `corrected-runtime-verification.json` v? `corrected-200-stable.json`.

Ki?m th? UI g?i m? c?a s? th?t v?i renderer/core ghi nh?n y?u c?u; thanh to?n v? ?? b?n ???c ki?m ri?ng tr?n CoreServer/CoreClient th?t trong fixture. Ch?a x?c nh?n thao t?c s?a b?ng nh?n v?t GameMa sau c?i; game ?ang ??ng.

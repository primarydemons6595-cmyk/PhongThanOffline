> Ph?n b? c?c 10?4 v? k?t lu?n UI trong b?o c?o n?y ?? ???c s?a l?i. Xem [b?n s?a b? c?c g?c v? PK bot](TRADE_RPS_REPAIR_PK_FIX_20261110.md).

# Giao dịch, oẳn tù tì, sửa đồ và tên chiến đấu — 10/11/2026

Đã cài vào `PhongThanRuntime-Loose`: Game.exe, CoreClient.dll, CoreServer.dll và 32 tệp giao diện/bảng Sách Phong Thần lấy nguyên bản. Biên nhận, mã băm trước/sau và thư mục sao lưu: `analysis/server_chat_crash_20261109/trade-deployment-installation.json`. Không cập nhật bộ Release thật, không chỉnh nhân vật hoặc cấp vật phẩm.

## Thay đổi

- Giao dịch và oẳn tù tì mở cạnh F4, theo chiều cao F4 và giới hạn màn hình. Lưới đặt vật phẩm hiển thị đúng tọa độ 10×4 của máy chủ; nền lưới che hình minh họa năm ô cũ. Đồ có kích thước nhiều đơn vị vẫn chiếm đủ diện tích.
- Hành trang 2, 3, 4 dùng cùng đường chuyển đồ với túi đầu. Chuột phải từ phần vật phẩm của mình trả về túi đang chọn, rồi tìm túi khác còn chỗ. Giữ kiểm tra khóa, xác nhận giao dịch và vật phẩm đang cầm. Hủy giao dịch khôi phục cả bảng chiếm ô của ba túi phụ.
- Oẳn tù tì trả vật phẩm thắng về đúng túi do bộ tìm chỗ trống chọn. Trước đây bộ tìm chỗ xét cả bốn túi nhưng bước thêm vật phẩm lại luôn dùng túi đầu.
- Chế độ sửa đồ nhận nhấc/click trang bị trong hành trang. Sửa toàn bộ xét đồ đang mặc và cả bốn túi, lấy giá hiện tại và gửi từng yêu cầu sửa qua máy chủ. Bot cũng xét trang bị mang theo qua đường sửa có trả bạc hiện có.
- Tin chiến đấu và sát thương dùng tên NPC đã được client giải mã và hiển thị trong thế giới; gói tin sai bản đồ vẫn bị loại.
- Nút Sách Phong Thần mở đúng cửa sổ tài nguyên gốc, bốn trang theo ba phái và thanh cuộn. Hiện mới là cửa sổ xem: chưa có giao dịch mở nút, tiêu tài nguyên, ấn ký hay cộng thuộc tính trên máy chủ.
- Phục hồi phần thực thi có bằng chứng gốc: Phúc Trạch Thiên Hựu tăng hồi phục của Bổ Tâm Chú trên thú triệu hồi thực sự; Cuồng Đao Tế và Ngự Sấu Bạo Phong dùng template/chi phí gốc. Không dùng các ô nhiệm vụ biến hình cũ cho hai thú mới để tránh đè ô nhiệm vụ thư.

## Kiểm tra

- 533 kiểm tra bộ thanh toán oẳn tù tì thật, gồm bốn túi, thiếu chỗ, khóa/ràng buộc và hoàn tác.
- CoreServer thật trong môi trường cô lập: gửi/rút đồ cả bốn túi, ô cuối x=9/y=3, từ chối khi khóa, hủy trả đúng vị trí cũ, không nhân đôi đồ; sửa trang bị trong từng túi thu đúng bạc và không thu lại khi đã sửa đầy.
- Game UI thật: chuột phải gửi/rút cả bốn túi, báo giá sửa dùng đúng túi, hai lưới đủ 40 đơn vị và từ chối đơn vị 41; 12 tổ hợp phái/trang Sách Phong Thần và cuộn.
- CoreClient thật: gói chiến đấu lấy tên Việt từ NPC trong thế giới và loại gói của bản đồ cũ; 99 lần tải/vẽ sprite, cùng các kiểm tra tooltip và kỹ năng trước đó.
- Bản cài không có export probe hay chế độ chạy kiểm thử; mã nguồn trong cây build sạch khớp nguồn chính. 22 sprite được cửa sổ Sách sử dụng đã có trong Loose. Hai sprite ở các mục hiệu ứng không sử dụng của INI gốc chưa có, không được giả bằng ảnh khác.

## Phần còn lại

Ngũ Chuyển Càn Khôn đã xác định đúng bốn lớp ảnh gốc gồm khung đỡ; HUD tích năng lượng/phần thi triển và Tụ Hoa Hóa Thương chưa phục hồi hoàn chỉnh. Không coi cửa sổ Sách hoặc đủ icon là bằng chứng mọi kỹ năng đã chạy đủ. Chưa có bằng chứng tải 1.200 người thật ổn định; các điểm nghẽn lưu nhân vật dùng chung vẫn cần tối ưu tiếp. Vé/ghép vé Vạn Tiên Trận chưa nằm trong bản sửa này.


## Ki?m tra sau c?i

GameServer PID 75240 v? Event PID 4028 ?? s?n s?ng, lease h?p l?, ?? 200 bot, k?t n?i Database/Relay/Gateway th?nh c?ng. Kh?ng th?y `Packing sync data failed` ho?c `SYNC_SEND_FAILED` trong console phi?n m?i. Ba c?a s? ?o sau kh?i t?o: t?ng 541 nh?p, 69 nh?p v??t ng?n s?ch, trung b?nh 33.519 ms, c?c ??i 140.858 ms. Thu ?? m?u kh?ng c? ngh?a ?? h?t lag; ch?a ??t b?ng ch?ng ?n ??nh 1.200 ng??i th?t. Console c?n thi?u m?t s? script chuy?n sinh/ho?n h?n. Chi ti?t: `trade-runtime-verification.json`, `trade-200-stable.json`.

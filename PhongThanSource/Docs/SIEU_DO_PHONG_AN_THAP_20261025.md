# Siêu Độ và Phong Ấn Tháp — 25/10/2026

## Siêu Độ

Script bổ sung trước đây chỉ cấp buff/thời hạn phướn, không tạo Chiêu Hồn Trận và không xét vị trí quái. Đây là lỗi của phần bổ sung.

Đã đối chiếu script Chiêu Hồn Phướn đang giữ logic gốc: tạo NPC 551–554, dùng SPR skill001–skill004, thời gian 200 giây. Client có đủ cả bốn SPR. Bản sửa dùng lại các mẫu đó, đặt trận cố định tại vị trí sử dụng phướn. Server chỉ ghi nhận quái đúng mục tiêu bị hạ trong bán kính 400 MPS quanh trận còn hiệu lực; kiểm tra thực thể, serial, chủ sở hữu và bản đồ. Thời gian hiệu lực lưu trong task hoặc buff không đủ để được ghi nhận. Tạo trận thất bại không mất phướn; không đặt thêm khi trận cũ còn hoạt động. Hủy nhiệm vụ, đổi bản đồ hoặc thoát phiên sẽ dọn trận.

Kiểm tra đạt: 52 kiểm tra engine trận, 88 kiểm tra hành trình Lua4, 366 kiểm tra thưởng hàng ngày và 144 kiểm tra áp tải/Tống Tửu. Build CoreServer Release thành công. Đã xuất bản ba file server, không thay binary client. World 99284 đã kết nối Database/Relay/Gateway, stderr trống; 334 hash và 99 vị trí nhiệm vụ cũ đạt trước lần sửa tháp tiếp theo. Dừng world trước sạch, exit 0, dữ liệu nhân vật không đổi trong khi thay file.

## Phong Ấn Tháp — Thiên Cương Tinh Quy Vị

Nguyên nhân sai hình: bản phục dựng trước gán template 399, có NpcResType `building031`, là nhân sâm/ô đất. Đây không phải thiếu SPR tháp. Đã tìm và xem trực tiếp tài nguyên `passerby102`, đúng hình tháp người dùng đưa. Bản sửa chỉ chuyển sáu bản ghi NPC tháp sang template 790 có tài nguyên này; không sửa template 399 dùng ở nơi khác. Tháp Bắc Hải chuyển từ (190,206) về vị trí người dùng xác nhận **(217,196)**; chọn ô đi được trong tọa độ này và bỏ bản ghi tháp cũ.

Đã kiểm tra lại PAK gốc trước khi chốt sửa: **sáu script Phong Ấn Tháp và script Dương Tiễn đều tồn tại, khớp nguyên byte với bảy file gốc đang có trên Loose**. Bằng chứng gốc nêu sáu bản đồ theo thứ tự: Bắc Hải, Yến Sơn, Miêu Cương, Cự Lộc, Thủ Dương Sơn, Tây Côn Lôn. Vì vậy có tháp ở Yến Sơn là có cơ sở từ script gốc. Nhận định trước đó rằng toàn bộ cách chia sáu nhóm là tự suy diễn là chưa chính xác; tọa độ các tháp ngoài Bắc Hải vẫn là vị trí bố trí của bản phục dựng, chưa xác nhận tọa độ server gốc.

Lần sửa hiển thị này **không chuyển toàn bộ chuỗi phục dựng trở lại script gốc**, không viết thêm logic nhiệm vụ và không thay đổi phần thưởng/trạng thái. Các bản sao hội thoại đang chạy vẫn dùng phần phục dựng trước; 70 file có bản sao bảng chỉ dẫn được sửa cùng một tọa độ Bắc Hải để tránh chỉ dẫn mâu thuẫn. Menu các NPC khác không đổi. Bảy script gốc giữ nguyên để đối chiếu và kiểm tra tương thích riêng; không coi kết quả kiểm thử phần phục dựng là bằng chứng tương đương hoàn toàn bản gốc.

Kiểm tra đạt: 70 payload Lua4 nạp được; 37 hành trình/2.147 điều kiện, gồm đủ 36 sao. Các bản ghi NPC khác giữ nguyên, mọi section địa hình ngoài NPC giữ nguyên byte. Publisher nội dung thử đọc lại 77 file khớp hash. Đã xuất bản 77 file nội dung server và khởi động lại world; không thay binary hay SPR client. Kết quả vị trí trực tiếp cuối nằm trong biên bản bên dưới. Chưa nghiệm thu trực tiếp hình tháp/trận trong game client.

World cuối **109920** đã kết nối Database/Relay/Gateway, stderr trống. Đạt **335 hash**, **99 vị trí nhiệm vụ** (đã dùng vị trí tháp mới), đủ sáu tháp template 790, tháp Bắc Hải đúng (217,196), bản ghi tháp ô đất cũ không còn. Đạt sáu NPC hàng ngày, bảy NPC Sùng Thành và vị trí Trấn Nguyên. World trước thoát sạch, exit 0; dữ liệu nhân vật không đổi khi thay file. Bảy script gốc được kiểm tra hash lại sau cập nhật và vẫn nguyên vẹn.

Bằng chứng:

- `analysis/sieu_do_circle_20261025/{manifest,publication,live_verification,clean_stop}.json`
- `analysis/tower_20261025/{manifest,original_evidence,publication,live_verification,clean_stop}.json`
- Gói nội dung: `Deploy/ProjectContent/PhongAnThap20261025`, publisher được gọi sau gói nhiệm vụ hàng ngày/Trấn Nguyên để giữ sửa lỗi khi triển khai lại.

Nguồn web tham khảo: [hướng dẫn Thiên Cương Tinh Quy Vị](https://pt.vnggames.com/cam-nang/nhiem-vu/tay-ky.html). Bản đồ sáu nhóm được xác nhận trực tiếp bằng script Dương Tiễn trong PAK, không suy ra chỉ từ bài hướng dẫn này.

# Event XP, đồ rơi theo map và nạp lại server

Ngày: 25/10/2026. Phạm vi: PhongThanRuntime-Loose; không sửa Client_Original.

## Cách dùng

Mở Studio → Quản lý → **Event XP / Drop**.

1. Bấm **Thêm event**, nhập tên, chọn **Bật event**.
2. **Chọn map cho event**: chọn nhiều map theo tên hoặc **Tất cả maps**.
3. Đánh dấu tăng XP, nhập 2 để x2 hoặc 10 để x10. XP này chỉ áp dụng khi đánh quái; không nhân thưởng nhiệm vụ.
4. Có thể tăng lượt quay đồ mặc định bằng hệ số 1–10. Đồ mặc định vẫn được quay trước. Tiền rơi không bị nhân; giới hạn vật phẩm native vẫn được giữ.
5. Để rơi thêm nguyên liệu, bật rơi thêm, tìm vật phẩm theo tên, chọn kết quả rồi bấm **Thêm vật phẩm**. Sửa tỷ lệ %, số lượng; chọn map cho từng dòng. Có thể bật/tắt từng dòng riêng.
6. Nhập ngày bắt đầu/kết thúc, chọn thứ trong tuần và giờ Việt Nam nếu cần. Để trống ngày nghĩa là không giới hạn ngày. Ví dụ cuối tuần chọn Thứ 7/CN, giờ 00:00–24:00. Khung giờ qua đêm chia thành hai event.
7. **Lưu và áp dụng**. Xem số phiên bản server xác nhận ở cuối giao diện. Bình thường server nhận trong khoảng một giây khi world đang hoạt động. Lưu khi server tắt sẽ được nhận khi server mở lại.

Ví dụ Tết: chọn tất cả map cần tham gia ở phạm vi event; thêm Gạo nếp và Dây lạt với nhóm map A, Đậu xanh và Lá dong với nhóm map B. Muốn nhóm C nhận x10 XP thì tạo một event XP riêng cho nhóm C. Chỉ chọn được vật phẩm đã có trong bảng hiện tại; hệ thống này không tự tạo ID hoặc hình ảnh nguyên liệu mới.

Tắt event rồi lưu sẽ dừng XP/đồ rơi ở các lượt hạ quái tiếp theo. Vật phẩm đã rơi không bị xóa. Nhiều event trùng map: XP và hệ số đồ mặc định lấy mức cao nhất; các dòng rơi thêm vẫn quay độc lập. Không nhân chồng hệ số.

## Các nút nạp lại

| Nút | Phạm vi thực tế |
|---|---|
| Nạp script này | Nạp đường dẫn Lua trong server; biên dịch thành công mới thay VM đang dùng. |
| Nạp toàn bộ script | Nạp lại các script đã đăng ký, chia theo từng nhịp world; giữ ID và alias. Không tự quét thêm toàn bộ script mới chưa được đăng ký. |
| Nạp TXT hỗ trợ | Áp dụng snapshot event; chính sách/bảng drop native vốn được đọc trực tiếp ở lần hạ quái kế tiếp. Không tuyên bố nạp lại mọi TXT. |
| Nạp map đã chọn | Nạp lại va chạm và trap của map đã mở. Chờ người chơi rời map; giữ NPC, vật phẩm, mission, danh sách region. |
| Nạp tất cả phần hỗ trợ | Script đã đăng ký, event/drop ở trên và geometry/trap các map đủ điều kiện. Bỏ qua bốn map VTT được bảo vệ. |
| Dừng lệnh nạp lại | Dừng phần việc chưa thực hiện, gồm lệnh chờ map hết người; không hoàn tác các file đã nạp. |

**Chưa hỗ trợ reload nóng bảng item/skill/NPC, NPC/object spawn tĩnh của region, cấu trúc/danh sách map, ảnh SPR và dữ liệu client.** Những thay đổi này vẫn cần khởi động lại thành phần tương ứng. Map VTT không bị thay geometry trong phiên đang vận hành. Không dùng Close/LoadMap trên world đang có nhân vật.

Lệnh gắn với PID server và có thời hạn 60 giây; không phát lại lệnh của phiên server cũ. Các script sai cú pháp giữ VM cũ. Nạp nhiều script không phải giao dịch toàn bộ: file hợp lệ áp dụng, file lỗi giữ cũ; bộ đếm lỗi và `Server/EventService/reload.errors.log` cho biết file lỗi. Reload script có thể đặt lại các biến Lua toàn cục của script đó.

## Dữ liệu và kiểm chứng

Studio giữ JSON có phiên bản, bản sao `.bak`, khóa chống hai cửa sổ cùng ghi. Snapshot nội bộ được ghi nguyên tử; người vận hành không phải sửa TXT. Server đọc trên world thread và chỉ thay policy sau khi kiểm tra đầy đủ. File lỗi/ghi dở giữ policy tốt cuối cùng. Cấu hình mới mặc định không có event bật.

Giới hạn: 32 event, 128 dòng vật phẩm, tổng lượng của các dòng bật tối đa 64/lượt, mỗi dòng 1–20 vật phẩm. Tỷ lệ tối đa bốn chữ số thập phân; hệ số tối đa ba chữ số thập phân. Map và tuple vật phẩm được đối chiếu dữ liệu hiện có trước khi lưu.

Bằng chứng build/test nằm trong `analysis/live_campaigns_20261025/`: test C# cấu hình/lịch/lưu/xung đột, test native policy + Lua registry thực, test tăng lượt quay trên drop plan thực, ảnh render giao diện WPF. `publication.json` và `live_verification.json` chỉ được tạo sau khi thực sự áp dụng/kiểm tra server.

## Kết quả triển khai

- Đã áp dụng CoreServer và Studio; world PID **112376**, đủ **105 map**. World trước thoát mã **0**, dữ liệu nhân vật không thay đổi trong lúc thay file.
- **132** kiểm tra native, **20** kiểm tra C# đạt. Tăng lượt quay giữ nguyên lượt gốc và tiền, tôn trọng giới hạn native; reload cùng script 100 lần không tăng số slot; lỗi cú pháp giữ VM cũ.
- Kiểm tra trực tiếp cùng PID: nhận cấu hình lịch tương lai, từ chối snapshot lỗi và giữ bản tốt, nhận lại bản hợp lệ qua thao tác lưu phiên bản mới, reload Lua phù, reload TXT hỗ trợ và geometry/trap map **1014** đều hoàn tất không lỗi. Cấu hình thử đã gỡ; phiên bản cuối **7**, danh sách event trống.
- Đã sửa xung đột Windows khi Studio đọc đúng lúc server thay file trạng thái: mở với quyền chia sẻ đọc/ghi/xóa và thử lại khi có khóa tạm thời. Snapshot cũng thử lại khi gặp khóa đọc ngắn hạn.
- Đối chiếu **357** file runtime và các vị trí nhiệm vụ cũ đạt; một khác biệt region Đào Trì đã có từ biên bản trước được giữ nguyên. 15 đối tượng thu thập và 99 vị trí mục tiêu nhiệm vụ vẫn được kiểm tra.
- Giao diện WPF đã render từ binary triển khai. Chưa dùng một nhân vật thật để đánh quái nhận vật phẩm event; logic quay đồ và XP đã được kiểm thử native, cấu hình/reload đã kiểm tra trên server chạy thật.

# Bot, Thần Kỹ và túi phụ — 09/11/2026

Phạm vi: Loose. Gói và biên nhận ở `analysis/bot_event_layout_20261109`. Chỉ coi là đã cài khi có `installation.json`; kiểm chứng server sau cài ở `live-verification.json`.

## Nguyên nhân và thay đổi

- Event: console bot chiếm cố định 200 điểm chiều cao dù rỗng. Chuyển thành vùng thu gọn mặc định; bảng bot chiếm phần còn lại và giãn theo cửa sổ. Vẫn mở console, lọc và xuất log được.
- Bot tân thủ: nhánh mục tiêu Lua chọn Thủ Khố đầu tiên cùng mức ưu tiên, không tính đường đi/khoảng cách; bộ chọn bãi luyện cũng xét khu tân thủ của cả ba phái. Bổ sung đánh giá khoảng cách/khả năng đi tới, chọn Thủ Khố quê nhà trước cấp 20; luyện tự động, nguồn nguyên liệu và mua bán tự động tránh khu tân thủ phái khác. Nhiệm vụ chủ tuyến rõ mục tiêu và yêu cầu trực tiếp từ người chơi vẫn có luồng riêng; không dịch chuyển cưỡng bức hoặc sửa vị trí lưu. Ưu tiên chủ tuyến khi bot còn tân thủ. Thêm trường mục tiêu chỉ đọc vào trạng thái để kiểm tra NPC/map đang nhắm tới.
- Thần Kỹ: `KPlayer::UpdataCurData` xóa mọi kỹ năng tạm khi tính lại trang bị. Sách vẫn còn nên lease còn hợp lệ, nhưng kỹ năng thực thi đã mất; kích hoạt lại đúng sách/cấp lại thoát sớm. Giữ kỹ năng Thần Kỹ khi lease/sách trong hành trang còn hợp lệ, giữ hồi chiêu, gửi lại trạng thái và khôi phục slot nếu kích hoạt lại cùng sách. Không trừ thêm đá cho lần kích hoạt lặp; sách cấp khác vẫn thay thế một slot; đưa sách ra ngoài hành trang vẫn thu hồi quyền dùng.
- Tooltip: sáu nhóm Thần Kỹ xóa mô tả gốc và cờ Attrib=1 khiến phần thuộc tính/cấp bị ẩn. Giữ mô tả, bổ sung loại/cấp, hồi chiêu nếu có và sát thương tính qua cùng hàm ghép thuộc tính chiến đấu. Không đổi tooltip kỹ năng phái; không đưa ghi chú Loose/Studio vào game.
- Link vật phẩm: thao tác click trước đó chỉ gọi tooltip rê chuột. Click nay mở cửa sổ chi tiết `KUiChatItem`; xem trước rồi thu hồi đối tượng tạm, không đưa vật phẩm vào hành trang.
- Giá bot: hệ số định giá mặc định quá cao. Giảm hệ số mặc định và phần giá theo chất lượng tương ứng, giữ bảng giá do quản trị cấu hình riêng; làm tròn giá tham chiếu/gian hàng/tin rao theo số vạn nguyên. Một xu vẫn tương đương 60 vạn theo cấu hình trước. Giá rao và giá gốc để giao dịch dùng chung, không chỉ làm đẹp chữ.
- Túi phụ: `FindRoom` quét cột trước trên cả lưới 7 hàng. Khi các ô đầu cột đã đầy tới hàng mở thứ 5, nó trả ô trống ở hàng khóa thứ 6; hàm gọi loại cả túi, bỏ sót ô trống ở cột sau. Thay bằng tìm hình chữ nhật trong đúng hàng đã mở. Các luồng nhận/nhặt/mua dùng lại cùng hàm; không mở miễn phí hàng khóa, không di chuyển hoặc xóa đồ.

## Kiểm tra

- WPF: 200 dòng, cửa sổ 760×500, 1050×700, 1600×1000, console đóng/mở; bảng giãn đúng, ảnh trước cài có trong thư mục analysis.
- Native: đổi hai tab trang bị 5 vòng giữ Thần Kỹ và hồi chiêu; kích hoạt lại cùng sách khôi phục slot mất mà không thu thêm đá; nâng cấp thay đúng một slot; bỏ sách ra ngoài không được giữ kỹ năng.
- Native: túi phụ mở 5 hàng, đầy cột đầu nhưng trống cột sau — tìm được cả ô 1×1 và 2×3, từ chối vật phẩm cao hơn vùng mở.
- Native: bot chọn đúng Thủ Khố và bãi tân thủ; giữ hồi quy nhiệm vụ Studio mới, điểm/ngày, mua/ghép native, Phi Tuyết ba phái và công thức cập nhật.
- Client: 15 nhóm Thần Kỹ có mô tả, không chứa Loose/Studio; Huyết Nguyệt Trảm gồm mô tả gốc và sát thương đã ghép. Bộ render giả chỉ phục vụ kiểm thử tooltip, không có trong DLL cài đặt.
- Giá/click: mã handler thật được trích để xác nhận click mở cửa sổ và giải phóng preview; giá mặc định mẫu 78 vạn, không phần lẻ, giá tùy chỉnh và tỷ lệ xu được giữ.
- Build Dev tách biệt, loại toàn bộ export probe trước đóng gói. Không build/cập nhật `PhongThanRelease`.

Kiểm thử tự động không thay thế nghiệm thu phiên chơi của người dùng cho hình ảnh tooltip/link và chuỗi đổi trang bị trên nhân vật đang dùng. Không hoàn sách hoặc chỉnh dữ liệu GameMa.

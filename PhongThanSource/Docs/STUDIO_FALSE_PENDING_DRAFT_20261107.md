# Studio hỏi bỏ chỉnh sửa ngay sau khi mở — 07/11/2026

Đã sửa và cài vào Studio Loose. Không build runtime Release, không thay Game/GameServer hoặc dữ liệu sự kiện.

## Nguyên nhân đã tái hiện

`CustomEventEditorView.Reset()` khôi phục tệp bản nháp Event khác và đặt `HasUnstaged=true` chỉ vì tệp tồn tại. Bản nháp `e1.json` hiện có giống hoàn toàn định nghĩa sự kiện đã lưu, nên không có chỉnh sửa thực tế. Khi khởi động với workspace hoặc bấm Nạp lần tiếp theo, cờ này khiến hộp “Bỏ patch/chỉnh sửa đang chờ để nạp workspace?” xuất hiện.

Không phải patch được tự áp dụng, cũng không phải lỗi dữ liệu server. Chẩn đoán trước sửa ghi nhận riêng `CustomEvents=true` sau nạp workspace; các mục còn lại đều false: [before.json](../../analysis/studio_dirty_startup_20261107/before.json).

## Thay đổi

So sánh toàn bộ giá trị đã lưu của định nghĩa sự kiện trước khi khôi phục bản nháp. Không dùng so sánh tham chiếu record/array, vì các mảng được deserialize là đối tượng khác dù nội dung giống nhau. Bản nháp giống bản đã lưu không bật cờ chỉnh sửa. Bản nháp khác, gồm sửa trường lồng nhau hoặc khác phiên bản sự kiện, vẫn được khôi phục và cảnh báo như trước.

Không xóa tệp bản nháp hoặc tắt hộp bảo vệ chỉnh sửa thật. Lần cài chỉ thay hai DLL Studio và hai PDB tương ứng, có sao lưu và đối chiếu SHA sau chép. Chín tệp dữ liệu sự kiện được theo dõi giữ nguyên hash.

## Kiểm chứng

- Tái hiện lỗi trước sửa trên bộ dữ liệu hiện có.
- Chín kiểm tra model: bản sao chứa mảng độc lập nhưng cùng giá trị không bị xem là thay đổi; sửa tên, trạng thái, tọa độ NPC, số lượng hàng và phí công thức vẫn được nhận diện; khác phiên bản sự kiện không bị bỏ qua.
- Kiểm tra cửa sổ Studio bằng mã WPF: khởi tạo, dựng Liquid, nạp workspace, ba vòng đổi Classic/Liquid và nạp lại đều sạch. Sửa văn bản thật vẫn bật cờ chỉnh sửa. Không bấm xác nhận để bỏ qua hộp thoại trong bài kiểm tra.
- Chạy lại cùng kiểm tra từ bản Studio đã cài thành công: [installed.json](../../analysis/studio_dirty_startup_20261107/installed.json).
- [Biên bản cài và bản sao lưu](../../analysis/studio_dirty_startup_20261107/deployment.json).

Người dùng mở lại Studio Loose để sử dụng bản sửa. Cảnh báo vẫn xuất hiện khi có patch chờ, bản nháp khác nội dung hoặc chỉnh sửa chưa lưu thực sự.

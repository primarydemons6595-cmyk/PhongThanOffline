# Hiệu ứng lỗ và ngọc khảm — 06/11/2026

Đã sửa phần hiển thị tooltip và cập nhật **Game.exe + CoreClient.dll của Dev Loose**. Hai file được sao lưu và kiểm tra SHA sau khi chép; game đã đóng ở thời điểm cập nhật. Không reset server, không thay trạng thái khảm/thuộc tính trang bị hoặc dữ liệu nhân vật. Chưa nghiệm thu trực quan trong phiên game thật sau cập nhật.

## Đối chiếu bản gốc

Đọc trực tiếp thứ tự PAK có sẵn trong `Client_Original/package.ini`, gồm vng00 và update0_171. Toàn bộ **15 bảng khảm, 128 sprite được tham chiếu** trùng byte với Loose. Native SPR loader đọc **522 frame, không lỗi**. Bằng chứng mới lưu ở `analysis/socket_glow_20261106/original-audit.json` và `native-sprites.log`.

- Bảng lỗ: cột 4 là ảnh/animation trên trang bị, cột 5 là ảnh/animation dùng ở menu. Có lỗ cơ bản 30 frame/80 ms và nhiều vòng màu 15 frame/50 ms; không thay bằng hình tròn tự vẽ.
- Bảng ngọc: cột 6 là ảnh trên trang bị, cột 7 là ảnh dùng ở menu. Giữ từng đường dẫn và số frame của tài nguyên tương ứng; không buộc ảnh ngọc tĩnh phải xoay hoặc nhấp nháy.
- Tọa độ bốn lỗ lấy từ bảng vẽ lỗ gốc theo loại trang bị. Ngọc được vẽ phía trên lớp lỗ; mỗi slot giữ trạng thái animation riêng qua các lần paint.
- Ảnh tooltip VNG đã lưu từ đợt đối chiếu trước (`analysis/lei_restore_20261026/web/phongthan_trangtrilo1.jpg`, `phongthan_trangtrilo2.jpg`) có icon lỗ cạnh dòng mô tả. Giữ cách dùng ảnh menu hiện tại, không tự thay icon đó bằng icon ngọc.

## Lỗi đã sửa

Trước đây khi có ngọc, `PtSocketVisuals` chỉ trả ảnh ngọc cho vị trí trang bị, làm mất lớp vòng sáng lỗ. `MouseHover` còn tạo mới ảnh và dùng frame 0 mỗi lần paint. Bản sửa trả riêng `HoleImage`, giữ cache animation của lỗ/ngọc theo slot, dùng thời gian và số frame của SPR gốc, căn giữa từng lớp theo kích thước frame thực tế. Lỗ trống chỉ vẽ một lần.

Query giữa Game/CoreClient được tăng phiên bản để cặp cũ/mới không ghi quá bộ đệm. Cần cập nhật hai file cùng nhau. Layout item lưu đĩa và packet trạng thái khảm không đổi.

## Hiệu ứng cả món đồ / quanh nhân vật

Đã đọc lại trực tiếp bảng trạng thái và **21 sprite trang bị gốc**, giải mã mọi frame; ảnh và bằng chứng ở `analysis/socket_glow_20261106/world-effects/`. Các nhóm có hiệu ứng bộ, Cam, Tiên/Ma, bộ +12 và vũ khí +12. Đây là nhóm khác với sprite lỗ/ngọc trong tooltip.

Trong Loose, đồ khảm vẫn giữ loại/bộ của trang bị nền. Glow quanh nhân vật tiếp tục được chọn bởi loại bộ, số món và nâng cấp, không được cấp một vòng mới chỉ vì vừa khảm ngọc. Những mốc kích hoạt bổ sung của Loose chưa phải luật server VNG đã được chứng minh; xem `EQUIPMENT_GLOW_LOOSE_20261029.md` và `ORIGINAL_EQUIPMENT_GLOW_20261029.md`. Đợt này **không đổi** các mốc đó và không kết luận khảm bất kỳ ngọc nào phải làm cả món đồ phát sáng.

## Kiểm tra và trạng thái

- Test dùng hàm paint mới cùng `IR_NextFrame` thật: tiến frame, quay vòng, giữ cache, thứ tự hai lớp, đổi đường dẫn, lỗ trống, sprite thiếu và slot sai.
- Test dùng Engine/KTabFile thật: đủ 100 ID lỗ và 53 ID ngọc, tọa độ gốc, slot thưa/đủ bốn, không ghi trạng thái khảm.
- CoreClient và Game Dev build không lỗi; kiểm tra các import của cặp ứng viên với DLL hiện tại qua. SecureRelease native/Event build qua, source certificate được tạo bằng quy trình build chuẩn.
- `deployment.json`: `APPLIED_SHA_VERIFIED`, chỉ hai file client; CoreServer không đổi. `socket-preview.gif` là bản dựng từ sprite gốc để xem animation, **không phải ảnh chụp trong game**.
- Patch Release **Client 1.04** đã tạo tại `PhongThanRelease/Update/Client 1.04`: hai binary, không thay resource; hash và chữ ký ECDSA-SHA256 qua. Chưa chép đè bản Release cài riêng. Biên nhận: `patch-verification.json`.

Phần sửa nhiệm vụ Thiên Duyên đã được hủy theo yêu cầu người dùng: toàn bộ source và Lua đã chuẩn bị được hoàn nguyên byte; kiểm tra hash toàn manifest qua. GameMa chỉ cần lấy Phong Thần bảng từ kho mang theo; không sửa quest hoặc inventory.

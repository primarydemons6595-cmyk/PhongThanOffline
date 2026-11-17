# Sửa Đầu Lĩnh template 220 và gắn script đối thoại — 28/10/2026

Người dùng xác nhận Đầu Lĩnh dùng NPC template 220. Kết luận trước đó chọn 1097 từ NPC tộc nhân thất lạc là sai đối với NPC này. Bản sửa đổi service Cự Lộc (239,192) sang 220, giữ tên Đầu Lĩnh và script Cứu Tế hiện có.

Nguyên nhân click không mở thoại đã được xác định trong code: Services() chỉ gán m_ActionScriptID nhưng để trống ActionScript. KPlayer::DialogNpc(BYTE*) kiểm tra ActionScript[0] trước khi gọi ExecuteScript bằng hash, nên yêu cầu click hợp lệ vẫn bị bỏ qua. Log client cũng ghi nhiều click vào Đầu Lĩnh với script rỗng.

PhongThanBindNpcDialogScript() gắn đầy đủ đường dẫn và hash, kiểm tra path rỗng/quá dài/hash 0. Services() dùng helper khi tạo NPC và sửa lại cả binding của NPC đã tồn tại. Sửa này cũng áp dụng các NPC do cùng Services() tạo, gồm hai dịch vụ Ngọc Hư mới bổ sung. Không mở rộng bán kính/điều kiện quyền tương tác khác.

DialogBindingTests.cpp nạp nguyên hàm DialogNpc production trích từ KPlayer.cpp: binding chỉ hash không gọi script; binding đầy đủ gọi main; kiểm tra khoảng cách, người chơi chết, entity không hợp lệ, path không hợp lệ và repair. Loader SPR native kiểm tra template 220 Kind=3, ảnh đứng 40 frame. CoreServer Release build qua.

Gói Leader220Dialog20261028 chỉ có CoreServer.dll. Chưa áp dụng khi ghi báo cáo này; Game và Studio đang mở. Xem publication.json và post_publish_verification.json trong analysis/leader220_20261028 để biết trạng thái sau phát hành. Chưa nghiệm thu click bằng nhân vật thật sau cập nhật.

Đã phát hành: world PID 80040, đúng một Đầu Lĩnh template 220 tại Cự Lộc, không còn template 1097 ở map này. CoreServer.dll đúng hash gói; receipt xác nhận dữ liệu nhân vật không đổi tại lúc phát hành. Chưa nghiệm thu click thật sau cập nhật.

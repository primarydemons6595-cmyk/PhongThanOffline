# Hội thoại mài và hoàn nguyên ngọc — 27/10/2026

Sửa theo ảnh client gốc và đính chính của người dùng: tất cả ngọc có cùng phí mài 10 vạn, xác nhận trước khi mài. Chọn đúng tên vật phẩm trong túi, kiểm tra lại serial, phạm vi NPC, khóa vật phẩm và giao dịch ở native.

Mài thành công hiện tên ngọc đã mài và dòng thuộc tính thực vừa sinh, màu xanh lam; sao xanh dùng icon 428 chỉ khi trị số đạt tối đa. Không lấy một dòng giả để trình bày kết quả. Phần mô tả server nạp MagicDesc.ini khi cần, vì trước đây bộ này chỉ khởi tạo ở client.

Thêm Hoàn nguyên ngọc vào riêng Lôi Chấn Tử Diêu Trì. Hội thoại chỉ nêu chi phí 1 Di Quang Kính (ibitem 509, particular 2) hoặc 18 Xu (questkey 47), với nút Xác định và Hủy. Native tự ưu tiên Kính dùng được trong túi, không có Kính mới trừ Xu; không đưa màn hình chọn cách thanh toán hay giải thích thứ tự này vào hội thoại. Kiểm tra cả túi phụ mang theo; tùy chọn lấy nguyên liệu ở túi phụ chỉ bật cho giao dịch hoàn nguyên, không đổi mặc định nhiệm vụ khác. Gỡ đúng ngọc theo serial rồi dùng giao dịch native để nhận lại đúng mẫu chưa mài; giữ cờ khóa, xóa roll cũ. Nếu thất bại phải trả lại đúng ngọc/roll/vị trí, không trừ nguyên liệu hoặc thử trừ Xu thay cho giao dịch Kính thất bại. Chỉ hỗ trợ các ngọc có bảng thuộc tính/ánh xạ đã kiểm chứng.

Tooltip bỏ các dòng trạng thái mài bổ sung phía dưới. Ngọc chưa mài phân biệt bằng tên có `(chưa mài)`; ngọc đã mài chỉ có tên ngọc và thuộc tính thực. Giữ sao xanh tại dòng đạt max.

Cẩm Nang bổ sung Di Quang Kính ở nguyên liệu Ngọc Bội/ngọc; icon được trích nguyên bản, có xem trước. Chỉ cập nhật Lua riêng NPC này và phần chung của Dẫn Tủy Châm, không đổi cách hiển thị hội thoại của NPC khác.

Kiểm chứng: native 22.185 lượt sinh Ngọc Bội và các giao dịch mài/hoàn nguyên, thiếu nguyên liệu, đủ 18 Xu, đầy túi, sai NPC, serial cũ, phí 10 vạn, giữ cờ khóa và xóa thuộc tính cũ qua. Lua 4 kiểm tra sáu nhánh gốc, đục/khảm cũ, chọn món theo trang, màn hình kết quả, xác nhận hoàn nguyên và năm hệ tẩy tủy qua. Cẩm Nang 3.966 menu/callback qua. CoreServer Release build qua. Chưa kiểm tra trực quan trong Game.

Gói: Deploy/ProjectContent/GemPolishFlow20261027. Biên bản áp dụng: analysis/gem_polish_flow_20261027/publication.json.

Đã áp dụng 8 file đúng hash, world PID 131124 sẵn sàng sau khi nạp 105/105 bản đồ và 4.044 NPC/quái. Console khởi động không có ScriptError/stack traceback; hồ sơ nhân vật và PID dịch vụ phụ không đổi. Người dùng có thể mở lại Game Loose để kiểm tra trực quan.

Bản đính chính: GemPolishCorrection20261027, bằng chứng analysis/gem_polish_correction_20261027. Kiểm thử thêm có cả Kính/Xu, Kính ở túi phụ, chỉ có Xu, thiếu Xu và giao dịch thất bại không trừ đồ. CoreServer/CoreClient Release build qua. Đã áp dụng 5 file đúng hash, world PID 114888 chạy lại, hồ sơ nhân vật và dịch vụ phụ không đổi.

# Dị Nhân và Đạo Sĩ — 27/10/2026

Trạng thái: **đã áp dụng 67 file vào Loose**. Biên nhận `analysis/two_faction_quests_20261027/publication.json` xác nhận hash và world sẵn sàng; `verification.json` xác nhận sáu NPC dịch vụ xuất hiện, Khoa Phụ/Hình Thiên nối đúng script, hình học vùng và dữ liệu nhân vật giữ nguyên. World PID 110168 → 130504; các dịch vụ đăng nhập giữ nguyên PID.

## Đối chiếu trước khi bổ sung

- Đọc thứ tự gói trong `Client_Original/package.ini`, lấy bản có hiệu lực thay vì chọn theo tên file hay ngày sửa file rời.
- Kiểm tra 111 Lua gốc liên quan nhiệm vụ hai phái và các NPC tham gia; không thiếu file trong tập đường dẫn đã tìm thấy. 88 file giữ nguyên byte, 90 file có phần đầu trùng bản gốc trong Loose trước lần sửa này. Đây là tập đã tìm được, không phải khẳng định mọi script phía server VNG đều nằm trong client.
- Pha nhiệm vụ, mã vật phẩm và luồng NPC đối chiếu Lua gốc; kinh nghiệm/tiền đối chiếu hướng dẫn Tam Giới hiện tại khi khác Lua cũ:
  - [Dị Nhân](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-di-nhan.html)
  - [Đạo Sĩ](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-dao-si.html)
- Băng Lang: 1.500 kinh nghiệm do người dùng xác nhận, cộng một Biến Thân Phù ngẫu nhiên. Hiện dùng 27 loại phù cơ bản có sẵn, ID 42–68; chưa có bằng chứng xác định chính xác tập phù và xác suất của server VNG.
- Tam Sách nhận một pháp bảo ngẫu nhiên, không phát cả bốn. Khảo Nghiệm/Thần Khí nhận pháp bảo ngẫu nhiên theo tập mã Lua gốc.

## Phạm vi gói

153 chuyển bước, nối vào 49 script NPC; giữ nội dung dịch vụ cũ và chỉ bổ sung nhánh theo đúng phái/trạng thái. Không sửa bộ dựng hội thoại chung của client.

- Đạo Sĩ: Bách Lý, Ngũ Thất, Khảo Nghiệm, Linh Lực, Băng Lang; Chinh Đồ, Thế Sở, Thần Oanh, Thần Mộc và phần chủ tuyến chung tiếp theo.
- Dị Nhân: Khai Trí, Tân Thức, Thần Khí, Cứu Tế; Mao Lư, Bất Túy, Cổ Đao, Ma Huyết và phần chủ tuyến chung tiếp theo.
- Hai phái: Trừ Họa, Giáo Huấn; các nhánh Tây Kỳ/Triều Ca và dịch vụ hiện hữu vẫn được giữ.
- Giáo Huấn chỉ chọn quái có sẵn quanh phái. Không nhập quái từ bản đồ khác về thôn. Đếm 40 con; 3 lượt/ngày trước cấp 29, một lượt cuối từ cấp 29.
- Trừ Họa dùng ấn ký 10 giây, nhặt sâm/liễu qua thanh tiến độ sẵn có; Ngưu Sát đúng template 12 tại Đồng Quan mới cấp Tĩnh Hóa Phù cho hai phái này. Không tính Hắc Phong template 18 thành Ngưu Sát trong nhánh mới.
- Băng Lang đếm 20 sói mỗi lần gọi một Vương, diệt ba Vương rồi trả Nam Cực Tiên Ông; Vương có chủ sở hữu, chống tính lặp xác và dọn khi đổi bản đồ/thoát nhân vật.
- Bốn vật phẩm nhiệm vụ 34/142/181/9201 nối riêng task 1/2; nhánh Giáp Sĩ hiện hữu được giữ.
- Hướng dẫn tự cập nhật, vật phẩm vàng/NPC xanh/quái đỏ, có số lượng cần thu thập. Bản ghi không đổi được bộ đệm server chặn gửi lặp.
- Sửa lỗi gán Series của phần thưởng áo/mũ/đai/giày: mã phái 1/2 không phải mã bộ trang bị. Dùng bộ nhiệm vụ gốc 6–10 và 11–15 để tránh lỗi không nhận được khi F4 trống.

## Kiểm tra

- CoreServer Release build thành công.
- Lua 4 nạp cả 57 file: 49 NPC, 4 vật phẩm, 3 nhánh chết, 1 hướng dẫn. Kiểm tra 153 chuyển bước: phái/cấp/bản đồ, mở xem không trao thưởng, giao dịch thất bại không mất tiền, đúng XP/tiền, không nhận thưởng lặp.
- Bộ nạp bảng thật xác thực 105 tuple vật phẩm, có đối chiếu Series như bộ sinh trang bị; không còn tuple thiếu.
- Kiểm tra native: đếm quái đúng bản đồ, túi đầy không chuyển bước, chống đếm lặp xác, ba mảnh nhiệm vụ, Giáo Huấn dừng 40, Băng Lang/Vương có chủ, ba hồn và thời hạn đánh boss Thông Thiên.
- Bộ nạp sprite thật giải mã 550 frame hiển thị của 13 mẫu NPC/quái được dùng.
- Kiểm tra nội dung hướng dẫn không vượt giới hạn 480 byte của API bản ghi mới.

Chưa thực hiện một lượt chơi thủ công toàn tuyến trong client. Kiểm thử giao dịch Lua dùng mô phỏng người chơi/túi đồ; kiểm thử mã vật phẩm và sprite dùng bộ nạp dữ liệu thật. Không coi hai phép thử này là bằng chứng đã chơi hết trong game.

## Những chỗ chưa thể gọi là phục hồi nguyên bản tuyệt đối

Lua gốc tìm được cho hai chủ tuyến kết thúc ở pha 83; phần tiếp đến 90 được nối theo tuyến hiện có/hướng dẫn, không gắn nhãn là Lua server VNG nguyên bản. Mẫu Hiên Viên/Xi Vưu dùng tài nguyên NPC có sẵn đã xác thực; chưa chứng minh đúng mẫu hình ảnh NPC của VNG. Trạnh Nanh và Bàn Cổ ở phần chung tiếp tục dùng vị trí/mẫu hiện hữu của tuyến Giáp Sĩ, cần đối chiếu thêm hình bản gốc nếu muốn xác nhận tuyệt đối.

Mọi lần áp dụng phải kiểm tra hash nền mới nhất, sao lưu, không có nhân vật online và không có Game/Studio Loose giữ file. Không tác động Client_Original.

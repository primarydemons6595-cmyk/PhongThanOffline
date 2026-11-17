# Sửa vòng Include Tứ Tượng / Thầy Tướng Số và khôi phục một phần script chuyển sinh

## Thầy Tướng Số

Người dùng xác nhận Thủ Khố Triều Ca vẫn bấm được; Thầy Tướng Số không mở thoại. Hai script fortune teller 1020_04 / 1021_03 Include daily/tu_linh.lua → daily/runtime.lua → restoration/tutuong.lua, rồi lại Include restoration/tutuong.lua ở cuối. Include dùng chung Lua state.

Bản cũ lưu hàm cũ vào biến global PTTT_OLD_JOURNAL; lần nạp thứ hai biến này trỏ về chính wrapper, gây đệ quy vô hạn khi PTSMenu gọi PTSJournal. Fixture Lua 4 trước sửa báo stack overflow. Bản sửa lưu hàm cũ qua upvalue Lua 4 và đánh dấu PTTT_WRAPPED_JOURNAL, không bọc lại chính mình.

Kiểm tra Lua 4 sau sửa: repeated-include base journal chạy đúng một lần; bốn NPC Thầy Tướng Số/Thủ Khố Tây Kỳ/Triều Ca chạy đầy đủ cây Include qua bộ inline fixture và mở menu hai lần không lỗi. Không thay logic thu phí, vật phẩm hay thưởng Tứ Tượng.

## Script chuyển sinh

Đối chiếu toàn chuỗi PAK Client_Original. Khôi phục được 7 script gốc dạng Latin và ánh xạ sang đường dẫn GBK mà Skills.txt đang yêu cầu, trên cả Client/Server: Bách Luyện Đoán Thể, Nhất Văn Thiên Ngộ, Phong Thần Chi Lực, Dĩ Chiến Dưỡng Chiến, Nghiệp Hỏa Phần Tâm, Hạo Nhiên Chính Khí, Cổ Hoặc Chúng Sinh. Công thức giữ nguyên payload gốc. Riêng Cổ Hoặc Chúng Sinh bổ sung dispatch skill_cost_v tới hàm Getskill_cost_v đã có sẵn nhưng bị script gốc bỏ quên.

Lua 4 kiểm tra mọi thuộc tính được Skills.txt yêu cầu ở cấp 1..10, trả tuple số hợp lệ. Native Engine loose-only kiểm tra cả 7 đường dẫn GBK nạp và thực thi thành công, không chỉ kiểm tra file tồn tại. Provenance/hash trong analysis/rebirth_skill_scripts_20261028/restored-proofs.json.

Trong sáu tên người dùng gửi, đã khôi phục Cổ Hoặc Chúng Sinh; chưa tìm được công thức xác minh cho Bách Chiến Dư Sinh, Vạn Độc Hộ Thân, Tuyệt Kinh Đoạn Mạch, Khô Mộc Phùng Xuân và Hình Thiên Chiến Ý. Người dùng xác nhận không có source/server khác. Không tạo stub rỗng hoặc dùng công thức của skill khác để che lỗi. Các file này vẫn là phần chưa hoàn tất.

## Gói

TuTuongDialog20261028 gồm 15 file: 1 script sửa đệ quy và 14 bản Client/Server của 7 skill khôi phục. Không đổi DLL, bảng drop hay dữ liệu nhân vật. Trạng thái phát hành theo analysis/tutuong_dialog_20261028/publication.json; không coi staging là đã áp dụng.

Đã áp dụng 19 file; world PID 145840, 8059 NPC/quái, 9.249 script failed=0; 9 file nhân vật không đổi hash. Gói cuối có thêm CoreServer/CoreClient và banner/runtime: Trận Siêu Độ, buff và F11 đều 300 giây, client vẽ trực tiếp 4 SPR gốc tại vị trí Trận. Các ảnh đã kiểm tra RLE/pixel khác trong suốt; 53 kiểm tra trạng thái trận và 21.600 trường hợp frame/painter qua. Chưa nghiệm thu hình trong game bằng nhân vật thật.

# Sửa Kho Rương, hành trang và thành phẩm sự kiện

- Khóa kho: thêm ảnh nút và hộp bảo vệ native, nút Khóa/Mở khóa, nhập mật khẩu;
  sửa độ dài và khởi tạo gói tin khóa trong CoreShell.
- Thuê kho: nút có ảnh, kích thước và nhãn tiếng Việt; đưa nút lên trên lớp nền.
  Báo khi không đứng gần Thủ Khố hoặc còn trong trạng thái chiến đấu/bận.
  Giữ giá mua/thuê rương 2–5: 45/9, 60/20, 99/30, 149/45 Xu; thuê 30 ngày,
  không xóa đồ khi hết hạn.
- Gửi/Rút tiền: hộp 300×140 dùng khung và nút trống đã có, vẽ chữ tiếng Việt;
  tiêu đề theo thao tác, số tiền bằng lượng, giới hạn bằng số tiền đang có.
  Cả các nút ở Kho Rương cũng chuyển thành nút có nhãn tiếng Việt.
- Mở hàng hành trang: vải được tìm trong các túi đang mang và ô dùng nhanh,
  không lấy từ kho. Ưu tiên vải, dùng Xu thay thế khi không trả bằng vải được.
  Bỏ bước chặn dựa trên cờ vải cũ ở client; máy chủ kiểm tra và báo kết quả.
- Thợ đồng Sùng Thành Doanh: bổ sung menu Thuộc tính trang bị, giữ cửa hàng số 2
  và chức năng cũ; cho phép đúng NPC Thợ đồng ở map1002 mở dịch vụ native.
- Cẩm nang: thao tác thêm 100/1000 sức lực chuyển sang cộng vĩnh viễn sức mang
  tối đa F4 bằng task CarryBonus đã có. Không trừ hay sửa thể lực nghề sống.
  Đổi nhãn và thông báo để nói rõ sức mang F4.
- Xoay Vòng Càn Khôn: bỏ hoạt ảnh hover ở nút Dừng. Luồng xoay vẫn chờ người
  chơi bấm Dừng, giải thưởng do máy chủ xác nhận.
- Studio: công thức có Tên hiển thị thành phẩm và công cụ chèn màu, xem trước;
  để trống dùng tên gốc. Tên tối đa 60 ký tự, một dòng. Metadata gắn theo tag
  công thức, không đổi ID, tỷ lệ thưởng hoặc tên các vật phẩm gốc không có tag.
  Tooltip dùng tên có màu; các API tên thuần dùng tên đã loại bỏ mã màu.

Kiểm tra: bản native và Studio biên dịch thành công; 51 kiểm tra dữ liệu Studio,
3 kiểm tra giao diện tên/màu; bộ nạp SPR thật đọc 49 tài nguyên/107 frame và hai
scheme kho; 22 kiểm tra trả vải/Xu; 5 kiểm tra xác nhận/hủy và giới hạn số tiền;
9 kiểm tra sức mang F4; menu Lua Thợ đồng và cú pháp hai script; 25 kiểm tra thuê
kho, 371 kiểm tra dịch vụ; 278 kiểm tra lịch/cửa hàng/nồi/trao thưởng sự kiện.
Các phép thử giao dịch dùng host cô lập, chưa thay thế kiểm thử trên nhân vật thật.

Nội dung sự kiện, bản nháp và các file bị thay được sao lưu trước khi nạp.
Không có SPR nào được bổ sung sang Server.

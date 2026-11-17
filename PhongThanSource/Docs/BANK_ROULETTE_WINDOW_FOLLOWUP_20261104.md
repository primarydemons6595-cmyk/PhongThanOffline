# Kho, rương trùng, Càn Khôn và vị trí cửa sổ

Các báo lỗi tiếp theo ngày 04/11/2026:

- Mật khẩu kho: ảnh native hiện tại là 182×113, tọa độ trong INI cũ thuộc một
  mẫu khác. Căn lại hai/ba ô theo viền thật, bố trí hai nút 47×19 trong khung;
  bàn phím ảo nằm phía dưới, tách hàng thông báo. Các nút mật khẩu/bàn phím
  dùng hover tĩnh, bỏ việc chạy vòng các frame gây nhấp nháy. Không tự đặt hoặc
  thay mật khẩu của người dùng trong kiểm thử.
- Thuê kho: đã tái hiện native popup “Hãy đến gần Thủ Khố” ngay cạnh Thủ Khố
  Tây Kỳ. Giữ nhận diện theo script gốc, bổ sung chuỗi ActionScript cố định
  thay vì chỉ m_ActionScriptID hiện hành và nhận diện mẫu 151 + tên Thủ Khố
  được đặt trong dữ liệu vùng. Không nhận mọi NPC mẫu 151 là Thủ Khố.
  Phạm vi và điều kiện thanh toán không nới. Bổ sung TSV giới hạn 256KB khi
  kiểm tra gần kho vẫn thất bại để đối chiếu chính xác dữ liệu live.
- Hai rương: bản đồ Tây Kỳ thường đặt Prop 1 ở 46959/98121, bản _S đặt Box 2
  ở 47151/98089; hai dịch vụ cùng script kho, cách nhau khoảng 195 đơn vị.
  Bộ nạp hỗn hợp có thể lấy cả hai ở hai ô kế nhau. Chỉ loại biến thể 1/2 trùng
  dịch vụ trong 384 đơn vị của cùng subworld, giữ Box 2. Không thay các rương
  khác loại, khác script hoặc cách xa. Không xóa file bản đồ hay đồ trong kho.
- Càn Khôn: trước đây khi dừng đổi từ elapsed/70 sang elapsed/120, làm nhảy
  vị trí, sau đó nhảy thẳng tới kết quả. Luồng mới tiếp tục từ ô đang hiện,
  qua ít nhất hai vòng theo chiều kim đồng hồ; thời gian giữa các bước tăng
  dần 70–400ms. Chỉ sang một ô kế tiếp mỗi nhịp hiển thị; tới ô server chỉ định
  thì đứng lại, chờ 600ms rồi gửi nhận thưởng. Vẫn phải bấm Dừng.
- Phím ~: ghi tọa độ ngoài cửa sổ trước Reset, khôi phục nguyên left/top
  ngay sau Reset, giữ kích thước mới. Bấm chuyển đổi tại nơi người dùng đặt
  cửa sổ; không đưa về góc trên trái.

Kiểm tra: 32 phép kiểm tra policy/mua kho production, luồng mật khẩu native
server, 144 cặp ô bắt đầu/kết quả vòng quay; bảo vệ phạm vi rương trùng. Native
CoreServer/CoreClient/Game đã biên dịch; nghiệm thu live sau nạp được ghi vào
`analysis/tet_event_shop_20261103/bank-spin-window-deployment.json`.

Đã thay ba binary, world cũ PID 200136 lưu/thoát mã 0; world mới PID 230272,
Bishop PID 203900. Sao lưu nhân vật và binary nằm trong
`analysis/tet_event_shop_20261103/bank-spin-before-20261104T042539Z`.
Trong lúc tải bản đồ, nhật ký native xác nhận đã bỏ biến thể Prop 1 và giữ Box 2
ở Tây Kỳ (47151/98089) và Triều Ca (54828/97202). Gỡ khỏi danh sách vùng trước
khi trả slot object, không để tham chiếu treo hoặc lệnh xóa chậm ảnh hưởng slot
vừa tái sử dụng. Chưa coi luồng bấm mua/thuê hoặc ảnh mật khẩu là nghiệm thu live
chỉ vì biên dịch/kiểm tra tự động đã qua.

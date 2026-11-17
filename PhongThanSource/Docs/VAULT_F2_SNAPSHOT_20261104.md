# Khóa riêng rương, thuê kho, F2, trang phục và Snapshot

- Thuê kho: SetTask(5345,page) lưu chuỗi số, GetTask đọc lại giá trị nhị phân
  50–53. Chuẩn hóa về trang 2–5 trước BankInfo/BankPurchase; mọi nhánh trang
  không hợp lệ có thông báo thay vì CloseDialog im lặng. Kiểm tra thực thi script
  trên VM Lua thật mô phỏng đúng SetTask chuỗi, cả mua hẳn/30 ngày và phản hồi.
- Tách khóa kho khỏi m_bLockState của nhân vật. Mật khẩu cũ task202 được giữ,
  chỉ mở rương sau khi đúng mật khẩu; đóng kho hoặc đăng nhập lại sẽ khóa kho.
  Máy chủ kiểm tra khóa riêng ở thao tác di chuyển đồ kho, sắp xếp, gửi/rút tiền.
  Không tự nhập, đặt hay đổi mật khẩu của người dùng để kiểm thử.
- Resize windowed của Represent2 giữ DirectDraw device, primary surface và
  image caches, thay canvas; Engine SetWindowStyle dùng SWP_NOMOVE. Không còn
  đưa cửa sổ về 0/0 rồi kéo lại vị trí cũ.
- F2: giữ 437 sản phẩm có giá/ID/giá gốc; 36 túi/lễ bao sang specialgoods (tab8),
  187 phù/biến thân sang pendantgoods (tab7). Nhãn hai tab được đổi tương ứng.
- Bốn trang phục Hỏa Lôi/Phượng Lôi/Mộng Nhiễu/Tình Khiên trỏ script không có
  trong Loose và các PAK gốc đang sẵn có. Dùng các script trang phục gốc tương
  ứng đã hoạt động (huolei/fenglei/mengrao/hunqian), sửa tuple sở hữu đúng từng
  ID và thời hạn mô tả. Bổ sung 10 biến thể gồm 1427/1428/1430/1431,
  5750/5751/5753/5754, 9234/9235. Giữ giới tính gốc, chặn trạng thái không hợp lệ,
  xác nhận mặc/cởi, đích morph454–457; không tiêu vật phẩm trước khi sử dụng.
  Cả 10 script qua VM Lua thật: main, xác nhận, trả hình dạng cũ, sai giới tính,
  tuple vật phẩm chính xác. Khôi phục script không đồng nghĩa đổi vật phẩm/SPR.
- Snapshot đọc canvas game thay vì primary desktop. Chụp đủ cả cột chat,
  chuyển RGB555/565 theo stride thật. Kiểm tra màu đỏ/xanh lá/xanh dương/trắng,
  padding, hai dòng và biên không hợp lệ.

Kiểm tra tự động đã qua; kiểm tra trực tiếp khi nạp sẽ được ghi trong biên bản
triển khai. Sự kiện người dùng hiện revision28, không ghi đè dữ liệu sự kiện.

Đã nạp theo yêu cầu thoát game: world cũ PID230272 lưu và thoát mã0; world mới
PID230332/Bishop225900. Đã đối chiếu hash bốn file client và 41 file nội dung,
bao gồm chỉ mục và manifest. Sao lưu nhân vật/binary:
`analysis/tet_event_shop_20261103/vault-final-before-20261104T074856Z`.
Sao lưu nội dung:
`analysis/tet_event_shop_20261103/vault-content-before-20261104T074902Z`.
Biên bản mới: `vault-final-deployment.json`; không dùng trạng thái READY của
world cũ làm bằng chứng world mới.

World mới đã READY: 105 bản đồ, gateway kết nối thành công; CustomEvents PID230332
revision28, NPC3, Error rỗng. Bốn binary client và 41 nội dung được đối chiếu
hash sau khi thay. Đang chờ người dùng đăng nhập để kiểm tra giao diện/thao tác
trực tiếp; không ghi nhận kiểm tra tự động là thao tác mua thành công trên nhân vật.

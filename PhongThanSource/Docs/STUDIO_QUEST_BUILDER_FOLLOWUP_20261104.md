# Trình tạo nhiệm vụ Studio — đang triển khai

Yêu cầu: nhiệm vụ chung hoặc theo Giáp Sĩ/Đạo Sĩ/Dị Nhân; NPC cũ hoặc mới;
gặp NPC theo bước, đánh đúng ID quái/Boss trên bản đồ, thu thập/nộp item,
đổi nhiều nguyên liệu lấy quà. Phần thưởng item/trang bị/vũ khí/thú cưỡi,
EXP, bạc, Bạc khóa, danh vọng và danh hiệu. Chỉnh nội dung/màu bằng UI,
chọn ID/ảnh và vị trí bản đồ; Studio sinh dữ liệu/script, không nhập Lua.

Đã có mã nguồn model/compiler/store `StudioQuests.cs` và runtime server
`PhongThanStudioQuests.inl`; runtime đang được biên dịch và kiểm thử, **chưa nạp**.
Các hook: điều hướng main của NPC có binding riêng (giữ chức năng cũ), kill
authoritative trong DeathPunish, life token chống đếm lại cùng mạng, khôi phục
F11 khi đăng nhập. Native task 5160–5223; một trạng thái đóng gói/quest, checkpoint
và receipt hoàn thành cùng transaction item. Chưa có giao diện đầy đủ, chưa
được báo hoàn tất. Cần tiếp tục UI, validation, Lua/native/model tests và nạp.

Bản F2/kho đã nạp khác với runtime nhiệm vụ đang làm:
`analysis/tet_event_shop_20261103/f2-vault-centre-deployment.json` READY,
world239864/Bishop224388, 105 maps, CustomEvents revision28/NPC3.
Studio sửa định dạng Bạc khóa đã được thay sau khi người dùng đóng Studio:
`studio-wallet-ascii-deployment.json`. Task 4847 phải lưu **chuỗi thập phân**,
vì native `KPlayerTask::GetSaveVal` dùng atoi; đã kiểm tra trực tiếp bytes và
export/reload bản ghi, không sửa tiền nhân vật thật.

Sau khi hoàn thành trình tạo nhiệm vụ, người dùng yêu cầu sửa tiếp theo thứ tự:

1. Chữ nhiều hộp thoại quá nhỏ và tràn/đè khung: tăng cỡ, căn và wrap đúng khung.
2. Sau sửa thời gian mồi câu, icon vật phẩm/buff không hiện: khôi phục icon cùng thời gian.
3. Tạp Hóa đang bán đồ lục Vũ Sĩ: đúng phải là đồ trắng (ảnh Kiêu Dũng/Liệp Ưng...).
4. Màu nội dung UI hồi sinh theo ảnh: tên lựa chọn xanh, tổn thất đỏ, chi phí vàng.
5. Giao chiến giữa người chơi có bang: thông báo trợ giúp trong bang và kênh
   Bản địa, tên/map/màu theo ảnh; tránh spam lặp theo mỗi hit.
6. Thuê kho: **bỏ điều kiện gần Thủ Khố/rương**. Chỉ sửa phạm vi thuê;
   bảo toàn giá, thời hạn, đủ tiền và tính đúng quyền sở hữu. Lỗi ảnh thông báo
   không thể mở thuê vẫn cần kiểm tra sau thay đổi này.

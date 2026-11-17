# Cẩm nang: học tối đa kỹ năng gọi đệ Dị Nhân

Nút học tối đa nay bao gồm chín kỹ năng triệu hồi được engine hỗ trợ,
ID 450–458, phái 2 (Dị Nhân), cấp 10. Đây là kỹ năng người chơi dùng gọi đệ;
không cấp các chiêu tấn công của NPC hoặc các ID triệu hồi chưa được hỗ trợ.
Generator cũng giữ danh sách này khi sinh lại manifest từ dữ liệu.

Kiểm thử native EventHandbookSkillTests: 102 kỹ năng, cách ly bốn phái,
chín chiêu triệu hồi, nâng chiêu đã học cấp thấp, gọi lại không thêm trùng,
không thay đổi khi thiếu dữ liệu hoặc thiếu chỗ trong danh sách. Build
CoreServer profile Dev thành công, các baseline của build đạt.

Đã triển khai Loose theo yêu cầu: gửi sự kiện lưu/dừng GameServer, xác nhận
process thoát mã 0, chạy Apply-Loose.ps1 và khởi động lại GameServer. DLL đang
triển khai có SHA256 `2D82CE8A0CA82BF127A9F5BC1D3C4A3DF230BD051939EE3966935CF1D55AC834`.
Gói và bản sao DLL cũ nằm trong
`PhongThanRuntime-Loose/Updates/HandbookSummonMax-20261030`.
Bộ cài từ chối thay DLL khi GameServer vẫn chạy. Không cập nhật Release.
Sau khởi động, Bishop PID 169936 xác nhận WORLD_READY service=1001 maps=105;
GameServer mới PID 154544 nghe cổng 6666, kết nối các dịch vụ 5632/5003/5001.
Chưa kiểm tra nút cẩm nang trực tiếp trong game sau triển khai.

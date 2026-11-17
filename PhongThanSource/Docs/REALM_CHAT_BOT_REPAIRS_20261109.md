# Bản sửa Loose ngày 09/11/2026

Đợt đầu đã cài 15 tệp lúc 02:22 (UTC+7). Biên nhận: `analysis/realm_bot_perf_20261109/installation.json`. Sao lưu: `analysis/realm_bot_perf_20261109/backup-20261109-022209`.

- Tiên/Ma sau đăng nhập lại: dữ liệu GameMa trên đĩa còn nguyên; bổ sung các biến công khai bị sót khi đồng bộ đăng nhập, gồm giá trị 0 để xóa trạng thái của phiên trước.
- F3/F4: ghi nhớ kết quả tính lực theo đầy đủ đầu vào trang bị. Không đổi công thức lực. Chưa có phép đo FPS trực tiếp của phiên người dùng sau cập nhật.
- Danh sách bot: tách giới hạn bot hoạt động khỏi lịch sử. Mức hiện tại 500 bot hoạt động, lịch sử 4.096; đây không phải kết luận 500 bot chiến đấu cùng lúc không lag.
- Chat ghim: lịch sử ngắn khoảng 1,5 trang, giữ vị trí cuộn khi đọc tin cũ.
- Hội thoại NPC: chữ giảm còn cỡ 12. Kích thước nút được xử lý tiếp trong đợt nâng cấp bot, không tính là đã cài trong đợt đầu.

Kiểm tra sau cài: DLL nạp được, Event có lease mới, 103 bot trực tuyến không báo lỗi trong lần lấy mẫu. 138 tệp nhân vật, 128 tệp nhân vật bot và 41 binary runtime Release không đổi. Không build runtime Release.

Các thay đổi tiếp theo: `BOT_AUTONOMY_AND_SCROLLS_20261109.md`. Phải đọc biên nhận cài của từng đợt trước khi kết luận đã cập nhật bản đang chạy.

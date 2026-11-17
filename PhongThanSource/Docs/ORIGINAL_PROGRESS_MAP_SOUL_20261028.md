# Thanh tiến trình gốc, bản đồ khu vực và trận Siêu Độ — 28/10/2026

Trạng thái: **đã sao lưu và áp dụng 7 file vào runtime lúc 15:17 ngày 28/10/2026 (Asia/Saigon)**. World mới đã sẵn sàng; chưa nghiệm thu trực quan trong client thật.

## Thanh thu thập

Bỏ hai hình chữ nhật tự vẽ. Dùng nguyên byte SPR gốc `\spr\Ui4\生活技能\进度.spr` (235×12, ID 661AF586) và `进度底板00.spr` (298×114, ID 15CF6ABA). Đặt bản sao tên ASCII dưới `spr/phongthan/gather`, giữ nguyên SHA-256. Lấy vùng rãnh thanh từ nền; phần xanh cắt theo tiến trình server, không co giãn hoặc tự vẽ pixel. Cấu hình gốc có Processing ở (30,39), Processing_Image 235 pixel. Bản xem trước giải mã từ SPR: `analysis/original_progress_map_20261028/original_progress_preview.png`.

## Bản đồ thế giới

Khi chọn một trong 32 khu vực, dùng đúng khung UiMiniMapBig, viewport 640×380 và ảnh khu vực ở tỉ lệ 1:1. Bốn nút gốc và phím mũi tên cuộn ảnh, giới hạn trong biên; nút về thế giới/Escape quay lại. Không còn nhánh co giãn loại ảnh lớn bị giới hạn 512 pixel. Nền dùng inverse alpha hợp lệ 0..32; giá trị 255 trước đó gây nhiễu màu trong MMX ClearAlpha. Ảnh minh họa là giải mã tài nguyên để kiểm tra, chưa phải chụp màn hình client thật.

## Siêu Độ

Bổ sung REF_SPOT để SPR 510×510 dùng điểm chân (255,293), khớp tọa độ NPC/tên trận. Native kiểm tra đúng bản đồ và khoảng cách tối đa 600 đơn vị MPS đến điểm nhiệm vụ, cho phép đứng gần. Trận xuất hiện ở chính chân nhân vật, không kéo về điểm nhiệm vụ. Sai nơi báo “Chưa phải nơi cần siêu độ.”; không sinh NPC hoặc trừ phướn.

Native sở hữu cả NPC, hạn 300 giây và buff 262..265. Lỗi tạo buff hoặc đổi vật phẩm sẽ hoàn tác. Hết hạn, đổi map, hủy/hoàn thành nhiệm vụ, mất NPC hoặc đăng nhập lại mà không có trận sống sẽ xóa buff và mốc hạn cũ. Không tự phục hồi NPC từ dữ liệu hạn lưu.

## Kiểm tra và cập nhật

Build Release CoreServer, CoreClient, GameClient thành công. 73 kiểm tra logic trận; 21.600 khung hoạt ảnh; 100 trạng thái thanh gốc; production select/paint/scroll/back cho 32 map; Engine nạp thật 38 tài nguyên; Lua4 chạy nhánh sai vị trí/thành công/thất bại/chưa nhận nhiệm vụ. Kết quả trong `analysis/original_progress_map_20261028`.

Gói `Deploy/ProjectContent/OriginalProgressMap20261028/manifest.json`: 7 file. Publisher yêu cầu Game/Studio đóng và không có nhân vật online, sao lưu file cũ, đối chiếu hash nhân vật trước/sau, dừng/khởi động world qua controller. Chưa nghiệm thu trực quan trong client thật.

Đối chiếu sau cập nhật: world PID 151804; 8059 NPC/quái; 9.249 script, failed=0; 9 file nhân vật giữ nguyên hash trước/sau. Bảy file triển khai đúng hash; các dịch vụ tài khoản giữ PID. Backup: `E:\ProjectAI\PhongThan\PT\PhongThanRuntime-Loose\ManagerStudio\PhongThanStudio\Backups\OriginalProgressMap20261028_20261028_151653`.

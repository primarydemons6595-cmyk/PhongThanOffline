# Thanh thu thập Trừ Họa — 28/10/2026

Kênh thu thập server đã có bộ đếm 3 giây và callback nhận vật phẩm, nhưng phần PaintTop/PaintPaceBar của nhân vật bị bỏ qua từ khi tên/máu chuyển sang final scene overlay. Hàm PaintPaceBar cũ còn vẽ tại vị trí y+819/y+430 cách xa nhân vật. Bản sửa thêm thanh 140px có phần trăm và nhãn Đang thu thập vào final scene overlay, ngay trên thông tin nhân vật; không dùng vòng thời gian client để tự phát thưởng.

Giữ giao thức player snapshot ProgressPercent hiện có, không đổi ABI. Server gửi snapshot lúc bắt đầu, cập nhật mỗi 3 tick game trong kênh thu thập, và khi dừng/kết thúc. Ngắt do đánh, di chuyển, đổi map, mất buff, đổi nhân vật giữ các guard cũ. Nhận vật phẩm và bitmask sở hữu vẫn là giao dịch server; không cấp thêm nếu đầy túi hoặc nhặt trùng. Khi hoàn thành, thông báo nhận Nguyên Sâm/Sơn Xuyên Liễu trong Lua vẫn giữ.

93 kiểm tra native thu thập qua, thêm kiểm tra đếm snapshot Start/Update/Cancel. Renderer fixture kiểm tra 100 mức tiến độ, phần trăm, tọa độ screen, giới hạn chiều dài và không vẽ khi còn lại 0/ngoài miền. CoreServer/CoreClient Release build qua. Chưa nghiệm thu trong client thật sau phát hành.

Gói GatherProgress20261028 chỉ có CoreServer.dll/CoreClient.dll; trạng thái áp dụng tại analysis/gather_progress_20261028/publication.json. Không dùng sự tồn tại của gói để kết luận đã cập nhật.

Đã áp dụng: hai DLL đúng hash; world PID 147860, 105 map, 8059 NPC/quái; 9.249 script failed=0. 9 file nhân vật giữ nguyên hash trước/sau; dịch vụ tài khoản giữ PID. Backup và receipt trong analysis/gather_progress_20261028. Chưa nghiệm thu thanh chờ trong client thật sau cập nhật.

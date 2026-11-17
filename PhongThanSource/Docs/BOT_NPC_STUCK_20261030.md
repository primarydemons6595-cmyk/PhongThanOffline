# Bot đứng lặp tại Đồng Quan / Thiếu Hạo

Quan sát world 163484 khoảng 01:04 (+07): LoneWolf99đ, Duy.Gamer.99, Long.Master và MinhAnh5659 ở map 1014 quanh 43900/99300, state quest_travel. Log lặp train_path_blocked_reselect nhiều phút khi tới portal marker 43152/98320, chặng 1014→1006. Chúng có thể ra nhặt đồ rồi quay lại: di chuyển vẫn hoạt động, mục tiêu đường đi bị thử lại vô hạn.

Nhóm Dị Nhân ở map 1004 quanh 50800/104680, state quest_dialog. Snapshot đọc độc lập cho oOThanhLongOo xác nhận Mao Lư 4960=1. Cả nhóm chưa tiến qua bước đầu gặp Thiếu Hạo. Chưa kết luận hoàn toàn nguyên nhân hội thoại; bổ sung log maolu_dialog_progress gồm phase, series, NPC/lastNPC, distance, token, answers, callback đầu và script thực.

Bản publish world 5320:

- Chặng portal tìm đường thất bại ba lần bị loại tạm 180 giây cho riêng bot, tính BFS đường thay thế trên graph gốc cùng NPC/portal, không dịch chuyển miễn phí hay đi xuyên tường. Chặn log lặp khi Failures>3.
- Mao Lư không tiến triển 90 giây thì tạm nghỉ ngẫu nhiên 45–135 giây để làm việc khác; tiến độ được giữ nguyên. Sau mỗi lượt NPC có độ trễ 3–15 giây riêng cho bot.
- Test tuyến tránh hỗ trợ chặng Đồng Quan 1014→1002 khi bỏ lối 1014→1006. Kiểm tra hơn 65 nghìn cặp đường cache bằng thuật toán độc lập Floyd–Warshall và tuyến đầu, thêm các trường hợp chặn cạnh đều đạt.
- Lua4 test mở script Thiếu Hạo thật 1004_00.lua, cấp host Dị Nhân 18/Mao Lư phase 1; main và pt_mao_claim tiến phase 2, chính xác 100 XP. Điều đó chỉ xác nhận logic script, chưa xác nhận NPC đang chạy trong world.

Đã lưu sạch world 163484, backup Core+GameServer+bot state, hash copy và khởi động 5320. Cần xác minh sau khi nạp xong map. Không đổi các flag hoàn thành nhiệm vụ để lấy kết quả giả.

## Kết quả xác minh

World 5320 đã READY. Log cho nhóm Đồng Quan ghi nhận cạnh 1014→1006 bị loại và chuyển sang 1014→1016→1005→1002. LoneWolf99đ, Duy.Gamer.99, Long.Master, MinhAnh5659 đã rời cụm kẹt; hai bot đầu sau đó được xác nhận ở map 1005 và tiếp tục quest_travel, không có Error.

Log Dị Nhân cho biết NPC 82 đúng Thiếu Hạo, series=2, Mao Lư phase=1, token hợp lệ nhưng callback đầu là pt3_book_view. Wrapper Cần Mẫn ở cuối script che menu Mao Lư bằng menu ngoài, có nút pt3_book_other mà bot chưa biết chọn. Đã thêm wrapper chỉ cho bot ưu tiên PTMaoMenu khi PTMaoReady, giữ menu người chơi; nạp lại riêng script 1004_00.lua bằng bộ reload chính thức. Command f1f10d301c964aafb9e5bed05648abd3, world 5320, completed 1/1, failed=0. Không restart world thêm.

Sau reload: sSieuNhanGaS và Hoang.Killa có log thưởng qua NPC; snapshot PTHC xác nhận cả oOThanhLongOo, sSieuNhanGaS, Hoang.Killa đã đạt task4960=5 (Mao Lư hoàn thành). Hai bot sau đã nhận Tân Thủ Thí Luyện task4963=1. Test Lua4 xác nhận bot được thấy pt_mao_claim và nhận đúng thưởng; nhân vật người chơi vẫn thấy pt3_book_view như trước. Sample hiệu năng world5320: 18.015 FPS, 4 tick quá ngân sách trong mẫu, không phải bảo đảm hiệu năng mọi thời điểm.

Source bổ sung chưa publish Core: helper chọn pt3_book_other khi đi qua menu ngoài; PTBotNpcVisits giới hạn hai bot tiếp cận một NPC, lease 8 giây; có test tranh slot, hết lease và release. Không coi cơ chế giới hạn này là đã hoạt động trên world5320. Bản sửa đã chạy thực tế cho hai lỗi ở ảnh là tránh cạnh portal lỗi, backoff nhiệm vụ và hot reload menu Thiếu Hạo.

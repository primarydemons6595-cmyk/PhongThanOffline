# Đồng bộ và xử lý tải dùng chung — 10/11/2026

Phạm vi triển khai: chỉ `PhongThanRuntime-Loose`. Không build hoặc cập nhật bộ `PhongThanRelease`. Không thay quyền quyết định tọa độ, va chạm hay kiểm tra tốc độ của server.

## Lỗi đồng bộ đã tái hiện và sửa

`KPlayer::Revive` từng gửi phản hồi riêng và gọi `KNpc::SendSyncData` tới `m_nNetConnectIdx=-999` của bot. Transport từ chối kết nối âm, tạo dòng `Packing sync data failed...`. Hiện phản hồi riêng chỉ gửi khi có client hợp lệ; broadcast hồi sinh tới người chơi xung quanh vẫn thực hiện. `KNpc::SendSyncData` và `KRegion::SendSyncData` cũng kiểm tra kết nối âm. Client số 0 vẫn hợp lệ. Lỗi gửi thật vẫn trả thất bại và ghi chẩn đoán giới hạn tần suất gồm client/entity/size/HRESULT, không giả thành công.

`KRegion::BroadCast` từng tiêu hao số người nhận cho cả bot không có socket, nhân vật ngủ và người ngoài phạm vi. Người chơi sau 100 mục này có thể không nhận sự kiện. Hiện chỉ người nhận thực tế thành công mới tiêu hao ngân sách. Giới hạn sự kiện trong vùng bằng `MAX_PLAYER`, tương ứng đường NormalSync, để người thứ 101 cũng nhận được; giữ phạm vi không gian hiện có. Đây là sửa tính đúng đắn; chi phí phát gói trong đám đông vẫn cần kiểm thử tải mạng.

Kiểm thử native tái hiện lỗi cũ và xác nhận sau sửa: bot hồi sinh không gửi socket âm, client 0 nhận bình thường, lỗi E_FAIL không bị che, broadcast hồi sinh giữ nguyên; một người thật sau 101 bot nhận cập nhật; cả 105 người thật gần nhau nhận sự kiện. Các kiểm thử tiến trình nhiệm vụ/vật phẩm/Thần Kỹ trước đó đều qua. Nhật ký: `analysis/server_chat_crash_20261109/broadcast-dense-after.log`.

## Điều phối xử lý gói dùng chung

Vòng đọc đầu vào trước đây rút hết hàng đợi từng client theo thứ tự cố định trước khi tiến hành world tick. Đã thay bằng lượt quay vòng, kiểm tra ngân sách trước khi lấy gói, tối đa 4 gói/client/lượt và khoảng 6 ms mỗi lượt. Gói chưa xử lý vẫn nằm trong FIFO transport. Không ngắt một handler đang thực thi, không phát lại hoặc bỏ giao dịch. Do một handler có thể chậm, 6 ms không phải trần thời gian tuyệt đối. Gateway/DB/relay cũng có giới hạn số gói và thời gian xử lý giữa các handler.

Tùy chọn `Server/settings/phongthan/ServerPerformance.ini`, `[InputBudget]`, `Enabled=1`, `Milliseconds=6`, `PacketsPerClient=4`, `ServicePackets=64`. `Enabled=0` trở lại cách rút hàng đợi cũ. Cấu hình được đọc lại theo chu kỳ.

Kiểm thử helper production với 1.200 hàng đợi tổng 12.000 gói: giữ FIFO, đúng một lần, chống một nguồn gửi dồn chiếm lượt; có trường hợp disconnect và handler chậm. Kiểm thử đường NPC thật dùng 1.200 yêu cầu trên **50 nhân vật fixture**, không phải 1.200 người online: lượt không giới hạn khoảng 410 ms; chia lượt tối đa khoảng 9,9 ms trong lần đo cuối. Các số này không đại diện độ trễ mạng hoặc sức chứa thực tế.

## Đo và phần còn lại

`server_host_performance.json` ghi thời gian cả host (message loop, lưu, đăng xuất, chuyển server, core, tổng công việc và khoảng cách frame), bổ sung cho `server_performance.json` của Core. Có đo đầu vào, NPC, Lua, toàn bộ lần lưu nhân vật và staging thuộc tính vật phẩm. Console host giữ exit code và đuôi console sau khi world thoát. Người dùng xác nhận lần world dừng khoảng 00:10 là thao tác reset; không phân loại lần đó thành crash.

Điểm còn cần tối ưu có bằng chứng: staging/commit dữ liệu nhân vật vẫn ghi bền vững trên world thread; một lần lưu có thể mất hàng chục ms. Không tắt flush hoặc báo lưu thành công trước commit để đổi lấy FPS. Cần thiết kế snapshot/version/ack và kiểm thử mất điện, đăng xuất, chuyển map/server, giao dịch trước khi chuyển I/O sang worker. Bot Service độc lập không tự giải quyết phần này vì người thật cũng đi qua đường lưu và NPC chung.

Chưa có bằng chứng đủ để tuyên bố ổn định 1.200 người chơi, hoặc đã hết mọi nguyên nhân giật lùi. Cần đo phiên chơi thật đồng thời client/server, nhiều phân bố map và tình huống dồn người/NPC/giao dịch. Các yêu cầu VTT và bổ sung skill từ ảnh còn là công việc riêng, chưa được coi là hoàn tất bởi bản tối ưu này.

Biên nhận triển khai và bản sao lưu: `analysis/server_chat_crash_20261109/shared-server-installation.json`, `observer-server-installation.json`. Bản CoreServer hiện tại SHA-256 `7967628ec7f70837966f68bf7e3e0e127ae7887b2226d7aeb0eddbfbcd9e92d5`.

## Đo sau triển khai, PID 63928

Sáu cửa sổ khoảng 60 giây với 200 bot, đã bỏ thời gian nạp map và nạp bot; không chạy build/probe trong lúc lấy mẫu. Core ghi 1.082 nhịp, 20 nhịp vượt 55,6 ms (1,85%), trung bình 22,575 ms, lớn nhất 109,947 ms. Mỗi lần lưu nhân vật trung bình 17,309 ms, lớn nhất 38,127 ms; staging thuộc tính trung bình 5,857 ms. NPC choice trung bình 0,066 ms, Lua 0,020 ms trong workload này, không có nghĩa mọi hội thoại đều nhanh như vậy.

Host ghi tổng công việc trung bình 23,353 ms, cực đại 350,678 ms và khoảng cách frame cực đại 365,088 ms. Có một khoảng đo đầu vào 292,693 ms dù cửa sổ đó không xử lý gói client; chưa có bằng chứng để quy nó thành handler NPC chậm. Cần phân biệt thời gian thực thi CPU, chờ khóa, page fault và luồng bị OS tạm ngừng trước khi sửa tiếp. Cửa sổ cuối có 82 gói client khi người dùng mở lại game; năm cửa sổ trước là 0 gói. Không phải thử tải người thật. Dữ liệu đầy đủ: `analysis/server_chat_crash_20261109/shared-200-stable.json`.

Đọc console sau đợt đo: không xuất hiện `Packing sync data failed` hay `SYNC_SEND_FAILED`; native/Event đúng PID, lease hợp lệ. Console vẫn có một số script skill chuyển sinh/hoán hồn chưa mở được; đây là lỗi tài nguyên/logic riêng còn tồn tại, không được che bởi bản sửa gửi gói. Không kết luận đã hết lag chỉ từ FPS trung bình gần 18.

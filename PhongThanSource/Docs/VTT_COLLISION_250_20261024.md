# VTT: hai cửa Phong, 250 quái và va chạm cổng/rào

Áp dụng cho Thổ 1067, Thủy 1068, Hỏa 1069 và Phong 1070. Mỗi trận có **250 quái thường**, không tính bốn hộ vệ, Thông Thiên, cổng và hàng rào. Mission, guard và boss dùng cùng danh sách tọa độ; giữ cấp độ, phần thưởng và lịch hoạt động hiện có.

Phong có hai cửa vật lý ở MPS (52080,101232) và (54320,103632). Thông Thiên nằm trong thành tại (52976,102416). Bốn hộ vệ ở phía bắc (49648,97072), đông (57168,102672), nam (52976,106896), tây (49360,102352). Hai cửa cùng mở sau khi hạ bốn hộ vệ và nộp đủ bốn lệnh bài một lần. Người chơi đi vào qua cửa, không dùng dịch chuyển thay cho cửa.

Kết luận trước đó cho rằng template 1418 khớp cổng/hàng rào trong ảnh là sai. Cổng đúng là frame 0 của `spr/npcres/building/building001_st1.spr`; frame đứng `building001_st.spr` là khung đang xây. File _st1 trong Loose cần giải nén từng frame. Hàng rào ngắn đúng là hai frame của `游戏资源/美术图素/自建房屋/刺桩栅.spr`, trích từ PAK gốc. 1418 là hình cự mã khác, giữ nguyên cho các nơi khác.

Bản sửa dùng template riêng **2704 (cổng), 2705/2706 (hai hướng hàng rào)**. Mỗi SPR mới chỉ chứa frame gốc đã chọn; giữ nguyên palette, pixel RLE và offset. Không sửa template 369/1418 hoặc PAK gốc. Đây là ID riêng của dự án gắn hình gốc, không phải tuyên bố về ID placement VNG. Ảnh đối chiếu: [cổng](../../analysis/vtt_gate_identity_20261024/verified_gate_fence_frames.png), [hàng rào ngắn](../../analysis/vtt_gate_identity_20261024/short_fence_candidates.png).

Các template hàng rào khai báo dạng tĩnh khi thêm NPC để bộ hiệu chỉnh vị trí quái không dời hàng rào. Sau khi thêm, server đặt hàng rào thành mục tiêu có 30.000 HP và chỉ cho gây sát thương trong giai đoạn chiến đấu.

Server và client dùng cùng bảng ô chắn, kiểm tra cả đoạn đường đi và góc chéo. Server gửi mặt nạ đoạn rào còn sống và trạng thái cửa qua gói 0x2016 mỗi giây, gửi ngay khi phá rào/mở cửa. Client chặn dự đoán di chuyển bằng mặt nạ này; khi chuyển bản đồ, xóa trạng thái cũ và tạm coi cửa/rào đóng trước gói đầu tiên. Quái nhiệm vụ còn bị chặn vào vùng an toàn của Đại Phu qua đi bộ, nhảy và di chuyển nhanh kể cả sau khi phá rào.

Kiểm thử trong `analysis/vtt_collision_20261024`:

- Lua 4 thật: 344 kiểm tra vòng đời, 250 quái, bốn hộ vệ, một boss, phần thưởng, đóng và mở lượt mới.
- Native server: 133 kiểm tra điều kiện đánh rào, từng đoạn chắn, cửa vật lý, không tự sinh lại rào đã phá, giao dịch lệnh bài và dọn trận.
- Native client: 428 kiểm tra các ô chắn dùng chung, đường đi, phá từng đoạn, mở cửa và từ chối gói cũ/sai bản đồ/sai kích thước.
- Đối chiếu terrain: hai cửa Phong độc lập đều dẫn được vào thành khi mở; khi cả hai đóng, không có đường vào boss; bốn hộ vệ nằm ngoài thành; quái không sinh trong vùng an toàn.
- CoreServer và CoreClient Release build thành công. Các dòng NPC cũ của từng phía được giữ riêng và đối chiếu nguyên vẹn.

Kết quả cập nhật/hash, đăng ký thử và đóng trận được ghi trong `publication.json`, `live_verification.json`, `events_registration.json`, `events_closed.json` của thư mục kiểm thử sau khi chạy. Bản cập nhật sao lưu trước khi thay file, dừng world bằng sự kiện lưu nhân vật và kiểm tra dữ liệu nhân vật không đổi trong lúc thay file. Không thay Game.exe hoặc Client_Original trong đợt này.

Giới hạn: công cụ điều khiển cửa sổ game không khởi tạo được trong phiên này. Các kiểm tra mã, Lua, terrain và snapshot server không thay thế việc nhìn và đánh thử trực tiếp trong client.


Hộ vệ phía đông Phong đã dời từ (56912,102672), chỉ cách ô chắn 1 ô, sang (57168,102672), cách ô chắn ít nhất 5 ô theo khoảng cách Chebyshev. Kiểm tra cả footprint vuông và đường đi ngoài thành; cùng tọa độ trong mission04/guard4/boss4.


Điều chỉnh theo phản hồi: hộ vệ phía bắc dời từ (52976,97936) tới (49648,97072), quãng đường ngắn nhất từ điểm vào tăng từ 41 lên 170 ô. Hộ vệ phía tây dời tới (49360,102352). Bốn hộ vệ đều cách ô chắn ít nhất 5 ô; khoảng cách đường đi từ điểm vào lần lượt 170, 616, 729, 570 ô. Không thay vị trí Đại Phu hoặc hai cửa. Biên nhận cuối: `publication_revision3.json`.

Đã triển khai bản cuối trên world PID 103704, dừng/lưu sạch exit 0 và giữ dữ liệu nhân vật trong lúc thay file. Đối chiếu 288 file hiện hành, 99 vị trí nhiệm vụ và 7 NPC Sùng Thành bổ sung đều đạt. Mở đăng ký thực tế cả bốn trận xác nhận 250 quái thường mỗi trận, số cửa lần lượt 1/1/1/2 và số đoạn rào 5/11/14/5, đúng vị trí nguyên gốc MPS đã cấu hình. Sau kiểm thử đã đóng cả bốn lượt và xác nhận quái/rào/cửa thử đều được dọn. Không đổi revision lịch hoạt động (vẫn 8). Tọa độ hộ vệ cuối được kiểm tra bằng terrain gốc, spawn domain và Lua 4 thật; chưa kiểm nghiệm giao chiến trực tiếp trong client.

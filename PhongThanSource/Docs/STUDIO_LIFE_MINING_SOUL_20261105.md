# Studio, kỹ năng sống và Hoán Hồn — 05/11/2026

Đã cập nhật bản chính tại `PhongThanRuntime-Loose`. Biên nhận, SHA và bản sao lưu trong `analysis/studio_life_20261105/`: `content-deployment.json`, `copper-deployment.json`, `binary-deployment.json`. Bản đầy đủ giữ sửa bố cục hội thoại của hotfix trước đó. Không phục hồi hay thay dữ liệu nhân vật bằng bản thử.

## Cách dùng

- Studio vẫn chọn được Classic hoặc Liquid. NPC/Item/Skill hiển thị tên, mô tả tiếng Việt theo đúng cột mã hóa; thuộc tính Trung có nhãn Việt đi kèm. Nhãn dịch chỉ dùng trong Studio, không ghi đè tên cột hay dữ liệu gốc. Trường không sửa giữ nguyên byte gốc; chữ không thể mã hóa trở lại sẽ bị từ chối.
- Khi áp patch **chỉ chứa Server settings/script**, có lựa chọn chỉ yêu cầu dừng/reset GameServer, giữ Client và các dịch vụ hỗ trợ. File nạp lúc khởi động cần reset GameServer; đây không phải cam kết mọi file đều tự hot reload.
- Mở Kỹ năng sống trong game, chọn nghề sản xuất để mở cửa sổ chế tạo/nấu ăn dùng khung, tọa độ và ảnh SPR có sẵn. Chọn công thức rồi thực hiện; server kiểm tra kỹ năng, nguyên liệu, tiền, thể lực/tinh lực và chỗ trong túi.
- Nút **Hoán Hồn** trong cửa sổ Kỹ năng sống mở bảng đặt trang bị, Hồn Chú và nguyên liệu. Xem phí trước khi thực hiện. Hồn Chú mới thay Hồn Chú cũ. Đây là đường mở bổ sung của Loose, chưa phải phục hồi NPC Nguyên Hồn Chân Nhân.
- Dùng Thiên Linh Thạch để mở bảng nạp linh khí, đặt trang bị và đúng viên đá vừa dùng vào bảng. Mỗi viên nạp tối đa 200 điểm; mở bảng/hủy/đầy linh khí không trừ đá. Kỹ năng từ Hồn Chú chỉ có khi mặc trang bị, đúng cấp/nghề và đủ linh khí. Tháo đồ hoặc hết linh khí không xóa kỹ năng đã học vĩnh viễn.
- Studio → Quản lý server → **Quản lý khai khoáng**: sửa từng mỏ, tầm tương tác, trữ lượng, hồi mỏ, vật phẩm, trọng số, số lượng, may mắn và câu thông báo. Đưa vào patch Server, áp dụng khi GameServer dừng rồi khởi động lại.

## Khai khoáng và điểm thu thập

Mỗi núi lửa có một điểm tương tác tại miệng. Khôi phục 68 mảnh cảnh nền gốc bằng cách ghép lại đúng bản ghi đã gỡ, không chép đè nguyên vùng map. Điểm sẵn sàng có hiệu ứng dung nham nhỏ và ánh sáng; hết khoáng chỉ tắt hiệu ứng này, giữ cảnh núi lửa gốc.

Dùng Bàn Cổ Khai Thiên để bắt đầu **ngồi đào liên tục** trong tầm miệng mỏ. Mỗi lượt xét may mắn độc lập; không có thanh tiến trình và không bảo đảm chờ đủ thời gian là nhận đồ. Di chuyển, đổi thao tác/dụng cụ, đổi map, bị đánh, chết, giao dịch hoặc đổi phiên sẽ dừng đào. Bấm lại không tạo thêm lượt tức thời. Thông báo hệ thống tô số lượng đỏ và tên vật phẩm vàng.

**Các tỷ lệ sau là cấu hình tái dựng của Loose, chưa xác minh là tỷ lệ VNG gốc:**

| Thông số mặc định | Giá trị |
| --- | --- |
| Cơ hội trúng mỗi lượt | 20% + 0,1 điểm phần trăm mỗi điểm may mắn, trần 100% |
| Bảng vật phẩm sau khi trúng | Đồng thau 80%, Hoàng đồng 10%, Đồng đỏ 7%, Xích đồng 3% |
| Số lượng | 1–2; Xích đồng 1; giới hạn bởi trữ lượng còn lại |
| Trữ lượng mỗi mỏ | 100 vật phẩm, dùng chung |
| Hồi sau khi cạn | 600 giây |

Các tỷ lệ vật phẩm trong bảng là tỷ lệ có điều kiện **sau khi trúng lượt**, không phải xác suất nhận mỗi lượt. Trữ lượng lưu trong `Server/MiningState.bin`, không nạp đầy lại chỉ vì reset server. Lỗi đọc/ghi trạng thái sẽ chặn phát khoáng. Trong trường hợp máy tắt đột ngột giữa ghi dự trữ và cấp đồ, có thể hụt trữ lượng; chưa có nhật ký giao dịch cấp khoáng bền vững theo nhân vật cho khoảng gián đoạn đó.

Hái thuốc, thu nguyên liệu và câu cá vẫn dùng luồng thu thập khác: hoàn tất thao tác hợp lệ nhận một nguyên liệu, tốn thể lực và cập nhật thuần thục; không dùng bảng may mắn của núi lửa. Các điểm cá được đặt theo bằng chứng sóng/nền nước và bờ có thể đứng, câu được từ bờ trong tầm 512, cần cần câu. Đã chuẩn bị 209 điểm cá trên 20 map, trong tổng 282 điểm kỹ năng sống; 34 điểm núi lửa cũ được giao cho bộ quản lý mỏ riêng. Các vị trí chưa xác định được map không được tự đặt phỏng đoán.

Đồng thau từng trỏ nhầm ảnh cuộn phù ngay trong bảng gốc. Đã thay riêng cột ảnh của vật phẩm genre 3/detail 3 ở Client và Server bằng sprite đồng có sẵn `spr/item/other/青铜.spr`; không đổi ID, tên, giá, thuộc tính hoặc các vật phẩm khác.

## Công dụng đầu ra

106 đầu ra món ăn/đan dược dùng thuộc tính, thời hạn, yêu cầu và loại vật phẩm gốc: buff thời hạn, hồi phục tức thì hoặc giải trạng thái. Dùng lại cùng buff làm mới thời hạn, không nhân chồng chỉ số. Các mã thuộc tính đặc biệt được xử lý riêng, không lấy nhầm ID kỹ năng chiến đấu cùng số. Chính sách thu hút quái và kích hoạt giảm kháng được tái dựng; chưa khẳng định trùng thuật toán server VNG.

Nguyên liệu trung gian được dùng tiếp trong công thức, khảm/luyện và Hoán Hồn; không biến tất cả nguyên liệu thành buff bấm dùng. Tiên Lộ Tinh Hoa xuất hiện trong danh sách thức ăn thú hộ, tăng trưởng thành và giới hạn ở 300; mức mặc định +6 là chính sách tái dựng, có thể chỉnh `pet.ini`, `[Feed] AdvancedEssenceGrowth`. Không dùng khi thú chưa đủ điều kiện hoặc đã đầy.

Hoán Hồn đọc 864 dòng yêu cầu gốc và 1.398 dòng định nghĩa công thức khôi phục từ PAK (418 định danh vật phẩm cụ thể; các dòng còn lại gồm chỗ giữ sẵn). Giữ phí, loại trang bị, nguyên liệu, kỹ năng, cấp/nghề và tiêu hao linh khí từ bảng. Ngũ Sắc dùng một thuộc tính bổ sung theo bảng `formulamagic.txt`; cách triển khai phần server này là tái dựng, chưa chứng minh toàn bộ thuật toán gốc.

Hoán Hồn không thay cấu trúc file nhân vật; trạng thái riêng được tham chiếu bởi ScriptParam và lưu trong `Server/settings/phongthan/soul_states`. **Sao lưu/chuyển server phải giữ thư mục này cùng dữ liệu nhân vật**, tương tự `socket_states`. Đừng chỉ chép file nhân vật đã có trang bị Hoán Hồn mà bỏ thư mục trạng thái. Nạp lại sẽ từ chối định nghĩa bị thiếu/hỏng thay vì âm thầm làm mất thuộc tính. Trữ lượng mỏ cũng cần giữ `MiningState.bin` khi sao lưu.

## Kiểm chứng và giới hạn

Hiệu ứng halo elip của bản vá đào trước đã bị loại bỏ sau phản hồi hình ảnh thực tế: che mất vân dung nham và dùng sai thang trong suốt native. Bản thay thế dùng sprite hiệu ứng riêng chỉ chứa điểm dung nham đỏ/vàng, bốn frame sáng theo nhịp; phần đá/map gốc giữ nguyên. Cài riêng CoreClient.dll + sprite hiệu ứng, không reset server. Chi tiết cuối ở `analysis/mining_fire_texture_20261105/REPORT.md` và `sprite-deployment.json`.

Bản vá phản hồi đào khoáng tiếp theo ngày 05/11: sửa điều kiện nhắm tọa độ/vũ khí riêng cho kỹ năng 62 (bảng gốc còn TargetOnly=1/EqtLimit=6 trong khi Cuốc chim là particular 8); nhận thi triển của chính người đào và trạng thái ngồi do server xác nhận. Mỏ còn khoáng thêm halo mềm nhịp sáng/tàn lửa, vẽ sau trang trí map để núi lửa không che hiệu ứng. Bản vá chỉ thay hai Core DLL, không đổi tỷ lệ/trữ lượng hoặc hội thoại. Chi tiết và biên nhận ở `analysis/mining_feedback_20261105/REPORT.md` và `deployment.json`.

Đã cài vào runtime chính ngày 05/11/2026: 68 tệp được đối chiếu SHA-256 sau cập nhật (gồm bản vá Event bên dưới). GameServer PID 8292 đã nạp xong map, có heartbeat mới; Event service PID 57772 đang chạy, health không báo lỗi. Biên nhận và bản sao trước cập nhật nằm trong `analysis/studio_life_20261105`, đặc biệt `final-verification.json`, `binary-deployment.json`, `content-deployment.json`, `copper-deployment.json`, `event-deployment.json`.

Đồng thau đã đổi đường dẫn hình trong cả bảng material Client/Server sang sprite đồng gốc `青铜.spr`; không đổi ID, thuộc tính hoặc nội dung sprite.

Event service: đã tái hiện thông báo `Access to the path is denied` khi native reader mở snapshot mà không chia sẻ quyền thay/xóa tệp. Bước thay snapshot nguyên tử nay thử lại có giới hạn khi gặp khóa Windows tạm thời; nếu vẫn thất bại, giữ tệp cũ và báo đường dẫn cụ thể. Kiểm thử riêng đã qua khóa tạm, khóa lâu, tệp chỉ đọc và dọn tệp tạm. Đây là nguyên nhân đã tái hiện khớp thông báo; log cũ chưa ghi tên tệp nên không khẳng định được đường dẫn của lần lỗi người dùng thấy. Đã cập nhật thư viện ở cả Studio và Event service, chỉ khởi động lại Event cho bản vá này; GameServer giữ phiên chạy.

Build CoreServer/CoreClient/GameClient Release thành công; Studio 0 lỗi/0 cảnh báo. Đã kiểm tra 128 công thức, 178 biến thể đầu ra và 128 đường gọi GUI trong Lua4 thật; 106 vật phẩm dùng được với thời hạn, làm mới, hết hạn, giải trạng thái và hoàn tác khi không trừ được đồ. Kiểm tra 864 giao dịch Hoán Hồn, 418 định danh kỹ năng trang bị, trữ lượng mỏ, lượt đào khi ngồi, dừng thao tác, chống lặp, túi đầy và lưu/nạp đều qua. Kiểm thử dùng mã production với môi trường nhân vật/túi cách ly, không dùng đồ hay tiền của nhân vật thật.

Studio đã kiểm tra hiển thị thuộc tính/NPC/Item/Skill, giữ nguyên byte gốc, ảnh vật phẩm, quản lý mỏ và thiết kế UI ở Classic/Liquid. Kiểm tra lại bố cục hội thoại vẫn qua. Đây chưa thay thế kiểm tra trọn quy trình bằng nhân vật thật trong game, đặc biệt tư thế đào, ánh sáng miệng mỏ và hiệu ứng chiến đấu.

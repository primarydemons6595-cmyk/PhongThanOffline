# Sửa HUD, hội thoại, vị trí shop và chiến đấu Vạn Tiên Trận — 24/10/2026

Đợt này sửa các lỗi được báo bằng ảnh sau [bản Sùng Thành/VTT](SUNG_THANH_VTT_UI_FIX_20261024.md). Chỉ triển khai ba binary; không đổi Lua NPC, vật phẩm, nhiệm vụ, bản đồ hoặc phần thưởng.

## Phạm vi sửa

- Khôi phục hợp đồng giao diện: `SayTask` là menu nút (Thủ Khố, Võ Sư và các dịch vụ dùng API này); `Say` thông thường là danh sách (dịch chuyển); `SayButtons` là nút theo yêu cầu rõ ràng. Client đọc cờ từ gói hội thoại thay vì ép tất cả NPC về một kiểu. Không phân loại bằng độ dài nhãn.
- Bỏ chế độ chỉ vì có một nhãn dài mà cả menu bị chia thành mỗi trang một đáp án `[1/6]`. Nút của riêng hộp thoại NPC giữ đủ nhãn, tự chọn ba/hai/một cột và ngắt dòng khi cần. Sáu lựa chọn ngắn/vừa hiện cùng trang; nhận xe giả/xe lương hiện cùng nhau. Menu có nhiều hơn sức chứa vẫn có phân trang. Không sửa nút dùng ở các UI khác.
- HUD dùng tiêu đề 16, nội dung 13; nền trong suốt và vùng cuộn giới hạn như bản trước. Tách vùng tiêu đề/nội dung để không phải dùng chung một cỡ chữ. Chuỗi `<enter>` trong nội dung được chuyển sang xuống dòng thật trước khi mã hóa; không nối các thẻ xuống dòng thừa ở cuối. Kiểm tra qua bộ mã hóa text native xác nhận không rò `ter>`.
- Chữ nội dung màu nhạt; tên NPC xanh lá, vật phẩm vàng, quái đỏ. Bộ từ đối chiếu 112 tên lấy từ đồ thị nhiệm vụ, bảng vật phẩm và các mục tiêu đang dùng. Ví dụ Huyết Thư vàng, Sùng Hầu Hổ xanh và Hoàn Cẩu đỏ. Đây là bộ từ cho các nhiệm vụ đã sửa, chưa phải bộ phân tích ngữ nghĩa cho mọi câu hội thoại trong game.
- Cụm tên/thanh máu người chơi được nâng thêm 24 px khi đi bộ và 32 px khi cưỡi thú, cùng điểm neo trên đầu hiện có. Không đổi điểm neo quái, NPC, linh thú hoặc thú triệu hồi. Khoảng cách là hiệu chỉnh dự án theo ảnh tham chiếu, chưa đo đối chiếu bằng ảnh client chạy thật ở mọi loại thú cưỡi.
- Shop mở có mép dưới trùng cửa sổ trang bị F4, giới hạn trong chiều cao màn hình. Sửa cả điểm neo cố định của hiệu ứng mở, vì chỉ gọi `SetPosition` trước `Show` sẽ bị hiệu ứng ghi đè về vị trí cũ.

## Lỗi boss Vạn Tiên Trận

Đã xác định hai lỗi độc lập:

1. Tính thuộc tính NPC bằng `nValue * nScale / 100` với số nguyên 32 bit gây tràn ở 30.000.000 HP × 100. Dùng số nguyên 64 bit cho phép nhân và chặn kết quả ngoài miền int; cũng xử lý an toàn phép cộng tham số nền. Không giảm HP để né lỗi. Thổ/Thủy giữ 8.000.000 HP, Hỏa/Phong giữ 30.000.000 HP theo bảng hiện có.
2. Các template VTT dùng AIMode 500 nhưng bộ điều phối engine chỉ có nhánh 1–7 và 11, nên AI 500 không làm gì. Chỉ với template 129–148 tại đúng map VTT tương ứng 1067–1070, nối mode 500 vào AI chủ động loại 1 đang có: tìm địch và dùng bốn kỹ năng theo tỷ lệ của bảng. Giữ các loại AI khác và NPC ở map khác nguyên trạng. Đây là bộ tương thích của dự án, không tuyên bố đã phục hồi mã AI 500 nguyên bản.

Bốn tên kỹ năng của từng boss 133/138/143/148 đều khớp bảng `Skills.txt`. Không thêm trạng thái bất tử hoặc chặn người chơi gây sát thương để che lỗi. Chưa có lượt chơi trực tiếp chứng minh giao chiến hoàn chỉnh; kiểm tra hiện tại xác nhận dữ liệu HP, nhánh điều phối và mã được build/phát hành.

## Kiểm thử và triển khai

- GameClient, CoreServer, CoreClient build Release thành công.
- 214 kiểm tra: đọc `Npcs.txt` bằng bộ đọc native, HP không tràn, AI giới hạn đúng map/template, mã hóa chữ và màu HUD, bố cục vùng cuộn thực, menu sáu nút, độ cao tên người chơi và căn mép dưới shop.
- Kiểm tra Lua hội thoại thực xác nhận `SayTask` giữ thứ tự/điều kiện/nút; `MsgBox` giữ callback; `Say` thông thường không bị đánh dấu thành nút.
- Kiểm tra quyết định AI chạy nguyên hàm `KNpcAI::ProcessAIType01` lấy từ source: tìm được địch, chọn cả bốn ô kỹ năng theo các nhánh xác suất và yêu cầu đuổi mục tiêu khi ngoài tầm. API world/chiến đấu được mô phỏng; không chứng minh va chạm đòn đánh trong client thật.
- Kiểm tra hồi quy VTT: 116 điều kiện native và 344 điều kiện Lua vòng đời đạt. Các điều kiện này không thay thế giao chiến trong client thật.
- Dừng server sạch (exit code 0), giữ dữ liệu nhân vật sau lưu, thay ba binary và khởi động lại PID 92044. Có bản sửa tiếp theo riêng `Game.exe` cho điểm neo hiệu ứng mở shop; không cần khởi động server lần nữa.
- World mới đã kết nối Database, Relay và Gateway; đối chiếu đạt 275/275 file theo nội dung đợt trước cùng binary mới, giữ đủ 99 vị trí nhiệm vụ và 7 NPC Sùng Thành bổ sung. Snapshot sau khởi động có 3.993 thực thể NPC/quái. Không phát hiện lỗi trong stderr khởi động.

Biên nhận: [ba binary](../../analysis/quest_ui_polish_20261024/publication.json), [điểm neo shop](../../analysis/quest_ui_polish_20261024/publication_revision2.json), [server mới](../../analysis/quest_ui_polish_20261024/world.json), [kiểm tra world sau khởi động](../../analysis/quest_ui_polish_20261024/live_verification.json). Script/test tái lập trong `analysis/quest_ui_polish_20261024`; source chính tại `UiMsgSel`, `UiTaskNote`, `UiShop`, `KNpc`, `KNpcAI`, `KScenePlaceC` và các header tương ứng.

Chưa nghiệm thu trực tiếp bằng mắt trong client: công cụ điều khiển cửa sổ của phiên bị lỗi khởi tạo. Không khẳng định các ảnh trên đã được chụp lại hoặc đã đánh boss thật. File nhiệm vụ/nội dung và NPC đã triển khai ở đợt trước được đối chiếu hash/điểm đứng, không sửa lại trong đợt này.

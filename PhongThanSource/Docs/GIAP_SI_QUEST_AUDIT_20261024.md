# Kiểm tra nhiệm vụ Giáp Sĩ — 24/10/2026

> Đây là biên bản đợt đầu. Bản cập nhật tiếp theo có sửa UI/native, nhiệm vụ chung và vị trí Giáo Huấn; xem [biên bản mới](SHARED_QUESTS_UI_AUDIT_20261024.md) để biết số liệu kiểm thử, file triển khai và giới hạn hiện tại. Các khẳng định bên dưới về số lượng file, binary không đổi và vị trí Bắc Hải áp dụng cho đợt đầu.

Nguồn đối chiếu: [Tam Giới – Giáp Sĩ, VNG](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-giap-si.html). Phạm vi gồm sáu nhiệm vụ tân thủ và mười nhiệm vụ chủ tuyến trên trang này.

## Kết quả đối chiếu và bổ sung

| Nhiệm vụ | Kết quả |
|---|---|
| Hộp Gấm, cấp 1 | Giữ chuỗi gốc và giao dịch nhận thưởng đã có; ba người nhận thông báo vẫn có thể gặp theo mọi thứ tự. |
| Tân Thức, cấp 3 | Đủ thứ tự NPC và yêu cầu 10 Đoản Kiếm; khóa bước trả thưởng và chống gọi lại bước đã hoàn thành. |
| Dũng Đao, cấp 7 | Đặt bước nhận tại Sùng Ứng Loan như hướng dẫn; lấy ba bộ phận ở Hoàn Cẩu Vương, báo Triều Điền, sửa tại Âu Thiên Hóa rồi trả thưởng. |
| Kiếm Ái, cấp 14 | Sửa ngưỡng cấp 12 thành 14; nối Hoàn Cẩu Tinh và thưởng 3.000 kinh nghiệm, 5 Hồi Thành Phù. Giữ sách khai khoáng từ chuỗi cũ để tương thích mở hệ phái. |
| Trừ Họa, cấp 18 | Bổ sung Mạnh Tân → Tam Sơn → Đồng Quan, cửa sổ ấn ký 10 giây, 10 điểm Nguyên Sâm và 5 điểm Sơn Xuyên Liễu. |
| Giáo Huấn, cấp 20 | Bổ sung giới hạn ba lượt nhận/ngày theo UTC+7 cho cấp 20–28; từ cấp 29 nhận một lượt cuối diệt 40 Thiên Hạo. |
| Trung Thành, cấp 25 | Đủ Huyết Thư; sửa thưởng kinh nghiệm cũ 300 thành 20.000, giữ 30.000 lượng. |
| Trầm Hương, cấp 35 | Giao Trầm Hương Mộc; sách Ban Môn Lộng Phủ và 80.000 kinh nghiệm. |
| Khuyên Hàng, cấp 45 | Lý Tịnh và Đặng Cửu Công có thể gặp theo hai thứ tự; ấn tín phải có trước khi báo tin và nhận thưởng. |
| Thần Long, cấp 55 | Nối Thần Long → Đắc Kỷ → Hồ Hỷ Mị; giữ bảng thật cho phần sau. |
| Bình An, cấp 65 | Nối Cự Dã với An Cư Đồ; phần thưởng sách lấy trong 15 bản ghi đúng khoảng cấp của ba hệ. |
| Thần Dụ Kính, cấp 70 | Sửa điều kiện và phần thưởng theo hướng dẫn; giữ kính để chuyển tiếp Tam Sách. |
| Tam Sách, cấp 75 | Hai bảng giao đi, một bảng thật giữ lại; ba sách phải đủ trước khi đổi lấy bộ bốn pháp bảo. |
| Cổ Kim, cấp 80 | Bổ sung NPC thời gian, Bộ Đầu, Trúc Giản và Khứ Lai Phù. |
| Thiên Duyên, cấp 85 | Đủ Đắc Kỷ nhỏ → Khương Tử Nha nhỏ → Đắc Kỷ nhỏ → Bàn Cổ → trả Thần Khí. |
| Thông Thiên, cấp 92 | Đủ ba hung khí, gọi ba hồn, giới hạn 180 giây diệt Giáo Chủ, Hàn Băng Nguyên Khí và ba nút thưởng Giày / Yêu đái / Mão. |

## Hội thoại và chỉ dẫn

Hội thoại bổ sung dùng `SayButtons`. Nhãn là tên nhiệm vụ hoặc hành động ngắn, tối đa 13 byte TCVN3, vừa nút 95 px với font 14 của client hiện tại. Tên đầy đủ và nội dung nằm ở phần lời thoại; không cắt tên bằng dấu ba chấm. Tên nhiệm vụ màu vàng, mục tiêu màu cyan, thưởng màu xanh lá. Các nút mở lời giải thích trước khi xác nhận chuyển bước. Dịch vụ và nhiệm vụ cũ được giữ qua nút riêng.

Các quái có tọa độ cụ thể trên trang được bổ sung trong đúng ô tọa độ hướng dẫn; có 18 ô mục tiêu được đối chiếu. Vị trí MPS trong ô được chọn trên phần đi được. Những nơi hướng dẫn chỉ nêu khu vực (thu thập dược liệu, Giáo Huấn, tìm hung khí) dùng vị trí do dự án bổ sung và có chỉ dẫn trong hội thoại. Không coi các vị trí này là tọa độ máy chủ VNG đã khôi phục.

Gói hiện có 62 bước chuyển, 29 vai trò NPC và 67 bản ghi NPC/quái/điểm thu thập mới. Chi tiết tọa độ, mức bằng chứng địa hình và phân biệt `VNG` / `AUTHORED_AREA` nằm trong [manifest](../Deploy/ProjectContent/GiapSiQuests/manifest.json). Với ô thiếu Region_S, chỉ dùng Region_C làm bằng chứng vị trí; không chuyển đổi hoặc chế tạo địa hình server. Các bản ghi NPC đang có và các phần địa hình được giữ nguyên.

## Lưu tiến độ và kiểm chứng

Giữ task 20, 21, 24, 25 và task chủ tuyến 3. Task mới dùng **4550–4569**; đã tránh dải 4661–4664 mà Vạn Tiên Trận dùng. Không sửa file nhân vật và không phát lại thưởng cho nhiệm vụ đã hoàn thành trước bản cập nhật.

Thông Thiên dùng NPC hội thoại mẫu 202 và boss nhiệm vụ riêng **2703**, có cùng tài nguyên hình ảnh với boss 148. Cách tách này tránh quy tắc engine tự dời mẫu 148 vào sân boss của sự kiện Vạn Tiên Trận. Boss 148 của sự kiện giữ script cũ. Mẫu nhiệm vụ mới có `LifeParam1=1000000`, được kiểm tra để phép nhân phần trăm sinh lực không tràn số nguyên; đây là tham số do dự án đặt, không khẳng định là chỉ số VNG. Client được bổ sung đúng hàng mẫu mới, giữ các hàng trước đó.

Đổi vật phẩm/thưởng dùng `QuestExchange`; kiểm tra lại hệ, bản đồ, cấp và bước ở hàm xác nhận. Khoản tiền được kiểm tra theo số dư thực tế và hoàn lại khi đổi vật phẩm thất bại. Bộ đếm thu thập và ba hồn có chặn ghi nhận lặp. Thời hạn dùng thời gian tuyệt đối để thoát game không kéo dài hiệu lực.

- 67 payload qua biên dịch và thực thi khởi tạo bằng Lua 4 thật.
- 12 tình huống hành trình, 1.143 kiểm tra: thứ tự, vật phẩm, thưởng, đầy túi, lặp, cấp/hệ/bản đồ, giới hạn ngày, thời gian và nhãn nút. Hộp Gấm được chạy qua script NPC hiện tại với cầu nối nút dịch vụ mới. Player/combat APIs trong bộ này là mô phỏng.
- Bộ nạp bảng native xác nhận 12 danh tính trang bị thưởng và bốn vật phẩm sử dụng được sửa; các kiểm tra thuốc/sách nền vẫn đạt.
- Kiểm tra cấu trúc và hash của gói; giữ toàn bộ bản ghi NPC và phần địa hình cũ trong các ô được chỉnh.

Chưa nghiệm thu bằng việc điều khiển nhân vật đi hết 16 chuỗi trong client; chưa có ảnh chụp hội thoại chạy thật. Phần bổ sung là dựng lại chức năng theo hướng dẫn và dữ liệu hiện có, không khẳng định toàn bộ là script server VNG nguyên bản.

## Mã nguồn và triển khai

- `Tools/giapsi/quests.cjs`: đồ thị nhiệm vụ và trạng thái.
- `Tools/giapsi/runtime.lua`, `items.lua`: nguồn UTF-8; builder xuất TCVN3.
- `Tools/build_giapsi_quests.cjs`: đóng gói và kiểm tra vị trí.
- `Deploy/Publish-GiapSiQuestContent.ps1`: cài gói vào runtime đã dừng; được gọi từ luồng publish nhiệm vụ và loose runtime.
- `Tests/Native/GiapSiQuestJourney.inl`: hành trình kiểm thử; runner và biên nhận triển khai tại `analysis/giapsi_20261024` ở thư mục workspace.

Gói đã được đưa vào `PhongThanRuntime-Loose`; file thay đổi có bản sao lưu, server được yêu cầu lưu và thoát sạch trước khi khởi động lại. Biên nhận hash và PID được lưu cùng runner. PAK, dữ liệu nhân vật và các binary không bị thay đổi.

World mới đã kết nối database, relay và gateway; snapshot native có 3.954 NPC/quái so với 3.887 trước cập nhật. Kiểm tra chi tiết các điểm thêm trong `analysis/giapsi_20261024/live_verification.json`.

Kiểm tra world cuối cùng đạt: **67/67 vị trí** theo tọa độ gốc của thực thể, toàn bộ quái tĩnh bổ sung có sinh lực dương; **116 file** triển khai khớp hash. Biên nhận cuối là `publication_revision2.json` và `world_revision2.json`.

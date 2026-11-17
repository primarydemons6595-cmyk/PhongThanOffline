# Lời thoại nhập môn ba phái và thưởng Loose — 29/10/2026

## Nguồn gốc và phạm vi

Tân Thức Giáp Sĩ (task 21), Ngũ Thất Đạo Sĩ (task 11) và Tân Thức Dị Nhân (task 31) đang chạy qua bảng bước phục dựng, không dùng trực tiếp toàn bộ callback Lua gốc. Bảng bước giữ điều kiện/chuyển bước nhưng phần xem nhiệm vụ chỉ có chỉ đường. Đã bổ sung lời thoại cốt truyện từ tài nguyên gốc vào 20 bước của ba chuỗi này; không tuyên bố khôi phục toàn bộ nhiệm vụ mọi cấp của ba phái.

Đã đọc lại 12 script gốc từ PAK thắng theo `Client_Original/package.ini`, xác nhận hash với các fixture đã kiểm toán và lưu bản đọc trong `analysis/starter_story_20261029/originals`. Lời thoại gồm 23 ID từ `settings/StringResource.txt`, bản thắng `vng00.pak`, entry 4810, ID EE5FAD05, SHA-256 `a911c9a7126e80ca3f6d3c8218a940dba15fc70a1d8d17dbb74f36b07fba4d15`. Danh sách script/ID/chuỗi gốc và mọi thay đổi tương thích nằm trong `provenance.json` của gói.

Bản bảng thoại gốc có một số tên và số lượng không khớp chính callback Lua gốc: Ngũ Thất mở đầu ghi 2 Băng Cơ nhưng callback đòi 10; còn gọi Hoàng Long/Nhiên Đăng bằng tên Thợ Đồng/Kim Hà; thoại trả Hỏa Thạch chỉ nhầm Xích Tinh Tử. Nhánh Dị Nhân cũng có tên Thợ Đồng và địa danh cũ. Chỉ sửa các chỗ này để khớp mục tiêu/callback đang chạy, giữ nội dung cốt truyện còn lại và thêm chỉ dẫn hiện tại. Chuẩn hóa `<c=...>` thành `<color=...>` để UI hiện màu đúng.

## Thưởng theo yêu cầu người dùng

Script gốc của cả ba chuỗi thưởng 500 EXP + 600 lượng cuối nhiệm vụ, không có 500 EXP mỗi lần gặp NPC. Theo yêu cầu mới của người dùng, bản Loose thưởng 500 EXP cho mỗi bước trung gian chuyển trạng thái thành công, và 5.000 EXP cho bước kết thúc; vẫn giữ 600 lượng cuối chuỗi.

| Chuỗi | Bước trung gian | EXP kết thúc | Tổng EXP | Lượng cuối |
|---|---:|---:|---:|---:|
| Giáp Sĩ — Tân Thức | 8 × 500 | 5.000 | 9.000 | 600 |
| Đạo Sĩ — Ngũ Thất | 4 × 500 | 5.000 | 7.000 | 600 |
| Dị Nhân — Tân Thức | 5 × 500 | 5.000 | 7.500 | 600 |

Chỉ phát EXP sau khi giao dịch vật phẩm/chuyển bước thành công. Mở xem, bấm lại callback, sai NPC/map/phái/bước, đang giao dịch/khóa hay thiếu đồ không nhận thưởng. Giao dịch thất bại hoàn lại lượng đã tăng trước đó, không phát EXP. Không sửa hay tự phát bù cho bước nhân vật đã hoàn tất trước khi cập nhật.

Giữ bộ kiểm tra vật phẩm và giao dịch hiện có thay vì gọi thẳng các callback gốc vốn có cấp vật phẩm/thưởng không kiểm tra kết quả. Bản bổ sung lời thoại bọc preview/apply của hai runtime, không ghi lại hàm dịch vụ khác của NPC. Các menu dịch vụ và nhiệm vụ ngoài ba task trên vẫn đi qua hàm cũ. Thêm guard cài đặt để không bọc đệ quy nếu payload được ghép lại.

## Kiểm tra và triển khai

- `python PhongThanSource/Tools/build_starter_story.py`: lấy/kiểm tra PAK, dựng gói và ghi provenance; không tự sửa runtime.
- `python PhongThanSource/Tools/test_starter_story.py`: thực thi 12 script staged qua Lua4 thật; 214 kiểm tra hành vi dùng hàm preview/apply sản xuất, cô lập API trạng thái nhân vật/túi/UI. Kiểm tra cả ba chuỗi, tổng EXP/lượng, xem trước không đổi bước, replay không trả thưởng lần hai, khóa/giao dịch/thiếu đồ và rollback giao dịch thất bại.
- Chỉ áp 12 Lua vào `PhongThanRuntime-Loose/Server`, có kiểm tra hash trước/sau và backup. Nạp lại từng script qua cơ chế reload hiện có: 12/12 thành công, failed=0, cùng world PID 130368; không khởi động lại server, không sửa file nhân vật, không thay client/Core DLL hoặc runtime Release.
- Cập nhật dữ liệu authoring mức thưởng và nối payload vào hai builder nhiệm vụ. `Publish-StarterStoryContent.ps1` được gọi sau publisher thông báo boss để bản publish sau không ghi đè bằng lời thoại cũ.

Gói: `Deploy/ProjectContent/StarterStory20261029`. Bằng chứng và backup: `analysis/starter_story_20261029`. Chưa nghiệm thu toàn bộ lời thoại bằng nhân vật thật; kết quả Lua4 không phải chứng nhận giao diện trong game.

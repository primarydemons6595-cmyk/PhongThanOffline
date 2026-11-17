# Thần Kỹ thử nghiệm — cập nhật Loose 07/11/2026

Người dùng đã chọn dùng **công thức Loose chỉnh được trong Studio**, có nhãn thử nghiệm, cho sáu nhóm còn thiếu công thức server gốc. Người dùng xác nhận phần hồi chiêu **chỉ áp dụng Thần Kỹ**; kỹ năng phái thường đang ổn và không được chỉnh.

Đã cài bản cập nhật vào `PhongThanRuntime-Loose`. Biên bản và sao lưu nằm trong `analysis/than_ky_experiment_20261107/installation.json`. Không build/publish bộ runtime Release.

## Các nhóm đã nối

| Nhóm | ID chính | ID hiệu ứng/chiêu phụ giữ từ bảng gốc | Hồi mặc định |
|---|---:|---|---:|
| Huyết Nguyệt Trảm | 1986 | 1987 → 1988 → 1989 | 0 |
| Huyền Ảnh Tàn Hoa | 1996 | 1997 → 1998 → 1999 | 3 nhịp |
| Phật Nộ Hỏa Liên | 1990 | 2000 | 50 nhịp |
| Lôi Hỏa Toàn/Tàn Phong | 1991 | 1992 | 50 nhịp |
| Truy Hồn Tán Hoa | 1993 | theo missile gốc | 50 nhịp |
| Vạn Kiếm Quy Tông | 1994 | theo missile gốc; không tự nối thêm biến thể 1995 | 40 nhịp |

Một giây là 18 nhịp game. Huyết Nguyệt Trảm có `TimePerCast=0` trong bảng gốc; các mức 3/40/50 trên cũng lấy từ bảng gốc. Thời gian động tác và kiểm tra khoảng cách vẫn có hiệu lực. Không tự thêm thời gian hồi cho chiêu Giáp Sĩ hay xóa thời gian hồi Đạo Sĩ.

Đã kiểm tra 39 tham chiếu hình ảnh, tương ứng 18 SPR riêng: icon, hình thi triển và missile đều có trong PAK gốc, có khung hình và được index Client Loose phân giải. Không chỉnh các trường hình ảnh, projectile, loại vũ khí, phạm vi hoặc chuỗi sự kiện gốc. Đây là kiểm tra tài nguyên/đường xử lý, chưa phải nghiệm thu trực tiếp một phiên chiến đấu.

Các số sát thương/mana còn thiếu được đặt riêng dưới nhãn **Loose thử nghiệm**, không nhận là số liệu VNG. Công thức là `cơ sở + (cấp sách − 1) × mức tăng`. Preset ban đầu: tăng vật lý 80% +20% mỗi cấp; sát thương nguyên tố 500–750, tăng 150–200 mỗi cấp; bỏ qua phòng thủ 10% +2% mỗi cấp khi thuộc tính đó có trong bảng. Hai thuộc tính bổ sung chưa xác minh tác dụng gốc để giá trị 0: điều chỉnh chống bạo kích phép và hiệu ứng Hỏa; muốn thử phải đặt hiệu lực hữu hạn. Các giá trị đều chỉnh được trong Studio.

## Quản lý trong Studio

Vào **Quản lý → Thần Kỹ**:

- Tab công thức thử nghiệm: chọn nhóm, xem icon gốc, bật/tắt, chỉnh mana, hồi chiêu theo giây và các thành phần sát thương/hiệu lực theo cấp. Chiêu phụ được chỉnh riêng; cấp sự kiện tự đi theo cấp sách.
- Tab hồi chiêu đã phục hồi: chín nhóm dùng công thức gốc mặc định không bị ghi đè. Có thể chủ động thử hồi chiêu khác cho đúng Thần Kỹ đã chọn.
- Đưa cấu hình vào patch Client + Server, áp dụng rồi khởi động lại game và GameServer. Các đối tượng kỹ năng được cache; không quảng cáo là đổi nóng trong phiên đang chạy.
- Native chỉ nhận đúng các ID/thuộc tính đã định nghĩa, kiểm tra giá trị ở cả cấp 1–10; cấu hình lỗi làm các nhóm thử nghiệm không thi triển, không áp giá trị lỗi sang chiêu phái khác.

Tệp dùng chung hai vai: `settings/phongthan/ThanKyExperimental.ini`. Công thức mới đi qua bộ diễn giải thuộc tính chiến đấu sẵn có trong KSkill; không cấp sát thương giả bằng thông báo Lua. Tooltip của các chiêu mới ghi rõ Loose thử nghiệm.

## Sách, Cẩm Nang và NPC

- 71 mục Thần Kỹ trong Cẩm Nang Sự Kiện giữ nguyên ID callback. Các sách mới đổi nhãn từ “chưa nối” thành “Loose thử nghiệm”.
- Nối 51 biến thể sách về 15 ID chiêu chính. Đổi/nâng sách cần kích hoạt lại; chiêu cùng nhóm được thay cấp trên một ID, không tạo hai lựa chọn Lv1/Lv2. Kiểm tra đúng serial, túi mang theo, phái, cấp và điều kiện Nhân Giới.
- Hồ Lô Đằng vẫn ở Ngọc Hư Cung (207,207), template 2711. Menu có 29 lựa chọn: 11 cũ, 6 sách cấp 1 mới và 12 cách nâng cấp 2.
- Nâng cấp 2 dùng sách cấp 1: cách có mảnh thêm 4 mảnh +2 Đa Bảo Lệnh Bài +2.000 Linh Thạch; cách không mảnh thêm 4 Đa Bảo Lệnh Bài +4.000 Linh Thạch. Có nhận các biến thể sách cấp 1 tương ứng trong cấu hình. Không dựng công thức NPC cấp 3–5 khi bài không ghi; các sách đó có thể nhận để thử trong Cẩm Nang.

Nguyên liệu và hai cách nâng theo [bài VNG](https://pt.vnggames.com/su-kien/than-ky/noi-dung-1443.html); số sát thương thử nghiệm do Loose cấu hình. Giữ phí 1 Linh Thạch khi kích hoạt; sách không bị tiêu hao khi chỉ kích hoạt.

## Kiểm chứng

- Native profile: 270 lượt thuộc tính/cấp; cấu hình tắt, sai khoảng sát thương và tràn số đều được kiểm tra. Các chiêu phái ngoài danh sách Thần Kỹ giữ giá trị hồi truyền vào.
- Mã runtime thực tế trong harness: chín nhóm trước và 40 sách mới, thay cấp/thu hồi/khôi phục; không trùng ID. Nhân vật/túi/địa hình trong harness là adapter, không phải phiên game thật.
- Lua cũ thật: 50 script sách/mảnh, menu 29 xác nhận/đổi, phân trang Cẩm Nang và menu Tiên/Ma cũ.
- Wrapper NPC và transaction native thật: 29 công thức, hai cách nâng cấp, không phát hai lần, 261 trường hợp lỗi/hoàn tác/kiểm tra điều kiện với túi giả lập.
- Managed settings: INI khớp native, đọc/ghi không đổi số, patch đồng bộ có hash nền, chặn sửa ID chiêu phái và cấu hình nguy hiểm.
- Studio build không lỗi/cảnh báo, ảnh render đã xem; mở/nạp lại không tự dirty. Hai DLL Dev build và nạp phụ thuộc sau cài đều qua.
- 87 tệp CharacterStore, 41 binary Release và bốn bảng Skills/Missles Client/Server không đổi.

Chưa có kiểm chứng đánh trong game. Bản này không đánh dấu các công việc bot, F3/F4/Avatar và script rơi đồ boss trước đó là hoàn tất. Gói dữ liệu: `Deploy/ProjectContent/ThanKyExperiment20261107`; các script tạo nội dung cũ trong thư mục audit chín nhóm chỉ còn là lịch sử, không dùng chúng để ghi đè bản mới.

# Loose: boss Tiên/Ma, hai công thức thiếu và ảnh nút F5

Đã cài vào `PhongThanRuntime-Loose` ngày 08/11/2026. Biên nhận và hash: `analysis/realm_trial_20261108/installation.json`. Bản sao lưu: `analysis/realm_trial_20261108/backup-20261108-025149`. 37 file được cập nhật; 87 file CharacterStore, 77 file lưu bot, 41 binary runtime Release và bốn bảng Skills/Missles giữ nguyên hash. Không build runtime Release.

## Nút Tiên-Ma F5

Chữ nằm trong ảnh SPR, không phải Text của nút. Đã dùng **image_gen tích hợp, chế độ edit** tạo lại ảnh theo nút gốc, sau đó chuyển định dạng thành SPR indexed có alpha, 60 × 22, bốn frame: bình thường, được chọn, vô hiệu hóa, rê chuột. Không vẽ chữ đè lên ảnh Trung Quốc.

- Ảnh sinh: `analysis/realm_trial_20261108/tien-ma-generated.png`.
- Bản xem trước giải mã từ SPR: `analysis/realm_trial_20261108/tien-ma-native-preview.png`.
- Prompt đầy đủ và chế độ tạo ảnh: `analysis/realm_trial_20261108/button-generation.json`.
- Ảnh runtime: `Client/spr/pt_ui/tien_ma_tab.spr`; cấu hình: `Client/ui/realm_skills.ini`.
- Giáp Sĩ/Vũ Sĩ: (140,39); Đạo Sĩ: (144,5); Dị Nhân: (140,28). Giữ điều kiện hiện tab khi đã lên Tiên/Ma.

Bộ đọc SPR của Engine client đã đọc được cả bốn frame. Bộ dựng trang F5 sao chép đủ Width/Height/CheckBox/CheckOver/OverFrame/DisableFrame từ cấu hình.

## Cơ chế thử nghiệm đã được người dùng cho phép

Nguồn hiện có vẫn thiếu thuật toán server VNG cho 11 script chết/rơi đồ và hai chiêu dưới đây. Bản cài ghi rõ **LOOSE_EXPERIMENT_NOT_VNG** trong Studio, cấu hình, script và nhật ký; các số dưới đây là mặc định thử nghiệm.

Studio → Quản lý → **Tiên/Ma thử nghiệm** có hai tab, dùng ảnh NPC/vật phẩm/icon gốc. Thay đổi được đưa vào patch rồi áp dụng theo luồng Studio hiện có.

### Boss · rơi đồ

Khôi phục đầu vào OnDeath cho Thân Công Báo, Ngũ Độc Xà, Minh Sa Ma, Boss Thiên/Địa/Nhân 2013, Chúc Long, Huyết Long, Giao Long, Ly Long, Di Long: 11 script, 14 mẫu NPC. Các ID native được kiểm tra cùng hash script chết, không dựa vào tên giống nhau.

Cấu hình: `Server/settings/phongthan/RealmBossExperimental.ini`. Có bật/tắt toàn bộ hoặc từng boss, lượt quay, Range, vật phẩm chọn bằng hình, số lượng và xác suất. Các biến thể của cùng boss dùng chung bảng.

Mặc định: một lượt/lần chết, Range 10.000; Mảnh Lam thủy tinh 80%, Lam thủy tinh 10%, không rơi 10%; số lượng 1. Đây không phải tỷ lệ VNG. Mỗi lượt chọn tối đa một dòng; nhiều lượt có thể trùng. Mỗi đơn vị được tạo thành vật phẩm trên đất qua bộ tạo item/object native và gắn quyền nhặt của người hạ boss.

Callback kiểm tra chủ sở hữu, cùng map, trạng thái chết, đúng mẫu NPC/script và dấu đã xử lý theo vòng đời. Chặn gọi lại thưởng hai lần. Dấu được xóa khi NPC tạo lại/hồi sinh. Với boss thuộc cơ chế này, đường rơi đồ mặc định không chạy thêm một lần nữa. Cần reset GameServer sau khi áp cấu hình.

### Hai kỹ năng bị động

Huyết Chiến Sa Trường (1481, Giáp Sĩ) và Độc Hành Thiên Hạ (1487, Dị Nhân) có SkillStyle=3 và hai thuộc tính gốc: `enhance_life_rate_damage`, `attackspeed_v`. Đã bổ sung hai script tính cấp độ đúng đường dẫn legacy trên cả Client và Server.

Mặc định cấp 1–10: hệ số sát thương theo sinh lực tối đa từ 10 đến 100 phần nghìn (1–10%); tốc độ đánh cộng từ 1 đến 10. Studio sửa được giá trị đầu/tăng mỗi cấp và bật/tắt từng chiêu. Đây là đầu vào của công thức chiến đấu native đang có, không phải lời cam kết tổng sát thương cuối bằng đúng tỷ lệ này trong mọi tình huống.

Script bị động trả tuple `giá_trị,-1,0` để thuộc tính được lưu thành trạng thái thường trực; dùng `giá_trị,0,0` sẽ bị phân loại thành tác động tức thời và không đi qua đường áp bị động. Kiểm tra native đã bắt được và sửa điểm này trước khi cài.

Studio tạo đồng thời INI và Lua cho hai phía. Cần mở lại game và GameServer để bỏ cache công thức. Không thêm hồi chiêu hay hiệu ứng đánh chủ động cho hai chiêu bị động. Không sửa hồi chiêu skill phái thường. Cẩm Nang Sự Kiện → học max Tiên/Ma giữ kiểm tra đủ năm chiêu trước khi học.

## Kiểm tra và giới hạn

- Build Studio: 0 lỗi, 0 cảnh báo. Dựng ảnh bằng chính view đã cài, nạp lại hai lần không phát sinh trạng thái chỉnh sửa: `analysis/realm_trial_20261108/installed-boss.png`, `installed-skills.png`.
- Managed: ánh xạ 11 nhóm/14 mẫu, biên trọng số/không rơi, từ chối NPC ngoài danh sách/cấu hình sai, roundtrip công thức và giới hạn từng cấp.
- Lua runtime gốc: 56 lượt kiểm tra gồm cấp 1–10, ngoài giới hạn và tắt công thức; giá trị đúng cả ba thành phần tuple. `lua-results.tsv`.
- CoreServer thật trong runtime tách biệt: học đủ năm chiêu cấp 10 cho Giáp Sĩ/Dị Nhân, hệ số sinh lực 100, tốc độ đánh cộng 10; tính lại thuộc tính hai lần không cộng dồn. 14 mẫu boss đều tạo đồ native, từ chối chủ sở hữu sai và NPC chưa chết, chặn callback lặp, cho thưởng lại ở vòng đời tiếp theo. `probe-final.stdout.log`. Bảng rơi đồ trong harness được đặt riêng thành 100% để kiểm tra xác định; bảng cài vẫn là 80/10/10.
- Clean rebuild hai Core Dev sau khi bỏ export kiểm thử; bản cài được kiểm tra không có PtProbeRealm/PtProbeBot. DLL đã cài nạp được; hash toàn bộ 37 file đã đối chiếu.
- Chưa nghiệm thu chiến đấu/boss/F5 trực tiếp trong một phiên chơi sau cài; chưa đo lại FPS F3/F4 trong game. Các bản sửa F3/F4 và bot trước đó được giữ lại trong bộ build này.

Gói nguồn nội dung và ảnh/prompt lưu tại `PhongThanSource/Deploy/ProjectContent/RealmTrial20261108`. Không dùng DLL của harness để triển khai.

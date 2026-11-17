# Thương Điếm và nhận nhiệm vụ ngày — Loose, 29/10/2026

## Nguyên nhân

- `GetNpcIdx` trong ScriptFuns.cpp trả về NPC của chính người chơi. Controller Bào Thương lại dùng nó để kiểm tra template 158, nên luôn từ chối nhân vật bình thường và đóng thoại.
- Task 60 là số thương điếm thuê lạc đà (1..9). Controller chỉ nhận giá trị 1, làm lạc đà thuê ở tám điểm khác không mua bán/nhận/giao được nhiệm vụ.
- Bốn script ngoài thành có RandomTask ở dòng menu thứ 5 nhưng luôn gán show=0. Chúng không cho nhận chuyến đầu dù đã thuê lạc đà.
- Hai callback nhiệm vụ ngày trả về im lặng khi sai vị trí, chưa đủ cấp, hết lượt hoặc giao dịch đổi vật phẩm không thành công. Vị trí kiểm tra dùng góc ô tọa độ thay vì điểm NPC thật, khiến nhân vật đứng sát NPC ở phía xa góc ô có thể bị từ chối.

## Sửa

- Nhận chỉ số NPC từ đối số `main(npc)` mà KPlayer::DialogNpc/ExecuteScript truyền vào. Lưu contact theo PlayerIndex và PlayerId, kiểm tra entity ID, template, map và khoảng cách mỗi callback. Không sửa nghĩa của GetNpcIdx.
- Chấp nhận nguồn thuê lạc đà 1..9; giữ nguyên cấp mở vùng, giá, sức chở, hàng tồn, phí và thưởng script gốc.
- Hiện dòng nhận chuyến tại bốn điểm ngoài thành. Giữ nguyên các menu gốc ở năm thành.
- Tứ Tượng Linh Tê kiểm tra điểm NPC Triều Ca (54912,100096), Tây Kỳ (44672,96512), đơn vị MPS; Thiên Cương Hồn kiểm tra Ân Giao (47216,99056). Bán kính 400 MPS giữ nguyên.
- Hiện lý do khi từ chối, gồm thiếu vật phẩm, tiền, hết lượt, khóa/giao dịch và vật phẩm không thể đổi. Thêm xác nhận khi nhận Thiên Cương thành công; kiểm tra danh sách đích trước khi thu phí. Nhiệm vụ hết hạn được dọn khi mở menu/nhận lại.
- Giữ điều kiện hiện có: Linh Tê cấp 65, 1 Sơn thạch và phí theo cấp; Thiên Cương cấp 75, 1 Thu thập + 1 Tứ Tinh và cấp×1000 lượng. Không khẳng định các điều kiện dựng trước đây là toàn bộ quy tắc gốc đã được phục hồi.

## Kiểm tra và áp dụng

`Tools/test_quest_service_repair.py` chạy Lua 4 thật, triển khai Include toàn cây. 147 kiểm tra đạt: gọi script gốc của cả 9 điểm, thuê lạc đà, menu nhận chuyến, chọn hàng khác điểm, thiếu hàng không thưởng, giao hàng/thưởng một lần, mua/bán/thanh toán và từ chối từ xa hoặc sai nhân vật; hai nhiệm vụ ngày kiểm tra thiếu vật phẩm/tiền, xa NPC, khóa, transaction thất bại, nhận thành công và chống thu phí trùng.

Các API game bên ngoài trong fixture được mô phỏng theo source hiện tại. Chưa nghiệm thu bằng nhân vật thật qua toàn tuyến trong client. Không coi harness là bằng chứng toàn bộ chín script đã chuẩn mọi hành vi gốc.

Gói `Deploy/ProjectContent/QuestServices20261029`: 13 file được xác minh; 8 file thay đổi, 5 script thành giữ nguyên byte. Nạp lại 14 Lua state liên quan trên world Loose PID 165632, failed=0; không restart server, không thay DLL/client, không sửa dữ liệu nhân vật và không áp Runtime Release. Không reload VM tick nhiệm vụ chung để giữ danh sách quái của nhiệm vụ đang chạy; helper mới phục vụ nhận nhiệm vụ được nạp qua VM NPC.

Bằng chứng: `analysis/quest_services_20261029/behavior.log`, `publication.json`, `final-verification.json`. Backup trước sửa nằm tại thư mục ghi trong publication.json. Publisher offline chỉ chấp nhận Loose Server và được nối sau StarterStory để các lần triển khai tiếp theo giữ bản sửa.

# Studio nhiệm vụ, ấn ký và các sửa lỗi — 05/11/2026

## Trạng thái

Đã biên dịch và nạp Game.exe, CoreClient.dll, CoreServer.dll cùng hai DLL Studio vào bản Runtime-Loose. Lần nạp cuối lúc 01:09 ngày 05/11/2026, có sao lưu và đối chiếu SHA-256. Biên nhận: `analysis/tet_event_shop_20261103/quest-seals-deployment.json`; lần nạp nền: `quest-followup-deployment.json` cùng thư mục.

Studio đã mở lại, nạp Client + Server và hiện tab Tạo nhiệm vụ cùng bộ chọn NPC có hình ảnh thật. Người dùng đang thao tác hộp chọn NPC nên giữ nguyên cửa sổ và nội dung đang nhập. Không phát hành nhiệm vụ mẫu, không sửa dữ liệu nhân vật hay các định nghĩa sự kiện đang có. Bộ cài không tự khởi động world.

## Tạo nhiệm vụ trong Studio

Vào **CHỈNH SỬA SERVER → Tạo nhiệm vụ**.

- Tạo nhiệm vụ chung ba phái hoặc riêng từng phái; điều kiện cấp độ, nhiệm vụ trước, làm một lần hoặc lặp theo ngày/tuần/không giới hạn.
- Chọn NPC có sẵn hoặc đặt NPC mới trực quan trên bản đồ, có ID và ảnh. NPC có sẵn giữ chức năng gốc qua nút riêng.
- Xếp chuỗi gặp NPC A/B/C, đánh đúng loại quái hoặc boss theo ID/bản đồ, thu thập, giao vật phẩm và đổi nguyên liệu lấy quà.
- Chọn vật phẩm bằng ID và ảnh; phần thưởng gồm vật phẩm/trang bị/thú cưỡi, kinh nghiệm, bạc, bạc khóa, danh vọng và danh hiệu theo ID máy chủ hỗ trợ.
- Cập nhật thưởng theo phái ngày 05/11: **Phần thưởng → Sửa phần thưởng** có bốn tab **Chung / Giáp Sĩ / Đạo Sĩ / Dị Nhân**. Bật “Dùng phần thưởng riêng” ở phái cần tách để cấu hình toàn bộ gói riêng (item, EXP, bạc, bạc khóa, danh vọng, danh hiệu). Gói riêng thay gói chung, không cộng cả hai; phái không bật dùng gói Chung. Dữ liệu nhiệm vụ cũ giữ cơ chế thưởng chung. Lưu phần thưởng, Lưu vào bản sửa và áp dụng Patch như bình thường.
- Nội dung chào, từng bước, hoàn thành và mô tả hỗ trợ màu; có khung xem trước. Có lưu bản nháp và sao lưu phiên bản nháp trước khi ghi đè.

**Lưu bản nháp** giữ nội dung đang soạn. **Lưu vào bản sửa** sinh cấu hình và Lua vào bản sửa của Studio; áp dụng bằng quy trình bản sửa hiện có. Việc lưu bản nháp riêng không tự công bố nhiệm vụ lên server.

## Điểm thu thập và ấn ký

Tab **Điểm thu thập** chọn Object thật bằng ID/ảnh, đặt trên map, sửa tên và thời gian hồi. Bước thu thập chọn thời gian thao tác, vật phẩm nhận được và số lượng cần. Dùng thanh tiến trình có sẵn của game; di chuyển, nhận sát thương hoặc chết sẽ hủy thao tác. Điểm dùng chung ngăn hai người cùng nhận một lượt; túi đầy không mất nguyên liệu hay tiến độ.

Tab **Ấn ký & thời hạn** hỗ trợ cả hai loại:

- **Buff:** hiện trên thanh buff bằng icon đã chọn, tên riêng, mô tả và số giây hiệu lực.
- **Vật phẩm F4:** vật phẩm riêng có tên, icon, mô tả và hạn sử dụng; không đổi tên toàn bộ vật phẩm nền. Chọn vật phẩm nền QuestKey, không dùng các ID tiền tệ.

Cả hai loại đều sửa được tên, chọn icon từ danh mục vật phẩm hoặc SPR client, đặt thời hạn 1–2.592.000 giây. Chọn ấn ký được cấp khi nhận nhiệm vụ ở phần sửa nhiệm vụ; chọn ấn ký bắt buộc ở phần sửa từng bước.

Ấn ký có thể là điều kiện thu thập ngoài đất **hoặc điều kiện hạ quái để nhận vật phẩm nhiệm vụ**. Hết hạn sẽ không nhận vật phẩm từ điều kiện đó; có thể xin lại tại NPC bắt đầu khi nhiệm vụ còn đang làm. Không gia hạn khi ấn ký vẫn còn hiệu lực. Có lựa chọn tiêu hao sau khi thu thập/nhận vật phẩm thành công; thất bại hoặc túi đầy không tiêu hao. Có thể yêu cầu thêm nguyên liệu F4 thông thường bên cạnh ấn ký.

Giới hạn hiện tại: 64 nhiệm vụ, 62 bước mỗi nhiệm vụ, 16 nhóm phần thưởng và 64 định nghĩa ấn ký. Kiểm tra khi lưu sẽ báo cấu hình vượt giới hạn hoặc không hợp lệ.

## Sáu nhóm sửa lỗi đã nạp

1. Hộp thoại: tăng chữ lên 16, căn vào vùng nội dung, phân trang nội dung dài và giữ đúng thứ tự xác nhận; hỗ trợ thẻ màu.
2. Buff vật phẩm: sửa xung đột mã truy vấn giữa buff và khóa rương, sửa đồng bộ timer và nạp lại thanh icon. Đã thấy trực tiếp icon Ngọc Tán, Lâm Tiên Lộ và Thiên Hương cùng thời gian còn lại trong game ở lượt kiểm tra trước. Lọc các icon kỹ năng nội tại không cần hiện; hiệu ứng bật vô hạn dùng nhãn “Đang bật”.
3. Tạp Hóa: loại đồ trắng Vũ Sĩ họ 310 khỏi nhận diện nhầm bộ lục; kiểm tra 25 mục trắng được loại và 15.100 dòng bộ thật được giữ. Đây là sửa phân loại cửa hàng, không đổi tỷ lệ boss.
4. Hồi sinh: giữ khung gốc, tô màu lựa chọn, tổn thất và chi phí theo mẫu; giữ nguyên cơ chế trừ phí của máy chủ.
5. PvP: chỉ phát thông báo khi nhân vật thực sự thương vong/ngã ngựa, gửi Bang và Bản địa với tên/map có màu. Không báo mỗi đòn đánh; lọc đấu tay đôi/chiến trường/bot và chống trùng.
6. Thuê kho: bỏ yêu cầu đứng gần Thủ Khố. Giữ điều kiện phiên chơi, khóa/giao dịch, giá Xu và thời hạn gốc; thông báo thành công/thất bại. Rương 2–5 mua hẳn lần lượt 45/60/99/149 Xu; thuê 30 ngày 9/20/30/45 Xu.

## Kiểm chứng và giới hạn

- Cả CoreServer, CoreClient và GameClient build Release không lỗi; Studio build không lỗi.
- Bộ kiểm tra runtime nhiệm vụ, Lua sinh ra trong VM thật, thu thập bằng tiến trình native, hồi điểm, tranh chấp hai người, túi đầy và giao dịch nguyên tử đều qua.
- Kiểm tra hai loại ấn ký: tên/icon, hết hạn, xin lại, không gia hạn sớm, không dùng nhầm vật phẩm nền hay ấn ký khác, chỉ tiêu hao sau thành công, không rơi vật phẩm khi thiếu/hết ấn ký đều qua.
- Đã kiểm tra lưu/nạp mô hình, sinh cấu hình hai phía, bản nháp và giao diện WPF; kiểm tra hồi quy trao đổi vật phẩm, Event, F2, bảo vật và nâng ngọc đều qua.
- Log chính: `quest-seal-final-tests.log`, `quest-checks-final.log`, `quest-seal-text-tests.log`, `quest-seal-ui-config-tests.log`, `quest-seals-final-native-build.log` trong `analysis/tet_event_shop_20261103`.

Sau lần nạp cuối, chưa chạy một nhiệm vụ mới trọn vẹn bằng nhân vật thật hoặc kiểm tra trực tiếp toàn bộ hộp thoại/hồi sinh/thuê kho trong phiên game mới. Kết quả hiện tại gồm kiểm tra native/Lua/WPF và xác minh bản đã cài; không coi đó là thay thế hoàn toàn việc thử trong game. Không tiêu Xu, dùng vật phẩm hoặc gây chết nhân vật thật để kiểm tra.

### Bản nạp bổ sung thưởng theo phái

Đã nạp CoreServer.dll, PhongThanStudio.Core.dll và PhongThanStudio.dll sau khi build thành công. Không đổi định nghĩa nhiệm vụ đang có, NPC, bản đồ hoặc nhân vật. Biên nhận và sao lưu: `analysis/quest_class_rewards_20261105/deployment.json`.

Đã kiểm tra server nhận đúng phái và chỉ cấp gói tương ứng, đủ các loại thưởng; chống nhận lặp; phái chưa cấu hình dùng gói chung; túi đầy hoàn tác tiền và tiến độ, nhận lại được. Kiểm tra WPF mở hộp thưởng, chuyển tab Đạo Sĩ, lưu và giữ ba gói khác nhau đã qua; ảnh `analysis/quest_class_rewards_20261105/checks/quest-class-rewards.png`. Cấu hình sai phái/trùng phái/tiền âm bị từ chối khi lưu. Chưa thử thưởng riêng bằng ba nhân vật thật trong game.


## Bày bán và chợ bot — 05/11/2026

Đã bổ sung quyền theo map/môn bài, timer online, tư thế ngồi, tích lũy cửa hàng và controller chợ bot. Sửa lỗi đóng bảng sạp rồi không mở lại để dừng được. Studio có tab Bày bán & lệnh bài. Chi tiết, phạm vi và giới hạn kiểm chứng: [STALL_AND_BOT_MARKET_20261105.md](STALL_AND_BOT_MARKET_20261105.md).

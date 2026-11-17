# Truy nã hàng ngày và menu đổi điểm trong Studio

Đã cài Game.exe, CoreServer.dll, PhongThanStudio.Core.dll và PhongThanStudio.dll. Đã dùng giao diện Studio để tạo mẫu, lưu vào bản sửa và áp dụng vào Loose. Studio báo Apply PASS; có sao lưu trước khi ghi. Chưa hoàn thành một lượt bằng nhân vật thật trong game.

## NPC mẫu đã công bố

- NPC **854**, tên **Quan Truy Nã**, tại **Tây Kỳ 176/193**, map 1020.
- Cấp nhận 60–200, chung ba phái. Mỗi lượt chọn ngẫu nhiên một trong bốn nhánh và lưu nhánh vào tiến độ nhân vật.
- Tối đa **3 lượt hoàn thành/ngày**, mỗi lượt **5 điểm truy nã + 50.000 kinh nghiệm**. Tối đa 15 điểm/ngày.
- Bốn bước mỗi nhánh: hạ 8 quái canh đường đúng loại → tiếp tục tìm chứng vật, xác suất 25% → nộp và đối thoại giải mã tại Quan Truy Nã → truy bắt thủ lĩnh, quay về trả nhiệm vụ.
- Chứng vật dùng QuestKey riêng **318–321**, không dùng vật phẩm cũ để vượt bước. Tỷ lệ chỉ áp dụng khi đang đúng bước; đủ số lượng thì ngừng cấp thêm.
- Lời chào, nhận án, 16 bước và trả thưởng đã có nội dung màu. Bản đang công bố có lời thoại điều tra riêng bốn vụ, không ghi ID kỹ thuật cho người chơi.

| Nhánh | Bản đồ | ID boss riêng | Cấp | HP |
|---|---|---:|---:|---:|
| Đạo Tặc | Long Vực (1040) | 2707 | 65 | 120.000 |
| Hải Tặc | Hải Cẩu (1039) | 2708 | 70 | 150.000 |
| Sát Thủ | Long Uyên (1041) | 2709 | 75 | 180.000 |
| Tội Phạm | Khổn Tiên Cung tầng 1 (1047) | 2710 | 80 | 210.000 |

Mỗi boss có bốn vị trí hợp lệ trong dữ liệu vùng đi được. Boss chỉ xuất hiện khi có người tới bước cuối tương ứng; nhật ký cập nhật tọa độ sống. Vị trí tiếp theo tránh vị trí vừa chọn trước đó trong phiên world. Thời gian chờ hồi là 60 giây. Khi không còn người cần mục tiêu đó, server dọn NPC nhiệm vụ. Boss là mục tiêu dùng chung theo mẫu nhiệm vụ trong world, chưa phải bản sao riêng theo từng người; người chơi vẫn phải tới đúng bước để được ghi công. Khởi động lại world khởi tạo lại lịch chọn vị trí.

Lưu thay đổi giá hoặc lời thoại không làm boss đang đánh biến mất. Xóa định nghĩa sẽ dọn actor do hệ thống nhiệm vụ tạo. Bốn dòng mẫu boss được nối thêm vào bảng, không sửa các dòng quái/Boss gốc; không gắn script tử vong, rơi đồ hay kinh nghiệm của mẫu nguồn.

## Menu và cửa hàng

Studio có tab **Menu NPC & đổi đồ**, **Điểm tích lũy**, **Mini boss nhiệm vụ**. Menu hỗ trợ nhóm lồng nhau, nút nhiệm vụ và trao đổi theo **Danh sách** hoặc **Cửa hàng**. Cửa hàng dùng khung, tooltip và hộp chọn số lượng gốc của cửa hàng thường, không dùng UI Event.

Có thể chọn vật phẩm qua ID/ảnh, số lượng, cấp yêu cầu, phái, giá bằng điểm hoặc vật phẩm cụ thể. Studio xem trước bằng ảnh client và lọc ba phái. Khi gắn vào NPC cũ, giữ chức năng gốc qua nút Chức năng cũ; callback không phải main không bị chuyển hướng. Phần nhiệm vụ tùy chỉnh có thể xóa độc lập với script gốc.

Bản mẫu ban đầu có 33 món: 24 món lục (áo/mũ/giày/đai, ba phái, 6x và 8x) và 9 vũ khí hoàng kim (ba phái, 6x/7x/8x). Không tạo bộ lục 7x giả vì dòng tương ứng trong bảng gốc yêu cầu cấp 80. Giá mẫu:

| Nhóm | Điểm/món |
|---|---:|
| Lục 6x | 60 |
| Lục 8x | 135 |
| Vũ khí hoàng kim 6x / 7x / 8x | 180 / 270 / 405 |

Với 15 điểm/ngày, bốn món lục và một vũ khí 6x cần 420 điểm, khoảng 28 ngày; bộ 8x cần 945 điểm, khoảng 63 ngày. Đây là giá mẫu có thể chỉnh. **Bản người dùng chỉnh trong Studio hiện có 36 mặt hàng; đã giữ nguyên toàn bộ khi bổ sung lời thoại.**

Mini boss tạo bằng hình mẫu có sẵn nhưng ID riêng; sửa tên, cấp, HP, sát thương, tối đa bốn kỹ năng và tối đa 16 vị trí qua bản đồ trực quan. Khi thêm ID mẫu hoặc QuestKey mới phải mở lại client/world để nạp bảng.

## Kiểm chứng

- Native CoreServer/GameClient và Studio Release biên dịch thành công.
- Kiểm tra C# authoring, JSON roundtrip, private template HP/damage/skills, ID vật phẩm và ảnh, phân phái, quota và nhánh ngẫu nhiên.
- Native kiểm tra toàn chuỗi bốn bước, sai mục tiêu, tỷ lệ chứng vật, không spawn sớm, tọa độ boss, vị trí khác lần trước, giới hạn ba lượt, ngày mới, không nhận lặp, đổi điểm/vật phẩm, túi đầy và giao dịch nguyên tử.
- Lua thực của game kiểm tra script NPC đã công bố: ba phái, 16 bước, nút nhận/trả, nội dung màu và menu cửa hàng gốc.
- Native loader đọc trực tiếp cấu hình đã công bố và thử nhận bằng mock nhân vật; không sửa file nhân vật thật.
- Đối chiếu byte xác nhận toàn bộ dòng NPC/QuestKey cũ giữ nguyên ở cả Client/Server. Vị trí 854 đúng một bản ghi tại 176/193; 16 điểm boss đều thuộc vùng cho phép.
- Chưa kiểm tra một vòng nhận–đánh–đổi thưởng bằng nhân vật thật trong phiên game mới. Server đang tắt ở lần kiểm tra tiến trình cuối.

Biên nhận và log: `analysis/quest_services_20261105/deployment.json`, `published-bounty-verification.json`, `native-checks.log`, `bounty-checks.log`, `dialogues/publish.log`. Sao lưu dữ liệu đầu tiên: `ManagerStudio/PhongThanStudio/Backups/20261105_065041_0b3b5fbc2e0645e8944afe3e9e6a6056`; bổ sung lời thoại: `20261105_065554_5c4df3bb992f4fd3bf3a15350a3a36b5`.

Nếu Studio còn giữ phiên trước lúc cập nhật lời thoại, dùng **Tạo nhiệm vụ → Nạp / khôi phục bản nháp** để đọc bản đã áp dụng. Bản nháp thuộc revision cũ được giữ riêng; không ghi đè bản công bố mới.

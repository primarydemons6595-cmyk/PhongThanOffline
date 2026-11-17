> Đính chính sau phản hồi: gói trước dùng sai series đồ nhân giới và chưa khởi tạo mã map cho painter. Xem [bản sửa tiếp theo](VISUAL_QUEST_CORRECTION_20261027.md) và receipt của gói đó trước khi kết luận đã áp dụng.

# Trang phục, Vũ Sĩ, khắc chế, nhiệm vụ và kiểm tra thú — 27/10/2026

## Gói trang phục/Vũ Sĩ đã triển khai

Gói `Deploy/ProjectContent/FactionEquipment20261027` gồm CoreServer.dll, CoreClient.dll, Game.exe và `ui/vusi_skills.ini`.

- Mapper cũ chọn dòng 6 cho trang phục tier 10. Giải mã SPR của sáu thân nam/nữ Giáp Sĩ/Đạo Sĩ/Dị Nhân cho thấy dòng 6 là thân Tiên Ma chưa mặc đồ. Bộ thường tier 9/10 dùng dòng 5; trang phục Tiên/Ma dùng dòng 7/8. Phi phong native chỉ có đến dòng 7. Ánh xạ family series 25–27/28–30 và trạng thái Tiên/Ma thay thế fallback theo row đồ vốn không tương ứng với dòng thân.
- Đồ mở rộng không còn bị từ chối chỉ vì particular lớn hơn 11. Ví dụ Tinh Quân Hỗn Nguyên Lệnh Ma particular 52 được ánh xạ phi phong 7. Trang phục thật của Đạo Sĩ/Dị Nhân nằm ở lớp áo/mũ; giữ cách dựng native không ghép phi phong Giáp Sĩ vào hai thân này.
- Vũ Sĩ tải danh sách kỹ năng sparse thay vì bị loader dải phái 0–2 chặn. F5 có 20 kỹ năng trên ba trang, giữ thứ tự registry sách. Event chuyển phái dựng lại thân, tái tính chỉ số dẫn xuất và yêu cầu lưu. Không cấp miễn phí các skill sách khi đăng nhập.
- Tên hiển thị dùng byte font TCVN3: **Giáp Sĩ, Đạo Sĩ, Dị Nhân, Vũ Sĩ**; key script không đổi.
- Chính sách PvP Loose được yêu cầu: Giáp → Đạo → Dị → Giáp, thuận vòng 115%, ngược vòng 85%, đồng phái 100%. Vũ Sĩ thuộc nhánh Giáp. Giải quyết chủ thú bằng ID hiện hành trước khi áp dụng; thú đánh người cũng chịu hệ số giảm PvP. Không áp vòng ngũ hành JX lên cặp người/chủ thú đã xác định. PvE và NPC trung lập giữ nhánh hiện có. Đây là chính sách mới của bản mô phỏng, không tuyên bố là công thức VNG gốc hay chứng minh tỷ lệ thắng PvP thực tế.

Build cả ba target qua. Test native: 29.430 tổ hợp hình; tải skill Vũ Sĩ production giữ level/XP và loại skill ngoại phái; Engine đọc layout F5 đủ 20 skill; kiểm tra cặp khắc, tràn số, PvP owner. Loader SPR gốc kiểm tra 978 sprite / 60.224 frame, không lỗi. Preview `analysis/faction_fix_20261027/body-contact.png` hiển thị các dòng thân 0–8.

Lần triển khai đầu dừng sạch world 130504, nạp world 130728 với 4.055 NPC. Hash toàn bộ file nhân vật trước/sau trùng; dịch vụ tài khoản giữ PID. Backup và receipt ở `analysis/faction_fix_20261027/publication.json`. Chưa có nghiệm thu hình in-game của mọi family/giới tính; sprite audit không thay thế thử đeo, đổi map và đăng nhập lại bằng client thật.

## Kiểm tra thú triệu hồi hiện tại

`PhongThanSummonServer.inl` lấy **GetCurrentLevel(skill)**, tạo NPC bằng `MAKELONG(level, template)` rồi `ApplyPhongThanNpcLevelData()` đọc công thức template. Không có nhánh lấy sức mạnh, HP, sát thương hoặc cấp nhân vật Dị Nhân cộng vào HP/damage thú.

- Cấp skill cao hơn làm tăng cấp NPC và cấp skill đánh trong template. Đây là phần đã có.
- HP theo `LifeParam1 + LifeParam2 × cấp skill`, nhân `LifeParam/100`, với `standard.lua` hiện tại. Ví dụ Lực Sĩ Tế (450/template 359): HP cơ sở 30 ở skill 1, 120 ở skill 10. Đây là HP base trước trạng thái/buff, không phải HP của pet thực tế đã được đo trong phiên game.
- Min/MaxDamageParam2 của chín template 359–362/403–407 đều bằng 0: **sát thương nền template không tăng tuyến tính theo cấp skill**. Kỹ năng đánh có cấp tăng theo skill triệu hồi; sát thương trúng cuối còn phụ thuộc skill, kháng, trạng thái và PvP.
- Trang bị cộng cấp skill có thể ảnh hưởng **gián tiếp** thông qua GetCurrentLevel. Chưa có cơ chế kế thừa trực tiếp chỉ số/trang bị/cấp nhân vật.
- Template được chọn theo **ID skill triệu hồi**, không đổi loại thú chỉ vì tăng level cùng skill. Không coi việc tăng skill là tự nâng “bậc” template. Chính sách hiện có một thú thường sống mỗi chủ; smoke ba thú là fixture, không phải server đã hỗ trợ bầy ba thú.
- Bảng summonskill có `adddamagescale/scaleparam`; source hiện chưa áp các cột đó thành kế thừa damage của chủ. Không tự thêm tỷ lệ khi chưa xác nhận ý nghĩa/công thức.

Dữ liệu từng skill/template tại `analysis/quest_attention_20261027/summon-current.json`; audit chỉ đọc, không đổi balance hay dữ liệu thú trong gói nhiệm vụ.

## Gói nhiệm vụ/nhấp nháy

`Deploy/ProjectContent/QuestAttention20261027` chứa ba binary và năm script bổ sung callback chỉ đọc.

- Projection nhiệm vụ v2 gửi entity, variant, tọa độ thế giới trên **toàn map hiện tại**, không giới hạn quanh người chơi. Client vẽ sách trên bản đồ Tab theo tọa độ ngay cả khi NPC chưa nằm trong entity stream. Có giới hạn 128 marker và payload 4 KB; khi đổi map/packet hết hạn 6 giây, marker cũ không dùng lại.
- Vàng cho tuyến chính đã có projection `PTGMarker`/`PT2Marker` và các nhánh story định danh; xanh lam cho daily/side. Các script có thể khai báo `GetPlayerTaskCategory()` để phân loại rõ, không dựa chênh lệch level. Giai đoạn đang làm/nhận thưởng giữ variant cuốn mở/xám ở trên đầu NPC; Tab chỉ vẽ nhiệm vụ đang có thể nhận.
- Bổ sung callback availability cho Xoay Càn Khôn và Thám Quân ở các đường dẫn/alias đang dùng. Kiểm tra cấp, hạn ngày, phase, giới hạn buff và vật phẩm phụ trợ; marker không trừ đồ, ghi task hay gọi phần thưởng. Tứ Linh/Tứ Tượng và các daily đã có GetPlayerTaskState dùng chung projection toàn map và màu xanh.
- F11 có viền nhấp nháy chu kỳ 400 ms khi InsertSystemRecord thành công cho một nhiệm vụ mới hoặc nhận lại sau khi hoàn thành. Cập nhật bước, packet trùng và kết thúc không bật cờ. Mở F11 tắt cờ.
- Kết bạn thành công qua callback `answerAgree` bật nhấp nháy F6; thông báo kết bạn hiện có vẫn chạy. Tải danh sách bạn/online/offline/từ chối không bật cờ. Mở F6 tắt cờ. Hai cờ độc lập, reset khi HUD khởi tạo lại.

Build CoreServer/CoreClient/GameClient qua. Native test kiểm tra parser tọa độ/map/category, packet trùng/lỗi/ngoài giới hạn, cập nhật nguyên tử, expiry và nhấp nháy. Lua 4 thật chạy năm callback mới với các biên ngày/cấp/item/buff/phase, xác nhận không ghi task/tiêu hao item. Chưa nghiệm thu click/ảnh HUD và hai tài khoản kết bạn trong client thật. Các NPC không có callback trạng thái nhiệm vụ vẫn cần metadata nghiệp vụ riêng; không đánh dấu bừa mọi NPC là có nhiệm vụ.

Chạy lại:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File PhongThanSource/Tests/Test-FactionEquipmentFix.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File analysis/quest_attention_20261027/test.ps1
```

Receipt gói nhiệm vụ sau khi triển khai ở `analysis/quest_attention_20261027/publication.json`; chưa có receipt nghĩa là gói vẫn chờ bước cập nhật runtime. Không dùng chỉ log build để khẳng định đã triển khai.

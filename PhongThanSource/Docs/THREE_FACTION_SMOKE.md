# Kiểm tra Lệnh, ngoại hình, Vũ Sĩ và smoke tam đại phái — 27/10/2026

Cập nhật 30/10: fixture3 thú bên dưới chỉ là regression cơ chế, không đại
diện Loose hiện tại (một đệ sống). Xem `THREE_FACTION_ONE_SUMMON_SMOKE_20261030.md`
về 21600 trận một đệ, giới hạn mô hình và chênh lệch giảm PvP lên đệ.

## Kết luận kiểm tra source

### ĐạoSĩNè không đeo được Lệnh Tinh Quân

Ảnh người dùng cung cấp: nhân vật cấp 200, món **Tinh Quân Hỗn Nguyên Lệnh Ma Cấp 10 (+1)** yêu cầu cấp 100, phái Đạo Sĩ, độ bền 52/52; client báo không thể mặc vật phẩm vào vị trí đó.

Đã xác định nhánh lỗi trực tiếp:

1. `Sources/Core/Src/KItemList.cpp:1259`, `KItemList::Equip()` xử lý slot `itempart_pendant` bằng `ResolvePhiPhong()`. Nếu không tìm được hình, hàm **return FALSE trước khi gắn item vào slot**. Vì vậy thiếu hình có thể biểu hiện thành lỗi không đeo được, dù đủ cấp/phái.
2. `Sources/Core/Src/KPhongThanAppearance.cpp:128`, `ResolvePhiPhong()` chỉ gọi mapper basic, không có bảng ánh xạ mở rộng.
3. `Headers/PhongThanEquipmentAppearance.h:6` chỉ nhận detail 2/7/9, particular 0–11, tier 1–10. Mọi particular lớn hơn 11 đều trả -1.
4. `Client/Server/settings/item/001/pendant.txt`, dòng vật lý **531** (template row **529**, khóa appearance **530**): đúng món trong ảnh, tuple **detail=9, particular=52, tier=10**, requirements **36=100, 37=1**. Native probe chạy nguyên hàm production xác nhận `ResolvePhiPhong(9,52,10)` trả FALSE. Control `ResolvePhiPhong(9,1,10)` thành công.

Hai requirements cấp/phái riêng lẻ chạy qua `KItemList::EnoughAttrib()` production đều pass với actor Đạo Sĩ cấp 100. Không nên sửa hệ phái của món đồ hoặc bỏ yêu cầu cấp để chữa lỗi này.

Phạm vi đã chứng minh: món Tinh Quân trong ảnh và các tuple khác nằm ngoài miền mapper. Chưa chứng minh mọi món trắng/lục/Phá Quân có cùng tuple. Lệnh Hồng Quân thường particular 1, tier 10 yêu cầu cấp **98**, phái 1, mapper nhận được. Nếu chính món đó cũng không đeo được, cần đối chiếu instance, vị trí và trạng thái nhân vật; không quy mọi lỗi cho mapper. Một số họ particular 12+ còn có điều kiện Tiên/Ma riêng (`218`, `219`), được kiểm tra bởi `PhongThanRealmEquipment.inl`.

### Trang phục cấp cao không hiện bên ngoài

Áo/mũ cũng dùng mapper basic trên, nhưng có fallback sang bảng `VNG_ArmorPart`/bảng helm. Phi phong không có fallback. Khi không tìm được mapping, `Equip()` trả lỗi trước khi cập nhật appearance; khi có fallback, hình phụ thuộc bảng và tài nguyên SPR thật. Do đó phải ánh xạ theo **tuple/row + phái/giới tính + tài nguyên**, không lấy màu trắng/lục/tím làm chỉ số hình và không gán đại một hình cho mọi đồ cao cấp.

Chưa nghiệm thu toàn bộ SPR/trang phục cao cấp trong client thật. Audit so sánh ba bảng pendant/armor/helm Loose: mỗi bảng 3.270 record; SHA256 Client và Server trùng nhau theo từng bảng. Đây là kiểm tra file Loose, không chứng minh hash DLL đang được process nạp hay mọi lớp PAK.

### Vũ Sĩ: đăng nhập, F5, thân nhân vật

- **Đăng nhập:** `KPlayerDBFuns.cpp:544` gọi `PhongThanGetProfessionSkillRange()` rồi trả -1 khi không có range. `KPhongThanProfessionSkills.h:22` chỉ hỗ trợ 0/1/2; Vũ Sĩ=3 thất bại, trước khi đọc danh sách kỹ năng đã lưu. Native probe đã tái hiện kết quả hàm. CharacterStore/wire hỗ trợ phái 3 không đủ để chứng minh toàn bộ login hoạt động.
- **F5:** `UiSkills.cpp:37` chọn layout PAK cho 0/1/2; phái 3 rơi vào layout Giáp Sĩ. `KSkillList.cpp:620` cũng chỉ có thứ tự slot cố định cho ba phái. Phái 3 rơi vào danh sách generic, không có cây/layout Vũ Sĩ tương ứng. Cần sửa cả dữ liệu sắp xếp và layout, không chỉ học thêm skill 1691.
- **Thân nhân vật:** `PhongThanWorldDispatch.inl:110` nhận event chuyển phái, chỉ gán `m_Series`, `m_nProfession` và thông báo dữ liệu cơ bản. Không gọi refresh thân. Trong khi `KNpc::ResetNpcTypeName(1)` mới xóa resource cũ, dựng thân đúng phái và gắn lại các phần trang phục. `GetPhongThanPlayerResTypeName()` đã có case 3, nên không phải hoàn toàn thiếu tên resource Vũ Sĩ. Refresh đang được gọi từ snapshot/full snapshot; event chuyển phái chưa thực hiện bước đó.

Đợt này **kiểm tra nguyên nhân và thêm test**, chưa sửa production, thay DLL, khởi động lại dịch vụ hoặc sửa dữ liệu nhân vật.

## Chạy test

Từ thư mục PT, cần Node và MSVC6 hiện có trong workspace:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File PhongThanSource/Tests/Test-ThreeFactionSmoke.ps1 -VerifyMutations
```

Đổi seed bằng `-Seed 42`. Chạy mô phỏng lỗi bằng `-DisableAoe` hoặc `-DisableBlocking` sẽ trả lỗi. `-VerifyMutations` yêu cầu hai biến thể này thất bại và chỉ pass khi phát hiện được cả hai. Không kết nối server, không mở client, không đọc/ghi CharacterStore. Output build nằm ở `PhongThanSource/Output/Tools/ThreeFactionSmoke/`.

Test có hai phần:

1. **Production probes:** script trích nguyên thân hàm từ source hiện tại rồi biên dịch native. Chạy `KMissle::ProcessCollision`, `KRegion::GetBarrierMin`, `KItemList::EnoughAttrib`, hai hàm resolver phi phong và helper range kỹ năng. World lookup/quan hệ địch–đồng minh/damage sink là mock. Xác nhận AOE đi tới ba mục tiêu thú ở các ô khác nhau, không đánh đồng minh/ngoài vùng, tôn trọng hit-count, và occupancy NPC chặn đường khi bật kiểm tra NPC. Các `KNOWN DEFECT` là kết quả audit, không được tính thành PASS đã sửa lỗi.
2. **Auto-battle reference model:** C++ headless chạy **300 trận mỗi cặp, tổng 900 trận**; mỗi trận tạo Actor mới, RNG seed cố định, initiative thay đổi, HP biến thiên ±10%, sát thương ±15%, tối đa 600 tick. Hòa/timeout không được tính thắng; ngưỡng strict >55% tương ứng ít nhất 166/300 thắng. Dùng trực tiếp hai hàm production `PhongThanComposeOriginalPhysics`/`PhongThanComposeOriginalElement` và `PhongThanAbsorbDamage`. AI/di chuyển/AOE/chặn thú/kháng là mô hình fixture tối giản, không chạy KNpc/skill/Lua/PK đầy đủ.

## Bộ chỉ số fixture

Không có bộ PvP “cân bằng chuẩn” đã được xác nhận trong source. Các giá trị sau là **fixture được chọn để kiểm tra độ nhạy của cơ chế**, không phải chỉ số nguyên tác hay đề xuất chỉnh balance server. Đặc biệt HP thú được chọn để thời gian dọn thú tạo khác biệt đo được giữa có/không chặn đường.

| Actor | HP cơ sở | Mana | Ngoại công | Nội công | Kháng vật lý/phép | Tốc độ / tầm / chu kỳ |
|---|---:|---:|---:|---:|---|---|
| Giáp Sĩ | 10.000 | 30.000 | 1.300 | 0 | 35% / 40% | 45 / 40 / 2 tick |
| Đạo Sĩ | 6.500 | 30.000 | 180 | 1.550 | 20% / 25% | 0 / 420 / 3 tick |
| Dị Nhân | 8.000 | 30.000 | 180 | 300 | 20% / 25% | 0 / 420 / 2 tick |

Dị Nhân có 3 thú: mỗi thú 4.000 HP, ngoại công 420, kháng 20%, tốc độ 30, tầm 40, đánh mỗi 2 tick. Đây không phải kiểm chứng số thú được phép gọi đồng thời trên server. AOE bán kính 120 chọn cụm thú sống gần nhất; thú chặn khiến Giáp Sĩ phải đánh đơn mục tiêu lên thú trước. Mana mỗi cast 50, không hồi sinh/thay thú. Sân đấu một chiều, khoảng cách ban đầu 400; Đạo Sĩ đứng bắn, không mô phỏng kite.

## Kết quả đã chạy

Seed mặc định `20261027`:

| Cặp đấu | Thắng của phái đứng trước | Tỷ lệ |
|---|---:|---:|
| Giáp Sĩ → Đạo Sĩ | 300/300 | 100% |
| Đạo Sĩ → Dị Nhân | 300/300 | 100% |
| Dị Nhân → Giáp Sĩ | 194/300 | 64,67% |

- Bỏ lợi thế HP Giáp Sĩ: còn 171/300 thắng; giảm tốc tiếp cận từ 45 xuống 10: 0/300.
- Tắt AOE: Đạo Sĩ còn 125/300 = 41,67%, không giết thú; smoke thất bại đúng yêu cầu.
- Tắt chặn đường: Dị Nhân còn 0/300; số đòn Giáp Sĩ đánh vào chủ tăng từ 1.628 lên 2.469; smoke thất bại đúng yêu cầu.
- Thử thêm seed 1, 42, 12345 đều pass; Dị Nhân thắng tương ứng 64,67%, 63,67%, 61,67%. Không có trận hòa ở các seed đã chạy.

Hai tỷ lệ 100% phản ánh kịch bản regression có ưu thế rõ của fixture, **không phải xác nhận cân bằng PvP thực tế**. Test mô hình không bắt được mọi thay đổi trong `KNpc::CalcDamage`, scheduler hoặc dữ liệu skill runtime; production probes chỉ bao phủ các hàm đã nêu. Không có hệ số thắng/khắc chế hardcode theo tên cặp đấu.

Bằng chứng: `analysis/faction_smoke_20261027/native-smoke.log`; audit file thật và hash tại `equipment-audit.json`, tái tạo bằng `node analysis/faction_smoke_20261027/audit.mjs`. Tên vật phẩm trong JSON giữ nguyên byte Latin-1 của dữ liệu TCVN; không chuyển mã bảng game.

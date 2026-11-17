# Smoke ba phái, Dị Nhân có gọi đệ — 30/10/2026

## Kết luận

Cơ chế khắc hệ hiện tại chạy đúng chính sách Loose: Giáp Sĩ -> Đạo Sĩ ->
Dị Nhân -> Giáp Sĩ, chiều thuận 115%, chiều ngược 85%. Đây không phải bằng
chứng rằng kết quả PvP đã cân bằng. Chưa có smoke chiến đấu đầy đủ trong
GameServer với bộ trang bị và nhân vật thật được kiểm soát.

Smoke triệu hồi production qua adapter đạt cho cả chín chiêu 450–458:
rank10, chủ90, gọi lại thay đệ cũ, chỉ một đệ sống; các case mana/cooldown,
template lỗi, khác phái, quan hệ chủ/đệ, teleport, chuyển map và slot tái sử
dụng đều đạt. Probe bảng NPC + Lua4 thật xác nhận Phong Quyển Tàn Vân HP1700,
AR4750, attack rank10. Công thức chiêu145 lấy từ Lua thật, descriptor vật lý225.

## Chênh lệch PvP đã xác nhận từ source production

Trích nguyên hai nhánh scale PvP/khắc hệ trong KNpc::CalcDamage để chạy native.
PKSet.ini không tồn tại trong Loose đang kiểm tra, nên KNpcSet dùng fallback
Rate=20. Với cùng đầu vào vật lý1000, cùng phe, sau bước này:

| Đối tượng nhận | Sát thương sau bước policy |
|---|---:|
| Nhân vật | 200 |
| Đệ có chủ | 1000 |
| Nhân vật bị khắc hệ | 230 |
| Đệ bị khắc hệ | 1150 |

Tỷ lệ 5 lần là tại bước scale với đầu vào giống nhau; không phải cam kết tổng
HP mất thực tế luôn gấp5 vì còn khác phòng ngự/kháng/hiệu ứng. Đệ được nhận
diện chủ để áp khắc hệ, nhưng điều kiện giảm PvP chỉ kiểm tra IsPlayer() của
nạn nhân. Có thể làm đệ dễ chết trước AOE/PvP. Lượt này ghi nhận chênh lệch,
chưa thay đổi policy hoặc tự nhận đây là cách server VNG gốc xử lý.

## Mô phỏng một đệ

3 cặp x 2 vị trí x 3 seed x 300 trận = 5400 trận/mode; bốn mode = 21600 trận.
Các mode: policy hiện tại; tắt khắc hệ; bỏ triệu hồi; giả định áp giảm PvP
cho đệ như nhân vật (chỉ mô phỏng, không deploy).

Thông số đệ được xuất bằng native test từ bảng NPC/summonskill/skills và Lua
hiện tại: HP1700, AR4750, descriptor225, tầm500, mana479, hồi gọi15 giây.
Người chơi vẫn là fixture chuẩn hóa của mô hình cũ, không phải nhân vật được
load từ CharacterStore: HP Giáp10000/Đạo6500/Dị8000, mức công/kháng/tốc độ
đặt sẵn. AI di chuyển/chặn đường/AOE một chiều; không kite, không hồi máu,
không buff/debuff hoặc trang bị đầy đủ; dodge người chơi giả định2000;
đệ đánh mỗi1giây, giả định một projectile trúng vùng mỗi lượt. Không mô phỏng
đầy đủ đường bay/overlap của chùm đạn. Các giả định này không phải dữ liệu VNG.

Policy hiện tại, một đệ:

| Cặp (phái đứng trước) | Thắng / tổng | Tỷ lệ |
|---|---:|---:|
| Giáp Sĩ -> Đạo Sĩ | 1800/1800 | 100% |
| Đạo Sĩ -> Dị Nhân | 1800/1800 | 100% |
| Dị Nhân -> Giáp Sĩ | 0/1800 | 0% |

Các mode còn lại cũng có cùng kết quả thắng/thua, dù số lượt gọi đệ và số
đòn đệ thay đổi rõ. Điều này cho thấy fixture bị chi phối mạnh bởi bộ chỉ số
và hành vi đặt sẵn; không thể dùng win rate này để kết luận tỷ lệ thắng PvP
thật hoặc suy ra tăng bao nhiêu damage/HP là đúng. Nó cũng không chứng minh
chỉ sửa scale nhận damage là đủ cân bằng.

Bộ reference cũ dùng3 đệ HP4000/công600, không đại diện số đệ hiện tại.
Nó vẫn được giữ làm regression cơ chế AOE/chặn đường; mutation tắt AOE và
chặn đường đều bị phát hiện. Không lấy tỷ lệ thắng64% của fixture cũ làm
bằng chứng Dị Nhân hiện đã cân bằng.

## Chạy lại và bằng chứng

`powershell.exe -NoProfile -ExecutionPolicy Bypass -File PhongThanSource/Tests/Test-ThreeFactionOneSummon.ps1`

Đọc `analysis/faction_smoke_20261030/`: runtime-summon-fixture.json,
summon-lifecycle.log, summon-stats.log, faction-matrix.log,
production-pvp-policy.log, one-summon-simulation.log và legacy-reference-smoke.log.
Không chỉnh nhân vật/trang bị hoặc policy gameplay để làm test pass.
Không thay đổi Release. Server Loose PID178036 đã WORLD_READY105 sau bản
sửa cấp đệ trước lượt smoke này. movement_trace.flag đã được tắt riêng.

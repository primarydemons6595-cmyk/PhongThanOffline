# Dị Nhân: tách cấp chiến đấu của đệ khỏi cấp skill

Người dùng báo Phong Quyển Tàn Vân skill 10, chủ cấp 90, chỉ 500 máu và đánh
Cửu Anh cấp 30 yếu/nhiều trượt. Đã tái hiện con số 500 chính xác từ template
407: Life=350+15*10. Luồng Loose trước đây dùng rank làm NPC level vì các
chiêu NPC chỉ hỗ trợ rank nhỏ; điều này đồng thời kéo HP/AR/phòng ngự xuống.

## Thay đổi Loose

Giữ bước cấp phát theo rank để nạp đúng các chiêu NPC trong giới hạn cache.
Sau đó giải lại Life/AR/Defense/ExDefense/MinDamage/MaxDamage bằng cùng hàm
GetNpcKeyData và Lua level script thật ở cấp nhân vật chủ. Cấp NPC chiến đấu
được đặt bằng cấp chủ; danh sách và cấp chiêu tấn công đã nạp được giữ nguyên.
Kiểm tra đủ dữ liệu trước khi đổi chỉ số; thất bại không trừ mana hay thay đệ cũ.
Các NPC khác không thay đổi. Đây là chính sách phục dựng Loose, chưa phải
công thức spawn được xác minh từ server VNG gốc.

Ví dụ chủ 90, Phong Quyển Tàn Vân rank 10:

| Thuộc tính | Trước | Sau |
|---|---:|---:|
| Cấp chiến đấu | 10 | 90 |
| Cấp chiêu đánh | 10 | 10 |
| Máu | 500 | 1.700 |
| Chính xác | 1.150 | 4.750 |
| Né tránh | 400 | 1.600 |

Không tự diễn giải adddamagescale=6 thành hệ số nhân sát thương/kế thừa chủ.
Không thêm HP hoặc damage ngoài công thức bảng gốc đang có.

## Kiểm tra sát thương và đánh trượt

Probe dùng bảng NPC Loose thật, Lua4 thật, thân hàm production GetNpcKeyData
và CheckHitTarget, cùng bộ ghép sát thương production. 9 template x 10 rank
đều giữ cấp skill; mốc chủ 200 không đẩy attack skill lên 200. Dữ liệu lỗi và
cấp chủ không hợp lệ không thay đổi một phần chỉ số.

Cửu Anh thường template 86 có dodge 110, ExDefense 176. Công thức hiện tại
cho xác suất hit 91% trước và 97% sau sửa (giữ nguyên quy tắc engine, không ép
100%). Với skill 145 rank10: physicalenhance=150%, base90 -> descriptor225;
trừ phòng ngự176/2 còn137 sát thương vật lý/đòn, trước các hiệu ứng khác.
Đây là kiểm tra công thức offline, không phải kết quả chiến đấu trực tiếp.
Vì vậy triệu chứng trượt toàn bộ chưa được chứng minh chỉ do level sai.

Log mới: `summon_combat_stats.log` ghi bộ chỉ số khi gọi; `summon_combat_hits.log`
giới hạn 32 hit/đệ, ghi target/template, AR/dodge, hit accepted và HP trước/sau.
Hai log giúp xác định đúng Cửu Anh thường/vàng và phân biệt miss với damage
bị phòng ngự hấp thụ. Chưa có phiên đánh boss trực tiếp sau bản cập nhật để
khẳng định mọi nguyên nhân trượt đã hết hoặc sức mạnh khớp server VNG.

## Triển khai

CoreServer profile Dev build thành công; Test-PhongThanSummon và
Test-SummonCombatStats đạt. Đã lưu/dừng GameServer Loose sạch (exit0), sao lưu
DLL vào `analysis/summon_combat_20261030/CoreServer.before.dll`, publish DLL
SHA256 `32BE532F3C3E641218FD70E51E7D7AAEFB7C28DC5A281485F17AD79DB0F725F3`
và khởi động GameServer PID 178036. Chi tiết readiness trong deployed.json.
Chỉ cập nhật Loose, không sửa Release. Cần gọi lại đệ sau khi đăng nhập.

## Log di chuyển

Đã đổi `PhongThanRuntime-Loose/Client/movement_trace.flag` thành
`movement_trace.disabled`. Phiên client kế tiếp không mở movement trace mới.
43 log cũ (~30MB) giữ nguyên. Phiên đang chạy có thể đã cache trạng thái bật;
bộ đếm ghi FRAME của nó vốn giới hạn 3.600 frame.

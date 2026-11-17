# Thú triệu hồi có kế thừa Dị Nhân trong bản gốc? — 27/10/2026

Cập nhật Loose 30/10: đã tách cấp chiến đấu của đệ theo cấp chủ, giữ cấp chiêu
theo rank; xem `SUMMON_COMBAT_LEVEL_20261030.md`. Các mô tả luồng Loose bên
dưới là trạng thái tại thời điểm audit 27/10. Kết luận thiếu bằng chứng về
công thức spawn/kế thừa của server VNG gốc vẫn giữ nguyên.

**Kết luận: chưa đủ bằng chứng để khẳng định có hoặc không có kế thừa trực tiếp cấp/trang bị/chỉ số của chủ trong server VNG gốc.** Bản client gốc có hệ số tăng sát thương theo cấp skill được đọc và tính thật. Source Loose hiện tại chưa áp hệ số đó thành kế thừa chỉ số chủ. Hai phát biểu này không tương đương với “bản gốc không kế thừa”.

## Đối chiếu trực tiếp từ bản gốc

Đọc lại `Client_Original/package.ini` và giải nén các bản thắng trong chuỗi PAK hiện có, không lấy file runtime làm nguồn gốc:

| Dữ liệu | PAK / entry ID | SHA256 |
|---|---|---|
| `settings/summonskill.txt` | vng00.pak / 27955344 | c73438cabef2fd089f44d2d4fffc9f2c8689fe689ad68b6538d715a539863029 |
| `settings/NpcS.txt` | vng00.pak / 38EBCB17 | 12363eb119d4f0f41876154196c908248df77d541cc5133b71d2d01c1e9f70e5 |
| `script/npclevelscript/standard.lua` | serverlist.pak / 75206854 | aedc45bfd1afae786c2d62f33cfa1e5ce4754dfbad42cc367744479579d55a6e |

Bảng triệu hồi runtime hiện tại trùng byte bảng gốc. Chuỗi package gốc đang thiếu `resupd.pak`, `resupd1.pak`, `Resource.pak`; kết quả chỉ áp dụng các package đang có và `game.dat` được kiểm tra, không chứng nhận mọi phiên bản VNG.

Chín skill 450–458 đều có `adddamagescale=1`, `scaleparam=512`. Đây không phải chỉ là tên cột chưa dùng:

- `game.dat:0x7C1CAD–0x7C1CD5` đọc hai cột vào record offsets `0x20/0x24`.
- `0x7C1EC0–0x7C1F08` là hàm load dữ liệu cấp skill của class triệu hồi (vtable slot 4, style 14). Nó tính và lưu hệ số vào object offset `0x0C` theo công thức:

```text
scale_raw = adddamagescale + ((scaleparam × skill_level) >> 10)
          = 1 + floor(skill_level / 2)
```

| Cấp skill | Hệ số thô |
|---:|---:|
| 1 | 1 |
| 2 | 2 |
| 5 | 3 |
| 10 | 6 |
| 20 | 11 |

Đã thực thi nguyên khối x86 đó trong Unicorn, bộ nhớ riêng, **54 trường hợp** (9 hàng gốc × 6 cấp kiểm tra), đối chiếu kết quả và vùng nhớ đọc. Hàm loader này chỉ đọc registry/stack/con trỏ bảng; không đọc actor chủ, HP, trang bị hay sức mạnh của Dị Nhân. Điều này chỉ chứng minh tính độc lập của **hàm loader**, không chứng minh phần spawn/damage của server cũng độc lập.

Không được diễn giải hệ số thô 6 thành “+6%”, “×6 sát thương của chủ” hoặc “kế thừa 600%”: chưa xác định được phép áp hệ số và đơn vị từ server gốc. Cùng hàm còn tính cost/maxcorpse; sự tồn tại của giới hạn thi thể cũng không đủ để suy ra chỉ số được kế thừa từ chủ hay thi thể.

## Cấp thú, máu và sát thương

`NpcS.txt` định nghĩa template riêng cho từng ID skill. `standard.lua` tính thuộc tính từ **Level được truyền cho NPC** cùng các tham số template. Nó không nhận đối số actor chủ hay danh sách trang bị.

Ví dụ template 359 (Lực Sĩ Tế):

```text
LifeParam=100; LifeParam1=20; LifeParam2=10
HP_base = 20 + 10 × NPC_Level
MinDamageParam1=MaxDamageParam1=10
MinDamageParam2=MaxDamageParam2=0
Level1=0|1
```

HP tăng theo cấp NPC; damage nền template là 10; cấp skill đánh trong template tăng theo cấp NPC. Tuy nhiên, **server gốc chọn NPC_Level bằng cấp chủ, cấp skill hay công thức phối hợp vẫn chưa được chứng minh** chỉ từ các bảng này.

Trong source Loose đã phục dựng, `PhongThanCastSummon()` truyền cấp skill qua `MAKELONG(level, template)` nên trường hợp skill 1/10 cho HP base 30/120. Đây là hành vi Loose, không phải bằng chứng rằng server gốc cũng chọn level theo cách đó. Cùng ID skill giữ template; muốn khẳng định tự đổi “bậc/loại thú” phải chứng minh nhánh đổi template riêng.

## Trả lời từng ý về kế thừa

| Ý cần kiểm tra | Bằng chứng từ bản gốc hiện có |
|---|---|
| Có hệ số phụ thuộc cấp skill triệu hồi | **Có**, xác nhận từ bảng, loader x86 và thực thi giả lập |
| Cấp thú lấy trực tiếp cấp nhân vật Dị Nhân | **Chưa xác nhận**; không có hàm spawn server gốc tương ứng |
| HP thú lấy một tỷ lệ HP/sinh lực chủ | **Chưa xác nhận**; Lua level template không đọc chủ |
| Sát thương thú lấy tỷ lệ sát thương/sức mạnh chủ | **Chưa xác nhận**; có scale nhưng chưa có phép áp lên chỉ số chủ |
| Trang bị trực tiếp cộng chỉ số cho thú | **Chưa xác nhận**; chưa tìm được đường chuyển modifiers từ item chủ sang thú |
| Trang bị cộng cấp skill ảnh hưởng gián tiếp | Có thể xảy ra nếu server dùng cấp hiệu lực; không đồng nghĩa kế thừa trực tiếp. Loose hiện dùng GetCurrentLevel |

`game.dat` có phần kiểm tra được phép cast, chi phí, hành động và hiển thị của class triệu hồi. Các đoạn này có đọc trạng thái actor phục vụ điều kiện cast/animation/cooldown; không được gọi mọi lần đọc actor là “kế thừa sức mạnh”. Chưa tìm được luồng tạo NPC và gắn chỉ số thú của **server VNG gốc** trong phần được kiểm tra.

Vì vậy cần sửa cách diễn đạt trước đây: **“Loose hiện chưa kế thừa trực tiếp” là kết luận về mã mô phỏng. “VNG gốc không kế thừa” chưa phải kết luận đã xác minh.** Không thay mã gameplay hoặc thêm hệ số dự đoán trong lượt audit này.

## Tái hiện và bằng chứng

Thư mục `analysis/summon_inheritance_20261027/`:

- `original-data.json`: nguồn PAK, hash, tham số chín template và các package thiếu.
- `evidence.json`: hash game.dat, vtable và xref tên cột.
- `summon-column-loader.asm`, `vtable-04-007c1ec0.asm`: loader bảng và loader cấp.
- `summon-cast.asm`: kiểm tra điều kiện cast và chuẩn bị hành động client.
- `oracle.json`: 54 trường hợp chạy mã x86 gốc; cost là fixture kiểm soát, không phải cost của cả chín skill.
- `audit_data.py`, `inspect_original.py`, `oracle.py`: script chỉ đọc nguồn gốc, ghi bằng chứng trong analysis; không can thiệp tiến trình game.

Để chốt công thức kế thừa cần thêm binary/source server đúng phiên bản có nhánh spawn và cập nhật combat của summon, hoặc dữ liệu đo có kiểm soát trên bản VNG tương ứng: giữ nguyên skill rồi thay riêng cấp/HP/sức mạnh/trang bị chủ, đo HP và sát thương thú trên cùng mục tiêu. Client gốc và tên cột không thay thế được bằng chứng đó.

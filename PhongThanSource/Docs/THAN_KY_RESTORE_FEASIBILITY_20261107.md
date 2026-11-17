# Khả năng khôi phục chín nhóm Thần Kỹ có script — 07/11/2026

Đã kiểm tra mã thực thi Loose, script công thức, nhánh skill phụ và đường sử dụng vật phẩm. Có dữ liệu để khôi phục từng nhóm, nhưng việc bỏ chặn sách rồi cấp ID kỹ năng chưa đủ để sử dụng đúng công dụng.

## Phần đã xác minh

- Chạy **380 lượt gọi** `GetSkillLevelData` qua **Engine.dll và LuaLibDll.dll đang dùng trong Loose**, lấy tham số từ bảng Skills.txt gốc, cấp 1–10. Tất cả trả về bộ giá trị hợp lệ. Có 9 skill chính và 6 nhánh phụ trong đồ thị được duyệt; những nhánh không có LvlSetting đang hoạt động không phát sinh lượt gọi.
- Không chỉnh lại công thức Lua. Kết quả phân số như `5.5,0,0` được giữ nguyên trong biên bản; bộ nạp KSkill gốc chịu trách nhiệm chuyển sang số nguyên, không sửa Lua để làm tròn theo ý riêng.
- Tìm được cả `settings/trans_to_creature.txt` và `settings/auto_activate_skills.txt` trong `Client_Original/data/serverlist.pak`. Hai bảng này là dữ liệu hữu ích cho biến thân và tự thi triển, nhưng C++ hiện chưa đọc/thực thi chúng cho các thuộc tính dưới đây.
- Script dùng sách của cả chín nhóm hiện tồn tại ở Loose dưới `script/vnevent/features/thanky/`, nhưng là **adapter `BLOCKED_SPEC`**, kiểm tra vật phẩm rồi báo chưa có đặc tả; không cấp kỹ năng và không tiêu hao sách. Chúng không phải script sử dụng sách khôi phục từ PAK gốc. Không nhầm script công thức với script kích hoạt sách.

## Đối chiếu thực thi

| Nhóm / ID chính | Dữ liệu công thức có sẵn | Phần server còn thiếu hoặc cần xác nhận |
|---|---|---|
| Họa Địa Vi Lao / 1560 | Chi phí; truyền cấp sang nhánh 1561 → 1562 → 1563; làm chậm ở 1562 | Chuỗi dùng các thuộc tính đã có đường xử lý. Đây là nhóm gần đủ nhất; vẫn cần nối sách, xác minh cách cấp/thu hồi chiêu và kiểm thử cờ cùng vùng tác động trong trận thực tế. |
| Triệu Hồi / 1551 | CD theo cấp; chi phí; cờ gọi thú về chủ | `magic_move_creature_b` chưa có mã thực thi; `magic_set_skill_cd_v` chưa gắn vào thời gian tái sử dụng. Không được nhầm với skill triệu hồi một thú mới. |
| Khô Mộc Phùng Xuân / 1547 | Hồi theo cấp và phần máu đã mất; khoảng nhịp ghi trong Lua | Thuộc tính hồi `magic_life_kmfc_dot_by_level_v` và `magic_life_kmfc_dot_by_self_p` chưa được thực thi. Không thể thay bằng tăng HP tối đa hoặc bình hồi máu thông thường. |
| Nam Minh Ly Hỏa / 1548–1550 | Sát thương hỏa; số đạn; tỷ lệ PvP/PvE; cờ trạng thái | `magic_damage_ratio_p` và `magic_damage_unchange_status_b` chỉ có khai báo, chưa có nơi áp dụng. Chỉ cho bắn đạn sẽ sai công dụng và cân bằng. |
| Xung Phong Hãm Trận / 1552 | Khoảng lao 550; tốc độ 20; lá chắn theo cấp, máu và thời hạn | `magic_forward_move_b` và `magic_forward_shield_p` chưa có mã thực thi. Cần kiểm tra vật cản, server xác nhận vị trí và lá chắn; không dùng dịch chuyển client để giả hiệu ứng. |
| Băng Hồn Tuyết Phách / 1553 | Chỉ số phục hồi khi trúng đòn (`fasthitrecover_v`); hút mana; bộ tự dùng skill | `mana_drain_v`, `magic_auto_activate_skills_v` chưa được thực thi. Bảng tự thi triển đã tìm thấy. Chưa xác nhận toàn bộ diễn giải công dụng trong một phiên chạy VNG. |
| Thiên Toàn Địa Chuyển / 1554–1555 | Khoảng đẩy lùi theo cấp; cờ chắc trúng; nhánh đạn phụ | `magic_move_back_b`, `magic_no_miss_v` chưa được thực thi. Cần kiểm tra đẩy lùi và vật cản ở server. |
| Hình Thiên Chiến Ý / 1556 | Thời hạn; loại biến thân; khóa kiểu đánh; miễn khống chế | Thiếu xử lý `magic_trans_to_creature_v` và `magic_disable_skill_by_lrinfo_v`. Miễn khống chế đã có một ánh xạ xử lý, chưa đủ để kết luận các biến thể Phong Uyển/Cuồng Đào/Ngự Sậu hoạt động đúng. |
| Đoản Binh Giao Tiếp / 1557 | Sát thương thật với đoản binh; hệ số theo loại binh khí | Thuộc tính `magic_extra_real_damage_with_short_weapon_v` và `magic_damage_ratio_by_short_weapon_p` chưa có mã thực thi. |

Các tên thuộc tính trên đã có trong `KMagicAttribRegistry.inc`. Kiểm tra chỉ thấy tên ở registry không đủ: `KNpcAttribModify::ModifyAttrib` trả về khi không có handler; các tên được liệt kê là thiếu cũng không có nơi tiêu thụ bằng mã C++ trong Core hiện tại. Kiểm toán đã tính cả alias enum, ví dụ `skill_eventskilllevel` → `magic_skill_reserve5`, nên không báo nhầm đường truyền cấp của Họa Địa Vi Lao là thiếu.

## Giới hạn cần giữ khi nối sách

Bảng vật phẩm và bài VNG cho biết thao tác chuột phải, tiêu hao Linh Thạch và dùng quyển cũ để nâng cấp. Chưa có script sử dụng sách gốc để xác minh thời gian cấp chiêu, thời điểm thu hồi, điều kiện mang sách hoặc xử lý khi đăng xuất. Không được mặc định biến thành học vĩnh viễn hoặc tự đặt một thời hạn rồi gọi đó là bản gốc. Cũng chưa chứng minh cùng tên giữa skill đời cũ và quyển Thần Kỹ mới nghĩa là cùng một biến thể.

Thứ tự có căn cứ để thực hiện: nối và kiểm thử riêng chuỗi Họa Địa Vi Lao; tiếp đến thao tác gọi thú về chủ và hồi máu; sau đó các nhánh đẩy/lướt, lá chắn, biến thân và điều chỉnh sát thương. Mỗi nhóm cần có cả kiểm thử tính toán lẫn kiểm tra một phiên chiến đấu, không chỉ kiểm tra biểu tượng.

## Bằng chứng và trạng thái

- `analysis/boss_skill_ui_20261107/than-ky-feasibility.json`: ID, thuộc tính đang được bảng gọi, alias, nơi có mã tiêu thụ, nhánh phụ và hai bảng phụ gốc.
- `feasibility-queries.tsv`, `feasibility-lua-results.tsv`: 380 tham số và kết quả thực thi native.
- `SkillLuaProbe.cpp`, `Run-SkillLuaProbe.ps1`: công cụ kiểm tra; không mô phỏng hay viết lại Lua.
- `feasibility-evidence.json`: hash DLL, source đã kiểm tra và đường script dùng vật phẩm.

Kết quả lần này là hoàn tất kiểm tra khả năng khôi phục và chỉ ra các điểm phải nối. Chưa có nhóm nào được chứng nhận đã dùng đầy đủ trong game; runtime, dữ liệu nhân vật và sách được giữ nguyên. Chỉ biên dịch công cụ kiểm tra trong analysis; không build bộ runtime Release hoặc tạo patch.

# Ngọc Bội, thủ tủy và mài ngọc — bản test 27/10/2026

## Sử dụng

- Cẩm Nang Sự Kiện → Ngọc Bội → Nguyên liệu tẩy tủy và mài ngọc: Dẫn Tủy Châm (MagicScript 739), Tẩy Tủy Châu (IBItem 1081, particular 2), năm Ngọc Tủy (material 1045–1049), ngọc và Quái Phù trước/sau mài. Có 114 lựa chọn nguyên liệu; các ID lựa chọn cũ trong Cẩm Nang giữ nguyên.
- Ngọc Bội tạo mới có một dòng ngẫu nhiên. Giới hạn số hệ và phẩm chất lấy đúng dòng mẫu. Món cũ chưa có dòng có thể dùng Dẫn Tủy Châm; không sửa trực tiếp kho hoặc tự sinh lại dòng cho vật phẩm đã lưu.
- Tẩy tủy: đeo Ngọc Bội, dùng Dẫn Tủy Châm, chọn Hỏa/Băng/Thổ/Lôi/Kim rồi xác nhận. Dùng một Dẫn Tủy Châm và năm Ngọc Tủy cùng hệ. Nếu thay dòng của hệ đã có, dùng thêm một Tẩy Tủy Châu. Cancel hoặc thiếu nguyên liệu không trừ vật phẩm. Châm chỉ bị trừ sau khi native hoàn thành, không trừ ngay khi mở menu.
- Lôi Chấn Tử tại Diêu Trì (198/199) → Thủ tủy Ngọc Bội: tháo Ngọc Bội vào túi, chọn đúng món và xác nhận. Ngọc Bội mất, nhận một Ngọc Tủy ngũ hành ngẫu nhiên. Giữ cờ hạn chế giao dịch/bán/rơi của vật phẩm đầu vào trên nguyên liệu đầu ra.
- Lôi Chấn Tử → Mài ngọc: chọn một ngọc/Quái Phù chưa mài trong túi, xác nhận. Mỗi lần mài một vật phẩm, phí **50.000 lượng = 5 vạn**, theo mức phí người dùng xác nhận. Không yêu cầu nguyên liệu phụ.
- Các nhánh đục lỗ/trang trí, chọn lỗ khảm và thuyết minh trước đó vẫn còn; không áp chức năng lên Lôi Chấn Tử ở map khác hay NPC khác.

## Dữ liệu gốc và giới hạn

[VNG: Tẩy tủy Ngọc Bội](https://pt.vnggames.com/tin-tuc/can-biet/thuoc-tinh-hap-dan-cua-ngoc-boi-sau-khi-tay-tuy.html) xác nhận điều kiện đeo, năm Ngọc Tủy, Dẫn Tủy Châm, Tẩy Tủy Châu và thủ tủy tại đúng NPC. Đã tải và xem hai ảnh thủ tủy gốc; ảnh lưu ở analysis/jade_restore_20261026/ngocboi-12.jpg và ngocboi-13.jpg. Script cũ Dẫn Tủy Châm hiện có được đối chiếu về quy trình; nhánh thực thi mới dùng native transaction thay cách xóa nguyên liệu trước khi gọi API chưa tồn tại. Không gọi đây là server Lua VNG mới nhất đã thu hồi.

Bảng thuộc tính byte-identical từ PAK đang có hiệu lực: settings/item/001/玉佩魔法属性表.txt, serverlist.pak, ID 19D9B862, SHA256 abfca63f0efbab52ec506c43f7e790eaf05483706e37088b687caac3551e845c. Triển khai dưới alias ASCII jade_magic.txt ở hai phía. Sinh đúng nhóm hệ, loại thuộc tính và khoảng giá trị theo bảng. Tỷ trọng theo phẩm chất dùng tổng trọng số thực trong từng nhóm; có hai cột nguồn tổng 10.500/9.750 thay vì 10.000, không sửa bảng nguồn hoặc bỏ bớt dòng. Mỗi hệ tối đa một dòng; tẩy lại hệ đó thay dòng, không cộng chồng. Mẫu jadependant.txt quyết định sức chứa 1–5 hệ và bậc 1–5.

Mài hỗ trợ **36 loại đầu vào** có cặp đầu ra trong material.txt: 28 ngọc thường 253–280 → 281–308; Quái Phù 374/383/392/401 → một trong tám quái vị kế tiếp; ba ngọc 2015 5225–5227 → 5228–5230; Quái Phù 2015 5231 → 5232–5239. Quái vị chọn ngẫu nhiên đều; chưa thu hồi được phân phối server gốc khác để đối chiếu. Ngọc thường giữ họ và bậc, không tự biến thành vật phẩm họ khác. Không nhận mài 14 dòng Thái Dương/Thái Âm 5524–5537 vì bộ material/khảm hiện có chưa có đầu ra tương ứng. Không bịa ID đầu ra và không thu phí cho các món chưa hỗ trợ.

## Native và lưu trữ

PhongThanJadeRules.h: lưu nhóm dòng, dòng bảng và giá trị đã roll vào 40 GeneratorLevel có sẵn; marker riêng ở ô 39. Không thay kích thước record nhân vật, mail, đấu giá, bản tin vật phẩm hoặc dùng registry con trỏ ngoài. Tải lại vật phẩm kiểm tra bậc, số hệ, nhóm dòng và khoảng giá trị, rồi áp lại đúng thuộc tính đã lưu; không roll lại. Các thuộc tính cơ bản/sự kiện có sẵn của Ngọc Bội vẫn giữ. Không đổi cách sinh các loại trang bị khác.

PhongThanJadeService.inl: kiểm tra nhân vật sống, khóa/giao dịch, serial vật phẩm và vị trí đang đeo/túi. Tẩy tủy tiêu hao qua giao dịch kho có rollback, sau đó áp lại chỉ số trang bị và đồng bộ native. Thủ tủy và mài kiểm tra lại đúng NPC/map/khoảng cách. Đầu ra phải đặt được vào túi trước khi hủy đầu vào hoặc tính phí; lỗi thanh toán/đặt đồ khôi phục nguyên vật phẩm, số lượng và vị trí. Không chỉ kiểm tra điều kiện ở Lua.

## Kiểm chứng

- CoreServer/CoreClient Release build thành công.
- Native production rules: 22.185 lượt sinh trên 51 mẫu × 5 hệ, tái tạo dòng đã lưu, kiểm tra sức chứa, sai serial, thiếu nguyên liệu, rollback khi túi đầy/thanh toán lỗi, cờ giao dịch khi thủ tủy, phạm vi NPC.
- CharacterStore thực: create/load byte-identical cho record Ngọc Bội có marker và giá trị âm; chuyển đủ 40 giá trị vào wire snapshot hợp lệ.
- Lua 4 thực: năm nhánh menu NPC, đủ bốn nhánh chọn lỗ khảm cũ, phân trang chọn vật phẩm, serial xác nhận, cả năm nhánh tẩy tủy. Cẩm Nang: 2.690 trang menu/callback hợp lệ, giữ sửa ô nhập tiền vạn trước đó.
- Loader SPR native: 704 file / 704 frame, 0 lỗi.
- Chưa thao tác trực tiếp nhận/mài/tẩy trên nhân vật thật; gói dành để người dùng test trong Game.

Gói: Deploy/ProjectContent/JadeServices20261027. Công cụ tái tạo: Tools/build_jade_services.py, event_handbook_catalog.py, build_event_handbook.py. Bằng chứng và biên nhận: analysis/jade_restore_20261026.

?? ?p d?ng 41 file, hash kh?p g?i. World PID 122232 n?p ?? 105 map / 4.044 NPC-qu?i, k?t n?i database/relay/gateway th?nh c?ng. H? s? nh?n v?t kh?ng ??i qua restart; script g?c C821F1CD c?a L?i Ch?n T? tr? ??ng file ?? s?a.

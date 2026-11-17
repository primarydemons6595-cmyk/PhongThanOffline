# Ngọc đã mài: dòng thuộc tính và sao xanh

Đã sửa lỗi cấp mẫu ngọc trống rồi chỉ sinh dòng khi khảm. 53 dòng ngọc/Quái Phù trong bảng bao_shi_shu_xing_biao_sc.txt hiện sinh thuộc tính khi tạo ngọc đã mài (Cẩm Nang, mài tại NPC). Chọn loại theo phân phối tích lũy và trị số trong khoảng min/max gốc. Lưu dòng/slot/giá trị vào ScriptParam hiện có, không đổi layout item. Tooltip hiện trạng thái Đã mài, dòng thực tế và sao xanh `<pic=428>` trước dòng đạt tối đa. Giữ sao ở phần mô tả ngọc sau khi khảm, so sánh giá trị gốc trước bonus trang trí lỗ.

Ngọc chưa mài hiện trạng thái Chưa mài và chưa có thuộc tính. Cẩm Nang chia nhóm riêng. Không loại bỏ hướng thiên/địa/thủy… trong nhãn Quái Phù như bộ lọc markup cũ; chuyển dấu ngoặc sang dạng đọc được trong menu.

Khảm ngọc có trạng thái đã roll dùng đúng dòng đã lưu, không quay lại thuộc tính. Ngọc cũ chưa từng có dòng được khôi phục bằng quy tắc ổn định theo mẫu khi nạp lại server, tránh thay dòng khi xem đấu giá/mail hoặc đăng nhập lại. Trạng thái tagged hợp lệ giữ nguyên. Stack có roll khác nhau được tách qua quy tắc GetParam hiện có.

Đính chính phạm vi mài trước đó: 36 cặp tên trước/sau mài tồn tại trong material.txt, nhưng bảng thuộc tính/khảm hiện có chỉ định nghĩa 53 đầu ra tương ứng **25 loại đầu vào** (21 ngọc ba họ và bốn Quái Phù). Bích Uyên, dòng 2015, Thái Dương/Thái Âm thiếu bảng thuộc tính phù hợp được tách thành mẫu chưa hỗ trợ; NPC không nhận mài/không thu phí, không cấp vật phẩm trống rồi báo thành công.

Kiểm chứng: 5.300 ca biên thấp/tối đa trên 53 dòng; sao chỉ khi max, copy/restore ScriptParam, khảm không đổi dòng, legacy migration ổn định. Bộ socket cũ 10.667 kiểm tra qua; Cẩm Nang 2.692 menu/callback Lua 4 qua. CoreServer/CoreClient Release build qua. Đã áp dụng 5 file đúng hash; world PID 114012 nạp 4.044 NPC/quái, hồ sơ nhân vật không đổi. Chưa kiểm tra trực quan trong Game sau cập nhật.

Gói: Deploy/ProjectContent/PolishedGem20261027. Bằng chứng: analysis/polished_gem_20261027; test rules: Tests/Native/PolishedGemTests.cpp.

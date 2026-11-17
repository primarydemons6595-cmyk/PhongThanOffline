# Đối chiếu bảng thăng cấp và nhiệm vụ ba phái — 29/10/2026

Đã xuất bản gói `ThreeFactionQuests20261029` vào **PhongThanRuntime-Loose**: 105 script NPC/vật phẩm và hai bảng tên nhiệm vụ ui3/ui4. Không thay binary trong đợt này. Không triển khai sang Release.

## Phạm vi đối chiếu

- Đọc đủ 840 dòng của sáu bảng tăng cấp Giáp Sĩ, Đạo Sĩ, Dị Nhân: ba bảng nhân giới 200 dòng và ba bảng nâng cao 80 dòng. Có 285 dòng chứa hướng dẫn nhiệm vụ. Đây là dòng dữ liệu, không phải 285 nhiệm vụ riêng biệt.
- Bảng nhân giới đã có ký tự `?` ngay trong dữ liệu nguồn. Không dùng những chuỗi hỏng làm căn cứ tự đoán nguyên liệu hoặc phần thưởng.
- Kiểm kê đồ thị nhiệm vụ hiện tại: Giáp Sĩ 62 chuyển bước, Đạo Sĩ 79, Dị Nhân 74. Các số này gồm cả nhiệm vụ phụ được khai báo trong đồ thị, không bao gồm mọi script NPC cũ/daily và không chứng minh toàn bộ gameplay hoàn chỉnh.
- Tham chiếu hướng dẫn Tam Giới hiện hành: [Giáp Sĩ](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-giap-si.html), [Đạo Sĩ](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-dao-si.html), [Dị Nhân](https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-di-nhan.html). Bảng thăng cấp và hướng dẫn này thuộc các thời kỳ khác nhau; ví dụ không tự đổi Giáo Huấn từ mốc 20 đã kiểm tra về 16 chỉ vì bảng cũ ghi như vậy.

## Phần bổ sung có giao dịch thực tế

| Phái | Nhiệm vụ | Cấp nhận | NPC | Điều kiện trả | Thưởng |
|---|---|---:|---|---|---|
| Giáp Sĩ | Cần Mẫn | 6 | Tô Hộ, Sùng Thành Doanh | Có sách Tế Huyết Trảm hoặc đã học skill 27 | 400 XP |
| Đạo Sĩ | Cần Mẫn | 3 | Vân Trung Tử, Ngọc Hư Cung | Có sách Chưởng Tâm Lôi hoặc đã học skill 3 | 400 XP và sách Lưu Tinh Thạch |
| Dị Nhân | Cần Mẫn | 4 | Thiếu Hạo, Xi Vưu Mộ | Có sách Lực Sĩ Tế hoặc đã học skill 450 | 400 XP |

Giữ sách khi trả. Giữ các biến gốc 1035/1073/1090; không sửa save nhân vật. Nhận/trả dùng QuestExchange so sánh pha, kiểm tra lại phái/map/cấp/khóa/giao dịch. Chỉ trao XP sau khi pha và vật phẩm thưởng đã commit; trả lỗi không chuyển pha, không nhận XP, không nhận lặp. F11 có tên và yêu cầu; NPC có marker.

Kiểm tra sách dùng truy vấn cấp `-1` của native API để nhận cả sách thưởng cũ lưu level 0 và sách shop lưu level 1, trong khi vẫn khớp chính xác detail/skill. Ba script đã được nạp lại bằng bộ LiveReload an toàn sau khi world sẵn sàng, không khởi động lại lần nữa; receipt `book-query-correction.json` xác nhận từng lệnh completed, failed=0.

Nguồn của ba nhánh là các bản script cộng đồng đã lưu có hash, **không phải** chứng cứ đã tìm được đầy đủ script server VNG nguyên bản. Bản triển khai chấp nhận cả sách đang giữ và kỹ năng đã học để tránh kẹt sau khi dùng sách. `provenance.json` của gói ghi chính xác ba payload, đường dẫn và hash.

## Sửa các nhánh đang có

- Menu Giáp Sĩ/Đạo Sĩ/Dị Nhân hiển thị lựa chọn xem yêu cầu ngay cả khi thiếu vật phẩm. Hàm trả vẫn kiểm tra vật phẩm, nên việc mở lựa chọn không nhận thưởng hoặc vượt bước.
- Marker nhiệm vụ từ script cũ được hợp nhất với marker của đồ thị bổ sung; marker nhận thưởng có ưu tiên cao hơn nhận nhiệm vụ mới. Trước đó Giáp Sĩ có thể bỏ qua marker cũ hoàn toàn, hai phái còn lại có thể che nó khi có nhiệm vụ mới.
- Giữ nguyên nội dung dịch vụ và chuỗi chính tuyến hiện có. Các đoạn runtime trùng trong cùng script cũng được sửa để định nghĩa cuối không ghi đè trở lại hành vi cũ.

## Kiểm tra và xuất bản

Lua 4 thật nạp và thực thi khởi tạo đủ 105 payload script cuối. Năm fixture hành vi dùng đúng phần code được xuất bản, kiểm tra ba nhánh Cần Mẫn, sách đã học, giữ sách, sai phái/map/cấp, khóa/giao dịch, lỗi giao dịch, trả lặp, menu thiếu vật phẩm và ưu tiên marker cũ. Kiểm thử hồi quy 153 chuyển bước Đạo Sĩ/Dị Nhân chạy với script NPC vừa sửa đã qua.

World 156344 lưu và thoát sạch, exit 0; chín file nhân vật không đổi trong lúc sao chép. Khởi động lại world 137688. Đối chiếu độc lập 107 file khớp SHA256; Lua registry 9249 loaded, 0 failed, alias registry PASS. Client cần mở lại để nạp bảng tên nhiệm vụ mới; không ép tắt client khi nó chưa thoát.

Bằng chứng ở `analysis/three_faction_completion_20261029`: `growth-catalog.json`, `implemented-graphs.json`, `source-hits.json`, các fixture, `publication.json`, `restart.json` và backup trước xuất bản.

## Chưa được xác nhận đầy đủ

Đợt này không chứng minh mọi nhiệm vụ được nhắc trong bảng thăng cấp đều đã chạy trọn tuyến. Các tên như Phản Khách Vi Chủ, Cầm Kỳ Thi Họa, Mạn Thiên Quá Hải, Phong Hồi Lộ Chuyển và Cửu Lai còn cần đối chiếu riêng từng callback, mục tiêu, NPC và API; việc có lời giới thiệu hoặc bản script cộng đồng không được ghi là hoàn tất. Các nhánh mới chưa được nghiệm thu bằng nhân vật thật. Học nghề/chế tạo tại Sinh Hoạt Sứ cũng chưa được hoàn thiện bởi gói này.

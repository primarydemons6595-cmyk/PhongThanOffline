# Kiểm tra bot mua ngựa và ghép trang bị — 09/11/2026

> Đây là bản kiểm tra trước khi bổ sung. Phần nhận diện NPC, kiếm công thức/nguyên liệu và giao dịch ghép/nâng cấp đã có cập nhật Loose sau đó; xem [BOT_NPC_PROGRESSION_20261109.md](BOT_NPC_PROGRESSION_20261109.md) để biết phạm vi đã cài, kiểm thử và giới hạn còn lại. Các dòng “chưa có” bên dưới ghi nhận trạng thái tại thời điểm kiểm tra ban đầu.

Phạm vi: đọc mã đang dùng cho bản Loose. Đây là kết quả kiểm tra, chưa bổ sung AI ghép đồ và chưa coi là kiểm chứng bot thực hiện toàn bộ chuỗi trong phiên chơi.

| Hành vi | Hiện trạng | Giới hạn |
| --- | --- | --- |
| Mua ngựa tại NPC | Có nhánh mua trang bị thay thế, gồm ô ngựa | Chỉ khi ô trống, ngựa không dùng được hoặc hỏng; phải có shop đăng ký, đúng điều kiện, đủ tiền, chỗ và sức mang. Chưa phải AI săn ngựa nâng cao. |
| Mặc ngựa/trang bị tốt hơn đã có | Có | So điểm thuộc tính, điều kiện hệ phái và khả năng trang bị; dùng thao tác kho đồ thật. |
| Mua đồ lục/hoàng kim từ người chơi | Có cơ chế hỏi mua/giao dịch đồ dùng được | Phải có món cụ thể, giá và ngân sách phù hợp. Không đồng nghĩa biết tự ghép đồ. |
| Làm nhiệm vụ lấy công thức ép ngựa | Chưa có luồng chuyên biệt | Bot có nhiệm vụ tân thủ/chủ tuyến được khai báo và giáo huấn; chưa có bộ mục tiêu công thức ngựa. |
| Săn nguyên liệu theo công thức ghép ngựa/trang bị | Chưa có | Chưa có kế hoạch nguyên liệu xuất phát từ công thức ghép, nguồn quái và số lượng cần tích lũy. |
| Ép ngựa xanh có dòng | Chưa có | Chưa chọn công thức và thao tác ghép qua NPC. |
| Ghép đồ lục/hoàng kim, ngựa hoàng kim | Chưa có | Giữ hoặc mua được món đồ không phải là tự ghép được món đó. |
| Thăng cấp đồ tại NPC | Chưa có | Chưa có bộ lập kế hoạch cấp, nguyên liệu, chi phí và giới hạn chấp nhận thất bại. |

Các đường thực thi đã kiểm tra:

- `Sources/Core/Src/PhongThanBotResources.inl`: `BotNpcReplacementWanted` có `itempart_horse`; `BotEquipmentUseful`, `BotGearScore` phân loại đồ đang có.
- `Sources/Core/Src/PhongThanBotShop.inl`: `BotShopPlan` di chuyển đến NPC, chọn callback thực tế, mua bằng `BuyItem`, kiểm tra vật phẩm sau giao dịch.
- `Sources/Core/Src/PhongThanBotEquipment.inl`: `BotManageEquipment` thay trang bị bằng giao dịch kho đồ; tin rao dùng vật phẩm bot thực sự sở hữu.
- `Sources/Core/Src/PhongThanBotStory.inl`, `PhongThanBotDaily.inl`: kế hoạch nhiệm vụ hiện có; chưa có kế hoạch lấy công thức ép ngựa.
- `Sources/Core/Src/PhongThanBotTraining.inl`: vòng quyết định hiện tại không gọi một bộ lập kế hoạch ghép/thăng cấp trang bị.

Để bổ sung đầy đủ cần nối kế hoạch từ công thức trong Studio → chọn món nâng cấp → dự trữ tiền/vật phẩm → kiếm nguyên liệu hoặc mua thật → đến NPC → thực hiện đúng giao dịch → kiểm tra kết quả và xử lý thất bại. Cần thử riêng các tình huống kho đầy, thiếu tiền, nguyên liệu đang khóa, NPC không sẵn sàng, công thức đổi và nâng cấp thất bại; tránh cấp thẳng vật phẩm để giả lập thành công.

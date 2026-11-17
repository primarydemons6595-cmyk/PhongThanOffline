# Tìm bảng Thuộc Tính Chi Tiết màu vàng — 27/10/2026

ĐÍNH CHÍNH: bản vàng có sẵn, chính là tài nguyên ID 101785AE đã tìm trước đó. Bộ giải mã xem trước đã đọc byte màu của SPR true-color theo RGB thay vì BGR, làm vàng thành xanh và dẫn đến kết luận sai. Đọc đúng BGR cho đúng khung vàng và cả ba tab Tấn Công/Phòng Thủ/Khác trong ảnh người dùng. Không cần tìm một ID khung vàng khác. Sau yêu cầu thay vào Loose, đã chuẩn hóa tám SPR có liên quan trong gói UiGoldChannels20261027; xem Docs/UI_GOLD_CHANNELS_20261027.md.

Đường dẫn gốc: `\spr\UI4\状态主界面\详细属性底板.spr`.
ID PAK: `101785AE`; kích thước canvas 282 × 413.
SHA256: `8a5ca865e3deef8cdfab3a6dde0aae8db2b5c6c08c0abb681bd470b288614715`.

Ba bản tìm được tại vng00.pak entry 242, 243 và update0_171-171.pak entry 1235 đều khớp từng byte. Tiêu đề/viền là vàng khi đọc đúng BGR. Các ảnh variants.jpg và candidate_sheet cũ dùng bộ xem trước sai thứ tự màu, không dùng làm bằng chứng màu của SPR true-color.

Lần rà trước đã duyệt index 22 PAK và các ứng viên kích thước, nhưng bộ lọc màu dùng cùng cách giải mã sai nên không thể hỗ trợ kết luận thiếu bản vàng. Sprite indexed dùng bảng màu RGB, còn sprite true-color này cần đọc BGR; không đổi màu tất cả loại SPR một cách đồng loạt.

Bằng chứng nguồn: analysis/status_detail_gold_20261027/proof.json và native_color_proof.json. Xem trước đúng màu: MainProperty_native_color.png và gold_detail_page_0.png, gold_detail_page_1.png, gold_detail_page_2.png. Script tái lập: analysis/status_detail_gold_preview.py. Các ảnh này đọc dữ liệu nguồn đúng thứ tự kênh, không tô lại màu hay sinh ảnh mới. Bản ghép trang dùng nhãn SPR gốc, chưa có số liệu nhân vật động hay trạng thái nút đang chọn.

Công việc trước đó đã hoàn tất áp dụng: GemPolishCorrection20261027, 5 file đúng hash; world PID 114888, 105 map, 4.044 NPC/quái, không có ScriptError/stack traceback khi khởi động, hồ sơ nhân vật không đổi.

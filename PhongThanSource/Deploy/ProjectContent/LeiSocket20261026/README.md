# Lôi Chấn Tử: bản test đục lỗ, khảm và trang trí — 26/10/2026

## Nguồn và phạm vi

Đây là implementation phục dựng được người dùng yêu cầu sau khi đã kiểm tra PAK và thao tác client gốc. Không gọi implementation này là Lua server gốc đã thu hồi. NPC Diêu Trì có script ID `C821F1CD`, alias mới `B89373F6`; không áp dịch vụ lên toàn bộ NPC hoặc lên Lôi Chấn Tử ở Tây Kỳ.

Nguồn dữ liệu chính là 15 bảng `settings/item/da_kong_xiang_qian/` đang có hiệu lực trong Client_Original. Hash bảng ở Client và Server Loose trùng bản gốc. Đã đối chiếu 128 SPR duy nhất liên quan; native SprLoaderAudit đọc được 522 frame, không lỗi.

Nguồn tham khảo người dùng cung cấp:

- [VNG, thông báo tính năng trang trí ngày 13/06/2014](https://phongthan.vinagame.com.vn/tin-tuc/tin-tuc/13-06-co-the-trang-tri-ngoc-quai-phu-tren-trang-bi.html). Đã tải và xem cả hai ảnh tooltip; lưu trong `analysis/lei_restore_20261026/web/`. Ảnh có một ngọc và ba lỗ ghi `(1/3)`: tooltip dùng **số lỗ đã khảm / số lỗ đã đục**.
- [Phong Vương ADNX, khảm nạm–đục lỗ–khai quang](https://phongvuong.adnxgames.vn/tinh-nang/tinh-nang-kham-nam-duc-lo-khai-quang). Đây là sản phẩm/phiên bản khác, có chỗ tự mâu thuẫn về số loại Quái Phù và xác suất; dùng để đối chiếu quy trình, không chép toàn bộ thành dữ liệu VNG.

Phí và phân phối loại lỗ lấy bảng PAK hiện tại, có dòng phụ thuộc cấp trang bị và khác phí cố định trong bài năm 2014. Thuộc tính trang trí lấy cột phụ gia theo đúng thuộc tính đã roll; lỗ cao cấp nhân đôi phụ gia. Một số con số cấp cao trong ảnh người dùng khác cột phụ gia PAK đang có; không sửa bảng ngọc toàn game để ép ra con số trong ảnh. Chưa xác minh thuật toán server gốc trong những chỗ phiên bản khác nhau này.

## Có trong bản test

- Nhánh riêng mở cửa sổ Ghép VNG có sẵn: đục lỗ/trang trí; khảm nạm chọn lỗ 1–4; thuyết minh. Dùng nút hội thoại; nút kết thúc dùng giao diện sẵn có. Không tự thêm ba dịch vụ còn lại của root menu gốc khi chưa có quy tắc xử lý.
- Mảnh/Hoàng Thủy Tinh, Tử Thủy Tinh và phụ liệu khớp tuple nguyên gốc, đủ số lượng, phí F4, loại/cấp/nhóm trang bị, thứ tự lỗ, quy tắc ghi đè và ngọc cùng nhóm loại trừ nhau.
- Giới hạn lỗ đọc kết hợp bảng giới hạn tổng và quy tắc từng công thức. Mũ/yêu đái/giày thường hai lỗ; áo/phi phong thường ba, áo/phi phong trắng hai; vũ khí/thú cưỡi bốn. Những họ trang bị mở rộng có particular/cấp được bảng chỉ định mới nhận lỗ bổ sung. Không chỉ dựa màu tên để cho mọi đồ lên bốn lỗ.
- Lỗ đục cơ bản có thể thất bại theo khoảng tích lũy trong bảng; thất bại mất nguyên liệu/phí, giữ trang bị. Bí Ngân thay phân phối loại lỗ theo dòng công thức tương ứng. Chưa rõ/không hợp lệ thì từ chối, không tự cấp kết quả.
- Trang trí lại cả khi đang có ngọc: giữ ID ngọc và giá trị gốc, cập nhật phụ gia theo loại lỗ mới. Khảm vào vị trí đã chọn có thể thay ngọc cũ; menu nêu việc thay thế. Không tự thay ngọc khi dùng chế độ tìm lỗ trống.
- 53 định nghĩa ngọc/Quái Phù và các thuộc tính hiện có trong PAK. Thuộc tính lỗ là lớp riêng, không ghi đè dòng xanh, dòng bộ hoặc dòng nâng cấp. Áp dụng/gỡ lớp này khi đeo/tháo, có cộng công lực theo bảng.
- Tooltip ghi số đã khảm/tổng lỗ, tên lỗ/ngọc, dòng phụ gia “Tăng thêm…”, màu chữ riêng, hình SPR ngọc/lỗ trên ảnh trang bị và hình lỗ nhỏ đầu dòng. Tọa độ lấy bảng vẽ lỗ gốc. Chưa có nghiệm thu bằng ảnh game Loose sau phát hành; không cam kết giống từng pixel chỉ từ fixture.

## Lưu và giao dịch

Giữ nguyên ABI item/character/mail/auction. `ScriptParam` của trang bị có socket tham chiếu một định nghĩa bất biến, có checksum, trong `Server/settings/phongthan/socket_states/`. Định nghĩa chứa tham số gốc của trang bị, bốn lỗ, bốn ngọc, phụ gia và giá trị đã roll. Ghi bền vững trước khi chấp nhận giao dịch; lỗi đặt đồ/thanh toán hoàn trả cùng serial và số lượng nguồn. File định nghĩa chưa được tham chiếu do rollback là orphan vô hại.

Tham số gốc của vũ khí tân thủ/trang bị rèn được giữ bên trong định nghĩa. Đổi người sở hữu, gửi thư, đấu giá và lưu nhân vật vẫn mang token qua trường item cũ. Client nhận định nghĩa từ server trước inventory snapshot, hoặc yêu cầu lại khi xem preview/link. Không tin số lỗ/thuộc tính do client gửi. Query chỉ đọc, có giới hạn tần suất.

**Khi sao lưu/khôi phục server có các trang bị này phải giữ thư mục socket_states cùng CharacterStore và dữ liệu giao dịch.** Thiếu định nghĩa thì luồng tải nhân vật từ chối thay vì cho mất socket âm thầm. Chưa chạy giao dịch hai tài khoản thật trong game; test hiện tại kiểm tra record/token và vòng đọc lại định nghĩa, chưa phải nghiệm thu đầu cuối thư/đấu giá.

## Hiệu chỉnh có chủ đích

Dòng 68 của `da_kong_xu_qiu_biao_sc.txt` có ngưỡng tích lũy `3300/6600/3300/10000`, giảm ngược ở ngưỡng thứ ba. Reader sửa đúng ô này thành `9900`, theo 11 dòng cùng họ công thức. Bảng gốc trên đĩa/PAK không đổi. Có test riêng hai biên 9800/9900. Không bình thường hóa tùy tiện các tỷ lệ khác.

## Giới hạn chưa phục dựng

Không tự thêm Khai Quang, Phục Nguyên Ngọc, hệ ngọc chưa có định nghĩa trong 53 dòng, hoặc buff tổ hợp Quái Vị từ lời mô tả ADNX. Các workflow đó cần đối chiếu riêng; bản test này chỉ triển khai nhánh đục lỗ/khảm/trang trí nêu trên. Chưa mở ghép nâng bậc nguyên liệu có tỷ lệ dưới 100% qua workstation cũ; không đổi thuật toán ghép Xích Tùng Tử để làm việc đó.

## Kiểm tra và áp dụng

`LeiSocketTests.cpp` chạy parser/planner/transaction/store production với bảng thật trong fixture riêng: 10.667 kiểm tra gồm 53 ngọc × 100 giá trị chọn thuộc tính, phí, loại trừ nhóm, giới hạn từng loại đồ/thánh, thay ngọc, trang trí giữ roll, serial, đọc lại từ đĩa, stack còn dư, thiếu tiền/chỗ trống, rollback và replay. Inventory/payment/RNG được cô lập; không trừ đồ tài khoản thật.

`LeiSocketUiTests.cpp` chạy Lua 4 thật: menu 3/4 nút, mode 2–6, đóng thoại/trợ giúp. Chạy encoder text Engine thật và custom-picture sink giả để kiểm tra marker được thay bằng picture nhị phân, cache và marker thiếu ảnh không lộ ra chữ. Kiểm thử ghép trang bị Xích Tùng Tử hiện có cũng đã qua.

Build GameClient/CoreClient/CoreServer Release đã qua. Bằng chứng ở `analysis/lei_restore_20261026/`: `tests.log`, `sprites_native.log`, `resource_proofs.json`, các log build, `content.json`; gói triển khai `Deploy/ProjectContent/LeiSocket20261026/manifest.json`.

**Trạng thái:** đã áp dụng gói 5 file, cả 5 đúng hash. World PID **121024** đã nạp **105 map**, **4.044 NPC/quái**; **9.223 script nạp thành công, không lỗi**. Dữ liệu nhân vật không đổi; PID các dịch vụ tài khoản giữ nguyên. Có backup trước thay và receipt tại `analysis/lei_restore_20261026/publication.json`, kiểm tra độc lập tại `post_publish_verification.json`. Chưa nghiệm thu thao tác/tooltip trong game Loose bằng nhân vật thật sau phát hành.

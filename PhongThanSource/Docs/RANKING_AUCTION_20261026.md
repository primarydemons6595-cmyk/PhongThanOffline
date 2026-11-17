# Bảng Phong Thần và đấu giá — 26/10/2026

## Đã áp dụng

24 file của gói `Deploy/ProjectContent/RankingAuction20261026` đã đưa vào Loose, gồm Game.exe, CoreClient.dll, CoreServer.dll, hai INI, 17 SPR gốc, client loose_index và metadata để lần rebuild index sau vẫn giữ các đường dẫn mới. Có backup, kiểm tra SHA256 sau thay file. Các bản sửa trước trong source được giữ lại.

GameServer cuối cùng: **PID 118716**, ConPTY host dùng bản Release của Studio. World sẵn sàng, nạp 105 map, có 4.044 NPC/quái trong snapshot; 60 map có NPC. Sáu file nhân vật giữ nguyên hash. Goddess 84428, AccountServer 116204, Relay 70604, Bishop 22100 không đổi PID. Client_Original không bị đóng/thay file.

## Bảng xếp hạng

Thay adapter cũ chỉ có tên và một chỉ số/10 dòng bằng sáu tab và bảng nhiều cột, tối đa 20 dòng/trang, nút trang và scrollbar. Các bảng mang tên “Thập đại” vẫn giới hạn 10 người. Dùng tài nguyên gốc `\ui\ui3\排名.ini`, winner vng00.pak entry 4866, SHA256 `9b645ec59468bdac5a08363821a3602a1f448abf1bd57d3dec3f0f35c5db6e49`; đường dẫn ảnh được đặt alias ASCII, không vẽ lại ảnh gốc.

Dữ liệu lấy từ các character snapshot hợp lệ, có kiểm checksum, rồi cập nhật bằng dữ liệu nhân vật online đã load xong. Chu kỳ 30 giây. Tiền dùng 64 bit khi cộng tiền túi và kho. Danh vọng dùng chính task của GetCredit trong server này; điểm Tiên/Ma dùng task GetJusticEvilCredit, tách dấu dương/âm theo script gốc. Lọc riêng ba phái, nhân gian/thiên giới, danh vọng, nghĩa sĩ và công lực.

Công lực offline khôi phục trang bị đang đeo bằng cùng catalog, upgrade và companion thuộc tính đã lưu như luồng load nhân vật, rồi gọi GetEquipmentPower. Không cấp item vào ItemSet, không sửa nhân vật; RNG được giữ nguyên qua việc tính. Có cache theo checksum và làm mới tối đa sau 5 phút. Bảng kinh tế quốc gia đọc ngân khố và thành viên từ nation store thật.

**Giới hạn còn lại:** server chưa có nguồn thống kê đầy đủ cho quân sự/vinh dự quốc gia, điểm sư đồ, vác nặng, Động Thiên Phúc Địa, Bá Vương và một số bảng chiến trường/liên máy chủ. Những mục này hiển thị thông báo chưa có thống kê; không đổ bảng cấp độ vào để giả lập. Không suy diễn sức mạnh quân sự từ cấp hoặc tiền. Dữ liệu Vũ Sĩ sẽ trống khi server chưa có nhân vật phái này.

## Đấu giá

Trước sửa, UiPtAuction chỉ mở ảnh nền và dòng thông báo; không có protocol/escrow server. Không tìm thấy backend gốc hoàn chỉnh trong cây source đang dùng. Phần dưới là **backend native mới**, không tuyên bố đã phục hồi nguyên bản backend VNG.

Đã nối năm tab gốc: xem bán, phiên đang trả giá, món của mình, xem đặt mua và đặt mua của mình. Có tìm tên, nhóm vật phẩm, phân trang, ảnh item, giá, người bán/người mua và số lượng. Nút giao hàng cho lệnh đặt mua dùng ảnh “Bán” gốc. Đồ/tiền được nhận qua nút Nhận đồ.

Hỗ trợ đăng bán, trả giá, mua ngay, hủy khi chưa có người trả giá, hoàn đồ/tiền khi hết hạn, hoàn tiền cho người bị trả giá vượt, tạo và cung cấp đủ số lượng của lệnh đặt mua. Lệnh đặt mua chỉ dùng vật phẩm thường có thể xếp chồng; nhận đúng genre/detail/particular/level/series/row, không ghép theo tên trùng. Giá lệnh mua là **tổng tiền cho cả số lượng**. Thời gian 1–72 giờ, giá tối đa 1 tỷ lượng, không tự đặt thêm phí chưa xác minh từ bản gốc. Giới hạn hiện tại 256 lot và 1.024 khoản nhận.

Đồ giữ nguyên snapshot thuộc tính đã roll/nâng cấp qua bộ snapshot vật phẩm hiện có. Server kiểm tra ownership từ serial thật, túi hợp lệ, khóa giao dịch, số tiền, revision và tình trạng lot; không tin tên hoặc thuộc tính do client gửi.

`CharacterStore/auction.dat` lưu book và escrow bằng ghi tạm, flush, thay file nguyên tử, checksum. Task engine 4892 là receipt nhân vật. Chỉ kích hoạt món bán/giá mới và xác nhận nhận đồ sau khi thấy receipt trong character file mà Goddess đã lưu. Mỗi nhân vật chỉ có một thao tác chờ; chống replay theo session ticket và sequence. Nếu offline mà chưa có receipt, giữ giao dịch chờ, không tự kết luận đã rollback. Khi đăng nhập lại với snapshot cũ, có thể xác định thao tác chưa được lưu và trả trạng thái phù hợp. Khi lỗi save, khôi phục cả tiền/đồ lẫn receipt trong bộ nhớ để autosave sau đó không nhận nhầm.

## Kiểm chứng và phạm vi

- Build Release GameClient/CoreClient/CoreServer và các baseline bắt buộc: PASS.
- 148 kiểm tra chạy mã auction production với fixture: giữ đồ/tiền, outbid/refund, mua ngay, hủy, hết hạn, lệnh mua/cấp hàng một phần stack, sai variant, túi đầy, trần tiền, save thất bại, crash trước/sau receipt, nhận lặp và book hỏng.
- 65 kiểm tra mã ranking production: đọc snapshot/checksum, tiền vượt 2 tỷ, phân trang, lọc phái/Tiên/Ma, thứ tự danh vọng/điểm/công lực, giới hạn bảng và packet không hợp lệ.
- Engine thật đọc cả hai INI theo tên/ID và loose-only; 45 SPR, 87 frame, 0 lỗi trên runtime đã publish.
- Đã xem bản ghép tham chiếu để kiểm bố cục và hình gốc. Đây không phải ảnh chụp giao diện Game đang chạy. **Chưa kiểm chứng thao tác mua/bán bằng hai cửa sổ Game thật.** Fixture kiểm tra logic không thay thế bước này.

Bằng chứng: `analysis/ranking_auction_20261026/publication.json`, `auction_test_results.txt`, `rank_test_results.txt`, `native_sprite_runtime.log`, `rank_asset_proofs.json`, các log build. Script publisher đã sửa cách chờ process controller: chờ chính controller thoát, không chờ cây con ConPTY/GameServer vốn cần tiếp tục chạy.

## Studio build

Đã áp dụng riêng checkbox Bypass Execution Policy và target Event trong Studio. Xem [STUDIO_BUILD_EVENT_20261026.md](STUDIO_BUILD_EVENT_20261026.md). Tùy chọn bypass chỉ áp dụng cho PowerShell build, không thay policy hệ thống.

# Kiểm tra Ngọc Bội và mài ngọc — 26/10/2026

## Kết quả

Ngọc Bội thông thường lấy từ Cẩm Nang hiện là mẫu nền. Các dòng Thanh Linh trong jadependant.txt không có thuộc tính cơ bản; một số mẫu sự kiện có thuộc tính cơ bản cố định. Không thể kết luận mọi Ngọc Bội trắng dòng đều đúng: Loose chưa nối phần thuộc tính ngẫu nhiên/tẩy tủy của Ngọc Bội.

Bảng gốc bổ sung `settings/item/001/玉佩魔法属性表.txt` đã tìm thấy trong PAK có hiệu lực: serverlist.pak, entry 272, ID 19D9B862, SHA256 abfca63f0efbab52ec506c43f7e790eaf05483706e37088b687caac3551e845c. Có loại thuộc tính, khoảng min/max, nhóm hệ, hiệu ứng, giới hạn sở hữu và tỷ trọng theo năm bậc. Bảng này chưa được nạp vào engine Loose; không tự roll dòng theo bảng MagicAttrib cũ của JX.

Script Dẫn Tủy Châm đã có trên đĩa Loose (`script/item/引髓针.lua`, magicscript 739). Script gọi `IsJadePendantEquip`, `GetJadePendantQL`, `AddJadePendantMagicAtr`; chưa có đăng ký/implementation các hàm này trong Core. Không tìm thấy payload script này trong chuỗi PAK client gốc đã đối chiếu, nên không khẳng định file Loose là đúng bản server VNG mới nhất. Script dùng 5 Ngọc Tủy tương ứng material 1045–1049; tẩy lại dùng Tẩy Tủy Châu IBItem 1081. Không chạy thử nhánh trừ nguyên liệu trên nhân vật thật khi native còn thiếu.

## Quy trình gốc và phạm vi hiện có

[VNG: Thuộc tính hấp dẫn của Ngọc Bội sau khi tẩy tủy](https://pt.vnggames.com/tin-tuc/can-biet/thuoc-tinh-hap-dan-cua-ngoc-boi-sau-khi-tay-tuy.html): đeo Ngọc Bội, dùng Dẫn Tủy Châm cùng 5 Ngọc Tủy của hệ muốn tẩy. Tẩy lại cần Tẩy Tủy Châu; thủ tủy ở Lôi Chấn Tử Diêu Trì (198/199) hủy Ngọc Bội để nhận tủy. Đây là tẩy tủy Ngọc Bội, khác mài ngọc chưa mài để khảm trang bị.

Đã rà script có chữ mài, material, magicscript, IBItem và workstation native. Có nguyên liệu ngọc chưa mài/ngọc đã mài và công thức ghép tăng bậc, nhưng chưa tìm thấy luồng mài ngọc hoàn chỉnh có thể thực hiện trong Loose. Ví dụ 5 material 260 -> material 261 là tăng bậc, không phải mài thành material 288. Không hướng dẫn người chơi dùng workstation cũ để giả lập mài. Phần Lôi Chấn Tử phục dựng trước đó mới hỗ trợ đục lỗ/khảm/trang trí, không bao gồm đầy đủ mài/ngọc bội.

## Sửa ô đeo

Ô JadePendant gốc là SPR 40×40; detail trang bị 12 -> itempart_cuff -> UIEP_FINESSE, đúng ô JadePendant. Bản sửa chỉ giới hạn ảnh trang bị Ngọc Bội vừa vùng 36×36 bên trong ô 40×40, giữ tâm ảnh, không đổi tọa độ/ánh xạ ô hay cách vẽ các trang bị khác. Chưa thay đổi thuộc tính hoặc dữ liệu nhân vật.

CoreClient Release build thành công. Test trích đúng hàm PaintEquipmentSlot production kiểm tra Ngọc Bội có inset/thu nhỏ và armor giữ cách vẽ cũ. Chưa kiểm tra trực quan trong Game sau thay DLL.

Đã áp dụng CoreClient.dll, hash trùng gói; không restart server. Gói: Deploy/ProjectContent/JadeSlot20261026. Biên nhận và bằng chứng: analysis/jade_audit_20261026.

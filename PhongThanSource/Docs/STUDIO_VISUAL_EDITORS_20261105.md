# Studio: danh mục ảnh và chỉnh thuộc tính

## Điều hướng

**Chỉnh sửa server → Map Editor** chứa Bản đồ, NPC Editor, Item Editor, Skill Editor và F3 Editor. Liquid cũng hiển thị các mục này thụt vào dưới Map Editor ở thanh bên. Mục **Sửa Icon Skill F5** đã bỏ khỏi menu; trường SkillIcon vẫn có trong thuộc tính Skill Editor.

NPC/Item/Skill/F3 và Quản lý hồi sinh quái mặc định mở danh mục có ảnh SPR thật. Nạp bảng, tìm theo tên/ID, chọn đối tượng rồi bấm để mở thuộc tính. Có lọc theo tên thuộc tính, chọn mã chữ khi sửa và trang danh mục tối đa 96 mục. Bảng dữ liệu nâng cao vẫn có. Áp dụng thuộc tính chỉ sửa mô hình đang soạn; đưa vào patch và áp dụng vẫn theo quy trình hiện có.

Quản lý hồi sinh chỉ sửa **ReviveFrame** nguyên bản, số nguyên không âm; không tự quy đổi ra giây hoặc khẳng định nạp nóng runtime. NPC/Item/Skill/F3 giữ schema, row ID và codec của bảng hiện có. Thẻ thiếu ảnh không tạo ảnh giả.

## Quản lý server

- **Rớt đồ quái/Boss:** danh sách vật phẩm có ảnh, bấm để sửa số lượng/Weight/Luck. Chọn quà theo ảnh, chọn bảng rơi, sửa thông số bảng và gán NPC. Range/Weight giữ công thức hiện có; không coi trọng số là phần trăm. Có tab cấu hình nâng cao và drop gốc riêng. Không thay cơ chế consumer native hay quy trình Stage/Apply.
- **Tặng quà / Phát quà:** chọn vật phẩm có ảnh/ID, nhập nhân vật, map và số lượng trong hộp thuộc tính.
- **Gọi Boss/NPC:** chọn template thật bằng ảnh và nhập yêu cầu trong hộp thuộc tính.

Hai mục quản trị cuối vẫn là **DRAFT_NOT_EXECUTED**, chưa gửi quà hay spawn vào game vì chưa có endpoint quản trị xác thực. Lưu nháp giữ JSON trong `ManagerStudio/AdminDrafts` như trước. Không trình bày lưu nháp là đã thực thi.

## Chỉnh sửa UI

Chọn INI trong Client/Ui, mở tab **Mô phỏng UI · tọa độ và SPR**. Canvas chọn 800×600, 1024×768 hoặc 1280×720; 1 đơn vị là 1 pixel client, zoom chỉ thay kích thước xem. Dùng ảnh SPR từ đường dẫn Image, frame đầu, kích thước thật; chọn section bằng danh sách hoặc bấm lên hình để sửa thuộc tính và áp dụng vào mô hình. Bản sửa được đưa vào patch Client như cũ.

**Giới hạn:** đây là trình dựng bố cục INI, chưa chạy engine UI. Quan hệ Parent ghi trong file được cộng tọa độ; section không có Parent được xem tương đối với Main và có thông báo. Game còn gắn con, FollowMove/FollowSize, căn theo độ phân giải, visibility và trạng thái nút bằng C++; các phần đó chưa được tái hiện đầy đủ. Section thiếu ảnh/kích thước hiện khung cam, không phải ảnh game chính xác. Cần đối chiếu trong game trước khi phát hành thay đổi UI.

## Kiểm tra

Biên dịch Release; nạp catalog thật NPC, item, skill, F3 và hồi sinh. Kiểm tra gallery không làm đổi bytes khi không sửa; đường áp dụng thuộc tính từ chối delimiter và ReviveFrame âm, sửa/hoàn tác khôi phục byte gốc. Render các danh mục và hộp thuộc tính. UI dùng file INI thật và kiểm tra mẫu Parent với tọa độ 100+20, 50+30; serialize không sửa giống byte gốc. Quà/Boss/drop dùng dữ liệu xem trước trong bộ nhớ, không lưu yêu cầu, không phát vật phẩm hay gọi Boss trong lượt kiểm tra.

Ảnh, log và bản sao lưu: `analysis/studio_appearance_20261105/visual-*` và `pre-visual-editors-release`.

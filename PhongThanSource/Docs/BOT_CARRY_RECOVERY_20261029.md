# Bot quá tải tại điểm hồi sinh

Trên GameServer 165632, HacAnh9808 (1cb6bac21d7e4576ad74d38b563bf84e) và LamLong5173 (882d5f59776246d292eafed320f9a74f) đứng tại map 1002, 51584/102144, đầy máu, state travel_training. LamLong trước đó giết quái/nhặt đồ, chết 22:45:27 và hồi sinh 22:45:35. Chẩn đoán Doing=stand. Đối chiếu tên vật phẩm inventory với cột tải trong bảng item/001: trọng lượng túi lần lượt 236 và 209, không có món không tra được. Sức chứa cơ bản 200; luật PhongThanCarryBlocked chặn cả đi bộ lẫn chạy. Vị trí hồi sinh chỉ là nơi quan sát được sự cố, không phải nguyên nhân cổng/map.

Lỗi cũ: BotLoot kiểm tra ô túi nhưng không kiểm tra sức tải. Đi shop không có ưu tiên theo tải. Khi đã quá tải, engine chặn di chuyển nhưng bot vẫn báo travel_training.

Sửa chung:

- Tính trọng lượng nhân số lượng trước khi nhặt. Đồ thường giữ trong 80% tải; thuốc, sách học được, đồ hiếm và đồ hữu ích được dùng phần còn lại nhưng không vượt trần.
- 80% tải ưu tiên kế hoạch tới NPC shop thực, giữ cooldown nếu không tìm được shop để không quét mỗi tick.
- Đồ trắng không hữu ích được đưa vào danh sách bán khi áp lực tải, không cần đủ ba bản trùng. Giữ đồ hiếm/khóa/script/hạn dùng/hữu ích.
- Khớp NPC shop qua đường dẫn script gốc hoặc đường dẫn Lua thực đã resolve alias; chuẩn hóa dấu slash đầu đường dẫn. Chỉ chọn callback thực sự được NPC phát ra, không bypass giao dịch/currency.
- Đã quá tải: state overloaded và lỗi có số tải/sức chứa; sau xác nhận của người dùng, tự đưa từng trang bị trắng dư vào tay rồi thả bằng request chuẩn. Chỉ xác nhận khi serial mất khỏi hành trang và item thật có trên đất. Nếu bị từ chối thì đưa lại ô cũ. Không xóa item trực tiếp, không thả đồ hiếm/đang mặc/khóa/thuốc/sách/hữu ích.
- Inventory snapshot bổ sung CarryWeight/CarryCapacity để chẩn đoán.

Kiểm tra helper gồm ngưỡng 80%, khối lượng stack, giới hạn 200 và không cho nhặt thêm khi đã vượt tải. Native BotStarterStateTests đạt; Core build 0 error, cảnh báo LNK4084 cũ. Publish graceful lưu world165632, backup đầy đủ, khởi động world147040. Runtime147040 xác nhận chính xác CarryWeight/CarryCapacity=236/200 và 209/200, state overloaded. Thao tác bag→hand ban đầu không thành công: ExchangeItem dành đường cross-container cho atomic move và tự hoàn trả source khi destination là hand. Sửa dùng request cùng ô nguồn để lấy vào tay, cùng kiểu client hiện tại; rollback cũng dùng request cùng ô nguồn. Không ghi nhận drop thành công trên147040.

Bundle thêm sửa câu hỏi trang bị bị model hiểu nhầm thành bảo mật quần áo cá nhân: Rules nhận câu hỏi đồ đang mặc/vũ khí/set đồ/giao dịch, điều hướng intent3; tham chiếu trang bị tìm món thật đang mặc, bỏ dấu # nội bộ khi so tên. Đồ đang mặc đang dùng không tự bán; trả lời đúng lý do. Prompt phân biệt dữ liệu trang bị công khai trong game với thông tin cá nhân. 74 kiểm tra managed đạt. Build/publish Core+Event+Studio, lưu sạch147040 và khởi động world7432; đang chờ xác minh runtime bản sửa thao tác tay.

Kiểm chứng world7432 vẫn không bỏ được đồ. Đọc snapshot pthc bằng tool inspect chỉ đọc: đồ trắng bình thường có MagicValue=10/20 và LockDrop=1, LockSell/LockTrade=0. Điều kiện BotRareEquipment dùng GetMagicPower()>0 đã đánh đồng công lực nền với phẩm chất hiếm; BotSurplus còn dùng cờ cấm thả để chặn bán shop. Đã đổi sang GetPhongThanQuality() cộng nâng cấp/set thật, bỏ kiểm tra GetMagicPower và LockDrop khỏi điều kiện bán. Thả đồ trắng theo đúng chính sách manual equipment drop đang có (bỏ qua CanDrop của template, giữ khóa cá nhân/khóa bán/giao dịch/hiếm/hữu ích). Lưu sạch7432, publish world162828; chờ xác minh runtime cuối.

AI Local: phát hiện đã chỉ còn model4B, nhưng Installed()/chọn GPU đòi file1.7B cũ. Đã sửa để profile4B đã xác minh chạy độc lập. 75 kiểm tra managed đạt. Event106548 xác nhận ModelReady=true, Qwen3-4B-Instruct-2507-Q4_K_M_gpu lúc23:27:09; không tải lại model cũ.

Giới hạn độc lập: nhặt đồ theo lệnh chat chưa có adapter chọn đúng vật phẩm người chơi chỉ định; thương lượng/chốt giao dịch chưa hoàn chỉnh; party đang có điều kiện bot riêng từ chối chênh hơn 30 cấp. Không coi những phần này là đã hoàn thành chỉ vì bot có chat AI.

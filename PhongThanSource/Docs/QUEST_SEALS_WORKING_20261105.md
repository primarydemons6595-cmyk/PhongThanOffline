# Phần ấn ký — ghi chép triển khai cũ

> Cập nhật 05/11/2026: đã hoàn tất build, kiểm tra và nạp cả hai loại Buff/F4. Nội dung bên dưới là lịch sử trong lúc triển khai, không phải trạng thái hiện tại. Xem [báo cáo đã nạp](STUDIO_QUESTS_AND_FIXES_20261105.md).

## Tiến độ mã mới (chưa build/nạp)

### Cập nhật mới nhất trong lượt đang chạy

- GUI đã thêm `StudioQuestSealEditor.cs`: tab Ấn ký & thời hạn, tên, chọn icon từ item catalog hoặc SPR client, số giây, Buff/F4, vật phẩm nền QuestKey, mô tả màu. `SealBox` đã nối vào EditQuest.StartSeal và EditStep.RequiredSeal/ConsumeSeal. Phần tiêu hao chỉ bật cho Gather hoặc Collect có nguồn quái; cần thêm validate compiler/runtime để không bỏ qua cờ sai nếu JSON sửa tay.
- Compiler SealsIni đã ghi hai phía; NpcScript có `renew_q`, chỉ hiện ở NPC đầu và native Act op2 chỉ cho nhận lại khi hết/đã tiêu hao. Lua phần này chưa có case riêng renew trong VM (case cũ vẫn pass).
- `Tools/wire_quest_seal_icons_20261105.py` đã áp dụng: LoadPowderScheme merge metadata Buff vào m_PtPowderIni, không thêm query/opcode. Đường dẫn C++ đã sửa đúng hai backslash. `ScriptFuns.cpp` ApplyNativeIBBuffState/ExpireNativeIBBuffState vừa thêm skip 9700..9763, bảo đảm ấn ký chỉ là marker, không thêm/trừ thuộc tính skill.
- Native `StudioQuestSealCases.inc` đã chạy PASS: buff grant/expiry/reissue, không gia hạn khi còn hiệu lực, chỉ rơi đồ khi còn buff, store48 slot đầy, consume sau success, full-bag rollback; F4 param riêng, loại trừ item nền/ấn ký khác, hết hạn còn trong túi cũng không bị dùng nhầm sau reissue, consume nguyên tử. C# fixture hiện 10 quests và 2 seals; generator expected count10.
- `QuestSealTextTests.cpp` chạy PASS thực KIniFile/Engine: tên khác nhau, icon SPR đúng, tooltip, không ảnh hưởng tag2D/2E và param thường. Log `quest-seal-text-tests.log`.
- Quan trọng: QuestExchange lọc thêm seal hết hạn qua GetExpireTime > clock. Macro PT_QUEST_ITEM_NOW mặc định KSG_GetCurSec, fake clock ở QuestExchangeTests. Mock cơ sở vừa đổi param thành `m_CommonAttrib.nParam`, GetParam/SetParam; các mocks CustomEventActionTests/F2RequestedGiftTests/JadeServiceTests/TreasureItemTests vừa thêm getters và clock, **chưa chạy các regression đó**.
- `quest-seal-final-tests.log` đã PASS bộ chính trước lượt đổi mock common-param (cần chạy lại một lần sau cùng). `quest-seals-dotnet-build.log` build WPF 0 lỗi. Native build đầu fail namespace Item-shadow đã sửa `::Item`; lượt build tiếp CoreServer/CoreClient/Game 0 lỗi nhưng ScriptFuns marker guards + expiry filter thay đổi sau đó nên **cần biên dịch lại trước nạp**.
- Chưa cài phần seals mới. Receipt nền `quest-followup-deployment.json` cài lúc UTC17:07:37 (00:07 ngày05/11), chỉ 10 file thay đổi, quy trình giữ world tắt. Installer cũ beforehash Lua/UI đã là bản mới nên không chạy lại y nguyên; lần seals chỉ thay binaries/App (không phát hành fixture/Seals.ini mẫu).
- Cần hoàn thành: validate ConsumeSeal cho đúng loại bước, thông báo thiếu/hết seal rõ ràng (hiện có nhánh chỉ trả tên seal); Native broad transaction regressions; C# checks/screenshot tab seals; native UI hex/metadata + opcode regression; final native/App build; nạp lần2 khi processes vẫn tắt; update báo cáo + final ngắn bằng tiếng Việt.

- Đã thêm `Headers/PhongThanQuestSeals.h` (IDs9700..9763, paramtag2C), `PhongThanQuestSealText.inl` cache metadata riêng `QuestSeals.ini`. KItem.GetName qua CustomItemName ưu tiên seal; GetImageName và tooltip dùng seal metadata. GetExpireTime trả raw cho seal để hết hạn không biến thành vĩnh viễn khi save. KItemList.NowEatItem/ExecuteScript chặn chạy script vật phẩm nền cho tag2C.
- QuestExchange thêm tham số cuối mặc định `const int* needParams=0`, macro read GetParam, lọc Param từng nguyên liệu và ngăn luồng thường tiêu hao seal2C nếu không chỉ định đúng param. Mock QuestExchangeTests thêm `param` và macro. Bộ test nền vẫn pass trước khi thêm SealOps; **sau khi thêm SealOps chưa chạy/build**.
- C# StudioQuests.cs đã có enum `StudioQuestSealKind`, record `StudioQuestSeal`, document.Seals/NextSealId, q.StartSeal, step.RequiredSeal/ConsumeSeal. Validation tên31/giây1..2592000/SPR/path, Item chỉ QuestKey; compiler SealsIni ghi metadata hai phía và Store.Changes thêm file. Chưa có GUI danh mục/chọn seal, chưa viết RequiredSeal/ConsumeSeal vào INI? (đã thêm hai Row sau DropPercent bằng script, kiểm tra lại). Chưa có NPC Lua renew.
- Runtime StudioQuests.inl đọc Seals.ini vào vector Seal, q.startSeal, step.requiredSeal/consumeSeal. Include `PhongThanStudioQuestSeals.inl` sau Owned trước Description. SealOps.Has/Grant/Exchange đã viết: Buff thật qua IBBuffStore, physical prototype param/expire/bindtrade, transaction needParams, consume sau khi thành công. Act op2 xin lại chỉ khi hết, cùng NPC đầu; Act0 dùng Grant. Killed kiểm tra requiredSeal, nguồn rơi dùng SealOps.Exchange; Gather.Keys/Finish cũng dùng SealOps. **Kill-only counter consumeSeal chưa nối**; kiểm tra để không hiện tùy chọn tiêu hao nếu không hỗ trợ.
- Cần kiểm tra SourceLoad SealIni path (`\\settings\\phongthan\\QuestSeals.ini`), getter item.param/expire mocks, thêm header QuestSeals vào native test mock.
- Cần GUI + icon merge vào UiPtConsumableBuff.LoadPowderScheme; Native Ops mocks IBBuff và KItem methods; tests mới; build cả server/client/Game; nạp lần 2 (installer cũ kiểm beforehash nội dung đã thay nên không chạy lại nguyên xi).

Ưu tiên: hoàn tất đúng cả hai loại, test hết hạn/name/icon/param tách biệt/túi đầy; không báo xong cho người dùng khi chưa nạp.

Người dùng xác nhận cần **cả hiệu ứng trên thanh buff và vật phẩm trong F4**, được chọn icon, đổi tên, đặt số giây. Ấn ký không chỉ dùng cho thu thập: phải có thể làm điều kiện đánh quái nhận vật phẩm. Hiện các bản đã nạp trước bước này chỉ có `KeyItem` nguyên liệu vật lý, chưa có ấn ký tùy chỉnh.

Bản nền đã cài: `analysis/tet_event_shop_20261103/quest-followup-deployment.json`, state INSTALLED_PROCESSES_STOPPED. Game/Studio/world hiện tắt. Chưa đưa nhiệm vụ mẫu lên live. Đã cài six fixes + Quest Designer và điểm thu thập native trước khi người dùng bổ sung ấn ký. Cần tiếp tục làm, không báo xong ấn ký.

Thiết kế tiếp theo:

- Danh mục `StudioQuestSeal`: ID, tên, kiểu Buff/Item, icon đường dẫn SPR client, số giây, item nền cho kiểu Item. Dùng ID buff riêng 9700–9763 (cần kiểm tra trùng), param vật phẩm riêng 0x2C000000 | id. Không đổi tên/template gốc.
- `StudioQuestDefinition.StartSeal` chọn ấn ký được nhận khi nhận nhiệm vụ; có thể xin lại tại NPC đầu khi ấn ký đã hết, nhiệm vụ còn đang làm. Không gia hạn miễn phí khi vẫn còn hiệu lực.
- `StudioQuestStep.RequiredSeal`, `ConsumeSeal` dùng cho nguồn rơi vật phẩm khi hạ quái và thu thập. Giữ `KeyItem` để có thể yêu cầu thêm nguyên liệu F4 thường.
- Buff dùng IBBuff store thật đã có: `AddIBBuffToStore`, `FindIBBuffSlot`, `RemoveIBBuffFromStore`, `PersistPlayerIBBuffStore`, IDs <=9999 UI. Không tạo query/opcode mới. Merge metadata icon/title/desc từ `QuestSeals.ini` vào `m_PtPowderIni` trong LoadPowderScheme; dùng nguyên bộ hiển thị/thời gian hiện có.
- Vật phẩm riêng dùng prototype `CreateVngNormalItem`, Param/ExpireTime lưu native sẵn; nParam đã được sync/persist. Thêm metadata riêng cho tag2C ở KItem getters Name/Image/tooltip, tránh thay tên toàn bộ item cùng template. `GetImageName()` ở KItem.h hiện trả Common.szImageName; `GetName()` đang hỗ trợ custom event param 2D/2E qua PhongThanCustomItemText.inl.
- Transaction giữ nguyên row ABI. Đề xuất thêm tham số cuối tùy chọn `const int* needParams=0` cho `PhongThanQuestExchange`, lọc thêm Param theo từng nguyên liệu, mặc định null để các luồng cũ không đổi. Macro read Param mặc định Common.nParam; mock QuestExchangeTests thêm param và macro riêng. Không sửa tuple toàn cục hoặc consume vật phẩm cùng template mà khác ấn ký.
- Grant/check/consume trên server, giao dịch nguyên tử khi túi đầy. Buff phải kiểm tra hiệu lực thật và slot trống trước grant. Prototype được xóa sau clone. Không dùng vật phẩm/tiền của nhân vật thật trong test.
- Cần thêm GUI danh mục ấn ký với ảnh thật, chọn icon từ item/client SPR, đổi tên, số giây, chọn kiểu; chọn ấn ký ở điều kiện bước và lúc nhận nhiệm vụ. Compiler tạo metadata hai phía, runtime config và script, áp dụng bằng patch hiện có.
- Cần test: icon/name/custom param qua lưu/nạp, hết hạn không nhận đồ, monster cần buff seal, physical seal không dùng nhầm item nền/ấn ký khác, túi đầy rollback không mất ấn ký, xin lại khi hết hạn, giữ icon buff cũ.

Nguồn đã kiểm tra:

- `ScriptFuns.cpp:12309` GetPlayerIBBuffStore compact hết hạn; AddIBBuffToStore ~12418, IsValidIBBuffId = 1..mask, không cần definition. Add tăng nTimes nên seal marker cần đặt nTimes=1 để không giả stack. Buff store 48 slot; snapshot task slots có budget.
- `CoreShell.cpp` PT_GDI_IBBUFFS lấy từ task, chấp nhận id<=9999. `UiPtConsumableBuff.inl` LoadPowderScheme và vòng IB icons đọc BuffInfo_ID Image/Title/Desc, đang dùng đúng icon gốc.
- `PhongThanProtocol.h` item record có Param và ExpireTime. `KItem.h` GetName ở ~181, GetImageName ~367.
- `PhongThanCustomItemText.inl` tag2D/2E per-instance tên/tooltip đã có, không ghi đè template. KItem.cpp dùng RichName ở1045 và Tooltip1128.
- Native prototype transaction đã có tham số prototype, carriedNeeds, exactNeed, bagNeedsOnly; signature cuối ở PhongThanQuestExchange.inl. Không đổi cấu trúc row nếu tránh được, vì nhiều luồng dùng chung.

Các yêu cầu trước đã kiểm tra/nạp: buff icon collision sửa và đã thấy trực tiếp trong game; thuê kho ở xa, white family310 không nhận nhầm set; PvP chỉ khi ngã ngựa; font/dialog pagination; hồi sinh màu; Studio nhiệm vụ + Object thu thập, native progress/movement-damage cancel, ấn ký F4 cơ bản và drop nguồn quái, nguyên tử/race tests. Cần cập nhật báo cáo cuối khi hoàn tất ấn ký mới.

"""Explain each recovered/unresolved request without fabricating replacement data."""
import json
import struct
from collections import Counter
from pathlib import Path
import build_offline_loose as b

root = Path(__file__).resolve().parents[2] / 'PhongThanRuntime-Loose'
report = root / 'completion/logged_missing_recovery.json'
records = json.loads(report.read_text(encoding='utf-8'))

specific = {
    'settings\\item\\phongthansetattributes.txt': ('ITEM_SET_ATTRIBUTES', 'Thuộc tính bổ sung theo SetID của trang bị; InitVngSetAttrib -> GetVngSetAttrib -> ApplyPhongThanSetAttrib. Không phải sprite/glow hay bảng tỷ lệ rơi đồ.', 'Core/Src/KBasPropTbl.CPP:264; Core/Src/KItemGenerator.CPP:43'),
    'settings\\item\\maskres.txt': ('MASK_APPEARANCE_TABLE', 'Bảng tài nguyên ngoại hình/mặt nạ g_MaskChangeRes; thiếu chỉ được ghi debug tại InitGameSetting, không có chứng cứ đây là bảng thuộc tính bộ đồ.', 'Core/Src/KCore.cpp:727'),
    'settings\\thiefskills.txt': ('LEGACY_THIEF_SKILL_TABLE', 'Cấu hình KThiefSkill (SKILL_SS_Thief / skill400), không phải bảng toàn bộ kỹ năng Dị Nhân.', 'Core/Src/KThiefSkill.h:6; Core/Src/KThiefSkill.cpp'),
    'settings\\npc\\player\\magic_level_exp.txt': ('SKILL_LEVEL_EXP_TABLE', 'KMagicLevelExp: EXP cần cho cấp kỹ năng; Init trả FALSE khi thiếu. Server vẫn READY không chứng minh tính năng này đầy đủ.', 'Core/Src/KPlayerSet.cpp:783'),
    'settings\\npc\\player\\stamina.ini': ('STAMINA_CONFIG', 'KPlayerStamina: hồi/tiêu hao thể lực khi chạy/ngồi; không phải bảng mana hoặc bình máu. Init trả FALSE khi thiếu.', 'Core/Src/KPlayerSet.cpp:940'),
    'settings\\tong\\tong_level.txt': ('GUILD_LEVEL_TABLE', 'KTongLevelAdd: EXP, hệ số và giới hạn thành viên theo cấp bang.', 'Core/Src/KTongData.cpp:98'),
    'settings\\npc\\pkset.ini': ('PK_CONFIG_WITH_NATIVE_DEFAULTS', 'Hệ số sát thương/PK/phạt và thời gian PK; KNpcSet có nhánh giá trị mặc định khi file không mở được. Không tự tạo bảng giả từ mặc định.', 'Core/Src/KNpcSet.cpp:59'),
    'ui\\ui3\\uiinformation.ini': ('LEGACY_MESSAGE_DIALOG_LAYOUT', 'Layout hộp thoại thông báo/xác nhận KUiInformation; source hiện còn compatibility dùng resource PT cho một số trường hợp.', 'GameClient/Ui/UiCase/UiInformation.cpp:19; GameClient/Ui/UiCase/UiSysMsgCentre.cpp:389'),
    'ui\\ui3\\uiinformation1.ini': ('LEGACY_MESSAGE_DIALOG_LAYOUT', 'Layout biến thể hộp thoại KUiInformation1.', 'GameClient/Ui/UiCase/UiInformation1.cpp:17'),
    'ui\\ui3\\uiselcolor.ini': ('CHAT_COLOR_MENU_LAYOUT', 'Menu chọn màu chữ chat.', 'GameClient/Ui/UiCase/UiSelColor.cpp:15'),
    'ui\\emote.ini': ('CHAT_EMOTE_PHRASES', 'Danh sách lệnh/chuỗi hành động chat và màu tên; không phải toàn bộ sprite emoji. LoadEntireEmote vẫn trả1 khi thiếu.', 'GameClient/Ui/UiChatPhrase.cpp:98'),
    'ui\\profession.ini': ('CHARACTER_SELECTION_PROFESSION_UI', 'Thông tin hệ phái ở tạo/chọn nhân vật.', 'GameClient/Ui/UiCase/UiNewPlayer.cpp:25; GameClient/Ui/UiCase/UiSelPlayer.cpp:30'),
    'script\\header\\taskid.lua': ('SERVER_LUA_TASK_CONSTANTS', 'Dependency Include của mission01/banghoi/playerlogin/task01: hằng task dùng chung. Thiếu gây các script gọi Include không nạp được; không suy đoán ID.', 'Server/script/missions/mission01.lua:1; Server/script/timertask/task01.lua:6'),
    'script\\lib\\worldlibrary.lua': ('SERVER_LUA_COMMON_LIBRARY', 'Thư viện Lua dùng chung của đăng nhập và timer; chưa có nội dung nên không thể liệt kê/API hay tái tạo an toàn.', 'Server/script/player/playerlogin.lua:11; Server/script/timertask/task01.lua:1'),
    'script\\lockipnguoichoi.lua': ('GENERATED_SERVER_IP_LOCK_STATE', 'Dữ liệu khóa IP do playerlogin.adminLock ghi bằng SaveData, rồi được Include lúc login. Là state server có thể sinh lúc chạy, không có lý do bắt buộc nằm trong client PAK. Không tạo bảng rỗng để vô hiệu hóa khóa.', 'Server/script/player/playerlogin.lua:7,120'),
}


def describe(name, status):
    p = name.lstrip('\\').lower()
    if p in specific:
        return specific[p]
    if p.startswith('spr\\recovered_by_id\\'):
        return ('ID_ONLY_VALIDATED_SPRITE', 'Sprite đúng ID yêu cầu, payload SPR được kiểm tra cấu trúc. Chưa có tên gốc hoặc dataflow tới control/item cụ thể; tên dưới spr/recovered_by_id là tên vận hành mới, không nhận là sprite item hay hiệu ứng cụ thể.', 'Engine/Src/KLooseFile.inl:63-73; log ID-only at per-process diagnostic limit')
    if name == '<ID_ONLY>':
        return ('ID_ONLY_UNRESOLVED', 'Chỉ có ID, chưa có tên/caller đầy đủ và chưa tìm thấy payload. Log chạm giới hạn4096 key/process nên dòng path tiếp theo có thể không được ghi; không đoán chức năng hay dựng đường dẫn.', 'Engine/Src/KLooseFile.inl:63-73')
    if 'npc_regions\\' in p:
        return ('OPTIONAL_AUTHORED_NPC_OVERRIDE_PROBE', 'Thăm dò bản NPC/Region_S phục hồi theo từng ô map. Đây là đường dẫn namespace compatibility mới, không phải tên trong PAK gốc; khi thiếu dùng section gốc hoặc ô rỗng (size0 trả TRUE). Không tạo spawn/geometry giả.', 'Core/Src/PhongThanRegionNpc.inl:13,37,46')
    if p.startswith('maps\\'):
        return ('MAP_REGION_PROBE', 'Dữ liệu ô map/trap/vật cản/NPC được ghép đường dẫn theo tọa độ. Nhiều ô thưa/ngoài biên có thể không tồn tại; chỉ log MISSING chưa chứng minh mất một ô bắt buộc hoặc mất spawn.', 'Core/Src/KRegion.cpp:198,208,259,316')
    if p.startswith('settings\\npcres\\'):
        return ('DYNAMIC_APPEARANCE_TABLE_VARIANT', 'Bảng part/animation/effect ghép từ tên resource, gồm các biến thể _effect/info. Tên do source sinh không bảo đảm mỗi tổ hợp đều được PT đóng gói.', 'Core/Src/KNpcResNode.cpp:218-248')
    if p.startswith('spr\\npcres\\human\\'):
        return ('HUMAN_PART_ANIMATION_SPRITE', 'Sprite từng phần nhân vật/trang bị/ngựa theo hệ/giới tính/động tác. Source ghép tổ hợp tên; tổ hợp không có trong PAK có thể là biến thể không được sản xuất hoặc contract khác, chưa đủ chứng cứ chọn một sprite khác thay thế.', 'Core/Src/KNpcRes.cpp:434,940; Core/Src/KNpcResNode.cpp:615')
    if p.endswith('.cur'):
        return ('LEGACY_CURSOR_VARIANT', 'Con trỏ theo thao tác/tương tác, gồm tên FOR2000. Chưa có chứng cứ đúng resource PT tương đương để đổi tên hoặc tạo thay thế.', 'GameClient cursor resource lookup; exact requested path recorded')
    if p.startswith('sound\\'):
        return ('SOUND_RESOURCE', 'Âm thanh client yêu cầu; cần payload đúng, không thay bằng âm khác chỉ để hết log.', 'Runtime exact path/hash; exact trigger not established')
    if status.startswith('INVALID_'):
        return ('UNREPRESENTABLE_LEGACY_PATH', 'Tên chứa ?? không hợp lệ trên Windows. Có các filename Unicode C1 trong inventory không round-trip qua ANSI hiện tại; đường dẫn runtime đã mất byte. Không thể dùng hash của tên đã hỏng làm bằng chứng tên gốc hoặc tạo file chứa dấu ?.', 'completion/*_loose_index_skipped.csv; native ANSI runtime path log')
    if p.startswith('spr\\'):
        return ('SPRITE_RESOURCE', 'Sprite UI/NPC/effect; chỉ phục hồi payload đúng ID. Chức năng chính xác của biến thể chưa được nối tới caller, không suy đoán.', 'Runtime exact requested path/hash')
    if p.startswith('settings\\item\\001\\'):
        return ('ACTIVE_ITEM_TABLE', 'Bảng item của registry001, đường dẫn ghép động từ KBasPropTbl.', 'Core/Src/KBasPropTbl.CPP:68')
    return ('UNRESOLVED_PURPOSE', 'Chưa có đủ bằng chứng xác định chức năng; giữ nguyên đường dẫn/ID để điều tra, không tạo nội dung giả.', 'Runtime exact requested path/hash')


verified = 0
for record in records:
    category, purpose, source = describe(record['path'], record['status'])
    record.update(category=category, purpose=purpose, source_contract=source)
    if record['status'] == 'RESOLVED':
        target = root / record['role'] / Path(record['path'].replace('\\', '/'))
        assert target.is_file() and b.sha(target.read_bytes()) == record['sha256']
        record['explanation'] = 'Payload tồn tại và đã phục hồi/hash-verify. Log thiếu là lịch sử trước phục hồi; không gọi đây là thiếu hiện tại.'
        verified += 1
    elif category == 'OPTIONAL_AUTHORED_NPC_OVERRIDE_PROBE':
        record['explanation'] = 'Namespace override do reconstruction thêm, nên không thể tìm tên/ID tương ứng trong PAK gốc; thiếu là fallback probe, không tự động là lỗi.'
    elif record['status'].startswith('INVALID_'):
        record['explanation'] = 'Boundary encoding/path contract; không phải bằng chứng payload gốc đã mất. Cần khôi phục tên/byte path trước.'
    elif category == 'ID_ONLY_UNRESOLVED':
        record['explanation'] = 'Boundary tên/caller và payload chưa rõ; không kết luận file bắt buộc hoặc chức năng dựa trên ID cuối log.'
    else:
        record['explanation'] = ('Không có exact-ID payload trong chain đang dùng và các PAK Client_Original hiện có. '
            'Source gọi tên không chứng minh file được phát hành trong bản PT này: có contract legacy, biến thể ghép động, dependency/state server. '
            'Client_Original thiếu resupd.pak/resupd1.pak/Resource.pak; chưa thể kết luận mọi bản gốc đều không có. Không tạo dữ liệu giả.')

# Check source-ID mapping explicitly, not just the newly assigned filename hash.
for role in ('Client', 'Server'):
    data = (root / role / 'loose_index.bin').read_bytes()
    count = struct.unpack_from('<I', data, 8)[0]
    offset = 12
    paths = {}
    for _ in range(count):
        eid, size = struct.unpack_from('<IH', data, offset)
        offset += 6
        paths[eid] = data[offset:offset+size]
        offset += size
    assert paths[0x2DF60386].lower() == b'\\settings\\item\\phongthansetattributes.txt'
report.write_text(json.dumps(records, ensure_ascii=True, indent=2), encoding='utf-8')

lines = ['# Loose: logged missing files and recovery', '',
    '## Scope and interpretation', '',
    'Log là lịch sử thăm dò, không phải danh sách toàn bộ file bắt buộc hiện còn thiếu. '
    'Mỗi request hợp lệ có path/ID, trạng thái, chức năng, source contract và lý do trong '
    '[logged_missing_recovery.json](logged_missing_recovery.json). Không phục hồi UserData/account state hoặc file test giả. '
    'Không sửa C++, Lua bytes, staging; không restart server trong bước này.', '',
    'Giới hạn log: Engine dedupe tối đa4096 key/process. Các lần chạy đã chạm giới hạn, '
    'nên inventory này chỉ bao phủ những request đã được ghi, không chứng nhận toàn bộ dependency closure. '
    'Hai ID-only ngoài bảng bộ đồ cũng được ghi trong JSON, không bỏ sót.', '',
    '## Bảng bộ đồ 2DF60386', '',
    '`Client/settings/item/PhongThanSetAttributes.txt` và `Server/settings/item/PhongThanSetAttributes.txt` '
    'là tên mới do dự án đặt theo yêu cầu, không phải tên gốc đã giải mã. '
    'Payload 5129 bytes thực sự có trong vng00.pak entry804, SHA256 '
    '`455cdcc29ef6a5c5ec9a1e7c642d0576aaa2476ff6447f1ecb47945cb46456b4`. '
    'PAK index lưu ID/offset/size/compression, không lưu đầy đủ bảng tên file; vì vậy có nội dung mà chưa biết tên. '
    'Source InitVngSetAttrib yêu cầu ID771097478/0x2DF60386, parse62 SetID cùng các cặp loại/giá trị thuộc tính, '
    'GetVngSetAttrib/ApplyPhongThanSetAttrib gắn thuộc tính bộ trang bị. Đây không phải bảng glow3/5, sprite+12 hay drop rate. '
    'Index Loose giữ ánh xạ ID gốc sang tên mới, đã xác nhận cả hai role. Không sửa source literal ID; không còn phụ thuộc PAK để đọc bảng. '
    'Tên cũ recovered_by_id/2DF60386.txt đã được chuyển, giữ bản sao trong completion/*_2DF60386_before_rename.txt.', '',
    '## Phân loại theo chức năng', '',
    '| Role | Nhóm | Trạng thái | Số đường dẫn |', '|---|---|---|---:|']
groups = Counter((r['role'], r['category'], r['status']) for r in records)
for (role, category, status), count in sorted(groups.items()):
    lines.append(f'| {role} | {category} | {status} | {count} |')
lines.extend(['', '## Giải thích các contract', ''])
seen = set()
for record in records:
    key = (record['category'], record['purpose'])
    if key in seen:
        continue
    seen.add(key)
    lines.extend(['### ' + record['category'], '', record['purpose'], '',
                  'Source: `' + record['source_contract'] + '`.', ''])
lines.extend(['## Giới hạn và bước tiếp theo', '',
    'Không invent MaskRes, taskid, thư viện Lua, sprite hay cấu hình gameplay. '
    'Không thay một file thiếu bằng file tên gần giống. Các file chỉ có trong source có thể thuộc contract khác hoặc state server; '
    'phải có bằng chứng trước khi viết lại. Các sprite phục hồi từ PAK hiện tại ghi chính xác winning entry/hash; '
    'payload chỉ có ở Client_Original được ghi HISTORICAL, không giả định là phiên bản active hiện tại. '
    'Sau khi đóng/mở client, kiểm tra log mới và ảnh hiển thị; các biến thể thiếu chưa được GAME_VERIFIED.'])
(root / 'completion/LOOSE_MISSING_FILES.md').write_text('\n'.join(lines) + '\n', encoding='utf-8')
print('Recovered payloads verified:', verified)
print('Original ID 2DF60386 -> new item path: PASS both roles')
print('Explained unique logged requests:', len(records))

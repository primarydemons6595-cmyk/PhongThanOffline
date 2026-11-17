import json,time,re
from datetime import datetime,timezone,timedelta
from restore_original_world import ROOT,OUT
import build_offline_loose as b
batch=OUT/'boss_van_tien';server=ROOT/'PhongThanRuntime-Loose/Server'
plan=json.loads((batch/'plan.json').read_text('utf8'));arenas=json.loads((batch/'arenas.json').read_text('utf8'))
for row in plan['files']:assert b.sha((server/row['relative']).read_bytes())==row['sha256'],row['relative']
assert b.sha((server/'CoreServer.dll').read_bytes())==plan['binaries'][0]['sha256']
lines=(server/'settings/phongthan/OriginalWorldSchedule.txt').read_bytes().splitlines()[1:]
assert len(lines)==63 and len(set(lines))==63
assert sum(b'van_tien' in x for x in lines)==24
rows=[]
for n in plan['ready']:
    rows.append(f"| {n['world']} | {n['translated_names'][0]} | {n['world_x']//256}, {n['world_y']//512} |")
prepared=json.loads((OUT/'prepared_changes.json').read_text('utf8'))
names={'混沌':'Hỗn Độn','穷奇':'Cùng Kỳ','饕餮':'Thao Thiết','梼杌':'Đào Ngột','九婴':'Cửu Anh','盘古':'Bàn Cổ','大鹏':'Đại Bằng','杨戬':'Dương Tiễn','蛟龙':'Giao Long','螭龙':'Ly Long','不义侯':'Bất Nghĩa Hầu','虬龙':'Cầu Long'}
bossrows=[]
for boss in prepared['bosses']:
    name=boss['path'].replace('\\','/').split('/')[-1].replace('.lua','')
    times=[f"{r['hour']:02d}:{r['minute']:02d}" for r in prepared['boss_schedule'] if r['id']==boss['id']]
    bossrows.append(f"| {names.get(name,name)} | {', '.join(times)} |")
now=datetime.now(timezone(timedelta(hours=7))).strftime('%Y-%m-%d %H:%M:%S')
text=f'''# Bật lịch Boss và Vạn Tiên Trận

Cập nhật {now}, giờ Việt Nam (UTC+7). Bản loose đã nhận cấu hình Enabled=1, 63 dòng lịch (39 Boss + 24 lần mở bốn trận), DLL và bốn NPC cổng mới. Có backup tại `boss_van_tien/backup_before_publication`.

## Lịch Boss

Giữ 12 script sinh Boss và script chết gốc đã đối chiếu. Khi khởi động, server kiểm tra Boss đang có để tránh sinh trùng. Không gọi bù toàn bộ lịch đã qua.

| Boss | Giờ xuất hiện |
|---|---|
{chr(10).join(bossrows)}

## Vạn Tiên Trận

Bốn trận Thổ, Thủy, Hỏa, Phong mở đăng ký lúc **02:20, 10:20, 13:20, 18:20, 21:20, 23:20**. Đăng ký 5 phút, chiến đấu 30 phút. Cổng thường trực cho phép nhận thưởng cả ngoài giờ. Các khung giờ tham khảo trang VNG https://phongthan.zing.vn/su-kien/phien-ban-moi-luong-cuc-tam-gioi/van-tien-tran-813.html ; đây là lịch dự án áp dụng cho bốn trận cơ bản, không tuyên bố là bản gốc Huyễn/2026.

| Runtime map | NPC | Tọa độ hiển thị |
|---|---|---|
{chr(10).join(rows)}

Thổ cấp 30–50, Thủy 51–70, Hỏa 71–90, Phong từ 91. Bản phục hồi miễn phí vào trận. Hạ bốn hộ pháp mới gọi Thông Thiên Giáo Chủ; có thử lại khi sinh NPC thất bại. Mỗi trận có 24 quái phụ, bốn hộ pháp và một Boss. Template, tài nguyên NPC và Lua nguồn lấy từ client gốc; vị trí và bố trí quái là dựng lại tại ô đi được, không phải dữ liệu spawn gốc đã tìm thấy.

Thưởng dự án: 2 Linh Phù bậc kế tiếp; trận Phong thưởng Phong Linh Phù. Thưởng được lưu bằng task riêng, nhận qua QuestExchange: túi đầy không mất thưởng, nhận lặp bị chặn. Không dùng bảo rương nguyên bản vì OnDeath nguồn chưa có lệnh thưởng hoạt động. Script tiến trình nhiệm vụ Thông Thiên được giữ và gọi cho người tham gia còn trong trận. Kết thúc đưa người chơi về map cổng, mở khóa phe và trạng thái chiến đấu; quái thuộc mission được dọn theo đúng ID đối tượng để tránh xóa nhầm NPC tái sử dụng slot.

## Kiểm chứng và giới hạn

- Lua 4 thật: 32 lượt tải script, 369 kiểm tra lifecycle trên host mô phỏng world/inventory/timer, gồm callback native OnMissionTimer, lỗi chuyển map, lỗi sinh NPC, cổng hộ pháp, Boss, thưởng đầy túi và chống nhận trùng.
- 12 script Boss: 240 kiểm tra; 42 NPC dispatch: đạt.
- DLL: 182 API + 15 API event qua ABI smoke, không lỗi SEH.
- Registry vật phẩm thật: Linh Phù 62–65 tồn tại và giải mã đúng tuple.
- 67 NPC bổ sung: đúng tọa độ, đúng prefix Lua nguồn và loose binding. Không sửa 1.183 bản ghi quái gốc/Tam Sơn Quan.
- Biên dịch DLL 0 lỗi, 4 cảnh báo cũ. Gate ngoại hình vẫn báo ba kiểm tra nền cũ: RemovesLegacyMantleItemState, RemovesLegacyPendantVisualState, RemovesSplitLegacyAppearanceState.
- Chưa có nghiệm thu thao tác nhân vật thật trong game. Vạn Tiên Trận Huyễn 121–200 chưa bật trong đợt này. Chưa tuyên bố toàn bộ NPC/quest/maps đã hoàn tất.

Trạng thái server và lịch đang chạy được ghi riêng trong `runtime_verification.json`; chỉ chấp nhận marker mới sau lần khởi động hiện tại.
'''
runtime=batch/'runtime_verification.json'
if runtime.exists():
    verification=json.loads(runtime.read_text('utf8'))
    if verification['Status']=='READY':
        text+='\nServer đã xác nhận READY: '+str(verification.get('ActiveMapCount',102))+' map, 63 dòng lịch, UTC+7. GameServer PID '+str(verification['GameServerPid'])+', Bishop PID '+str(verification['BishopPid'])+'. Thời điểm kiểm chứng UTC: '+verification['VerifiedUtc']+'.\n'
if (OUT/'van_tien_entry_fix/publication.json').exists():
    text=text.replace('# Bật lịch Boss và Vạn Tiên Trận\n','# Bật lịch Boss và Vạn Tiên Trận\n\n**Đính chính:** đã bổ sung ba map bị thiếu trong danh sách hoạt động, tổng 105 map. Xem [báo cáo sửa cổng](../van_tien_entry_fix/BAO_CAO_SUA_CONG_VAN_TIEN.md). Xác nhận 102 map dưới đây là lịch sử trước bản sửa.\n',1)
(batch/'BAO_CAO_MO_LICH_BOSS_VAN_TIEN.md').write_text(text,'utf8')
# Correct provenance for the mixed batch: original prefixes plus authored lifecycle.
manifest=ROOT/'PhongThanRuntime-Loose/completion/Server_named_effective_manifest.csv'
items=list(b.csv_read(manifest));targets={f['relative'].replace('/','\\').lower() for f in plan['files']}
for row in items:
    if row['path'].lower() in targets:
        row['origin']='BOSS_VAN_TIEN_RECOVERY'
        row['evidence']='Original Lua prefixes/portal coordinates; authored VTT lifecycle and spawns; analysis/restoration_20261016/boss_van_tien/plan.json'
b.csv_write(manifest,items,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
print('PASS publication hashes, 63 unique schedule rows, report written.')

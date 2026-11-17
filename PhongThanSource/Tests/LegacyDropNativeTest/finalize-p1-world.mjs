import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
const workspace=path.resolve(import.meta.dirname,'../../..'),report=path.join(workspace,'analysis/blue_magic_p1_20261018');
const read=n=>JSON.parse(fs.readFileSync(path.join(report,n),'utf8')),sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const iso=read('isolation.json'),world=read('world_progress.json'),canonical=read('canonical_comparison.json'),ui=read('ui_observations.json'),stop=read('final_stop.json'),fault=read('load_fault_admission_fix.json'),save=read('save_failure_20261018_120539/result.json');
const client=fs.readFileSync(path.join(report,'client_world.jsonl'),'utf8').trim().split('\n').map(JSON.parse);
if(world.acceptancePending.length||!world.p0ModuleUnchanged||!world.allObservedBluePayloadsExact||!world.snapshotThenPropertiesServerOrder||world.restoredServerPids.length<2||world.receivingClientPids.length<2||!save.PriorStateByteExact||!fault.canonicalUnchanged||stop.WorldExitCode!==0||stop.LivePid!==49460)throw Error('P1 acceptance evidence incomplete');
if(!client.some(e=>e.pid===ui.clientPid&&e.serial===101&&e.event==='CLIENT_PROPERTIES_REJECT_TYPE'))throw Error('Final Client invalid packet test missing');
const final=path.join(report,'final');
for(const directory of [iso.Server,iso.Client])for(const name of ['p1_blue_magic.jsonl','goddess_character_diag.log','gameserver_login_diag.log','core_login_diag.log','bishop_login_diag.log','item_action_diag.log','equipment_effect_trace.log']){const source=path.join(directory,name);if(fs.existsSync(source))fs.copyFileSync(source,path.join(final,(directory===iso.Server?'server_':'client_')+name));}
// Archive companions bound to both the committed result and comparison baseline.
for(const file of [world.canonical.path,path.join(report,'load_fault.before.pthc'),path.join(iso.Server,'CharacterStore/role_70316f6c647768697465.pthc')]){
 const key=sha(fs.readFileSync(file).subarray(16));for(const ext of ['.magic-v1','.required']){const from=path.join(iso.Root,key+ext);if(fs.existsSync(from))fs.copyFileSync(from,path.join(final,key+ext));}
}
const paths=['PhongThanSource/Sources/Core/Src/P0ItemMagicPersistence.h','PhongThanSource/Sources/Core/Src/P1BlueMagicTrace.h','PhongThanSource/Sources/Core/Src/P1BlueWorldTest.h','PhongThanSource/Sources/Core/Src/KItemList.cpp','PhongThanSource/Sources/Core/Src/KItemList.h','PhongThanSource/Sources/Core/Src/KPlayer.cpp','PhongThanSource/Sources/Core/Src/KPlayerDBFuns.cpp','PhongThanSource/Sources/Core/Src/KProtocolProcess.cpp','PhongThanSource/Sources/MultiServer/Bishop/Application.cpp','PhongThanSource/Sources/MultiServer/Bishop/GamePlayer.cpp','PhongThanSource/Sources/MultiServer/GameServer/KSOServer.cpp'];
const manifest=paths.map(p=>({path:p,sha256:sha(fs.readFileSync(path.join(workspace,p)))}));
for(const [dir,names] of [[iso.Server,['GameServer.exe','CoreServer.dll','Bishop.exe']],[iso.Client,['Game.exe','CoreClient.dll']]])for(const name of names){const p=path.join(dir,name);manifest.push({path:p,sha256:sha(fs.readFileSync(p))});}
fs.writeFileSync(path.join(report,'world_manifest.json'),JSON.stringify(manifest,null,2));
const acceptance={status:'PASS',phase:'P1_BLUE_MAGIC_WORLD_PERSISTENCE_AND_CLIENT_SYNCHRONIZATION',utc:new Date().toISOString(),scope:'ISOLATED_REAL_GAMESERVER_GODDESS_BISHOP_AND_GAME_EXE',worldBlueItems:1,worldWhiteItems:2,legacyWhiteItems:1,blueSerial:101,checks:{groundPickupF4:true,actualSavePlayerItemListLoadPlayerItemList:true,logoutLogin:true,gameServerProcessRestart:true,newClientProcessReceivesMagic:true,fullTypeThreeValuesExact:true,baseRequirementQualitySerialMetadataExact:true,equipUnequip:true,equippedLoginStatsFixed:true,resyncNoDuplicateNoCumulativeStats:true,invalidTypeRejectedBeforeMutation:true,saveFailurePriorCommitPreserved:true,missingCompanionLoadFailsClosed:true,missingCompanionRecoveryWithoutGatewayReset:true,legacyWhiteWorldCompatibility:true,p0HeaderUnchanged:true,liveDataNotEnabled:true},evidence:['world_progress.json','canonical_comparison.json','ui_observations.json','final_stop.json','load_fault_admission_fix.json','save_failure_20261018_120539/result.json','server_world.jsonl','client_world.jsonl','world_manifest.json'],limits:['P1 integration acceptance uses 1 blue item, not a new 500-item world stress run','500/500 belongs to the prior isolated P0 native acceptance','No global live rollout or Kingsoft parity claim']};
for(const name of ['acceptance.json','P1_BLUE_MAGIC_WORLD_REPORT.md']){const source=path.join(report,name),backup=path.join(report,'historical_'+name);if(fs.existsSync(source)&&!fs.existsSync(backup))fs.copyFileSync(source,backup);}
fs.writeFileSync(path.join(report,'acceptance.json'),JSON.stringify(acceptance,null,2));
fs.writeFileSync(path.join(report,'P1_BLUE_MAGIC_WORLD_REPORT.md'),`# P1 — BLUE MAGIC WORLD PERSISTENCE + CLIENT SYNCHRONIZATION

**Kết luận: PASS trong môi trường Loose cách ly.** Hoàn tất ngày 18/10/2026. Đã dùng GameServer, Goddess, Bishop và Game.exe thật; không dùng native fixture hoặc mock packet để thay thế nghiệm thu world. Dừng tại P1, chưa mở Blue Drop LIVE cho người chơi.

## Mẫu thử và kết quả thực tế

Account thử \`p1blueonly\`, CharacterStore và database \`${iso.AccountDatabase}\` riêng. Nhân vật \`TestDropRate\` có đúng **3 item**: trắng serial 100, xanh serial **101**, trắng serial 102. Nhân vật \`P1OldWhite\` riêng có 1 record trắng định dạng cũ, ban đầu không companion. Không sao chép nhân vật người chơi.

Item xanh là **Huyền Vũ Yêu Đái**, tuple 0/6/0, native Level **1**, requirement cấp **4**, quality **1**, durability **12**. Hai dòng native:

| TYPE | VALUE 0 | VALUE 1 | VALUE 2 | F4 |
|---|---:|---:|---:|---|
| 85 | 17 | -1 | 6 | Sinh lực +17 điểm |
| 86 | 23 | 2 | 9 | Sinh lực +23% |

Cả 20 slot TYPE + 3 VALUE được đối chiếu, gồm 18 slot rỗng. UI hiển thị đúng tên xanh, hai dòng magic và yêu cầu cấp 4: [ảnh F4](f4_blue_tooltip.png). Native Level 1 được xác nhận bằng log, không suy từ requirement.

| Ca kiểm tra | Bằng chứng / số liệu |
|---|---|
| Ground → pickup → F4 | GameServer PID 44132 tạo object thật bằng native item adapter/ObjSet; serial 101 vào inventory và Client nhận properties đầy đủ |
| SavePlayerItemList / LoadPlayerItemList | SAVE_CAPTURE / LOAD_RESTORED của GameServer thật; Goddess commit \`saved=1 result=0\` |
| Logout → login | Magic và metadata giữ nguyên theo serial; đủ 3/3 item |
| Restart GameServer → login | Serial 101 được load bởi PID 48428 và 29872, khác PID tạo ground 44132; các lần dừng trước restart ExitCode=0 |
| Tiến trình Client mới | PID 35784, 43152, 52372 nhận cùng dữ liệu; ${world.clientAppliedEvents} event áp properties cho serial 101 |
| Trang bị / tháo / resync | HP tối đa 42 → 68 → 42; resync lặp khi đang mặc giữ 68, không cộng dồn |
| Login khi đang mặc đồ | Trước sửa: HP 42; sau sửa Client: HP 68; tháo lại về 42 |
| Packet lỗi | GameServer gửi TYPE ngoài miền hợp lệ qua packet thật; Client từ chối trước mutation, sau đó resync hợp lệ thành công |
| Lỗi save | Khóa file canonical để atomic replace thất bại; Goddess trả \`saved=0 result=5\`; bản commit cũ byte-exact, bỏ khóa rồi save thành công |
| Lỗi load | Tạm giữ companion bắt buộc, giữ .required; LoadPlayerItemList từ chối, canonical không đổi; khôi phục companion và login lại cùng PID 29872, không reset gateway |
| Item trắng cũ | Record 268 byte không companion đã load ở world, hiện trong F4 quality 0, yêu cầu cấp 4; save/login/restart vẫn đủ 1/1 item |

**Đây là nghiệm thu P1 với 1 item xanh world**, không phải tuyên bố 500 item đã đi qua world. Kết quả 500/500 vẫn thuộc nghiệm thu P0 native trước đó.

## Lỗi đã sửa và implementation

1. \`KItemList::SyncItem\` gửi snapshot rồi properties trên cùng target connection. \`SyncItemMagicAttrib\` nhận optional target; bỏ lần gọi properties dư tại login STEP_ITEM_LIST. Layout và MessageType giữ nguyên. Log cho thấy mọi SERVER_PROPERTIES_SEND có SERVER_SNAPSHOT_SEND liền trước cùng PID/serial.
2. \`KProtocolProcess::s2cSyncItem\` tái sử dụng serial đã tồn tại khi resync, từ chối identity mismatch. Không tạo thêm item cho snapshot IsNew lặp.
3. \`s2cSyncMagic\` kiểm tra toàn bộ TYPE trước khi ghi; áp TYPE + 3 VALUE bằng native setter, giữ durability, refresh inventory. Với item đang mặc, gọi **UpdataCurData()** hiện có sau khi nhận properties để tính lại equipment/state theo native; không gọi ReCalcEquip cộng chồng lên chỉ số cũ. Trước đó snapshot đã Equip item catalog trước khi properties tới nên Client bỏ sót hiệu lực magic khi login. Xem [trước sửa](equipped_login_before_fix.png), [sau sửa và resync](equipped_login_after_fix.png), [tháo sau sửa](unequipped_after_fix.png).
4. \`KSwordOnLineSever::SendGameDataToClient\` gửi sự kiện LEAVING trước cleanup khi load thất bại, unbind relay, dọn player timeout và xóa player index ở caller; không gọi save inventory dở dang.
5. \`CGamePlayer::WaitForGameSvrPermit\` ghi nhận quyền sở hữu account ở GameServer trước khi chuyển permit cho Client. Trước đây chỉ ghi nhận tại ENTER nên PopAccount bỏ qua phiên load lỗi. Việc ghi nhận không gọi ConsumeMoney; bước tính tiền ENTER giữ nguyên. Bản sửa GameServer đơn lẻ chưa đủ đã được thử và ghi riêng trong load_fault_session_fix.json; nghiệm thu dùng lần sửa admission cuối trong load_fault_admission_fix.json.

Các thay đổi nằm trong source Core/Bishop/GameServer; binary chỉ copy vào root P1. Backup trước sửa nằm ở [before/](before/). Build GameServer/Bishop thành công: [build_admission_reservation.log](build_admission_reservation.log); CoreClient cuối thành công: [build_equipped_properties.log](build_equipped_properties.log). Hash source/binary nghiệm thu: [world_manifest.json](world_manifest.json). Manifest cũ là lịch sử phiên trước.

## Persistence và tương thích

\`P0ItemMagicPersistence.h\` giữ nguyên SHA-256 **97e7439081217103f222056ea793e0e8ab481521984fdacb1ccd1b56f676f106**, companion **version 1**. Không sửa item/protocol ABI, GeneratorLevel hoặc catalog. GameServer dùng lại nguyên trạng Save/Load P0 và native SetMagicAttrib.

[canonical_comparison.json](canonical_comparison.json) đối chiếu revision **102 → ${canonical.blueRole.afterRevision}**, đúng 3 serial duy nhất. Record canonical 268 byte giữ nguyên toàn bộ ngoài vị trí inventory/equip do thao tác thử; companion giữ byte-exact magic, base, requirement, quality và serial (bỏ ordinal vì thứ tự lưu có thể đổi). P0 Restore cũng kiểm tra reconstructed base/requirement trước khi chấp nhận load. Không mất/trùng item trong các ca lỗi đã chạy.

Fixture trắng cũ được tạo bằng **CharacterStore::Create** native, sao chép record trắng đã sinh hợp lệ sang nhân vật thử mới, không companion. Khi vào role mới, KItemList::Add đổi OwnerId sang ID role nhận theo hành vi native hiện có (1692869802 → 188022069); đây là thay đổi có chủ đích của fixture sao chép giữa hai role, không phải mất metadata của serial 101. Các trường canonical còn lại giữ nguyên ngoài vị trí. Mẫu trắng cũ là dữ liệu thử tương thích, không phải bản lưu người chơi thật.

## Bằng chứng có thể kiểm tra lại

- [acceptance.json](acceptance.json), [world_progress.json](world_progress.json): kết quả và số liệu; [server_world.jsonl](server_world.jsonl), [client_world.jsonl](client_world.jsonl): đối chiếu đầy đủ theo PID/sequence/serial.
- [load_fault_admission_fix.json](load_fault_admission_fix.json): hash canonical trước/sau load lỗi không đổi; [save fault](save_failure_20261018_120539/result.json): commit cũ được bảo toàn.
- [ui_observations.json](ui_observations.json): quan sát Game.exe thật và ảnh before/after; ảnh không thay cho log full 3 VALUE.
- [final/](final/): CharacterStore sau logout/dừng sạch, companion bound của kết quả và baseline, log Server/Client thô. Các script compare-p1-records.mjs, analyze-p1-world.mjs và finalize-p1-world.mjs chỉ đọc bằng chứng để kiểm tra/tổng hợp.

## Cách ly, trạng thái kết thúc và giới hạn

Root thử: \`${iso.Root}\`. Marker/root/account guard chỉ cho phép lệnh test trong môi trường này. Ground blueprint chỉ phục vụ test; không nối death hook. Nhân vật thử được tăng level 4 bằng API native để kiểm tra requirement, không đổi cấp item hoặc NPC18. Bishop có mutex riêng trong root P1 để chạy song song với Loose đang hoạt động.

Đã logout, đóng Client thử, dừng GameServer P1 PID **29872** sạch **ExitCode=0**, rồi dừng support services P1; giữ nguyên root, DB thử và mọi bản lưu/backup. [final_stop.json](final_stop.json) xác nhận server đang hoạt động PID **49460**, StartUtc **2026-10-18T02:07:49.8902934Z** và CoreServer SHA **bac0354cc1c691f35801a73eb1067a555d6b8e031299dd52aa65dde7a5d79907** không đổi.

Không bật P0 cho dữ liệu người chơi hiện tại; không sửa bản lưu đang hoạt động, Staging, Build_Fix, Client_Original, catalog, generator, RNG, droprate, spawn hoặc NPC18. Không audit lại 480 dòng magic, không làm P2–P5. Các ảnh vẫn có nền map đen của bản Client thử; sửa render map nằm ngoài nghiệm thu P1. PASS này không chứng minh Kingsoft parity và không tự cho phép rollout toàn server.
`);
console.log(JSON.stringify({status:acceptance.status,blueSerial:101,clientAppliedEvents:world.clientAppliedEvents,canonicalRevision:canonical.blueRole.afterRevision,report:path.join(report,'P1_BLUE_MAGIC_WORLD_REPORT.md')},null,2));

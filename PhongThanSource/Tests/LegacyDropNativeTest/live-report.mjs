import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
import {analyze,writeReport} from '../../Tools/LegacyDropLogAnalyzer.mjs';
const root=path.resolve(import.meta.dirname,'../../..'),report=path.join(root,'analysis/legacy_drop_20261018_live'),server=path.join(root,'PhongThanRuntime-Loose/Server');
const json=file=>JSON.parse(fs.readFileSync(file,'utf8').replace(/^\uFEFF/,''));
const hash=file=>crypto.createHash('sha256').update(fs.readFileSync(file)).digest('hex');
const log=path.join(server,'legacy_drop_debug.jsonl');const records=fs.readFileSync(log,'utf8').trim().split(/\r?\n/).map(JSON.parse),result=analyze(records);
writeReport(result,path.join(report,'runtime_analysis'));
const deployment=json(path.join(report,'deployment.json')),state=json(path.join(report,'runtime_state.json'));
const finalSha=hash(path.join(server,'CoreServer.dll'));if(finalSha!==deployment.Sha256.toLowerCase())throw Error('Published binary differs from receipt');
if(finalSha!==hash(path.join(root,'PhongThanSource/Output/Server/CoreServer.dll')))throw Error('Output binary differs from runtime');
const audit=json(path.join(root,'analysis/legacy_drop_20261017/audit.json'));let originalTables=0;
for(const t of audit.tables){if(hash(path.join(server,'settings/item/droprate',t.name))!==t.sha256)throw Error('Original INI changed: '+t.name);originalTables++;}
// Reuse input fingerprints captured by the native fixture preparation; no new
// classification/audit and no catalog rewriting.
const fixture=json(path.join(root,'PhongThanSource/Output/LegacyDropNativeTest/LooseFixture/fixture_hashes.json'));let catalogInputs=0;
for(const entry of fixture){const p=entry.path.replaceAll('\\','/');if(!p.startsWith('settings/item/')&&p!=='settings/phongthan/Npcs.txt')continue;
 if(hash(path.join(server,p))!==entry.sha256)throw Error('Native catalog/drop/NPC input changed: '+p);catalogInputs++;}
const expectedQ=path.join(root,'analysis/legacy_drop_20261017/phase2/quarantine.txt');if(hash(expectedQ)!==hash(path.join(server,'settings/phongthan/LegacyDropQuarantine.txt')))throw Error('Quarantine altered');
const unexpected=result.issues.filter(i=>!i.kind.startsWith('REVIEW_'));
const eventCounts=records.reduce((o,e)=>(o[e.event]=(o[e.event]??0)+1,o),{});
const finalPid=String(deployment.GameServerPid);
const perPid={};for(const g of result.groups){const p=perPid[g.pid]??={deaths:0,created:0,ground:0,rolled_back:0,picked:0};p.deaths+=g.deaths;p.created+=g.created;p.ground+=g.ground;p.rolled_back+=g.rolled_back;p.picked+=g.picked;}
const finalContext=result.groups.filter(g=>g.pid===finalPid),finalLive=finalContext.filter(g=>g.mode==='LIVE_TEST');
const monitorFiles=fs.readdirSync(report).filter(n=>n.endsWith('.world_monitor.log'));
const monitorStatus=monitorFiles.map(file=>({file,text:fs.readFileSync(path.join(report,file),'utf8')}));
const fatal=monitorStatus.flatMap(m=>m.text.split(/\r?\n/).filter(l=>l.includes('EXCEPTION ')&&l.includes('first=0')).map(line=>({file:m.file,line})));
const exits=monitorStatus.flatMap(m=>[...m.text.matchAll(/EXIT pid=(\d+) code=(\d+) hex=(\w+)/g)].map(x=>({file:m.file,pid:x[1],code:Number(x[2]),hex:x[3]})));
const completed=result.runs.filter(r=>r.event==='SELF_TEST_END');
const config=fs.readFileSync(path.join(server,'settings/phongthan/LegacyDropExperiment.ini'),'utf8');
const checks={originalTablesUnchanged:originalTables,catalogDropNpcFingerprintsUnchanged:catalogInputs,quarantineUnchanged:true,allSelfTestHeapsOK:completed.length>0&&completed.every(r=>r.heap==='HEAPOK'),uniqueDeathIds:result.deaths===eventCounts.NPC_DEATH,noUnexpectedAnalyzerIssues:unexpected.length===0,noUnhandledMonitorException:fatal.length===0,allCreatedAccountedForByGroundOrRollback:!result.issues.some(i=>i.kind==='CREATED_WITHOUT_GROUND'),allTestGroundCleaned:result.pickup.every(p=>p.status==='SELF_TEST_CLEANUP_NOT_PICKUP_TEST'),modeOff:state.Mode==='OFF',autoSelfTestDisabled:/^SelfTestEnabled=0\r?$/m.test(config)&&/^SelfTestCustomScript=0\r?$/m.test(config)&&/^SelfTestGeneratorFailAfter=0\r?$/m.test(config),currentWorldReady:state.Ready===true};
const status={status:Object.values(checks).every(Boolean)&&finalLive.some(g=>g.ground>0)&&finalContext.some(g=>g.mode==='SHADOW')?'EXPERIMENTAL_LIVE_TEST_AVAILABLE':'VALIDATION_IN_PROGRESS',scope:'FUNCTIONAL_TEST',parity:'KINGSOFT_PARITY_UNVERIFIED',productionGate:'CLOSED',productionAcceptance:'NOT_GRANTED',fullPlayerRuntimeAcceptance:false,playerPickup:'NOT_OBSERVED',playerQuestRuntime:'NOT_OBSERVED_SELFTEST_CALLBACKS_SUPPRESSED',checks,eventCounts,perPid,currentPid:finalPid,errors:result.errors,expectedNegativeCases:['CATALOG_LEVEL_MISSING for divisor NPC125/80','ITEM_BUDGET_EXCEEDED for repeated boss125 with MaxItems64','CUSTOM_DROP_SCRIPT_UNCLASSIFIED for a real script binding on an isolated test entity'],monitorExits:exits,unhandledExceptions:fatal,limits:['Native catalog only magic, no newly invented blue generator','Nonzero money rejected; existing Lua money remains independent','Custom DropScript blocks LIVE until classified','No parity or production approval','No live inventory/save/quest reward certification from unowned selftest']};
fs.writeFileSync(path.join(report,'acceptance.json'),JSON.stringify(status,null,2)+'\n');
const snapshot=path.join(report,'runtime_evidence.jsonl');fs.copyFileSync(log,snapshot);
const sources=['LegacyDropEngine.h','LegacyDropExecutionPolicy.h','LegacyDropNativeCatalog.h','LegacyDropItemAdapter.h','LegacyDropNativeLoader.h','LegacyDropDebugLogger.h','LegacyDropRuntime.h','LegacyDropRuntime.cpp','KNpcTemplate.h','KNpcTemplate.cpp','KNpc.cpp','KNpcSet.cpp','KPlayer.cpp','KSubWorldSet.cpp','ScriptFuns.cpp','PhongThanQuestDeathContext.inl'];
const files=[...sources.map(n=>'PhongThanSource/Sources/Core/Src/'+n),'PhongThanSource/Sources/Core/Core.dsp','PhongThanSource/Tools/LegacyDropLogAnalyzer.mjs','PhongThanSource/Tools/Deploy-LegacyDropLoose.ps1','PhongThanSource/Tools/Set-LegacyDropMode.ps1','PhongThanSource/Tools/Test-LegacyDropLoose.ps1','PhongThanRuntime-Loose/Server/CoreServer.dll','PhongThanRuntime-Loose/Server/Engine.dll','PhongThanRuntime-Loose/Server/settings/phongthan/LegacyDropExperiment.ini','PhongThanRuntime-Loose/Server/settings/phongthan/LegacyDropQuarantine.txt','analysis/legacy_drop_20261018_live/runtime_evidence.jsonl','analysis/legacy_drop_20261018_live/LIVE_TEST_GUIDE.md'];
fs.writeFileSync(path.join(report,'artifact_hashes.json'),JSON.stringify(files.map(file=>({file,sha256:hash(path.join(root,file))})),null,2)+'\n');
status.expectedNegativeCases.push('SELF_TEST_GENERATOR_FAILURE: isolated fault injection, three allocated items removed before ground');
fs.writeFileSync(path.join(report,'acceptance.json'),JSON.stringify(status,null,2)+'\n');
const sums=Object.values(perPid).reduce((o,p)=>({deaths:o.deaths+p.deaths,created:o.created+p.created,ground:o.ground+p.ground,rolled_back:o.rolled_back+p.rolled_back}),{deaths:0,created:0,ground:0,rolled_back:0});
const levelCases=[[10,10,'ALLOW','ALLOW',200],[20,10,'ALLOW','ALLOW',105],[21,10,'ALLOW','REJECT',86],[80,10,'ALLOW','REJECT',1],[80,80,'ALLOW','ALLOW',200]];
const md=`# Legacy Drop — live test và native debug logger, 2026-10-18

**${status.status}. FUNCTIONAL_TEST / KINGSOFT_PARITY_UNVERIFIED. Production gate CLOSED.**

Đã có đường thử nghiệm GameServer thật, không chỉ LoadLibrary hoặc mock. Không đổi production thành PASS. Trạng thái hiện tại xem acceptance.json và runtime_state.json; pickup/inventory và Lua reward trong phiên người chơi chưa được xác nhận bởi self-test không có người chơi.

## Kết quả thực tế

| Tiến trình / build | Death native | Item native tạo | Ground thành công | Rollback | Pickup |
|---|---:|---:|---:|---:|---:|
${Object.entries(perPid).map(([pid,p])=>`| ${pid} | ${p.deaths} | ${p.created} | ${p.ground} | ${p.rolled_back} | ${p.picked} |`).join('\n')}

Tổng ${sums.deaths} NPC chết, ${sums.created} item tạo, ${sums.ground} ground. Mỗi entity được gọi DoDeath hai lần trong harness; số NPC_DEATH vẫn bằng số spawn, không có duplicate death/item diagnostic. Đã đọc ${records.length} JSON event, ${result.parseErrors.length} lỗi JSON, ${unexpected.length} diagnostic bất thường ngoài review heuristic. ${completed.length} lần kết thúc self-test đều heap HEAPOK: ${checks.allSelfTestHeapsOK}. Ground test đã cleanup hết: ${checks.allTestGroundCleaned}. Toàn bộ shutdown/exit evidence nằm ở mục giới hạn bên dưới.

NPC2506 là quái Kind normal với Treasure1/0, boss125 dùng Treasure50/2 và npcdroprate-boss2.ini. Cả hai ngoài quarantine. Pangu82 tiếp tục bị manifest chặn. Mỗi batch thử năm cặp level đã yêu cầu. Player level trong batch là metadata mô phỏng, owner ground=-1; không đăng nhập hay sửa một nhân vật thật.

Các từ chối có chủ đích: ${JSON.stringify(result.errors)}. Chúng giữ nguyên ERROR và không được tính là Legacy drop thành công với0 item. Catalog divisor tại boss125 level80 thiếu tuple/cấp ở slot0 section20. Repeated trials boss125 worst-case2340 item vượt budget64 nên reject trước RNG. Khi custom DropScript chưa phân loại, Legacy từ chối để giữ nhánh cũ và tránh chồng hai nguồn normal drop. Budget error slot/section ghi trong bản đầu là vị trí cuối preflight, không phải khẳng định section đó hỏng.

MAIN_CATALOG đã ghi ${result.issues.filter(i=>i.kind==='REVIEW_LOW_NPC_HIGH_ITEM_TIER').length} mẫu equip tier cao hơn heuristic ceil(npcLevel/10); xem issues.csv theo death_id/serial. Đây là hệ quả có thể quan sát của giả thuyết không áp Scale, không giả mạo thành lỗi/parity Kingsoft. Divisor là phương án khác có cận thấp hơn nhưng không bao phủ boss125/80; không tự chuyển toàn hệ thống sang divisor hoặc sửa dữ liệu để ép nó đạt. Cấu hình mặc định vẫn explicit MAIN_CATALOG + SUCCESS_CAP và OFF.

## Bằng chứng native và regression

- Native policy mới:28 plan thực thi được,16 quarantine rejection và4 divisor rejection đúng dự kiến,218 item sinh/cleanup,15.242 assertions,0 failures. Chạy với Engine.dll của Loose trong fixture, không tạo world/network ở bài test này.
- Bộ ExecutionPolicy cũ:2.760 assertions/catalog Client và Server,525/525 explicit Level sinh đúng,100 magic differential samples/catalog; không thay expected cũ.
- Shared Core preflight:478 INI,15 parser rejection giữ nguyên,2.949 resolver path,1.000 draws mỗi bảng hợp lệ,0 mismatch. Không gọi đây là test world.
- DropRules hiện có:31 checks; production .inl:10 checks. Phép exhaustive1.000.000 roll của test cũ là duyệt boundary/cumulative picker, không phải chạy lại một triệu lượt Legacy simulation.
- Quest: balanced stack1.000; exchange/rollback/repeat guards; five original chains1.112 assertions; native Lua4 death progress68 assertions. Mock state API không phải bằng chứng quest/save runtime người chơi.
- Hai fixture test cũ thiếu enum item_townportal=5 trong stub, đã bổ sung đúng giá trị để compile; không bỏ assertion hoặc đổi tiêu chí.
- CoreServer/CoreClient build0 errors. Server giữ4 warning có sẵn (PDB/image lớn); không đưa bộ client binary mới vào Loose. Không sửa Engine.dll hoặc service binary.
- Logger native test: byte escaping, rotation4 bản, missing-directory failure không crash. Analyzer fixture: duplicate, missing ground, failed/pending/success pickup, rollback, tier review và so sánh CSV.

## Death chain và an toàn

CalcDamage → DoDeath guard(m_Doing/region) → CalcExp chọn owner → DeathPunish → Legacy snapshot/plan → đúng một nhánh normal → Lua DropRate/LastDamage → animation → OnDeath/DeathSelf → DelNpc hoặc revive.

OFF/SHADOW/reject trước commit giữ PtDropNpc. LIVE plan hợp lệ thay riêng PtDropNpc; MISS hoặc gate rejection đã chọn Legacy không chạy lại engine cũ. Lua quest/event không bị thay thế. Nonzero custom DropScript chặn LIVE. Snapshot template/instance/level/special/owner/party/bốn file/Treasure được chụp trước callback. Các callback được capture scalar identity và kiểm tra entity/region sau khi chạy; context log không đọc NPC đã bị DelNpc. Reset theo spawn/Revive và serial/world object ID giúp tránh nhầm slot tái sử dụng.

Native generator kiểm tra toàn plan/candidate trước RNG, tạo và validate tất cả item trước ground đầu tiên, remove item chưa commit khi thất bại. Ground dùng ObjSet.Add và SetItemBelong; pickup đi qua hàm ServerPickUpItem đang có, chỉ thêm observer/RAII kết quả. Không sửa thuật toán inventory. SHADOW không gọi ItemSet.Add từ Legacy. Logger có UTC/PID/sequence, khóa ghi và rotation; lỗi I/O trả false, không ném qua pipeline. Không tuyên bố ASan hay mọi dạng lỗi bộ nhớ đã được kiểm chứng.

## Level gate theo hệ thống

| Player / NPC | Legacy mẫu NONE (runtime metadata) | Lua Bát Quái điều kiện level, còn cần npcchr>=0 | EXP ví dụ base200 trước bonus |
|---|---|---|---:|
${levelCases.map(r=>`| ${r[0]} / ${r[1]} | ${r[2]} | ${r[3]} | ${r[4]} |`).join('\n')}

Legacy MAX_OVERLEVEL=10 là lựa chọn riêng có boundary test (ALLOW,ALLOW,REJECT,REJECT,ALLOW). NpcDropPolicy hiện có không có gate chênh level. normal.lua:285 giới hạn10 chỉ cho đoạn ThrowItem Bát Quái, không áp cho mọi gear. EXP trong KPlayer::AddSelfExp dùng abs(diff), full khi<=5, hệ số(295-19*diff)/200 khi<=15, còn lại1; còn nhánh NPC>=100, g_ExpRate, bonus và chia party riêng. Ví dụ trong bảng là phân tích source khi đủ điều kiện khác và chưa thêm bonus, không phải EXP đã cấp trong GameServer cho người chơi. Quest/event khác theo script riêng, không được gán một gate chung. Các đường thưởng này giữ nguyên.

## Dữ liệu, vận hành và giới hạn

${originalTables}/478 bảng nguyên SHA-256;${catalogInputs} fingerprint catalog/drop/NPC khớp fixture native; manifest quarantine nguyên hash. Không sửa Staging, Client_Original hoặc Build_Fix. Không convert bảng sang NpcDropPolicy, không tạo item ID/file giả, không đổi RandRate/Count/section. Production BuildProductionPlan vẫn từ chối.

Runtime hiện tại: ${JSON.stringify(state)}.

Native monitor exit đã quan sát: ${JSON.stringify(exits)}. Unhandled exception đã quan sát: ${JSON.stringify(fatal)}. Lần cập nhật đầu gặp exit chưa xác nhận; tool đã chặn copy, giữ support services. Recovery chỉ thực hiện sau khi chứng minh không có log đăng nhập mới và không có file CharacterStore ghi kể từ lúc world test khởi động; không đánh dấu lần exit chưa biết là sạch. Một lần self-test đầu còn timeout trước WORLD_READY do cold load105 map, không được tính PASS; sau đó chờ READY mới rồi chạy lại thành công. Teardown dưới monitor kéo dài và bộ nhớ giảm dần; chỉ kết luận sạch khi monitor ghi EXIT0, chưa quy nguyên nhân độ chậm cho Legacy. Không force-kill khi chờ save/stop. World bàn giao chạy bình thường, không kèm debugger.

Pickup/inventory, ownership theo người chơi/party và Lua reward trong phiên chơi thật vẫn NOT_OBSERVED. Self-test chỉ dùng ground unowned, tắt callback trên các NPC mới được spawn để test và dọn item ngay. Không tự tuyên bố đã nghiệm thu quest/save/network. Nonzero money còn unsupported, native catalog-only magic không phát minh random blue, profile không tự theo special, Scale còn giả thuyết. Không dùng những giới hạn này để chặn mọi thử nghiệm có cấu hình, nhưng cũng không cấp production PASS.

Xem [LIVE_TEST_GUIDE.md](LIVE_TEST_GUIDE.md) để bật/tắt, NPC allowlist, công thức, đường log, analyze/compare và rollback. Sau mỗi phiên chơi, chạy LegacyDropLogAnalyzer trên log mới và dùng death_id/serial để điều tra; chưa có bằng chứng thì không tự đổi policy hoặc xác suất.
`;
const finalMd=md.replace(`item tạo, ${sums.ground} ground.`,`item tạo, ${sums.ground} ground, ${sums.rolled_back} rollback trước ground.`)
 .replace('thiếu tuple/cấp ở slot0 section20.','không có cấp catalog phù hợp ở slot0 section20: medicine tuple1/0/0 tồn tại, nhưng khoảng4..8 loại hết các cấp sẵn có.')
 +'\n## Generator rollback có kiểm soát\n\nSELF_TEST_GENERATOR_FAILURE chỉ được bật cho NPC self-test mới spawn. Ba ITEM_ROLLBACK xác nhận ItemSet.Remove đã chạy trước ground; không có serial tương ứng trên đất. zero_items.csv đếm death không có Legacy ground, kể cả rollback/SHADOW/lỗi; không kết luận các nguồn thưởng độc lập cũng không rơi. SelfTestEnabled, SelfTestCustomScript và SelfTestGeneratorFailAfter đều đã tắt khi bàn giao.\n';
fs.writeFileSync(path.join(report,'LIVE_TEST_REPORT.md'),finalMd);
console.log(JSON.stringify({status:status.status,totals:sums,currentPid:finalPid,checks}));

import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {fileURLToPath} from 'node:url';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..'),workspace=path.dirname(source),root=path.join(workspace,'analysis/legacy_drop_20261017'),out=path.join(root,'phase2');
const read=p=>{const b=fs.readFileSync(p);return b.toString(b[0]===255&&b[1]===254?'utf16le':'utf8').replace(/^\uFEFF/,'');};
const sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const put=(name,text)=>fs.writeFileSync(path.join(out,name),text+'\n');
const quote=v=>'"'+String(v??'').replaceAll('"','""')+'"';
const csv=rows=>rows.map(r=>r.map(quote).join(',')).join('\n');
const classification=JSON.parse(read(path.join(out,'classification.json'))),before=JSON.parse(read(path.join(out,'before/acceptance.json')));
const audit=JSON.parse(read(path.join(root,'audit.json')));
const parseNative=(file,re)=>{const t=read(file),match=t.match(re);if(!match)throw Error('Missing native completion '+file);return match;};
const simulations=[10000,100000,1000000].map(deaths=>{const m=parseNative(path.join(root,`native_${deaths}.log`),/NATIVE_RESULT files=(\d+) rejected=(\d+) rows=(\d+) checks=(\d+) failed=(\d+)/);return {deaths,files:+m[1],rejected:+m[2],rows:+m[3],checks:+m[4],failed:+m[5]};});
function probe(file){const m=parseNative(file,/NATIVE_ITEM_RESULT tuples=(\d+) generated=(\d+) unresolved=(\d+) assertions_failed=(\d+) magic_tested=(\d+) magic_identical=(\d+)/),n=parseNative(file,/NATIVE_NPC_RESULT examined=(\d+) isolated=(\d+) allowed=(\d+) no_drop=(\d+)/),p=parseNative(file,/NATIVE_PLAN_RESULT prepared=(\d+) blocked=(\d+)/);return {tuples:+m[1],generated:+m[2],unresolved:+m[3],failed:+m[4],magicTested:+m[5],magicIdentical:+m[6],npcs:+n[1],isolated:+n[2],allowed:+n[3],noDrop:+n[4],preparedSlots:+p[1],blockedSlots:+p[2]};}
const server=probe(path.join(out,'item_build.log')),client=probe(path.join(out,'client_item_build.log'));
const coreTest=parseNative(path.join(out,'core_shared_test.log'),/CORE_SHARED_RESULT load=PASS files=(\d+) rejected=(\d+) resolved_paths=(\d+) selection_draws_per_valid_table=1000 failed=(\d+)/);
const coreLogs=['Server','Client'].map(role=>{const file=path.join(source,`Build/Logs/Core${role}-Release.log`),text=read(file),m=text.match(/Core(?:Server|Client)\.dll - (\d+) error\(s\), (\d+) warning\(s\)/);if(!m)throw Error('Incomplete Core compilation');fs.copyFileSync(file,path.join(out,`Core${role}-Release.log`));return {role,errors:+m[1],warnings:+m[2]};});
const baseline=read(path.join(out,'appearance_baseline.log'));const baselinePass=/PASS\s+19/.test(baseline);
const originalInputs=JSON.parse(read(path.join(out,'before/input_hashes.json'))),inputChanges=originalInputs.filter(f=>sha(fs.readFileSync(path.join(workspace,f.path)))!==f.sha256);
const fixtureChanges=[],auxiliarySourceChanges=[];for(const role of ['Server','Client']){const fixture=path.join(source,'Output/LegacyDropNativeTest',role==='Server'?'LooseFixture':'LooseFixture-Client'),manifest=JSON.parse(read(path.join(fixture,'fixture_hashes.json')));for(const item of manifest){const live=path.join(workspace,'PhongThanRuntime-Loose',role,item.path);if(!fs.existsSync(live)||sha(fs.readFileSync(live))!==item.sha256){const change={role,path:item.path,missing:!fs.existsSync(live)};if(/\.zip$/i.test(item.path)&&sha(fs.readFileSync(path.join(fixture,item.path)))===item.sha256)auxiliarySourceChanges.push(change);else fixtureChanges.push(change);}}}
if(classification.zip.originalPresent===false&&!auxiliarySourceChanges.some(c=>c.role==='Server'&&/droprate\.zip$/i.test(c.path)))auxiliarySourceChanges.push({role:'Server',path:'settings\\item\\droprate.zip',missing:true});
const a=read(path.join(out,'genre_Before.csv')).trim().split(/\r?\n/).slice(1).map(l=>l.split(',')),b=read(path.join(out,'genre_After.csv')).trim().split(/\r?\n/).slice(1).map(l=>l.split(','));
const beforeBinary=fs.readFileSync(path.join(out,'genre_Before.csv.bin')),afterBinary=fs.readFileSync(path.join(out,'genre_After.csv.bin')),recordSize=2240;
if(a.length!==b.length||beforeBinary.length!==a.length*recordSize||afterBinary.length!==beforeBinary.length)throw Error('Catalog regression size mismatch');
const genreChanges=[],unexpectedGenre=[];
for(let i=0;i<a.length;i++){const old=a[i],now=b[i],oldRecord=Buffer.from(beforeBinary.subarray(i*recordSize,(i+1)*recordSize)),newRecord=Buffer.from(afterBinary.subarray(i*recordSize,(i+1)*recordSize));if(oldRecord.equals(newRecord))continue;
  oldRecord.fill(0,0,80);newRecord.fill(0,0,80);oldRecord.fill(0,80,84);newRecord.fill(0,80,84);
  const change={row:i,detail:+old[2],oldGenre:+old[1],newGenre:+now[1],oldNameHex:old[3],newNameHex:now[3],rawAnchor:+now[5],rawNameHex:now[6],otherFieldsByteIdentical:oldRecord.equals(newRecord)};genreChanges.push(change);
  if(change.oldGenre!==0||change.newGenre!==6||change.oldNameHex!==change.newNameHex+'36'||change.rawAnchor!==4||change.rawNameHex!==change.oldNameHex||!change.otherFieldsByteIdentical)unexpectedGenre.push(change);
}
put('genre_diff.json',JSON.stringify({records:a.length,changed:genreChanges.length,unexpected:unexpectedGenre.length,changes:genreChanges},null,2));
const failures=[];if(inputChanges.length||fixtureChanges.length)failures.push('ORIGINAL_INPUT_CHANGED');if(simulations.some(s=>s.failed)||server.failed||client.failed)failures.push('NATIVE_ASSERTION_FAILURE');if(+coreTest[4]||coreLogs.some(c=>c.errors)||!baselinePass)failures.push('CORE_OR_BASELINE_FAILURE');if(unexpectedGenre.length)failures.push('UNEXPECTED_GENRE_CHANGE');
const npcLines=read(path.join(out,'native_npc_preflight.csv')).trim().split(/\r?\n/).slice(1).map(l=>l.split(','));const nativeBlocked=npcLines.filter(r=>r[2]==='QUARANTINED').map(r=>+r[0]).sort((a,b)=>a-b);
if(JSON.stringify(nativeBlocked)!==JSON.stringify(classification.quarantineNpcs))failures.push('QUARANTINE_NATIVE_MANIFEST_MISMATCH');
const critical=[{id:'EXECUTION_CONTRACT_INCOMPLETE',severity:'CRITICAL',status:'OPEN',file:'Sources/Core/Src/LegacyDropEngine.h',npcs:'All production drop requests',reason:'LAB row-selection policy does not implement or validate execution of MagicRate, MoneyRate/MoneyScale, NPC-derived levels/scales, Treasure or automatic slot choice; BuildProductionPlan deliberately returns PRODUCTION_POLICY_NOT_APPROVED. PARITY_UNVERIFIED alone is not the blocker.',action:'Specify and test an explicit compatible execution policy for these fields or supply original evidence. Do not enable the death hook.'}];
for(const id of failures)critical.push({id,severity:'CRITICAL',status:'OPEN',reason:'Verification failure; see logs'});
const resolvedCritical=[{id:'RNG_ZERO_RANGE',severity:'CRITICAL',status:'RESOLVED',file:'LegacyDropEngine.h',reason:'Zero bound could divide by zero; TryBelow fails without consuming RNG.'},{id:'STALE_PREPARED_PLAN',severity:'CRITICAL',status:'RESOLVED',file:'LegacyDropEngine.h',reason:'A rejected replan now invalidates the previous plan before checking quarantine.'},{id:'QUARANTINE_UNINITIALIZED',severity:'CRITICAL',status:'RESOLVED',file:'LegacyDropEngine.h',reason:'Missing, truncated, duplicate, wrong-version or malformed manifest leaves quarantine unready; all plans blocked.'},{id:'CONTROL_BYTE_PATH',severity:'CRITICAL',status:'RESOLVED',file:'LegacyDropEngine.h',reason:'Reject embedded NUL/control characters, traversal, device/drive path, wildcards and excessive length before hash/file lookup.'},{id:'TEST_TMPFILE_NULL',severity:'CRITICAL',status:'RESOLVED',file:'LegacyDropNativeTest.cpp',reason:'New negative-test fixture used tmpfile without a null guard and crashed with 0xC0000005; now uses an explicit workspace output path and checks fopen. All three final runs completed.'},...['RemovesLegacyMantleItemState','RemovesLegacyPendantVisualState','RemovesSplitLegacyAppearanceState'].map(id=>({id,severity:'CRITICAL',status:'RESOLVED',file:'Sources/Core/Src/KNpc.h',npcs:'All NPC class instances',reason:'Six declarations had zero uses across Sources/Headers; removed only dead declarations, rebuilt both Cores, all 19 item baseline checks pass. Internal KNpc layout changes; never hot-copy into a running server.'}))];
let indexDelta=null;const deltaPath=path.join(out,'loose_index_delta.json');if(fs.existsSync(deltaPath)){indexDelta=JSON.parse(read(deltaPath));resolvedCritical.push({id:'SOURCE_INDEX_CHANGED_DURING_RUN',severity:'CRITICAL',status:fixtureChanges.some(c=>c.path==='loose_index.bin')?'OPEN':'RESOLVED_REVALIDATED',file:'PhongThanRuntime-Loose/Server/loose_index.bin',reason:`Source changed outside this task: ${indexDelta.beforeCount}→${indexDelta.afterCount} entries, ${indexDelta.added.length} additions, ${indexDelta.removed.length} removals, ${indexDelta.changed.length} remaps. Native server probe was rerun from current index; NPC/slot/item outcomes must still pass. Delta retained.`});}
const knownGenreIssues=[223,240,890].map(id=>{const occurrences=[];for(const table of audit.tables)for(const section of table.sections)if(section.fields.Genre==='6'&&section.fields.Detail==='1'&&+section.fields.Particular===id)occurrences.push({file:table.name,section:section.name});const files=new Set(occurrences.map(o=>o.file));const npcs=audit.npcs.filter(n=>n.links.some(l=>files.has(l.file))).map(n=>n.id);return {group:'GENRE',id:'SCRIPT_'+id,severity:'CRITICAL',status:'RESOLVED',files:['Sources/Core/Src/KBasPropTbl.CPP','settings/item/001/magicscript.txt',...occurrences.map(o=>o.file+'#'+o.section)],npcs,reason:'Original known wrong-genre drop item; verified as part of complete 5692-record before/after comparison. Genre 0→6, fused trailing 6 removed from native name only; all remaining record bytes identical.',action:'Keep parser anchor fix, keep original catalog/drop bytes unchanged; all 308 affected records listed in genre_diff.json.'};});
const archiveWarnings=auxiliarySourceChanges.map(c=>({group:'ARCHIVE',id:c.role+'/'+c.path,severity:'WARNING',status:'VERIFIED_SNAPSHOT_PRESERVED',files:[c.path],npcs:[],reason:'Auxiliary ZIP disappeared/changed at source after snapshot. The runtime does not consume ZIP files; all 478 INI and Npcs original hashes remain checked independently.',action:'Use the preserved ZIP snapshot with matching SHA256 for recovery evidence; do not restore or overwrite the source automatically.'}));
const issues=[...classification.issues,...knownGenreIssues,...resolvedCritical,...archiveWarnings,...critical];put('issue_classification.csv',csv([['group','id','severity','status','files','npc_ids','reason','action'],...issues.map(i=>[i.group??'CODE',i.id,i.severity,i.status,(i.files??[i.file??'']).join(';'),Array.isArray(i.npcs)?i.npcs.join(';'):i.npcs??'',i.reason,i.action??''])]));
const status='BLOCKED';const summary={status,phase2FunctionalReady:false,parity:'PARITY_UNVERIFIED',deathHookChanged:false,deployed:false,criticalOpen:critical,verificationFailures:failures,before:{...before.item,badTables:15,missingReferences:58,appearanceFailures:3},after:{server,client,badTables:15,missingReferences:58,appearanceFailures:baselinePass?0:3,genreRecordsCompared:a.length,genreRecordsCorrected:genreChanges.length,unexpectedGenreChanges:unexpectedGenre.length,quarantineTables:classification.quarantineTables.length,quarantineNpcs:classification.quarantineNpcs.length},simulations,coreLogs,coreShared:{files:+coreTest[1],rejected:+coreTest[2],paths:+coreTest[3],failed:+coreTest[4]},inputChanges,fixtureChanges,auxiliarySourceChanges};
put('acceptance.json',JSON.stringify(summary,null,2));
const comparison=[['metric','before','after','interpretation'],['INI files parsed',478,478,'All files included; no row/rate/Count repair'],['Schema-rejected tables',15,15,'DATA_BLOCKED; quarantine only'],['Unresolved item tuples',10,server.unresolved,'4 resolved using exact catalog rows'],['Real generated candidates',2909,server.generated,'Additional category-zero native objects; not world drops'],['Missing file references',58,58,'No fake replacement/PAK fallback'],['Appearance baseline failures',3,baselinePass?0:3,'No test relaxation'],['Catalog records compared','not previously measured',a.length,'Full before/after native records'],['Catalog records whose genre/name correction is observed','3 known in drop tuples',genreChanges.length,'All other bytes identical'],['Native NPC snapshots examined','2949 nonempty refs',server.npcs,'All 2703 NPCs through native KTabFile'],['Test-only quarantine NPCs',0,server.isolated,'No production quarantine installed'],['Allowed NPCs with nonempty slots','not measured',server.allowed-server.noDrop,'Lab only'],['Allowed native slot plans','not measured',server.preparedSlots,'Native loader/parser; before RNG'],['Rejected slot plans','not measured',server.blockedSlots,'No draw possible after rejection'],['Parser fuzz cases',0,10000,'Per final native run'],['Original drop/NPC file hash changes',0,inputChanges.length,'Against saved pre-phase2 baseline']];
put('before_after.csv',csv(comparison));
const artifacts=['Output/Server/CoreServer.dll','Output/Client/CoreClient.dll','Output/LegacyDropNativeTest/LegacyDropNativeTest.exe','Output/LegacyDropNativeTest/NativeItemProbe.exe','Output/LegacyDropNativeTest/CoreDropPreflightTest.exe'].map(p=>({path:'PhongThanSource/'+p,sha256:sha(fs.readFileSync(path.join(source,p))),deployed:false}));put('artifact_hashes.json',JSON.stringify(artifacts,null,2));
put('PHASE2_RESOLUTION_REPORT.md',`# Legacy drop — xử lý BLOCKED giai đoạn 2, 17/10/2026

**${status}. Chưa PHASE2_FUNCTIONAL_READY; PARITY_UNVERIFIED.** Đã xử lý lỗi code và baseline có bằng chứng, cách ly dữ liệu lỗi trong fixture test. Còn thiếu hợp đồng thực thi đầy đủ các trường drop; vì thế chưa tích hợp death hook, chưa triển khai Build_Fix hoặc thay runtime Loose. Staging không được sử dụng.

## Số liệu trước / sau

${comparison.slice(1).map(r=>'- '+r[0]+': **'+r[1]+' → '+r[2]+'**. '+r[3]).join('\n')}

## Từng nhóm dữ liệu

15 bảng đều có ít nhất một bản nguồn PAK/ZIP cùng SHA256 với Loose hiện hành. Những lỗi Count, section, thiếu dấu bằng và thiếu RandRate vì vậy được xác nhận tồn tại trong dữ liệu đã phục hồi; không phải bằng chứng loader được phép sửa chúng. Giữ nguyên bytes, từ chối cả bảng; không bỏ hàng sau Count hoặc chuẩn hóa rate. Bảng không có NPC tham chiếu được ghi rõ, vẫn bị cách ly để test spawn trong tương lai không vô tình dùng.

| File | NPC liên quan | Lỗi | Mức độ / xử lý |
|---|---|---|---|
${classification.issues.filter(i=>i.group==='TABLE').map(i=>`| ${i.id} | ${i.npcs.join(', ')||'Không có trong Npcs.txt hiện hành'} | ${i.errors.join('; ')} | DATA_BLOCKED — reject toàn bảng |`).join('\n')}

Đối chiếu 22 PAK bằng native filename hash (đã kiểm tra thuật toán hash với output native), tổng 54 query gồm đường dẫn chính xác, catalog và ứng viên tên NPC CP936; trích xuất các hit bằng PakIdSetExtract. Kiểm tra central-directory và payload của ZIP gốc gồm 478 entry. Hai file qinyuanniao/shuijing không có hit đường dẫn chính xác trong các nguồn đã xét. Các ứng viên tên NPC không được tự coi là alias hoặc dùng để đổi tên. Chi tiết hash, vị trí archive và kết quả extract: archive_lookup.json, classification.json.

| Tuple | Sau xử lý | Số bảng tham chiếu | NPC liên quan |
|---|---|---:|---|
${classification.issues.filter(i=>i.group==='TUPLE').map(i=>`| ${i.id} | ${i.status} | ${new Set(i.files).size} | ${i.npcs.join(', ')||'Không có'} |`).join('\n')}

Bốn tuple (6,0,1..4) có hàng nguyên bản category=0 trong catalog Client/Server và archive. Adapter chỉ chấp nhận khi raw row, native record, SPR và khóa first-match cùng khớp; dùng generator/allocator thật, không tạo ID mới. Các alias skillbook hiện có được giữ nguyên, không bổ sung alias 1776→1786 hoặc 48→1721 bằng suy đoán.

Script 994 có trong serverlist.pak cũ nhưng không có record dùng được trong catalog hiện hành; vng00.pak ở đầu chuỗi ưu tiên là một phiên bản khác. Không trộn một hàng từ archive thấp ưu tiên vào catalog active để ép PASS. Script 2020/2021/2022 và hai book tuple chưa có ánh xạ native an toàn. Mọi bảng chứa chúng đều bị cách ly, kể cả hàng có rate 0.

58 tham chiếu thiếu: qinyuanniao=54, shuijing=4. Hai native hash là B78D12DC và 26DD4EB6; đã kiểm tra cả loose_index.bin lẫn _loose_id/XX/HASH.bin trên Loose Client và Server: không có alias hay payload fallback. LOOSE_ONLY không tự mở PAK; kết quả quét PAK ở đây chỉ là bằng chứng recovery offline. Danh sách NPC, file và mức độ cho từng trường hợp nằm trong issue_classification.csv; không gộp thành một cảnh báo chung. Không tạo file thay thế.

## Genre: phạm vi thực tế và bảo toàn dữ liệu

Báo cáo trước chỉ biết 3 item trong dữ liệu drop bị sai Genre: 223, 240, 890. So sánh toàn bộ **${a.length} native MagicScript record** từ cùng một fixture, build trước/sau đúng thay đổi điều kiện cột, phát hiện **${genreChanges.length}** record được sửa trong bộ nhớ. Tất cả phải có raw SPR anchor=4, raw tên kết thúc bằng byte 36 (chữ 6), Genre 0→6 và chỉ bỏ byte 36 bị dính vào cuối tên. So sánh nhị phân từng record: ngoài vùng genre/name, mọi byte giống hệt; unexpected changes=${unexpectedGenre.length}. Không dùng hash FNV đơn lẻ để kết luận: hai binary dump đầy đủ được so byte trực tiếp.

Đây là sửa cả lớp lỗi mất delimiter, không chỉ hardcode ba item. Không sửa magicscript.txt, item ID, RandRate hoặc dữ liệu gốc. Có genre_Before/After.csv(.bin), genre_diff.json và backup KBasPropTbl.CPP trước sửa để review. Hash ${originalInputs.length} file drop/NPC gốc thay đổi=${inputChanges.length}; dữ liệu fixture Client/Server so với nguồn thay đổi=${fixtureChanges.length}.

Kiểm tra cuối ghi nhận ${auxiliarySourceChanges.length} biến động archive phụ: ${auxiliarySourceChanges.map(c=>c.role+'/'+c.path).join(', ')||'không có'}. ZIP đã không còn/đã đổi ở nguồn sau khi chụp fixture; bản sao đã được giữ và xác minh SHA256 (${classification.zip.sha256}). Không có thao tác xóa/khôi phục archive nguồn trong công việc này. Đây là WARNING về provenance của nguồn recovery, không phải hạ mức lỗi runtime: Native loader đọc các INI đã hash, không đọc ZIP. Báo cáo giữ riêng mọi lỗi CRITICAL và vẫn BLOCKED.

${indexDelta?`Index Server thay đổi ngoài tác vụ: ${indexDelta.beforeCount}→${indexDelta.afterCount}, thêm ${indexDelta.added.length} entry bản đồ, xóa ${indexDelta.removed.length}, remap ${indexDelta.changed.length}. Đã chụp delta và chạy lại native lookup/generator/resolver với index hiện tại trước khi khóa báo cáo. Mô phỏng xác suất không cần đổi đầu vào vì SHA256 của toàn bộ 478 INI không đổi.`:''}

## Appearance baseline và build

Ba gate giữ nguyên biểu thức kiểm tra và nay PASS (19/19): loại sáu biến chỉ còn khai báo trong KNpc.h, không còn bất cứ tham chiếu nào trong Sources/Headers. Không xóa/render lại hệ appearance đang hoạt động. KNpc layout nội bộ đổi 24 byte, nên đã build lại cả CoreClient và CoreServer; không hot-copy vào process hiện hành. Không có module nguồn ngoài Core include KNpc.h qua kiểm tra source.

Build cuối: ${coreLogs.map(c=>`Core${c.role}: ${c.errors} errors / ${c.warnings} warnings`).join('; ')}. LNK4084 cảnh báo image server lớn vẫn được ghi nhận; đã LoadLibrary và chạy preflight thật thành công, chưa coi đó là test world/server lâu dài. Cảnh báo PDB debug không làm mất baseline gate. Binary chỉ ở Output; artifact_hashes.json ghi deployed=false.

## Quarantine và lỗi code

Manifest test-only có version V1: ${classification.quarantineTables.length} tên bảng (gồm hai file chưa có), ${classification.quarantineNpcs.length} NPC. Cách ly cả NPC nếu bất kỳ slot nào bị ảnh hưởng, không chỉ skip một hàng hoặc slot. Native reader kiểm tra đủ 2703 NPC qua KTabFile; tập NPC bị chặn phải khớp JSON audit độc lập. NPC không có bảng vẫn là không có bảng, không nhận drop mặc định.

Load manifest thất bại/thiếu version/trùng ID/truncated thì toàn bộ plan không được phép chạy. BuildLabPlan xóa plan cũ trước mọi kiểm tra, rồi quarantine → resolver → KPakFile load → parser → chuẩn bị thuật toán. Plan bị chặn không thể Draw, không dùng RNG hay cấp item. Quarantine chỉ tồn tại trong test, không được cài vào server đang chạy.

${resolvedCritical.map(i=>'- **'+i.id+' — '+i.severity+' / '+i.status+'**: '+i.reason).join('\n')}

## Một CRITICAL còn mở

${critical.map(i=>'- **'+i.id+'**: '+i.reason+' '+(i.action??'')).join('\n')}

Chưa chứng minh thuật toán Kingsoft chỉ dẫn tới PARITY_UNVERIFIED. Điều thực sự ngăn FUNCTIONAL_READY là engine lab mới chọn hàng, chưa thực hiện/kiểm thử chính sách item-level/scales, MagicRate, money và quyết định slot/Treasure trong một execution plan hoàn chỉnh. BuildProductionPlan trả lỗi rõ ràng thay vì bỏ qua các trường đó. Không biến khoảng trống chức năng này thành WARNING hoặc tuyên bố ready chỉ vì dữ liệu lỗi đã bị cách ly.

Chuẩn bị giai đoạn tích hợp: dùng chung native code đã test, giữ manifest và checksums fixture, hoàn thiện hợp đồng thực thi cho các trường còn mở, rồi chạy lại nghiệm thu. Chỉ sau đó mới nối NPC death và kiểm thử world/ownership/packet/pickup/persistence trong môi trường riêng từ Loose. Không yêu cầu người quản lý vào game test.
`);
put('PHASE2_TEST_REPORT.md',`# Native retest — phase 2

Trạng thái: **BLOCKED**, không có RUNTIME_PASS hoặc KINGSOFT_PARITY_VERIFIED.

| Cỡ mẫu mỗi bảng qua schema | File parse | Từ chối | Hàng xác suất | Assertions | Fail |
|---:|---:|---:|---:|---:|---:|
${simulations.map(s=>`| ${s.deaths} | ${s.files} | ${s.rejected} | ${s.rows} | ${s.checks} | ${s.failed} |`).join('\n')}

Seed=20261017. Không thay tiêu chí Bernstein đã dùng: M=50000, alpha=10^-6; cùng RNG, rate và range. Thêm quantity bins 0/1/2/≥3 với kỳ vọng Poisson-binomial; tổng kiểm định thống kê vẫn dưới 50000. Tất cả 463 bảng hợp lệ qua parser đều mô phỏng, kể cả các bảng bị quarantine do tuple: đây là test thuật toán thuần, không phải cho phép chúng tạo item. Không giảm mẫu hoặc bỏ trường hợp lỗi để có PASS.

Mỗi lần chạy có 10000 input fuzz byte ngẫu nhiên, kiểm tra heap CRT, Count quá lớn, NUL/encoding/path bất hợp lệ, RNG bound 0, prepared-plan reuse, manifest thiếu/version/trùng và duplicate death-token. 10000 Add/Remove thật ở mỗi probe, serial/metadata/heap checks đạt. Một lỗi crash của fixture tmpfile mới đã được sửa và lưu mô tả trong báo cáo resolution; ba final run đều có completion record, failures=${simulations.reduce((n,s)=>n+s.failed,0)}. Không có ASan với VC6; đây không phải chứng minh không leak hay deadlock khi chạy GameServer dài hạn.

| Data root | Tuple | Tạo ứng viên native thật | Chưa giải quyết | Assertions fail | NPC xét | NPC cách ly | Plan slot chuẩn bị | Slot chặn |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Loose Server copy | ${server.tuples} | ${server.generated} | ${server.unresolved} | ${server.failed} | ${server.npcs} | ${server.isolated} | ${server.preparedSlots} | ${server.blockedSlots} |
| Loose Client copy | ${client.tuples} | ${client.generated} | ${client.unresolved} | ${client.failed} | ${client.npcs} | ${client.isolated} | ${client.preparedSlots} | ${client.blockedSlots} |

Đây là cùng production server generator chạy trên hai bản dữ liệu; CoreClient cũng build thành công, nhưng không gọi đó là test gameplay client. Level thiếu vẫn chỉ dùng các ứng viên catalog/script level=1 cho probe; scale chưa được giả làm thuật toán gốc.

## Cùng code với candidate CoreServer

- LegacyDropEngine.h: Parse, Resolve, TryBelow/Selected, PreparedTable, Quarantine và lỗi preflight. Không có bản thuật toán thứ hai trong runner.
- LegacyDropItemAdapter.h: Map và GenerateMapped, đối chiếu raw/native catalog, gọi ItemSet.Add/Remove thật, kiểm tra identity trước khi trao item. CoreServer preflight dùng cùng adapter Map khi catalog của host đã init.
- LegacyDropNativeLoader.h: KPakFile bounded read, error trên missing/short read/oversized input.
- LegacyDropPreflight.cpp được Core.dsp build vào CoreServer, chỉ cung cấp preflight vô tác động world; không thêm drop vào KNpc::DoDeath.
- Test LoadLibrary chính DLL mới build: ${coreTest[1]} file, ${coreTest[2]} rejects khớp; 1000 draw mỗi bảng hợp lệ có cùng total và trace với test độc lập; ${coreTest[3]} đường dẫn resolver khớp; failed=${coreTest[4]}. Image-size warning không bị giấu; load/unload và các entrypoint này chạy thật trong process test riêng.

Magic array 0-vs-10 vẫn giống ở 100/100 mẫu; giữ observation, chưa nhận PASS MagicRate. Death/world/quest/Lua DelNpc sớm/packet/pickup/persistence trên GameServer thật chưa chạy vì gate functional còn CRITICAL.

Logs: native_*_run.log, item_build.log, client_item_build.log, core_shared_test.log, genre_regression.log, appearance_baseline.log, core_build.log. Chi tiết xác suất giữ tại ../probability_*.csv. Tái chạy: powershell.exe -NoProfile -ExecutionPolicy Bypass -File PhongThanSource/Tests/LegacyDropNativeTest/Run-All.ps1. Exit 2 là gate BLOCKED, exit 1 là lỗi chạy/xác minh.
`);
put('LEGACY_DROP_UNRESOLVED.md',`# Blockers còn lại

**BLOCKED; chưa PHASE2_FUNCTIONAL_READY.**

${critical.map(i=>'- '+i.severity+' / '+i.id+': '+i.reason).join('\n')}

Dữ liệu chưa phục hồi giữ DATA_BLOCKED: 15 bảng schema, 6 tuple, 58 tham chiếu tới hai file. Chúng đã cách ly trong fixture test ở 164 tên bảng / 502 NPC; không phải đã được sửa hoặc được phép dùng trong production.

Các lỗi appearance và các lỗi code an toàn nêu trong PHASE2_RESOLUTION_REPORT.md đã RESOLVED, không đổi severity thành WARNING để né nghiệm thu. Cả ba item Genre từng phát hiện nằm trong lớp 308 record đã kiểm tra toàn catalog; file dữ liệu gốc không đổi, không có biến đổi ngoài genre/name dự kiến.

PARITY_UNVERIFIED giữ nguyên. Không thay death hook, không copy Build_Fix, không restart server, không dùng Staging.
`);
const changedFiles=['Sources/Core/Src/KNpc.h','Sources/Core/Core.dsp','Sources/Core/Src/LegacyDropEngine.h','Sources/Core/Src/LegacyDropItemAdapter.h','Sources/Core/Src/LegacyDropNativeLoader.h','Sources/Core/Src/LegacyDropPreflight.cpp',...fs.readdirSync(path.join(source,'Tests/LegacyDropNativeTest')).map(f=>'Tests/LegacyDropNativeTest/'+f)];put('CHANGED_FILES.md','# Source files\n\n'+changedFiles.map(f=>'- PhongThanSource/'+f).join('\n')+'\n\nKBasPropTbl.CPP giữ thay đổi một byte từ lượt trước, có full regression dump để review; không thêm sửa dữ liệu.');
for(const [name,target] of [['LEGACY_DROP_AUDIT.md','PHASE2_RESOLUTION_REPORT.md'],['LEGACY_DROP_NATIVE_TEST_REPORT.md','PHASE2_TEST_REPORT.md'],['LEGACY_DROP_INTEGRATION_TEST_REPORT.md','LEGACY_DROP_UNRESOLVED.md'],['LEGACY_DROP_UNRESOLVED.md','LEGACY_DROP_UNRESOLVED.md']]){const p=path.join(root,name),backup=path.join(out,'before',name);if(fs.existsSync(p)&&!fs.existsSync(backup))fs.copyFileSync(p,backup);fs.writeFileSync(p,`# Cập nhật giai đoạn 2\n\nTrạng thái hiện tại: **BLOCKED / PARITY_UNVERIFIED**. Báo cáo chi tiết mới nhất: [${target}](phase2/${target}).\n\nBáo cáo lượt trước đã lưu trong phase2/before. Chưa tích hợp death hook hoặc triển khai runtime.\n`);}
fs.copyFileSync(path.join(out,'acceptance.json'),path.join(root,'acceptance.json'));fs.copyFileSync(path.join(out,'before_after.csv'),path.join(root,'legacy_drop_test_results.csv'));
console.log(JSON.stringify({status,verificationFailures:failures,server,client,genreChanges:genreChanges.length,unexpectedGenre:unexpectedGenre.length,criticalOpen:critical.map(i=>i.id)},null,2));
if(failures.length)process.exitCode=1;

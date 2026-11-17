import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..');
const folder=path.join(root,'analysis/legacy_drop_20261017/execution_policy');
const evidence=JSON.parse(fs.readFileSync(path.join(folder,'evidence.json'),'utf8'));
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const roleResults={};
for(const role of ['Server','Client']){
  const bytes=fs.readFileSync(path.join(folder,`native_${role}.log`));
  const log=bytes[0]===255&&bytes[1]===254?bytes.subarray(2).toString('utf16le'):bytes.toString('utf8');
  const summary=log.match(/NATIVE_FOCUSED_RESULT (.*)/),scale=log.match(/SCALE_DIFFERENTIAL (.*)/),lab=log.match(/LAB_RESULT (.*)/);
  if(!summary||!scale||!lab)throw Error(`Incomplete ${role} log`);
  const values=Object.fromEntries([...summary[1].matchAll(/(\w+)=(\d+)/g)].map(m=>[m[1],Number(m[2])]));
  if(values.failures||values.generated!==525||values.explicit_rows!==525||values.npc_profile_cases!==16||values.magic_cases!==100||values.checks<2700)throw Error(`Focused ${role} test failed`);
  const rows=fs.readFileSync(path.join(folder,`npc_results_${role}.csv`),'utf8').trim().split(/\r?\n/).slice(1).map(l=>l.split(','));
  if(rows.length!==16||rows.some(r=>r[2]!=='REJECT'))throw Error(`Invalid ${role} rejection evidence`);
  roleResults[role]={...values,lab:lab[1].trim(),scale:scale[1].trim(),reasons:rows.reduce((o,r)=>(o[r[3]]=(o[r[3]]??0)+1,o),{}),schemaWarningLines:log.split(/\r?\n/).filter(l=>l.includes('[ITEM][SCHEMA]')).length};
}
const audit=JSON.parse(fs.readFileSync(path.join(root,'analysis/legacy_drop_20261017/audit.json'),'utf8'));
for(const t of audit.tables)if(hash(fs.readFileSync(path.join(root,'PhongThanRuntime-Loose/Server/settings/item/droprate',t.name)))!==t.sha256)throw Error(`INI changed after test ${t.name}`);
if(hash(fs.readFileSync(path.join(root,'PhongThanRuntime-Loose/Server/settings/phongthan/Npcs.txt')))!==evidence.npcSha256)throw Error('NPC registry changed after test');
const status={status:'BLOCKED',parity:'PARITY_UNVERIFIED',critical:['EXECUTION_CONTRACT_INCOMPLETE'],functionalReady:false,experimentalModuleTests:'PASS',roles:roleResults,originalTablesUnchanged:478,productionActivated:false,deathHookChanged:false,millionDrawRun:false,remaining:['Runtime condition for profile A/B and Treasure/Treasure1 meanings unbound','Nonzero Min/MaxItemLevelScale unresolved in all 478 original tables','Positive MagicRate unresolved in all 478 original tables; nonzero money synthetic only'],memoryScope:'VC6 CRT _heapchk and allocation cleanup; no ASan, no live world'};
fs.writeFileSync(path.join(folder,'acceptance.json'),JSON.stringify(status,null,2)+'\n');
const files=['PhongThanSource/Sources/Core/Src/LegacyDropExecutionPolicy.h','PhongThanSource/Tests/LegacyDropNativeTest/ExecutionPolicyTests.cpp','PhongThanSource/Tests/LegacyDropNativeTest/Test-ExecutionPolicy.ps1','PhongThanSource/Tests/LegacyDropNativeTest/execution-policy-evidence.mjs','PhongThanSource/Tests/LegacyDropNativeTest/execution-policy-report.mjs','analysis/legacy_drop_20261017/LEGACY_DROP_EXECUTION_SPEC.md','analysis/legacy_drop_20261017/phase2/quarantine.txt',...['LegacyDropEngine.h','LegacyDropNativeLoader.h','LegacyDropItemAdapter.h','KItemGenerator.CPP','KBasPropTbl.CPP','KItem.cpp','KNpcTemplate.cpp','KNpcSet.cpp','KNpc.cpp','PhongThanWorldScriptCompat.inl','PhongThanLuaWave7.h','GameDataDef.h','ScriptFuns.cpp','Scene/SceneDataDef.h'].map(n=>`PhongThanSource/Sources/Core/Src/${n}`),...['Server','Client'].flatMap(r=>[`analysis/legacy_drop_20261017/execution_policy/native_${r}.log`,`analysis/legacy_drop_20261017/execution_policy/npc_results_${r}.csv`])];
fs.writeFileSync(path.join(folder,'artifact_hashes.json'),JSON.stringify(files.map(file=>({file,sha256:hash(fs.readFileSync(path.join(root,file)))})),null,2)+'\n');
const table=Object.entries(roleResults).map(([role,r])=>`| ${role} catalog | ${r.checks} | ${r.failures} | ${r.generated}/${r.explicit_rows} | ${r.magic_attrib_identical}/${r.magic_cases} | ${r.quarantine_rejects} |`).join('\n');
const report=`# Kết quả execution policy — 2026-10-17

**BLOCKED — PARITY_UNVERIFIED.** Test module thí nghiệm đạt; CRITICAL về hợp đồng thực thi dữ liệu gốc vẫn còn. Không hạ mức lỗi, không đổi tiêu chí nghiệm thu.

[Executable specification](../LEGACY_DROP_EXECUTION_SPEC.md) quy định từng field, bằng chứng, giả thuyết, thuật toán synthetic, điều kiện reject và kết quả mong đợi. [acceptance.json](acceptance.json) ghi trạng thái máy đọc được. Báo cáo Phase2 trước đó giữ nguyên.

## Kết quả native mới

| Fixture | Assertions | Failures | Section Level tường minh sinh đúng | Magic attributes giống nhau khi input0/1/10 | Quarantine rejection |
|---|---:|---:|---:|---:|---:|
${table}

Mỗi fixture có 16 NPC/profile case: 14 từ chối do quarantine và 2 do MAGIC_POLICY_UNVERIFIED (NPC 2506, cả hai profile). [Server](npc_results_Server.csv), [Client](npc_results_Client.csv). Không có trường hợp trả thành công với drop rỗng để che lỗi. Draw trên plan bị từ chối giữ nguyên RNG và không có intent item/tiền.

Các test xác định phân biệt Treasure=3 với hai hàng guaranteed: repeated=6 item intent, cap=2. Tổng RandRate=200/range=100 vẫn giữ nguyên hai row. Uniform 1..10 có 1.000 replay deterministic, cả 10 cấp reachable. Money synthetic kiểm tra 0, rate=1 tại threshold, guaranteed, negative, overflow INT_MAX trước RNG. Một plan synthetic gắn catalog trang bị thật được nối qua adapter cũ để tạo 2 item native và cleanup. Heap cuối cả hai lần _HEAPOK, ItemSet trở về 0. Không có ASan hay live-world test; không kết luận không còn mọi lỗi bộ nhớ.

Ba phép scale chưa được chọn: 1+20=21; 1*20=20; 1+floor(80*20/100)=17. Cả ba không tìm thấy cấp đó cho 525 tuple gốc có Level=1 trên mỗi catalog. Không clamp hoặc sửa INI để làm chúng hợp lệ. Native generator magic 0/1/10 cho cùng thuộc tính magic ở 100 mẫu mỗi catalog; không chứng minh MagicRate=1 bị bỏ qua trong Kingsoft.

Log vẫn chứa cảnh báo schema catalog có sẵn: Server ${roleResults.Server.schemaWarningLines} dòng, Client ${roleResults.Client.schemaWarningLines} dòng. Không sửa parser/catalog hoặc bỏ cảnh báo để làm test đạt. Các mẫu thuộc tính/power expression đó không được chứng nhận bởi thử nghiệm 525 section này. Client ở đây là dữ liệu Client trong executable native server, không phải GameClient runtime.

## Trước / sau

| Tiêu chí | Trước | Sau |
|---|---|---|
| CRITICAL execution contract |1|1 — chưa đủ bằng chứng để chọn thuật toán dữ liệu thật|
| Đặc tả executable theo field |Chưa có đặc tả riêng đầy đủ|Đã có, phân biệt verified / synthetic / unbound|
| Module giả thuyết cấu hình được |Chỉ chọn row độc lập/slot thủ công|Hai profile tường minh,2 Treasure modes,zero-scale level,money synthetic,budget/catalog/error gates|
| Tự suy ra profile hoặc Scale cho NPC thật |Không cho phép|Tiếp tục không cho phép|
| 478 INI gốc |Snapshot Phase2|478/478 SHA-256 giữ nguyên sau test|
| Quarantine Phase2 |502 NPC /164 tên bảng|Giữ nguyên manifest;14 focused rejection đúng|
| Million-drop simulation |Đã có kết quả trước|Không chạy lại|
| Death hook / deployment / Build_Fix |Chưa được phép|Không thực hiện|

## Điều kiện còn thiếu

1. Điều kiện runtime chọn A/B và ý nghĩa Treasure/Treasure1: chưa có source reader hoặc trace tham chiếu phân biệt hai thuật toán. Kind0, tên plus/blue/boss và special mutable không đủ chứng minh.
2. Đơn vị và công thức Min/MaxItemLevelScale, kể cả quan hệ với Level override. Cả 478 bảng gốc khác 0 ở hai Scale; miền zero-scale synthetic không bao phủ chúng.
3. Cách thực thi MagicRate1 thành thuộc tính hoặc flag có cơ sở. Money nonzero chỉ có định nghĩa synthetic, chưa có đơn vị tiền production.

Không thể cấp PHASE2_FUNCTIONAL_READY bằng cách chỉ công bố miền synthetic trong khi dữ liệu đích đều bị reject. Chưa chạy regression thực thi reward thật cho boss/quái thường/quái xanh vì chưa có policy đầy đủ. Đã thực hiện negative preflight cho chúng, giữ gate production đóng. Quarantine tương lai phải làm preflight triển khai/spawn thất bại có diagnostic; không cho phép NPC âm thầm mất drop.

Chạy lại test có trọng tâm bằng PowerShell: powershell.exe -NoProfile -ExecutionPolicy Bypass -File PhongThanSource/Tests/LegacyDropNativeTest/Test-ExecutionPolicy.ps1. Runner trả 0 khi test module đạt; xem acceptance.json để đọc trạng thái nghiệm thu BLOCKED. Không cần quản lý vào game kiểm thử.
`;
fs.writeFileSync(path.join(folder,'EXECUTION_POLICY_REPORT.md'),report);
console.log(JSON.stringify({status:status.status,experimentalModuleTests:'PASS',checks:Object.fromEntries(Object.entries(roleResults).map(([k,v])=>[k,v.checks])),critical:status.critical}));

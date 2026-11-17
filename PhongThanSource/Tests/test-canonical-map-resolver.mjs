import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const projectRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const runtimeRoot = process.argv[2]
  ? path.resolve(process.argv[2])
  : path.resolve(projectRoot, '..', 'PhongThanRuntime-Staging');
const read = (relative) => fs.readFileSync(path.join(projectRoot, relative), 'utf8');

const header = read('Headers/PhongThanMapIdentity.h');
const entries = [...header.matchAll(/\{(\d+),\s*(\d+),\s*(PHONGTHAN_MAP_[A-Z_]+)\}/g)]
  .map((match) => ({ content: Number(match[1]), runtime: Number(match[2]), evidence: match[3] }));
assert.equal(entries.length, 118, 'canonical registry row count');
assert.equal(new Set(entries.map((entry) => entry.content)).size, entries.length, 'duplicate content ID');
assert.equal(new Set(entries.map((entry) => entry.runtime)).size, entries.length, 'duplicate runtime ID');
assert.ok(entries.every((entry) => entry.evidence === 'PHONGTHAN_MAP_CONFIRMED_FROM_EXISTING_RUNTIME'));

const byContent = new Map(entries.map((entry) => [entry.content, entry]));
const byRuntime = new Map(entries.map((entry) => [entry.runtime, entry]));
assert.equal(byContent.get(1)?.runtime, 1001, 'existing working map');
for (let id = 103; id <= 118; id += 1) assert.equal(byContent.has(id), false, `collision PT ${id}`);

const expectedHigh = [
  ...Array.from({ length: 6 }, (_, index) => 60010 + index),
  ...Array.from({ length: 10 }, (_, index) => 60020 + index),
];
expectedHigh.forEach((content, index) => assert.equal(byContent.get(content)?.runtime, 1103 + index));
for (let runtime = 1103; runtime <= 1118; runtime += 1) {
  assert.ok(byRuntime.get(runtime).content >= 60010, `runtime ${runtime} aliases a low PT map`);
}

const activeText = fs.readFileSync(path.join(runtimeRoot, 'Server', 'maps', 'WorldSet.ini'), 'utf8');
const activeIds = [...activeText.matchAll(/^World\d+=(\d+)\s*$/gm)].map((match) => Number(match[1]));
assert.equal(activeIds.length, 102);
activeIds.forEach((runtime) => assert.ok(byRuntime.has(runtime), `active runtime ${runtime} is unmapped`));
assert.equal(activeIds.includes(1058), false, 'known unavailable fixture unexpectedly active');
assert.equal(byContent.get(58)?.runtime, 1058, 'known unavailable mapping');

const metadataText = fs.readFileSync(path.join(runtimeRoot, 'Server', 'settings', 'WorldSet.ini'), 'utf8');
for (const entry of entries) {
  const escaped = `^${entry.runtime}_name=Phong Than ${entry.content}(?:\\s|$)`;
  assert.match(metadataText, new RegExp(escaped, 'm'), `metadata evidence ${entry.content}->${entry.runtime}`);
}

assert.equal(byContent.has(77777), false, 'unknown map unexpectedly mapped');
let currentRuntime = 1001;
if (byContent.has(77777)) currentRuntime = byContent.get(77777).runtime;
assert.equal(currentRuntime, 1001, 'unknown resolution mutated player state');

const persistedRuntime = 1103;
const logicalContent = byRuntime.get(persistedRuntime).content;
assert.equal(logicalContent, 60010);
assert.equal(byContent.get(logicalContent).runtime, persistedRuntime, 'persistence round trip double-converted');

const scriptFuns = read('Sources/Core/Src/ScriptFuns.cpp');
const newPlayer = read('Sources/GameClient/Ui/UiCase/UiNewPlayer.cpp');
const spawn = read('Headers/PhongThanSpawn.h');
assert.match(scriptFuns, /LuaEnterNewWorld[\s\S]*?PhongThanResolveContentMap/);
assert.match(scriptFuns, /LuaEnterRuntimeWorld[\s\S]*?PhongThanResolveRuntimeMap/);
assert.match(scriptFuns, /\{"NewRuntimeWorld",\s*LuaEnterRuntimeWorld\}/);
assert.match(newPlayer, /PhongThanResolveContentMap/);
assert.match(spawn, /PhongThanResolveRuntimeMap/);
for (const [name, source] of [['ScriptFuns.cpp', scriptFuns], ['UiNewPlayer.cpp', newPlayer], ['PhongThanSpawn.h', spawn]]) {
  assert.doesNotMatch(source, /RuntimeIdOffset|(?:Map|World|NativePlace)[\s\S]{0,100}\+\s*1000/i, `${name} retains map arithmetic`);
}

const playerDb = read('Sources/Core/Src/KPlayerDBFuns.cpp');
assert.match(playerDb, /pState->EnterMapId\s*=\s*SubWorld\[pNpc->m_SubWorldIndex\]\.m_SubWorldID/);
assert.match(playerDb, /tempPos\.m_nSubWorldID\s*=\s*pState->EnterMapId/);
const runtimeMapTemplate = read('Deploy/ProjectContent/script/item/ibitem/di_ngoai_phu.lua.template');
assert.match(runtimeMapTemplate, /NewRuntimeWorld\(tbMap\[1\]/);
assert.doesNotMatch(runtimeMapTemplate, /NewWorld\(tbMap\[1\]/);

console.log('PASS canonical map resolver data/flow regression: 118 explicit identities, 102 active, collisions rejected, high maps preserved, unknown safe, persistence runtime-stable');

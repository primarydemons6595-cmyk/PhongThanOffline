import fs from 'node:fs';

const source = fs.readFileSync(new URL('../Sources/Core/Src/ScriptFuns.cpp', import.meta.url), 'latin1');
const registry = fs.readFileSync(new URL('../../analysis/repair/original_ui_pak_all/BA82AE3C.bin', import.meta.url), 'latin1').split(/\r?\n/);
const first = registry.indexOf('[Task_7]');
const last = registry.indexOf('[Task_8]');
if (first < 0 || last < 0) throw new Error('Authentic Task_7 registry is absent.');

const block = source.match(/static const char \*s_Task7Steps\[\] =[\s\S]*?\n    \};/)?.[0];
if (!block) throw new Error('Task-20 F11 reconstruction is absent.');
const rows = [...block.matchAll(/^\s*((?:"(?:[^"\\]|\\.)*"\s*)+),?$/gm)].map((match) => {
  const joined = [...match[1].matchAll(/"((?:[^"\\]|\\.)*)"/g)].map((part) => part[1]).join('');
  return Buffer.from(joined.replace(/\\x([0-9a-f]{2})/gi, (_, hex) => String.fromCharCode(Number.parseInt(hex, 16))), 'latin1');
});
if (rows.length !== 11) throw new Error(`Expected 11 reconstructed steps, got ${rows.length}.`);

for (let step = 0; step <= 10; step++) {
  const prefix = `Step_${step}=$`;
  const line = registry.slice(first, last).find((value) => value.startsWith(prefix));
  if (!line) throw new Error(`Authentic Step_${step} is absent.`);
  const expected = Buffer.from(line.slice(prefix.length), 'latin1');
  if (!rows[step].equals(expected)) throw new Error(`Step_${step} differs: ${rows[step].toString('hex')} != ${expected.toString('hex')}`);
}
if (!source.includes('nTaskId == 7 && nStep == 10) ? 2'))
  throw new Error('Task 7 completion is not projected as REA_FINISH.');
console.log('PASS task20 F11 client-contract reconstruction: 11 exact steps');

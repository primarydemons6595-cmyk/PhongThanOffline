import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import { fileURLToPath } from 'node:url';
const project = path.dirname(path.dirname(fileURLToPath(import.meta.url)));
const workspace = path.dirname(project);
let checked = 0;
for (const runtime of ['PhongThanRuntime-Loose', 'PhongThanRuntime-Staging']) {
  for (const role of ['Server', 'Client']) {
    const root = path.join(workspace, runtime, role);
    const load = name => fs.readFileSync(path.join(root, 'settings/item/001', `${name}.txt`))
      .toString('latin1').split(/\r?\n/).slice(1).map(row => row.split('\t'));
    for (const file of ['player/playerlogin.lua', 'item/camnangtanthu.lua']) {
      assert.deepEqual(fs.readFileSync(path.join(root, 'script', file)),
        fs.readFileSync(path.join(project, 'Deploy/ProjectContent/script', file)));
    }
    const bags = load('magicscript').filter(r => r[1] === '6' && r[2] === '1' && r[3] === '9173');
    assert.equal(bags.length, 1);
    const bag = bags[0];
    assert.equal(bag[14], '\\script\\item\\camnangtanthu.lua');
    for (const col of [23, 24, 25, 26, 27, 30]) assert.equal(bag[col], '0');
    for (let career = 0; career < 3; ++career) {
      for (const [name, detail] of [['armor', 2], ['helm', 7], ['belt', 6], ['boot', 5], ['pendant', 9]]) {
        const rows = load(name).filter(r => +r[2] === detail && +r[3] === 6 + career && +r[11] === 1);
        assert.equal(rows.length, 1);
        const row = rows[0];
        assert.match(row[0], /N\.v/);
        assert.equal(+row[9], 1 + career * 5);
        for (const col of [59, 60, 65]) assert.equal(row[col], '0');
        if (name === 'armor') assert.equal(row[45], '20');
        ++checked;
      }
      for (const particular of career === 0 ? [27, 28] : [28 + career]) {
        const row = load('meleeweapon').find(r => +r[3] === particular && +r[11] === 1);
        assert.ok(row); assert.match(row[0], /N\.v/);
        assert.equal(+row[9], 1000); // source resolves template marker to requested neutral series=5
        assert.equal(row[45], '10');
        assert.ok(row.slice(44, 56).some((v, i, a) => i % 2 === 0 && v === '37' && +a[i + 1] === career));
        for (const col of [59, 60, 65]) assert.equal(row[col], '0');
        ++checked;
      }
      const mount = load('horse').find(r => +r[3] === 24 + career && +r[11] === 1);
      assert.ok(mount); assert.equal(+mount[9], 1000);
      assert.equal(+mount[47], career); ++checked;
    }
    for (const detail of [0, 3]) {
      // Staging keeps potion.txt in PAK; its native lookup is checked by the
      // engine catalog probe, not by substituting the Loose copy here.
      if (!fs.existsSync(path.join(root, 'settings/item/001/potion.txt'))) continue;
      const row = load('potion').find(r => +r[2] === detail && +r[3] === 0 && +r[11] === 1);
      assert.ok(row); assert.equal(row[12], '100'); ++checked;
    }
  }
}
// Project task IDs are outside the native 4800-4999 reservation. Check direct
// VNG task calls too, rather than confusing equipment IDs with task IDs.
function scan(dir) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const file = path.join(dir, entry.name);
    if (entry.isDirectory()) scan(file);
    else if (entry.name.endsWith('.lua') && !['playerlogin.lua', 'camnangtanthu.lua'].includes(entry.name)) {
      assert.doesNotMatch(fs.readFileSync(file).toString('latin1'), /(?:GetTask|SetTask)\s*\(\s*470[012]\b/);
    }
  }
}
scan(path.join(workspace, 'PhongThanRuntime-Staging/Server/script'));
console.log(`PASS BEGINNER_GIFT_CATALOG ${checked} reward templates; four runtimes in sync; bag flags; task collisions`);

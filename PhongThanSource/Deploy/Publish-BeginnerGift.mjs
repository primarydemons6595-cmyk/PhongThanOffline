import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
const project = path.dirname(path.dirname(fileURLToPath(import.meta.url)));
const workspace = path.dirname(project);
const stamp = new Date().toISOString().replace(/[:.]/g, '-');
const backup = path.join(workspace, 'analysis', 'repair', `beginner-gift-${stamp}`);
function write(target, bytes) {
  const saved = path.join(backup, path.relative(workspace, target));
  fs.mkdirSync(path.dirname(saved), { recursive: true });
  if (fs.existsSync(target)) fs.copyFileSync(target, saved);
  fs.mkdirSync(path.dirname(target), { recursive: true });
  fs.writeFileSync(target, bytes);
}
for (const runtime of ['PhongThanRuntime-Loose', 'PhongThanRuntime-Staging']) {
  for (const role of ['Server', 'Client']) {
    const root = path.join(workspace, runtime, role);
    for (const script of ['player/playerlogin.lua', 'item/camnangtanthu.lua']) {
      write(path.join(root, 'script', script), fs.readFileSync(path.join(project, 'Deploy/ProjectContent/script', script)));
    }
    // Byte-preserving edit: Vietnamese names and CP936 SPR paths in the rest
    // of this VNG table must not be transcoded by a UTF-8 text writer.
    const target = path.join(root, 'settings/item/001/magicscript.txt');
    const text = fs.readFileSync(target).toString('latin1');
    let rows = text.split(/\r?\n/);
    let count = 0;
    const gift = Array(34).fill('');
    Object.assign(gift, {
      0: '#Tui Qua Tan Thu', 1: '6', 2: '1', 3: '9173',
      4: '\\spr\\item\\questkey\\taskobj025.spr', 5: '39', 6: '1', 7: '1',
      8: '#Chi phat o cap 1; mo tu cap 10 de nhan qua theo he.',
      10: '0', 11: '1', 12: '0', 13: '0',
      14: '\\script\\item\\camnangtanthu.lua', 15: '0', 16: '1', 17: '1',
      18: '0', 19: '0', 20: '0', 21: '0', 22: '0', 23: '0',
      24: '0', 25: '0', 26: '0', 27: '0', 28: '2', 30: '0', 31: '0'
    });
    rows = rows.map(line => {
      const columns = line.split('\t');
      if (columns[1] === '6' && columns[2] === '1' && columns[3] === '9173') {
        ++count;
        return gift.join('\t');
      }
      return line;
    });
    if (count > 1) throw new Error(`Duplicate gift identity: ${target}`);
    if (!count) { while (rows.length && !rows.at(-1)) rows.pop(); rows.push(gift.join('\t'), ''); }
    write(target, Buffer.from(rows.join('\r\n'), 'latin1'));
  }
}
console.log(`Published beginner gift to Loose/Staging Server/Client. Backup: ${backup}`);

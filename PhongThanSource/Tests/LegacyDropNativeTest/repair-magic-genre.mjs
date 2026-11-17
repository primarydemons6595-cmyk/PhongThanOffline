import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..');
const p=path.join(source,'Sources/Core/Src/KBasPropTbl.CPP');
const bytes=fs.readFileSync(p),text=bytes.toString('latin1');
const newline=text.includes('\r\n')?'\r\n':'\n';
const old='if (nCol >= 4 &&'+newline+'\t\t\t\t\tpTF->GetInteger(nRow, nCol - 3, -1, &nParsedGenre) &&';
const replacement=old.replace('nCol >= 4','nCol >= 5');
if(text.includes(replacement)){console.log('MagicScript SPR-anchor fix already present');process.exit(0);}
if(text.split(old).length!==2)throw Error('MagicScript anchor changed; review required');
const backup=path.join(path.dirname(source),'analysis/legacy_drop_20261017/KBasPropTbl.CPP.before_magic_anchor');
if(!fs.existsSync(backup))fs.writeFileSync(backup,bytes);
fs.writeFileSync(p,Buffer.from(text.replace(old,replacement),'latin1'));
console.log('Fixed minimum explicit genre anchor: SPR column 4 has the name in column 1, not a genre. Original bytes preserved.');

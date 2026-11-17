import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..');
const original=path.join(source,'Sources/Core/Src/KBasPropTbl.CPP');
let bytes=fs.readFileSync(path.join(path.dirname(source),'analysis/legacy_drop_20261017/KBasPropTbl.CPP.before_magic_anchor'),'latin1');
bytes=bytes.replace(/#include\s+"([^"\r\n]+)"/g,(line,inc)=>{const p=path.resolve(path.dirname(original),inc);return fs.existsSync(p)?`#include "${p.replaceAll('\\','/')}"`:line;});
fs.writeFileSync(path.join(source,'Output/LegacyDropNativeTest/BPTBefore.cpp'),Buffer.from(bytes,'latin1'));

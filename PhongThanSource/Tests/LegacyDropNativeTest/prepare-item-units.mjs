import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import crypto from 'node:crypto';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..');
const out=path.join(source,'Output/LegacyDropNativeTest');
fs.mkdirSync(out,{recursive:true});
const hashes=[];
// Compile unchanged production bodies needed for item generation. The omitted
// methods operate the world/combat and are never called by this offline probe.
function emit(file,transform){const p=path.join(source,'Sources/Core/Src',file),bytes=fs.readFileSync(p);let text=transform(bytes.toString('latin1'));
  text=text.replace(/#include\s+"([^"\r\n]+)"/g,(line,inc)=>{const local=path.resolve(path.dirname(p),inc);return fs.existsSync(local)?`#include "${local.replaceAll('\\','/')}"`:line;});
  fs.writeFileSync(path.join(out,file),Buffer.from(text,'latin1'));hashes.push({file,sha256:crypto.createHash('sha256').update(bytes).digest('hex')});}
// Braces-only removal is guarded by matching the expected next method boundary.
emit('KSubWorldSet.cpp',t=>{const end=t.indexOf('int KSubWorldSet::SearchWorld');if(end<0)throw Error('Subworld boundary changed');return t.slice(0,end);});
emit('KItem.cpp',t=>{const start=t.indexOf('void KItem::ApplyMagicAttribToNPC'),end=t.indexOf('#include "../../../Headers/PhongThanUpgradePower.h"',start);if(start<0||end<0)throw Error('Item method boundaries changed');return t.slice(0,start)+t.slice(end);});
fs.writeFileSync(path.join(out,'production_subset_hashes.json'),JSON.stringify(hashes,null,2));

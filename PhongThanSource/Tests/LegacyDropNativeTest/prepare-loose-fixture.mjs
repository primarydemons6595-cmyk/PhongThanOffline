import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import crypto from 'node:crypto';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..');
const role=process.argv[2]??'Server';if(!['Server','Client'].includes(role))throw Error('Invalid role');
const workspace=path.dirname(source),live=path.join(workspace,'PhongThanRuntime-Loose',role);
const fixture=path.join(source,'Output/LegacyDropNativeTest',role==='Server'?'LooseFixture':'LooseFixture-Client');
fs.mkdirSync(fixture,{recursive:true});
const hashes=[];
function copy(relative){const src=path.resolve(live,relative),dst=path.resolve(fixture,relative);if(!src.startsWith(live+path.sep)||!dst.startsWith(fixture+path.sep))throw Error('Unsafe copy');if(!fs.existsSync(src))return;
 const bytes=fs.readFileSync(src);fs.mkdirSync(path.dirname(dst),{recursive:true});fs.writeFileSync(dst,bytes);hashes.push({path:relative,sha256:crypto.createHash('sha256').update(bytes).digest('hex')});}
function tree(relative){for(const entry of fs.readdirSync(path.join(live,relative),{withFileTypes:true})){const next=path.join(relative,entry.name);if(entry.isDirectory())tree(next);else copy(next);}}
for(const file of ['package.ini','loose_only.flag','loose_index.bin'])copy(file);
for(const dir of ['settings/item','settings/phongthan','settings/powervalue'])if(fs.existsSync(path.join(live,dir)))tree(dir);
// The existing generator loads its original set table by native hash.
const index=fs.readFileSync(path.join(live,'loose_index.bin'));if(index.subarray(0,8).toString()!=='PTLOOSE1')throw Error('Unknown index');
let offset=12;for(let i=0;i<index.readUInt32LE(8);i++){const id=index.readUInt32LE(offset),len=index.readUInt16LE(offset+4);offset+=6;const name=index.subarray(offset,offset+len).toString('latin1');offset+=len;if(id===771097478)copy(name.replace(/^\\/,'').replaceAll('\\',path.sep));}
const hex=(771097478).toString(16).toUpperCase().padStart(8,'0');copy(path.join('_loose_id',hex.slice(0,2),hex+'.bin'));
fs.writeFileSync(path.join(fixture,'fixture_hashes.json'),JSON.stringify(hashes,null,2));
console.log(`LOOSE_FIXTURE copied=${hashes.length} source=${live} destination=${fixture}`);

import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..'),workspace=path.dirname(source);
const previous=path.join(workspace,'analysis/legacy_drop_20261017'),out=path.join(previous,'phase2');
fs.mkdirSync(path.join(out,'before'),{recursive:true});
for(const name of ['acceptance.json','item_probe.csv','input_hashes.json','native_references.csv','LEGACY_DROP_UNRESOLVED.md']){const dst=path.join(out,'before',name);if(!fs.existsSync(dst))fs.copyFileSync(path.join(previous,name),dst);}
const audit=JSON.parse(fs.readFileSync(path.join(previous,'audit.json'),'utf8'));
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
function nativeHash(bytes){let id=0n;for(let i=0;i<bytes.length;i++){let c=bytes[i];if(c>=65&&c<=90)c+=32;if(c>=128)c-=256;const sum=(id+BigInt((i+1)*c))&0xffffffffn;id=((sum%0x8000000bn)*0xffffffefn)&0xffffffffn;}return Number(id^0x12345678n);}
const invalid=[...new Set(fs.readFileSync(path.join(previous,'probability_10000.csv'),'utf8').split(/\r?\n/).filter(l=>l.includes(',LOADER,')).map(l=>l.split(',')[0].replaceAll('"','')))];
const queries=new Map();
function query(virtualPath,kind){const bytes=Buffer.isBuffer(virtualPath)?virtualPath:Buffer.from(virtualPath,'latin1'),hex=bytes.toString('hex');if(!queries.has(hex))queries.set(hex,{pathHex:hex,pathDisplay:new TextDecoder('gbk').decode(bytes),kind,id:nativeHash(bytes),matches:[]});}
for(const name of [...invalid,'npcdroprate-qinyuanniao.ini','npcdroprate-shuijing.ini'])query('\\settings\\item\\droprate\\'+name,'DROP');
for(const version of ['000','001'])for(const name of ['magicscript.txt','skillbook.txt','potion.txt','meleeweapon.txt'])query(`\\settings\\item\\${version}\\${name}`,'CATALOG');
query('\\settings\\phongthan\\Npcs.txt','NPC');
for(const n of audit.npcs.filter(n=>n.links.some(l=>l.exists===false))){query(Buffer.concat([Buffer.from('\\settings\\item\\droprate\\','ascii'),Buffer.from(n.rawNameHex,'hex'),Buffer.from('.ini','ascii')]),'NPC_NAME_CANDIDATE_NOT_ALIAS');}
const references=fs.readFileSync(path.join(previous,'native_references.csv'),'utf8').trim().split(/\r?\n/).slice(1);
for(const line of references.slice(0,20)){const c=line.split(',');if(nativeHash(Buffer.from(c[2],'hex'))!==Number(c[3]))throw Error('Hash disagrees with native output');}
const archives=[];
for(const name of fs.readdirSync(path.join(workspace,'Client_Original/data')).filter(n=>/\.pak$/i.test(n))){
 const full=path.join(workspace,'Client_Original/data',name),fd=fs.openSync(full,'r'),header=Buffer.alloc(32);fs.readSync(fd,header,0,32,0);
 if(header.subarray(0,4).toString()!=='PACK'){fs.closeSync(fd);archives.push({name,error:'NOT_PACK'});continue;}
 const count=header.readUInt32LE(4),offset=header.readUInt32LE(8),index=Buffer.alloc(count*16);fs.readSync(fd,index,0,index.length,offset);fs.closeSync(fd);
 const byId=new Map();for(let i=0;i<count;i++)byId.set(index.readUInt32LE(i*16),{offset:index.readUInt32LE(i*16+4),size:index.readInt32LE(i*16+8),flags:index.readUInt32LE(i*16+12)});
 const hits=[...queries.values()].filter(q=>byId.has(q.id));archives.push({name,count,hits:hits.length,indexSha256:hash(index)});
 if(!hits.length)continue;
 const dest=path.join(out,'archive_entries',name.replace(/\.pak$/i,''));fs.mkdirSync(dest,{recursive:true});
 const ids=path.join(dest,'ids.txt'),catalog=path.join(dest,'extraction.csv');fs.writeFileSync(ids,[...new Set(hits.map(q=>q.id.toString(16)))].join('\n')+'\n');
 const run=spawnSync(path.join(source,'Output/Tools/PakIdSetExtract.exe'),[full,ids,dest,catalog],{encoding:'utf8',windowsHide:true});
 fs.writeFileSync(path.join(dest,'extract.log'),`${run.stdout??''}\n${run.stderr??''}\nexit=${run.status}`);
 for(const q of hits){const payload=path.join(dest,q.id.toString(16).toUpperCase().padStart(8,'0')+'.bin');const match={archive:name,index:byId.get(q.id),extractExit:run.status};if(fs.existsSync(payload)){match.payload=path.relative(workspace,payload);match.sha256=hash(fs.readFileSync(payload));}q.matches.push(match);}
}
const looseLookup=[];
for(const role of ['Server','Client']){
 const runtime=path.join(workspace,'PhongThanRuntime-Loose',role),index=fs.readFileSync(path.join(runtime,'loose_index.bin')),entries=new Map();let position=12;
 for(let i=0;i<index.readUInt32LE(8);++i){const id=index.readUInt32LE(position),length=index.readUInt16LE(position+4);position+=6;entries.set(id,index.subarray(position,position+length).toString('latin1'));position+=length;}
 for(const q of [...queries.values()].filter(q=>/npcdroprate-(qinyuanniao|shuijing)\.ini$/.test(q.pathDisplay))){const hex=q.id.toString(16).toUpperCase().padStart(8,'0'),alias=entries.get(q.id)??null,fallback=path.join(runtime,'_loose_id',hex.slice(0,2),hex+'.bin');looseLookup.push({role,virtualPath:q.pathDisplay,id:q.id,hex,indexAlias:alias,indexAliasExists:alias?fs.existsSync(runtime+alias):false,idFallbackExists:fs.existsSync(fallback)});}
}
const result={archives,queries:[...queries.values()],looseLookup,originalClientPackage:fs.readFileSync(path.join(workspace,'Client_Original/package.ini'),'latin1')};
fs.writeFileSync(path.join(out,'archive_lookup.json'),JSON.stringify(result,null,2));
console.log(JSON.stringify({archives:archives.length,queries:queries.size,dropMissing:[...queries.values()].filter(q=>q.pathDisplay.includes('qinyuanniao')||q.pathDisplay.includes('shuijing')),extractFailures:result.queries.flatMap(q=>q.matches.filter(m=>!m.payload))},null,2));

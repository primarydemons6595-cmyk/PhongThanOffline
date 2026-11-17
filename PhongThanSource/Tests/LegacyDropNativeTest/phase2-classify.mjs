import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import zlib from 'node:zlib';
import {fileURLToPath} from 'node:url';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..'),workspace=path.dirname(source),report=path.join(workspace,'analysis/legacy_drop_20261017'),out=path.join(report,'phase2');
fs.mkdirSync(out,{recursive:true});
const audit=JSON.parse(fs.readFileSync(path.join(report,'audit.json'),'utf8')),archive=JSON.parse(fs.readFileSync(path.join(out,'archive_lookup.json'),'utf8'));
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const csv=text=>text.trim().split(/\r?\n/).map(l=>l.split(',').map(s=>s.replace(/^"|"$/g,'')));
const tableErrors=new Map();for(const row of csv(fs.readFileSync(path.join(report,'probability_10000.csv'),'utf8')).slice(1).filter(r=>r[2]==='LOADER')){if(!tableErrors.has(row[0]))tableErrors.set(row[0],[]);tableErrors.get(row[0]).push(row[7]);}
const originalTuples=csv(fs.readFileSync(path.join(out,'before/item_probe.csv'),'utf8')).slice(1).filter(r=>r[7]?.includes('UNRESOLVED')||r[7]?.includes('UNSUPPORTED'));
const currentTuples=csv(fs.readFileSync(path.join(report,'item_probe.csv'),'utf8')).slice(1);
const key=r=>r.slice(0,4).join('/'),currentByKey=new Map(currentTuples.map(r=>[key(r),r]));
const refs=name=>audit.npcs.filter(n=>n.links.some(l=>l.file===name||l.cp936.split('\\').at(-1)===name)).map(n=>n.id);
// Read the ZIP's original central names and payload bytes without extracting into runtime.
const originalZip=path.join(audit.server,'settings/item/droprate.zip'),savedZip=path.join(out,'source_archives/droprate.zip');
fs.mkdirSync(path.dirname(savedZip),{recursive:true});
if(fs.existsSync(originalZip)&&!fs.existsSync(savedZip))fs.copyFileSync(originalZip,savedZip);
if(!fs.existsSync(savedZip)){
 const fixture=path.join(source,'Output/LegacyDropNativeTest/LooseFixture/settings/item/droprate.zip');
 const previous=JSON.parse(fs.readFileSync(path.join(out,'classification.json'),'utf8'));
 if(!fs.existsSync(fixture)||hash(fs.readFileSync(fixture))!==previous.zip.sha256)throw Error('Missing ZIP with no verified snapshot');
 fs.copyFileSync(fixture,savedZip);
}
const zipFile=fs.existsSync(originalZip)?originalZip:savedZip,zip=fs.readFileSync(zipFile),zipRows=[];
let eocd=-1;for(let i=zip.length-22;i>=Math.max(0,zip.length-65557);--i)if(zip.readUInt32LE(i)===0x06054b50){eocd=i;break;}
if(eocd<0)throw Error('ZIP central directory missing');let offset=zip.readUInt32LE(eocd+16);
for(let i=0;i<zip.readUInt16LE(eocd+10);i++){if(zip.readUInt32LE(offset)!==0x02014b50)throw Error('Invalid ZIP central header');const method=zip.readUInt16LE(offset+10),size=zip.readUInt32LE(offset+20),nameLen=zip.readUInt16LE(offset+28),extra=zip.readUInt16LE(offset+30),comment=zip.readUInt16LE(offset+32),local=zip.readUInt32LE(offset+42),rawName=zip.subarray(offset+46,offset+46+nameLen);const name=new TextDecoder('gbk').decode(rawName).replaceAll('\\','/');offset+=46+nameLen+extra+comment;if(name.endsWith('/'))continue;const payloadOffset=local+30+zip.readUInt16LE(local+26)+zip.readUInt16LE(local+28),compressed=zip.subarray(payloadOffset,payloadOffset+size),bytes=method===0?compressed:method===8?zlib.inflateRawSync(compressed):null;zipRows.push({name,rawNameHex:rawName.toString('hex'),method,sha256:bytes?hash(bytes):null});}
const issues=[],quarantine=new Set();
for(const [name,errors] of tableErrors){const table=audit.tables.find(t=>t.name===name),queries=archive.queries.find(q=>q.kind==='DROP'&&q.pathDisplay.endsWith('\\'+name)),matches=queries?.matches??[],zipMatch=zipRows.filter(z=>z.name.split('/').at(-1)===name);const hasExact=matches.some(m=>m.sha256===table.sha256)||zipMatch.some(m=>m.sha256===table.sha256);
 const problem=errors.some(e=>e.startsWith('DUPLICATE_SECTION'))?'Duplicate section; identity/precedence ambiguous':errors.some(e=>e.startsWith('DUPLICATE_KEY'))?'Missing section marker inferred from repeated keys; cannot infer which row to renumber':errors.some(e=>e==='MALFORMED_LINE')?'Particular15 lacks equals sign in section 17':errors.some(e=>e.includes('RandRate'))?'Section 59 has no RandRate field':'Count differs from section numbers or a section is missing';
 issues.push({group:'TABLE',id:name,severity:'DATA_BLOCKED',status:'QUARANTINED_TEST_ONLY',files:['settings/item/droprate/'+name],npcs:refs(name),reason:problem,errors,originalByteMatch:hasExact,archiveMatches:matches,zipMatches:zipMatch,action:'Reject entire table and all NPC plans referencing it in any slot; do not edit Count, sections or rates.'});quarantine.add(name);
}
const catalogRoots=['PhongThanRuntime-Loose/Server','PhongThanRuntime-Loose/Client'];
function catalogMatches(root,genre,detail,part){const file=genre==='6'?'magicscript.txt':'skillbook.txt',full=path.join(root,'settings/item/001',file);if(!fs.existsSync(full))return [];
 return fs.readFileSync(full,'latin1').split(/\r?\n/).flatMap((line,index)=>{if(!index)return [];const c=line.split('\t'),anchor=c.findIndex(x=>/\.spr$/i.test(x));return anchor>=3&&c[anchor-2]===detail&&c[anchor-1]===part?[{file:path.relative(workspace,full),row:index+1,fieldsHex:c.slice(0,anchor+1).map(v=>Buffer.from(v,'latin1').toString('hex')),name:new TextDecoder('gbk').decode(Buffer.from(c[0],'latin1'))}]:[];});}
for(const row of originalTuples){const [g,d,p,l]=row,current=currentByKey.get(key(row)),resolved=!!current&&Number(current[6])>0;const occurrences=[];for(const table of audit.tables)for(const section of table.sections)if(section.fields.Genre===g&&section.fields.Detail===d&&section.fields.Particular===p&&(section.fields.Level??'-1')===l)occurrences.push({file:table.name,section:section.name,rate:section.fields.RandRate??null});
 const rawCatalog=catalogRoots.flatMap(root=>catalogMatches(path.join(workspace,root),g,d,p));
 // Archive rows are inspected directly because payload names are native IDs.
 const archiveRows=[];for(const q of archive.queries.filter(q=>q.kind==='CATALOG'&&q.pathDisplay.includes(g==='6'?'magicscript':'skillbook')))for(const m of q.matches){if(!m.payload)continue;const lines=fs.readFileSync(path.join(workspace,m.payload),'latin1').split(/\r?\n/);for(let i=1;i<lines.length;i++){const c=lines[i].split('\t'),a=c.findIndex(x=>/\.spr$/i.test(x));if(a>=3&&c[a-2]===d&&c[a-1]===p)archiveRows.push({archive:m.archive,virtualPath:q.pathDisplay,row:i+1,payload:m.payload});}}
 issues.push({group:'TUPLE',id:[g,d,p,l].join('/'),severity:'DATA_BLOCKED',status:resolved?'RESOLVED_EXACT_CATALOG_ADAPTER':'QUARANTINED_TEST_ONLY',files:occurrences.map(o=>o.file),occurrences,npcs:[...new Set(occurrences.flatMap(o=>refs(o.file)))].sort((a,b)=>a-b),rawCatalog,archiveRows,reason:resolved?'Existing exact category-zero catalog row now validated against production native record':'No safe native candidate in active catalog; lower-priority archives are evidence, not automatic replacement',action:resolved?'Map category-zero identity only when raw row, native record, sprite and first-match key agree':'Reject entire table/NPC plan, including zero-rate unresolved rows; no ID invention or alias guess'});
 if(!resolved)for(const o of occurrences)quarantine.add(o.file);
}
for(const name of [...new Set(audit.missing.map(m=>m.cp936.split('\\').at(-1)))]){const query=archive.queries.find(q=>q.pathDisplay.endsWith('\\'+name)),zipMatches=zipRows.filter(z=>z.name.split('/').at(-1)===name);issues.push({group:'MISSING_FILE',id:name,severity:'DATA_BLOCKED',status:'QUARANTINED_TEST_ONLY',files:['settings/item/droprate/'+name],npcs:refs(name),referenceCount:audit.missing.filter(m=>m.cp936.endsWith('\\'+name)).length,reason:'Exact filename absent in Loose Client/Server, checked original PAK indexes and ZIP',nativeHash:query?.id,archiveMatches:query?.matches??[],zipMatches,action:'No PAK fallback in LOOSE_ONLY; quarantine NPC plan before RNG; never create replacement drop file'});quarantine.add(name);}
const blockedNpcs=[...new Set([...quarantine].flatMap(refs))].sort((a,b)=>a-b);
fs.writeFileSync(path.join(out,'quarantine.txt'),'V1\n'+[...quarantine].sort().map(name=>'T \\settings\\item\\droprate\\'+name).concat(blockedNpcs.map(id=>'N '+id)).join('\n')+'\n');
fs.writeFileSync(path.join(out,'classification.json'),JSON.stringify({issues,quarantineTables:[...quarantine].sort(),quarantineNpcs:blockedNpcs,zip:{file:path.relative(workspace,zipFile),originalPath:path.relative(workspace,originalZip),originalPresent:fs.existsSync(originalZip),sha256:hash(zip),entries:zipRows}},null,2));
console.log(JSON.stringify({tables:tableErrors.size,tuplesBefore:originalTuples.length,tuplesResolved:issues.filter(i=>i.group==='TUPLE'&&i.status.startsWith('RESOLVED')).length,quarantineTables:quarantine.size,quarantineNpcs:blockedNpcs.length,zipEntries:zipRows.length},null,2));

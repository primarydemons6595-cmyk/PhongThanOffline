import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'../../..'),client=path.join(root,'Client_Original'),out=path.join(root,'analysis/legacy_drop_20261018_equipment/script2020');fs.mkdirSync(out,{recursive:true});
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
function idFor(s){let v=0;const b=Buffer.from(s.toLowerCase(),'ascii');for(let i=0;i<b.length;i++)v=Math.imul(((v+(i+1)*b.readInt8(i))>>>0)%0x8000000b,0xffffffef)>>>0;return(v^0x12345678)>>>0;}
const virtual=['\\settings\\item\\magicscript.txt',...['000','001','002','003'].map(v=>'\\settings\\item\\'+v+'\\magicscript.txt')];
const queries=virtual.map(p=>({path:p,id:idFor(p)}));fs.writeFileSync(path.join(out,'ids.txt'),queries.map(q=>q.id.toString(16)).join('\n')+'\n');
const packages=[...fs.readFileSync(path.join(client,'package.ini'),'latin1').matchAll(/^\d+=(.+)\r?$/gm)].map(m=>m[1].trim()),results=[];
for(const name of fs.readdirSync(path.join(client,'data')).filter(n=>/\.pak$/i.test(n)))if(!packages.some(p=>p.toLowerCase()===name.toLowerCase()))packages.push(name);
for(const [priority,name] of packages.entries()){
 const file=path.join(client,'data',name);if(!fs.existsSync(file)){results.push({pak:name,priority,status:'PAK_NOT_PRESENT'});continue;}
 const fd=fs.openSync(file,'r');let h=Buffer.alloc(32),idx;
 try{fs.readSync(fd,h,0,32,0);const count=h.readUInt32LE(4);if(h.toString('ascii',0,4)!=='PACK'||count>1000000)throw Error('Invalid PAK');idx=Buffer.alloc(count*16);if(fs.readSync(fd,idx,0,idx.length,h.readUInt32LE(8))!==idx.length)throw Error('Short index');}finally{fs.closeSync(fd);}
 const hits=[];for(let i=0;i<idx.length;i+=16)if(queries.some(q=>q.id===idx.readUInt32LE(i)))hits.push({id:idx.readUInt32LE(i),entry:i/16,offset:idx.readUInt32LE(i+4),size:idx.readInt32LE(i+8),stored:idx.readUInt32LE(i+12)});
 if(!hits.length){results.push({pak:name,priority,status:'NO_CATALOG_ENTRY'});continue;}
 const folder=path.join(out,path.parse(name).name);fs.mkdirSync(folder,{recursive:true});
 const r=spawnSync(path.join(root,'PhongThanSource/Output/Tools/PakIdSetExtract.exe'),[file,path.join(out,'ids.txt'),folder,path.join(folder,'catalog.csv')],{encoding:'utf8',windowsHide:true});if(r.status!==0)throw Error('Extract failed '+name);
 for(const id of [...new Set(hits.map(h=>h.id))]){
 const entries=hits.filter(h=>h.id===id);if(new Set(entries.map(e=>[e.offset,e.size,e.stored].join('/'))).size!==1)throw Error('Conflicting duplicate catalog: extract individual offsets before proceeding');
 const payload=path.join(folder,id.toString(16).toUpperCase().padStart(8,'0')+'.bin'),bytes=fs.readFileSync(payload),rows=bytes.toString('latin1').split(/\r?\n/).map(l=>l.split('\t'));
 const matches=rows.flatMap((c,i)=>{const sprite=c.findIndex(x=>/\.spr$/i.test(x));return sprite>=3&&c[sprite-2]==='1'&&['2020','2021','2022','2023'].includes(c[sprite-1])?[{row:i+1,rawHex:Buffer.from(c.join('\t'),'latin1').toString('hex'),genre:c[sprite-3],category:c[sprite-2],particular:c[sprite-1],spriteHex:Buffer.from(c[sprite],'latin1').toString('hex'),scriptHex:Buffer.from(c[14]??'','latin1').toString('hex')}]:[]});
 results.push({pak:name,priority,path:queries.find(q=>q.id===id).path,entries,sha256:hash(bytes),payload:path.relative(root,payload),matches});
 }
}
fs.writeFileSync(path.join(out,'evidence.json'),JSON.stringify({queries,results},null,2)+'\n');
const classification=JSON.parse(fs.readFileSync(path.join(root,'analysis/legacy_drop_20261017/phase2/classification.json'),'utf8'));
const registry=fs.readFileSync(path.join(root,'PhongThanRuntime-Loose/Server/settings/phongthan/Npcs.txt'),'latin1').split(/\r?\n/).map(l=>l.split('\t')),headers=registry[0];
const quarantine=fs.readFileSync(path.join(root,'PhongThanRuntime-Loose/Server/settings/phongthan/LegacyDropQuarantine.txt'),'latin1').split(/\r?\n/);
const blocked=[0,7,11,15,86].map(id=>({id,npcQuarantined:quarantine.includes('N '+id),slots:[0,1,2,3].map(slot=>{const raw=registry[id+1][headers.indexOf(slot?'DropRateFile'+slot:'DropRateFile')],name=raw?.split('\\').at(-1);return {slot,path:raw,tableQuarantined:quarantine.includes('T '+raw),reasons:classification.issues.filter(i=>i.status==='QUARANTINED_TEST_ONLY'&&i.files.some(f=>f.split(/[\\/]/).at(-1)===name)).map(i=>({kind:i.group,id:i.id,reason:i.reason,sections:i.occurrences?.filter(o=>o.file===name)}))};})}));
fs.writeFileSync(path.join(out,'blocked_npcs.json'),JSON.stringify(blocked,null,2)+'\n');
console.log(JSON.stringify(results.filter(r=>r.payload).map(r=>({pak:r.pak,path:r.path,sha256:r.sha256,matches:r.matches})),null,2));

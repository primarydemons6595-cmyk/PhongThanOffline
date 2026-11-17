import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'../../..');
const client=path.join(root,'Client_Original'),out=path.join(root,'analysis/legacy_drop_20261018_live/field_npcs/client_recovery');
fs.mkdirSync(out,{recursive:true});
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
function idFor(s){let value=0;const b=Buffer.from(s.toLowerCase(),'ascii');for(let i=0;i<b.length;i++){value=Math.imul(((value+(i+1)*b.readInt8(i))>>>0)%0x8000000b,0xffffffef)>>>0;}return (value^0x12345678)>>>0;}
const paths=['\\settings\\npcs.txt','\\settings\\phongthan\\npcs.txt'];
const ids=paths.map(p=>({path:p,id:idFor(p)}));
fs.writeFileSync(path.join(out,'ids.txt'),ids.map(x=>x.id.toString(16)).join('\n')+'\n');
const packageBytes=fs.readFileSync(path.join(client,'package.ini'));
const packages=[...packageBytes.toString('latin1').matchAll(/^\d+=(.+)\r?$/gm)].map(m=>m[1].trim());
const names=JSON.parse(fs.readFileSync(path.join(root,'analysis/monster_ui_20261017/template_names.json'),'utf8')).names;
const wanted=[0,7,18,15,11,86],columns=['Treasure','Treasure1','DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3','DeathScript'];
const results=[];
for(const [priority,name] of packages.entries()){
 const pak=path.join(client,'data',name);if(!fs.existsSync(pak)){results.push({pak:name,priority,status:'PAK_MISSING'});continue;}
 const fd=fs.openSync(pak,'r');let header=Buffer.alloc(32),index;
 try{fs.readSync(fd,header,0,32,0);if(header.toString('ascii',0,4)!=='PACK')throw Error('Invalid PAK '+name);
 const count=header.readUInt32LE(4),offset=header.readUInt32LE(8);if(count>1000000)throw Error('Index bound');
 index=Buffer.alloc(count*16);if(fs.readSync(fd,index,0,index.length,offset)!==index.length)throw Error('Short index');
 }finally{fs.closeSync(fd);}
 const hits=[];for(let i=0;i<index.length;i+=16){const id=index.readUInt32LE(i);if(ids.some(x=>x.id===id))hits.push({id,entry:i/16,offset:index.readUInt32LE(i+4),size:index.readInt32LE(i+8),compressed:index.readUInt32LE(i+12)});}
 if(!hits.length){results.push({pak:name,priority,status:'NO_NPC_ENTRY',indexSha256:hash(index)});continue;}
 const folder=path.join(out,path.parse(name).name);fs.mkdirSync(folder,{recursive:true});
 const extract=spawnSync(path.join(root,'PhongThanSource/Output/Tools/PakIdSetExtract.exe'),[pak,path.join(out,'ids.txt'),folder,path.join(folder,'catalog.csv')],{encoding:'utf8',windowsHide:true});
 if(extract.status!==0)throw Error('Native extraction failed '+name+' '+extract.stdout+' '+extract.stderr);
 for(const hit of hits){const payload=path.join(folder,hit.id.toString(16).toUpperCase().padStart(8,'0')+'.bin'),bytes=fs.readFileSync(payload);
 const rows=bytes.toString('latin1').split(/\r?\n/).filter(l=>l.trim()).map(l=>l.split('\t')),h=rows.shift();
 const selected=wanted.map(id=>{const expected=names.find(n=>n.template===id),r=rows[id];
  if(!r)return {id,status:'ROW_MISSING'};
  const nameHex=Buffer.from(r[0],'latin1').toString('hex');return {id,nameHex,identityMatches:nameHex===expected.raw_hex,fields:Object.fromEntries(columns.map(k=>[k,h.includes(k)?r[h.indexOf(k)]??'':null]))};});
 results.push({pak:name,priority,status:'EXTRACTED',logicalPath:ids.find(x=>x.id===hit.id).path,...hit,indexSha256:hash(index),sha256:hash(bytes),bytes:bytes.length,payload:path.relative(root,payload),selected});
 }
}
const evidence={scope:'READ_ONLY_CLIENT_ORIGINAL_BINDING_SEARCH',packageSha256:hash(packageBytes),paths:ids,results};
fs.writeFileSync(path.join(out,'evidence.json'),JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify(results.filter(r=>r.status==='EXTRACTED'),null,2));

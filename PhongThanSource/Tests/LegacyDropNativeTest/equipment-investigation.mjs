import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..'),out=path.join(root,'analysis/legacy_drop_20261018_equipment');fs.mkdirSync(out,{recursive:true});
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const input=path.join(root,'PhongThanRuntime-Loose/Server/legacy_drop_debug.jsonl');
const es=[4,3,2,1,0].flatMap(n=>{const f=input+(n?'.'+n:'');return fs.existsSync(f)?fs.readFileSync(f,'utf8').trim().split(/\r?\n/).filter(Boolean).map(JSON.parse):[]});
const ids=new Set(es.filter(e=>e.event==='NPC_DEATH'&&e.template==='18'&&e.npc_level==='20').map(e=>e.death_id));
const equipFiles=['meleeweapon','rangeweapon','armor','ring','amulet','boot','belt','helm','cuff','pendant','horse'];
const catalogs=Object.fromEntries(equipFiles.map((f,i)=>[i,fs.readFileSync(path.join(root,'PhongThanRuntime-Loose/Server/settings/item/001',f+'.txt'),'latin1').split(/\r?\n/).slice(1).filter(Boolean).map((l,row)=>({file:f+'.txt',row:row+2,cells:l.split('\t')}))]));
const items=es.filter(e=>ids.has(e.death_id)&&e.event==='ITEM_CREATE'&&e.genre==='0').map(e=>{
 const catalog=(catalogs[+e.detail]??[]).filter(r=>r.cells[3]===e.particular&&r.cells[11]===e.level);
 return {event:e,catalog:catalog.map(r=>({file:r.file,row:r.row,nameHex:Buffer.from(r.cells[0],'latin1').toString('hex'),level:r.cells[11],requirements:Array.from({length:6},(_,i)=>r.cells.slice(44+i*2,46+i*2))})),chain:es.filter(x=>x.death_id===e.death_id&&['NPC_DEATH','DROP_PROFILE','ITEM_LEVEL','ITEM_CREATE','GROUND_SPAWN','ITEM_PICKUP'].includes(x.event))};});
fs.writeFileSync(path.join(out,'before_items.json'),JSON.stringify({capturedUtc:new Date().toISOString(),logSha256:hash(fs.readFileSync(input)),items},null,2)+'\n');
fs.copyFileSync(input,path.join(out,'before.jsonl'));
console.log(JSON.stringify({equipment:items.length,level10:items.filter(i=>i.event.level==='10').map(i=>({death_id:i.event.death_id,tuple:[i.event.genre,i.event.detail,i.event.particular],serial:i.event.serial,quality:i.event.quality,magic_lines:i.event.magic_lines,catalog:i.catalog,chain:i.chain.filter(e=>e.event==='ITEM_LEVEL'||e.event==='GROUND_SPAWN')}))},null,2));

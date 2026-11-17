import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..');
const base=path.join(root,'analysis/legacy_drop_20261017');
const out=path.join(base,'execution_policy');fs.mkdirSync(out,{recursive:true});
const a=JSON.parse(fs.readFileSync(path.join(base,'audit.json'),'utf8'));
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const npcFile=path.join(root,'PhongThanRuntime-Loose/Server/settings/phongthan/Npcs.txt');
const npcBytes=fs.readFileSync(npcFile),npcRows=npcBytes.toString('latin1').split(/\r?\n/).map(l=>l.split('\t'));
const columns=npcRows[0];
const ids=[82,85,86,97,440,62,2075,2506];
const selected=ids.map(id=>{
  const n=a.npcs.find(n=>n.id===id),row=npcRows[id+1];
  const fields=Object.fromEntries(['Kind','Camp','Series','Treasure','Treasure1','DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3','LevelScript','DeathScript'].map(k=>[k,row[columns.indexOf(k)]]));
  for(const k of ['Treasure','Treasure1','DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3'])if(fields[k]!==n[k])throw Error(`Snapshot changed: NPC ${id} ${k}`);
  return {id,name:n.nameCP936,...fields};
});
const combos={},explicit=[];let unchanged=0,identical02=0,identical13=0;
for(const n of a.npcs){if(n.links[0].cp936&&n.links[0].cp936===n.links[2].cp936)identical02++;if(n.links[1].cp936&&n.links[1].cp936===n.links[3].cp936)identical13++;}
for(const t of a.tables){
  const bytes=fs.readFileSync(path.join(root,'PhongThanRuntime-Loose/Server/settings/item/droprate',t.name));
  if(hash(bytes)!==t.sha256)throw Error(`Original changed: ${t.name}`);unchanged++;
  const m=t.sections.find(s=>s.name==='Main').fields;
  const k=['MinItemLevel','MaxItemLevel','MinItemLevelScale','MaxItemLevelScale','MagicRate','MoneyRate','MoneyScale'].map(k=>m[k]).join('/');combos[k]=(combos[k]??0)+1;
  for(const s of t.sections)if(s.fields.Level!==undefined)explicit.push({table:t.name,section:s.name,...s.fields});
}
fs.writeFileSync(path.join(out,'explicit_levels.txt'),explicit.map(s=>[s.Genre,s.Detail,s.Particular,s.Level].join(' ')).join('\n')+'\n');
// IDs and paths stay tied to existing snapshot, not guessed filename aliases.
fs.writeFileSync(path.join(out,'npc_cases.txt'),selected.map(n=>[n.id,n.Treasure===''?-1:n.Treasure,n.Treasure1===''?-1:n.Treasure1,...['DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3'].map(k=>n[k]||'-')].join(' ')).join('\n')+'\n');
const report={scope:'TARGETED_EXISTING_SNAPSHOT_NO_NEW_AUDIT',originalTablesUnchanged:unchanged,npcSha256:hash(npcBytes),selected,mainCombinations:combos,explicitLevelRows:explicit.length,slot02SameNonempty:identical02,slot13SameNonempty:identical13,parity:'PARITY_UNVERIFIED'};
fs.writeFileSync(path.join(out,'evidence.json'),JSON.stringify(report,null,2)+'\n');
console.log(JSON.stringify({originalTablesUnchanged:unchanged,explicitLevelRows:explicit.length,selectedNPCs:ids,mainCombinations:Object.keys(combos).length}));

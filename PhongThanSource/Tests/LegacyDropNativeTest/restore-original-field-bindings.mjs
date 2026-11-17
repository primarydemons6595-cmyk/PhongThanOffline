import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..');
const report=path.join(root,'analysis/legacy_drop_20261018_live/field_npcs/client_recovery');
const source=path.join(report,'Settings/38EBCB17.bin');
const target=path.join(root,'PhongThanRuntime-Loose/Server/settings/phongthan/Npcs.txt');
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const evidence=JSON.parse(fs.readFileSync(path.join(report,'evidence.json'),'utf8'));
const original=fs.readFileSync(source),entry=evidence.results.find(r=>r.pak.toLowerCase()==='settings.pak'&&r.status==='EXTRACTED');
if(hash(original)!==entry?.sha256)throw Error('Client source hash mismatch');
const snapshot=fs.readFileSync(target),beforeHash=hash(snapshot);
const backup=path.join(report,'Npcs.before.txt');
const audit=JSON.parse(fs.readFileSync(path.join(root,'analysis/legacy_drop_20261017/audit.json'),'utf8'));
const expected=audit.fingerprints.find(x=>x.path.replaceAll('\\','/').endsWith('/settings/phongthan/Npcs.txt'));
if(beforeHash!==expected.sha256)throw Error('Unexpected current NPC data; refuse overwrite');
const names=JSON.parse(fs.readFileSync(path.join(root,'analysis/monster_ui_20261017/template_names.json'),'utf8')).names;
const mapping=fs.readFileSync(path.join(root,'PhongThanSource/Tools/Export-StarterBagLookup.ps1'),'utf8');
const array=k=>mapping.match(new RegExp('\\$'+k+'=@\\(([^)]+)'))[1].split(',').map(x=>Number(x.trim()));
const from=array('from'),to=array('to'),chars=new Map(from.map((c,i)=>[c,to[i]]));
const decode=b=>[...b].map(c=>String.fromCodePoint(chars.get(c)??c)).join('');
const norm=s=>s.replace(/^#+/,'').trim().normalize('NFC').toLocaleLowerCase('vi');
const oldLines=snapshot.toString('latin1').split(/(\r\n|\n)/),clientLines=original.toString('latin1').split(/\r?\n/);
const headers=oldLines[0].split('\t'),ch=clientLines[0].split('\t');
const fields=['DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3'],ids=[0,7,18,15,11],changes=[];
const drops=path.join(root,'PhongThanRuntime-Loose/Server/settings/item/droprate');
const originalDropHashes=audit.tables.map(t=>({name:t.name,sha256:hash(fs.readFileSync(path.join(drops,t.name)))}));
for(const t of originalDropHashes)if(t.sha256!==audit.tables.find(x=>x.name===t.name).sha256)throw Error('Original drop changed: '+t.name);
const qFile=path.join(root,'PhongThanRuntime-Loose/Server/settings/phongthan/LegacyDropQuarantine.txt'),qBytes=fs.readFileSync(qFile);
for(const id of ids){const rowIndex=(id+1)*2,raw=oldLines[rowIndex],row=raw.split('\t'),client=clientLines[id+1].split('\t'),n=names.find(n=>n.template===id);
 if(Buffer.from(row[0],'latin1').toString('hex')!==n.raw_hex)throw Error('Server identity mismatch '+id);
 const localized=decode(Buffer.from(client[0],'latin1')),display=decode(Buffer.from(n.display_hex,'hex'));
 if(norm(localized)!==norm(display))throw Error('Localized identity mismatch '+id+': '+localized+' vs '+display);
 for(const k of ['Kind','Treasure','Treasure1'])if(row[headers.indexOf(k)]!==client[ch.indexOf(k)])throw Error('Structural identity mismatch '+id+' '+k);
 for(const field of fields){const pos=headers.indexOf(field),value=client[ch.indexOf(field)];
  if(pos<0||row[pos]!==''||!value||!/^\\settings\\item\\droprate\\[a-z0-9_-]+\.ini$/i.test(value))throw Error('Unsafe binding '+id+' '+field);
  const name=value.split('\\').at(-1),file=path.join(drops,name);if(!fs.existsSync(file))throw Error('Missing original INI '+name);
  changes.push({id,originalNameHex:n.raw_hex,localizedSourceName:localized,field,before:'',after:value,tableSha256:hash(fs.readFileSync(file)),tableQuarantined:qBytes.toString('latin1').split(/\r?\n/).includes('T '+value)});row[pos]=value;
 }
 oldLines[rowIndex]=row.join('\t');
}
if(changes.length!==20)throw Error('Expected twenty empty cells');
const next=Buffer.from(oldLines.join(''),'latin1');
// Reconstruct the entire original byte stream by undoing only the allowed cells.
const reversed=next.toString('latin1').split(/(\r\n|\n)/);
for(const id of ids){const i=(id+1)*2,r=reversed[i].split('\t');for(const k of fields)r[headers.indexOf(k)]='';reversed[i]=r.join('\t');}
if(!Buffer.from(reversed.join(''),'latin1').equals(snapshot))throw Error('Changes outside permitted twenty cells');
if(fs.existsSync(backup)&&!fs.readFileSync(backup).equals(snapshot))throw Error('Conflicting backup');
fs.writeFileSync(backup,snapshot);
const prepared=path.join(report,'Npcs.restored.txt');fs.writeFileSync(prepared,next);
const record={status:'VERIFIED_RESTORATION_PREPARED',sourcePak:'Client_Original/data/Settings.pak',sourcePath:'\\settings\\npcs.txt',sourceSha256:hash(original),sourcePriority:entry.priority,
 sourcePrecedence:'Older lower-priority original table: explicit restoration requested; not current winning vng00 payload',beforeSha256:beforeHash,afterSha256:hash(next),changedCells:changes.length,
 unchangedOutsideTwentyCells:true,unchangedOriginalDropTables:originalDropHashes.length,quarantineSha256:hash(qBytes),npc86Unchanged:true,activation:'REQUIRES_NPC_REGISTRY_RELOAD_OR_GAMESERVER_RESTART',changes};
fs.writeFileSync(path.join(report,'restoration.json'),JSON.stringify(record,null,2)+'\n');
console.log(JSON.stringify({status:record.status,changedCells:20,beforeSha256:beforeHash,afterSha256:record.afterSha256,unchangedTables:originalDropHashes.length,tableQuarantined:changes.filter(c=>c.tableQuarantined).map(c=>[c.id,c.field,c.after])}));

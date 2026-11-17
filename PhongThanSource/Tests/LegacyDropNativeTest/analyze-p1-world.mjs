import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..'),report=path.join(root,'analysis/blue_magic_p1_20261018'),iso=JSON.parse(fs.readFileSync(path.join(report,'isolation.json'),'utf8'));
const sha=b=>crypto.createHash('sha256').update(b).digest('hex');
const events=role=>fs.readFileSync(path.join(iso[role],'p1_blue_magic.jsonl'),'utf8').trim().split(/\r?\n/).filter(Boolean).map(JSON.parse);
const server=events('Server'),client=events('Client');
const ground=server.find(e=>e.event==='SERVER_TEST_GROUND'&&e.quality===1);if(!ground)throw Error('No real-world blue ground event');
const fields=e=>JSON.stringify([e.serial,e.quality,e.level,e.durability,e.magic,e.requirements]);
const blue=server.filter(e=>e.serial===ground.serial),received=client.filter(e=>e.serial===ground.serial);
for(const e of [...blue,...received])if(fields(e)!==fields(ground))throw Error('Real-world magic/requirement/quality drift: '+JSON.stringify(e));
const sent=blue.filter(e=>e.event==='SERVER_PROPERTIES_SEND');const restored=blue.filter(e=>e.event==='SERVER_LOAD_RESTORED');
const packetPairs=sent.every(e=>server.some(p=>p.pid===e.pid&&p.event==='SERVER_SNAPSHOT_SEND'&&p.serial===e.serial&&p.seq===e.seq-1));
const canonicalPath=path.join(iso.Server,'CharacterStore/role_7465737464726f7072617465.pthc'),file=fs.readFileSync(canonicalPath),state=file.subarray(16),key=sha(state);
const snapshot=fs.readFileSync(path.join(iso.Root,key+'.magic-v1')),required=fs.readFileSync(path.join(iso.Root,key+'.required'),'ascii');
if(sha(snapshot)!==required||snapshot.readUInt32LE(4)!==1||snapshot.subarray(16,80).toString('ascii')!==key)throw Error('Invalid bound companion');
const count=state.readUInt32LE(404),entrySize=snapshot.readUInt32LE(8);if(snapshot.readUInt32LE(12)!==count)throw Error('Count mismatch');
const persisted=[];for(let i=0;i<count;i++){const offset=144+i*entrySize;const serial=snapshot.readUInt32LE(offset+4);persisted.push({ordinal:i,serial,quality:snapshot.readInt32LE(offset+8),magic:Array.from({length:20},(_,a)=>Array.from({length:4},(_,v)=>snapshot.readInt32LE(offset+12+a*16+v*4)))});}
const persistedBlue=persisted.find(e=>e.serial===ground.serial);if(!persistedBlue||JSON.stringify(persistedBlue.magic)!==JSON.stringify(ground.magic)||persistedBlue.quality!==1)throw Error('Persisted blue lost');
const result={status:'P1_IN_PROGRESS',scope:'REAL_GAMESERVER_AND_GAME_EXE',blueSerial:ground.serial,blueMagic:ground.magic.filter(e=>e[0]),quality:ground.quality,nativeLevel:ground.level,requirements:ground.requirements.filter(e=>e[0]),groundPid:ground.pid,restoredServerPids:[...new Set(restored.map(e=>e.pid))],receivingClientPids:[...new Set(received.map(e=>e.pid))],clientAppliedEvents:received.filter(e=>e.event==='CLIENT_PROPERTIES_APPLIED').length,invalidTypeRejected:received.some(e=>e.event==='CLIENT_PROPERTIES_REJECT_TYPE'),snapshotThenPropertiesServerOrder:packetPairs,allObservedBluePayloadsExact:true,canonical:{path:canonicalPath,sha256:sha(file),stateHash:key,revision:state.readUInt32LE(8),itemCount:count,persisted},acceptancePending:['Equip/unequip at a valid player level','Repeat missing-companion recovery after session cleanup fix','Old-format white world test'],p0ModuleUnchanged:sha(fs.readFileSync(path.join(root,'PhongThanSource/Sources/Core/Src/P0ItemMagicPersistence.h'))).toUpperCase()===iso.P0Sha256};
const observed=path.join(report,'ui_observations.json'),fault=path.join(report,'load_fault_admission_fix.json'),comparison=path.join(report,'canonical_comparison.json');
if([observed,fault,comparison].every(fs.existsSync)){
 const ui=JSON.parse(fs.readFileSync(observed,'utf8')),f=JSON.parse(fs.readFileSync(fault,'utf8')),c=JSON.parse(fs.readFileSync(comparison,'utf8'));
 result.acceptancePending=[];
 if(ui.hpMaximum.equippedLoginAfterFix!==68||ui.hpMaximum.unequippedAfterFix!==42)result.acceptancePending.push('Equipped login/client stats');
 if(!f.canonicalUnchanged||!received.some(e=>e.pid===ui.clientPid&&e.event==='CLIENT_PROPERTIES_APPLIED')||!restored.some(e=>e.pid===ui.serverPid))result.acceptancePending.push('Failed load recovery');
 if(!c.legacyWhite.legacyCompanionAbsent||c.legacyWhite.quality!==0||!ui.legacyWhiteSeenInRealF4)result.acceptancePending.push('Old white compatibility');
 result.status=result.acceptancePending.length?'P1_IN_PROGRESS':'P1_EVIDENCE_COMPLETE';
}
fs.writeFileSync(path.join(report,'world_progress.json'),JSON.stringify(result,null,2));
for(const [name,value] of [['server_world.jsonl',server],['client_world.jsonl',client]])fs.writeFileSync(path.join(report,name),value.map(e=>JSON.stringify(e)).join('\n')+'\n');
console.log(JSON.stringify({...result,canonical:{revision:result.canonical.revision,itemCount:count}},null,2));

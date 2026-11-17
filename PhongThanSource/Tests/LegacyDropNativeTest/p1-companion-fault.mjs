import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
const workspace=path.resolve(import.meta.dirname,'../../..'),report=path.join(workspace,'analysis/blue_magic_p1_20261018'),iso=JSON.parse(fs.readFileSync(path.join(report,'isolation.json'),'utf8'));
const label=process.argv[3]||'load_fault';if(!/^[a-z0-9_]+$/.test(label))throw Error('Invalid evidence label');
const receipt=path.join(report,label+'.json'),role=path.join(iso.Server,'CharacterStore/role_7465737464726f7072617465.pthc');
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
if(process.argv[2]==='hide'){
 if(fs.existsSync(receipt))throw Error('Existing fault receipt; refusing overwrite');
 const bytes=fs.readFileSync(role),key=hash(bytes.subarray(16)),snapshot=path.join(iso.Root,key+'.magic-v1');
 if(!fs.existsSync(snapshot))throw Error('Bound snapshot missing before fault');
 fs.copyFileSync(role,path.join(report,label+'.before.pthc'));fs.renameSync(snapshot,snapshot+'.p1held');
 fs.writeFileSync(receipt,JSON.stringify({key,snapshot,canonicalSha256:hash(bytes),companionSha256:hash(fs.readFileSync(snapshot+'.p1held')),hiddenUtc:new Date().toISOString()},null,2));console.log('Isolated required snapshot hidden: '+key);
}else if(process.argv[2]==='restore'){
 const r=JSON.parse(fs.readFileSync(receipt,'utf8'));if(hash(fs.readFileSync(role))!==r.canonicalSha256)throw Error('Canonical changed during failed load');
 fs.renameSync(r.snapshot+'.p1held',r.snapshot);r.restoredUtc=new Date().toISOString();r.canonicalUnchanged=true;fs.writeFileSync(receipt,JSON.stringify(r,null,2));console.log('Snapshot restored; committed canonical unchanged');
}else throw Error('hide or restore required');

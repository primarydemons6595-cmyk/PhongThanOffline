import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..'),report=path.join(root,'analysis/blue_magic_p1_20261018'),iso=JSON.parse(fs.readFileSync(path.join(report,'isolation.json'),'utf8'));
const sha=b=>crypto.createHash('sha256').update(b).digest('hex');
function read(file){const raw=fs.readFileSync(file),s=raw.subarray(16);let fnv=2166136261;for(const v of s)fnv=Math.imul(fnv^v,16777619)>>>0;if(raw.readUInt32LE(12)!==fnv)throw Error('Canonical checksum: '+file);
 const count=s.readUInt32LE(404),start=408+12*(s.readUInt32LE(392)+s.readUInt32LE(396))+20*s.readUInt32LE(400),key=sha(s),comp=path.join(iso.Root,key+'.magic-v1');
 const snapshot=fs.existsSync(comp)?fs.readFileSync(comp):null;const items=[];
 for(let i=0;i<count;i++){const canonical=Buffer.from(s.subarray(start+i*268,start+(i+1)*268));const entry=snapshot?.subarray(144+i*748,144+(i+1)*748);items.push({canonical,entry,serial:entry?.readUInt32LE(4),quality:entry?.readInt32LE(8)});}
 return {file,key,revision:s.readUInt32LE(8),role:s.subarray(16,48).toString('ascii').split('\0')[0],count,items};}
function stable(item){const record=Buffer.from(item.canonical);record.fill(0,12,24);return record.toString('hex');}
const baseline=read(path.join(report,'load_fault.before.pthc')),current=read(path.join(iso.Server,'CharacterStore/role_7465737464726f7072617465.pthc'));
if(current.count!==3)throw Error('Missing/duplicate world item');const comparisons=[];
for(const prior of baseline.items){const next=current.items.find(x=>x.serial===prior.serial);if(!next||stable(prior)!==stable(next)||!prior.entry.subarray(4).equals(next.entry.subarray(4)))throw Error('Item metadata/base/magic changed serial '+prior.serial);comparisons.push({serial:prior.serial,quality:prior.quality,canonical268StableExceptPlacement:true,companionMagicBaseRequirementQualitySerialExact:true,containerBefore:prior.canonical.readInt32LE(12),containerAfter:next.canonical.readInt32LE(12)});}
const legacy=read(path.join(report,'legacy_white.before.pthc')),oldNow=read(path.join(iso.Server,'CharacterStore/role_70316f6c647768697465.pthc'));
const legacyCopy=Buffer.from(legacy.items[0].canonical);legacyCopy.writeUInt32LE(oldNow.items[0].canonical.readUInt32LE(240),240);
if(legacy.count!==1||legacy.items[0].entry||oldNow.count!==1||oldNow.items[0].quality!==0||stable({canonical:legacyCopy})!==stable(oldNow.items[0]))throw Error('Old white compatibility mismatch');
const result={blueRole:{beforeRevision:baseline.revision,afterRevision:current.revision,count:current.count,comparisons},legacyWhite:{role:legacy.role,beforeRevision:legacy.revision,afterRevision:oldNow.revision,legacyCompanionAbsent:true,canonicalExactExceptPlacementAndExpectedNewRoleOwner:true,ownerBefore:legacy.items[0].canonical.readUInt32LE(240),ownerAfter:oldNow.items[0].canonical.readUInt32LE(240),ownerReason:'Fixture copied TestDropRate record into a NEW role; native KItemList::Add sets owner to the receiving role ID at line 267',count:1,quality:0,currentSerial:oldNow.items[0].serial}};
fs.writeFileSync(path.join(report,'canonical_comparison.json'),JSON.stringify(result,null,2));console.log(JSON.stringify(result,null,2));

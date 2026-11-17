import fs from 'node:fs';
import path from 'node:path';
import crypto from 'node:crypto';
const root=path.resolve(import.meta.dirname,'../../..');
const report=path.join(root,'analysis/legacy_drop_20261018_live/field_npcs');
const server=path.join(root,'PhongThanRuntime-Loose/Server');
const registry=path.join(server,'settings/phongthan/Npcs.txt');
const config=path.join(server,'settings/phongthan/LegacyDropExperiment.ini');
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const input=fs.readFileSync(registry),audit=JSON.parse(fs.readFileSync(path.join(root,'analysis/legacy_drop_20261017/audit.json'),'utf8'));
const recorded=audit.fingerprints.find(x=>x.path.replaceAll('\\','/').endsWith('/settings/phongthan/Npcs.txt'));
if(!recorded||recorded.sha256!==hash(input))throw Error('NPC registry differs from recorded native audit');
const lines=input.toString('latin1').split(/\r?\n/),headers=lines.shift().split('\t');
const rows=lines.filter(l=>l.trim()).map(l=>l.split('\t'));
const dec=new TextDecoder('gbk');
const targets=[{id:0,name:'Kiếm Nhân',original:'靖人'},{id:7,name:'Hoàn Cẩu',original:'环狗'},
 {id:18,name:'Hắc Sát Phong',original:'黑煞蜂'},{id:15,name:'Quỷ Ngư / Quỷ Ngự',original:'鬼驭'},
 {id:11,name:'Hạ Canh Thi',original:'夏耕尸'},{id:86,name:'Boss Cửu Anh',original:'九婴'}];
const qpath=path.join(server,'settings/phongthan/LegacyDropQuarantine.txt');
const qbytes=fs.readFileSync(qpath),q=qbytes.toString('latin1').split(/\r?\n/);
for(const t of targets){const row=rows[t.id];t.actualName=dec.decode(Buffer.from(row[0],'latin1'));
 if(t.actualName!==t.original)throw Error('Template identity mismatch: '+t.id);
 t.treasure=row[headers.indexOf('Treasure')];t.treasure1=row[headers.indexOf('Treasure1')];
 t.slots=['DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3'].map(k=>row[headers.indexOf(k)]??'');
 t.quarantined=q.includes('N '+t.id);t.legacyStatus=t.quarantined?'NPC_QUARANTINED':t.slots.every(s=>!s)?'SLOT_PATH_INVALID':'REQUIRES_NATIVE_PLAN';
}
fs.mkdirSync(report,{recursive:true});
const original=fs.readFileSync(config),text=original.toString('ascii');
const mode=text.match(/^Mode=(OFF|SHADOW|LIVE_TEST)\r?$/m)?.[1];if(!mode)throw Error('Unknown current mode');
if(!fs.existsSync(path.join(report,'LegacyDropExperiment.before.ini')))fs.writeFileSync(path.join(report,'LegacyDropExperiment.before.ini'),original);
const main=text.split(/(?=^\[NPC_)/m)[0].replace(/^SelfTestEnabled=.*$/m,'SelfTestEnabled=0').replace(/^HitDetails=.*$/m,'HitDetails=1');
const sections=targets.map(t=>`; ${t.original} / ${t.legacyStatus}\n[NPC_${t.id}]\nEnabled=1\nProfile=A\nTreasureRule=SUCCESS_CAP\nLevelRule=MAIN_CATALOG\nMagicRule=NATIVE_CATALOG_ONLY\nMoneyRule=ZERO_ONLY\nLevelGate=NONE\nMaxOverLevel=10\nMaxItems=64\nMaxTrials=256\n`);
// Config is ASCII. Human names remain in UTF-8 reports, never lossy INI text.
const next=main.trimEnd()+'\n\n'+sections.join('\n').replace(/^; .*$/gm,'; Requested field NPC; existing data guards remain active.')+'\n';
fs.writeFileSync(path.join(report,'LegacyDropExperiment.selected.ini'),next,'ascii');
const evidence={selectedUtc:new Date().toISOString(),mode,scope:'FIELD_NPC_ALLOWLIST',parity:'KINGSOFT_PARITY_UNVERIFIED',productionGate:'CLOSED',registrySha256:hash(input),quarantineSha256:hash(qbytes),previousConfigSha256:hash(original),selectedConfigSha256:hash(Buffer.from(next,'ascii')),targets};
fs.writeFileSync(path.join(report,'selection.json'),JSON.stringify(evidence,null,2)+'\n');
console.log(JSON.stringify(evidence,null,2));

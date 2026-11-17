import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const source=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'../..');
const fields=['m_HelmType','m_ArmorType','m_PenDType','m_WeaponType','m_HorseType','m_MantleType'];
const target=path.join(source,'Sources/Core/Src/KNpc.h');
const uses=[];
function walk(dir){for(const entry of fs.readdirSync(dir,{withFileTypes:true})){const p=path.join(dir,entry.name);if(entry.isDirectory()){if(!['Release','Debug','ServerRelease','ClientRelease','ServerDebug','ClientDebug'].includes(entry.name))walk(p);}else if(/\.(h|cpp|inl|c)$/i.test(p)&&p!==target){const t=fs.readFileSync(p,'latin1');for(const field of fields)if(new RegExp('\\b'+field+'\\b').test(t))uses.push({p,field});}}}
walk(path.join(source,'Sources'));walk(path.join(source,'Headers'));
if(uses.length)throw Error('Live references remain: '+JSON.stringify(uses));
const bytes=fs.readFileSync(target),text=bytes.toString('latin1');
const backup=path.join(path.dirname(source),'analysis/legacy_drop_20261017/phase2/before/KNpc.h');
if(!fs.existsSync(backup))fs.writeFileSync(backup,bytes);
let result=text;for(const field of fields){const re=new RegExp('^\\tint\\t+'+field+';[^\\r\\n]*(?:\\r?\\n)','gm');const matches=[...result.matchAll(re)];if(matches.length>1)throw Error('Ambiguous declaration');result=result.replace(re,'');}
fs.writeFileSync(target,Buffer.from(result,'latin1'));
console.log('Removed six unused legacy declarations; all Sources/Headers checked for references. No data or wire definitions changed. Rebuild both Cores before use.');

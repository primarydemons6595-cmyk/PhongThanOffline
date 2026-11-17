import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const out=path.join(root,'Output/Tools/ThreeFactionSmoke');
fs.mkdirSync(out,{recursive:true});
const targets=[
  ['KMissle.cpp','int KMissle::ProcessCollision(int nLauncherIdx','CollisionProduction.inl'],
  ['KRegion.cpp','BYTE\tKRegion::GetBarrierMin(','BarrierProduction.inl'],
  ['KItemList.cpp','BOOL KItemList::EnoughAttrib(','RequirementsProduction.inl'],
  ['KPhongThanAppearance.cpp','BOOL KPhongThanAppearance::ResolveOriginalTierPart(','TierProduction.inl'],
  ['KPhongThanAppearance.cpp','BOOL KPhongThanAppearance::ResolvePhiPhong(','CapeProduction.inl'],
  ['KPlayerDBFuns.cpp','int KPlayer::LoadPlayerFightSkillList(','LoginSkillsProduction.inl'],
  ['KProfession.cpp','BOOL KProfession::Init()','ProfessionNamesProduction.inl']
];
for(const [file,signature,destination] of targets){
  const text=fs.readFileSync(path.join(root,'Sources/Core/Src',file)).toString('latin1');
  const start=text.indexOf(signature);
  if(start<0)throw Error(`Missing production signature ${signature}`);
  // Mask comments and string literals while preserving offsets for brace matching.
  const masked=text.replace(/\/\*[\s\S]*?\*\/|\/\/[^\r\n]*|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'/g,
    s=>s.replace(/[^\r\n]/g,' '));
  let end=masked.indexOf('{',start),depth=0;
  if(end<0)throw Error('Missing function body');
  for(;end<masked.length;++end){if(masked[end]==='{')++depth;if(masked[end]==='}'&&!--depth)break;}
  if(depth!==0||end===masked.length)throw Error('Unbalanced production function');
  fs.writeFileSync(path.join(out,destination),text.slice(start,end+1),'latin1');
}
console.log('Prepared unmodified production collision, barrier, and requirement functions.');

// The current engine reads fixed VNG modifiers from equipment rows. Rebuild
// the old AddNormalItem2 blue-mount presentation as explicit persistent rows;
// move one existing property to a blue slot, preserving its original values.
// No undocumented stat bonus is invented.
module.exports=function({fs,path,S,put,enc,dec,mounts}){
 const rel='settings/item/001/horse.txt';
 const rows=fs.readFileSync(path.join(S,rel),'latin1').split(/\r?\n/).filter(Boolean).map(x=>x.split('\t'));
 const projections=[];
 for(const recipe of mounts){
  const index=rows.findIndex(r=>r[1]==='0'&&r[2]==='10'&&+r[3]===recipe[0]&&+r[11]===recipe[1]);
  if(index<1)throw Error('Missing source mount '+recipe);
  const source=rows[index],part=5000+recipe[0];
  let target=rows.findIndex(r=>r[1]==='0'&&r[2]==='10'&&+r[3]===part&&+r[11]===recipe[1]);
  const blue=[...source];blue[0]=enc(dec(Buffer.from(source[0],'latin1'))+' (xanh)').toString('latin1');blue[3]=String(part);
  let found=false;
  for(let n=0;n<5;n++)if(+blue[14+n*3]>0){
   for(let slot=5;slot<10;slot++)if(!+blue[14+slot*3]){
    for(let k=0;k<3;k++){blue[14+slot*3+k]=blue[14+n*3+k];blue[14+n*3+k]='';}found=true;break;
   }if(found)break;
  }
  if(!found)throw Error('Mount has no source property for blue projection');
  if(target<0){target=rows.length;rows.push(blue);}else rows[target]=blue;
  recipe.push(part);projections.push({sourceRecord:index,record:target,part,level:recipe[1]});
 }
 const bytes=Buffer.from(rows.map(r=>r.join('\t')).join('\r\n')+'\r\n','latin1');put(rel,bytes);put('client/'+rel,bytes);
 for(const name of ['VNG_HorsePart.txt','PhongThanHorseVisual.txt']){
  const file='settings/item/'+name,lines=fs.readFileSync(path.join(S,file),'latin1').split(/\r?\n/).filter(Boolean).map(x=>x.split('\t'));
  for(const p of projections){const original=lines.find(r=>+r[0]===p.sourceRecord);if(!original){if(name==='PhongThanHorseVisual.txt')continue;throw Error('Missing mount appearance '+name+' '+p.sourceRecord);}
   const next=[String(p.record),...original.slice(1)],i=lines.findIndex(r=>+r[0]===p.record);if(i<0)lines.push(next);else lines[i]=next;
  }
  const b=Buffer.from(lines.map(r=>r.join('\t')).join('\r\n')+'\r\n','latin1');put(file,b);put('client/'+file,b);
 }
 return projections;
};

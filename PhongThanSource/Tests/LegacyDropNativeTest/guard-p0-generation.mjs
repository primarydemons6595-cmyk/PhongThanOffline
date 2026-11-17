import fs from 'node:fs';
import path from 'node:path';
const file=path.resolve(import.meta.dirname,'../../Sources/Core/Src/KPlayerDBFuns.cpp');
let source=fs.readFileSync(file,'latin1');
const target='\t\tif (!bGenerated) { if(hasMagicSnapshot)return -1; continue; }';
if(!source.includes(target)){
 const anchor=/\t\tif \(!bGenerated\)\r?\n\t\t\tcontinue;/g;
 if([...source.matchAll(anchor)].length!==1)throw Error('Expected one native generation failure guard');
 source=source.replace(anchor,target);
 fs.writeFileSync(file,Buffer.from(source,'latin1'));
}

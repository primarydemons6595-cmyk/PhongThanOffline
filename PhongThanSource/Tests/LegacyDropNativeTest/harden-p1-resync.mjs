import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),file=path.join(root,'PhongThanSource/Sources/Core/Src/KProtocolProcess.cpp');let all=fs.readFileSync(file,'latin1');
const start=all.indexOf('void KProtocolProcess::s2cSyncItem(BYTE* pMsg)'),end=all.indexOf('void KProtocolProcess::s2cSyncMagic(BYTE* pMsg)',start);if(start<0||end<0)throw Error('snapshot function boundaries');let s=all.slice(start,end);
if(!s.includes('bool createSnapshotItem')){
 const eol=s.includes('\r\n')?'\r\n':'\n';
 const anchor='\tint nIndex = 0;';if(s.split(anchor).length!==2)throw Error('snapshot index anchor');
 s=s.replace(anchor,['\tint nIndex = ItemSet.SearchID(pItem->ItemId);','\tbool createSnapshotItem=pItem->IsNew && nIndex<=0;','\tif(nIndex>0&&(Item[nIndex].GetGenre()!=pItem->Genre||Item[nIndex].GetDetailType()!=pItem->DetailType||Item[nIndex].GetParticular()!=pItem->ParticularType||Item[nIndex].GetRow()!=pItem->TemplateRow)){P1BlueTrace("CLIENT_SNAPSHOT_REJECT_IDENTITY",Item[nIndex]);return;}'].join(eol));
 s=s.replaceAll('if (pItem->IsNew)','if (createSnapshotItem)').replaceAll('if(pItem->IsNew)ItemSet.Remove(nIndex);','if(createSnapshotItem)ItemSet.Remove(nIndex);');
 s=s.trimEnd()+eol+eol;
 all=all.slice(0,start)+s+all.slice(end);fs.writeFileSync(file,Buffer.from(all,'latin1'));
}

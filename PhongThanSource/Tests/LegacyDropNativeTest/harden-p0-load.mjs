import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),file=path.join(root,'PhongThanSource/Sources/Core/Src/KPlayerDBFuns.cpp');let text=fs.readFileSync(file,'latin1');
function replace(old,next){if(text.includes(next))return;if(!text.includes(old)){old=old.replaceAll('\n','\r\n');next=next.replaceAll('\n','\r\n');}if(text.split(old).length!==2)throw Error('P0 load hardening anchor');text=text.replace(old,next);}
replace('if(!P0ItemMagic::Load(pState,magicSnapshot,hasMagicSnapshot,magicError)){CoreLoginDiag','if(!P0ItemMagic::Load(pState,magicSnapshot,hasMagicSnapshot,magicError)){P0ItemMagic::Block(pState);CoreLoginDiag');
replace('if(ItemSet.SearchID(magicSnapshot[serial].serial)){CoreLoginDiag','if(ItemSet.SearchID(magicSnapshot[serial].serial)){P0ItemMagic::Block(pState);CoreLoginDiag');
replace('\tconst int nBegin = (int)nParam;','\tP0ItemMagic::LoadGuard<KItemList> magicGuard(m_ItemList,magicSnapshot,pState,hasMagicSnapshot);\n\tif(hasMagicSnapshot&&MAX_ITEM-1-ItemSet.GetItemNumber()<(int)magicSnapshot.size())return -1;\n\tconst int nBegin = (int)nParam;');
replace('\tint nEnd = nBegin + DBLOADPERTIME_ITEM;','\tint nEnd = hasMagicSnapshot?nItemCount:nBegin + DBLOADPERTIME_ITEM;');
replace('\t\tif (nIndex > 0)\n\t\t\tm_ItemList.Add(nIndex, pItemData->Container,\n\t\t\t\tpItemData->SlotX, pItemData->SlotY, false);','\t\tif(hasMagicSnapshot){\n\t\t\tif(nIndex<=0||!m_ItemList.Add(nIndex,pItemData->Container,pItemData->SlotX,pItemData->SlotY,false))return -1;\n\t\t}else if (nIndex > 0)\n\t\t\tm_ItemList.Add(nIndex, pItemData->Container,\n\t\t\t\tpItemData->SlotX, pItemData->SlotY, false);');
replace('\treturn nParam >= (unsigned int)nItemCount ? 1 : 0;','\tmagicGuard.Commit();\n\treturn nParam >= (unsigned int)nItemCount ? 1 : 0;');
fs.writeFileSync(file,Buffer.from(text,'latin1'));

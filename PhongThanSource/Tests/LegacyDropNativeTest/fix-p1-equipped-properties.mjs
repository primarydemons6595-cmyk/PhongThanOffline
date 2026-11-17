import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),file=path.join(root,'PhongThanSource/Sources/Core/Src/KProtocolProcess.cpp');let s=fs.readFileSync(file,'latin1');
const anchor='\tP1BlueTrace("CLIENT_PROPERTIES_APPLIED",Item[nIdx]);';
if(s.split(anchor).length!==2)throw Error('Unique properties apply anchor missing');
s=s.replace(anchor,`\t// Snapshot equips before the following properties packet arrives. Rebuild
\t// through the normal native path so login/resync uses the restored magic.
\tfor(int slot=0;slot<itempart_num;++slot)
\t\tif(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(slot)==(int)nIdx)
\t\t{Player[CLIENT_PLAYER_INDEX].UpdataCurData();break;}
${anchor}`);fs.writeFileSync(file,Buffer.from(s,'latin1'));

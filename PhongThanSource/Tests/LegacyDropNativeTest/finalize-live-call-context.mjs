import fs from 'node:fs';import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),src=path.join(root,'PhongThanSource/Sources/Core/Src');
function edit(file,old,next){let s=fs.readFileSync(path.join(src,file),'latin1');if(s.includes(next)||s.includes(next.replaceAll('\n','\r\n')))return;if(!s.includes(old)){old=old.replaceAll('\n','\r\n');next=next.replaceAll('\n','\r\n');}if(s.split(old).length!==2)throw Error(`Anchor mismatch ${file}`);fs.writeFileSync(path.join(src,file),Buffer.from(s.replace(old,next),'latin1'));}
edit('KPlayer.cpp','    LegacyDropPickup(legacyPickupSerial,m_nPlayerIndex,nObjIndex,"ATTEMPT");','    LegacyDropPickupAttempt legacyAttempt(legacyPickupSerial,m_nPlayerIndex,nObjIndex);');
edit('KPlayer.cpp','            LegacyDropPickup(legacyPickupSerial,m_nPlayerIndex,nObjIndex,"PICKED_UP");','            legacyAttempt.Complete();');
edit('KNpc.cpp',`            LegacyDropLua(m_Index,m_DropScriptID,"DropRate_BEGIN",-1);
\t\t\tPlayer[nPlayer].ExecuteScript(m_DropScriptID, "DropRate", m_Index);
            LegacyDropLua(m_Index,m_DropScriptID,"DropRate_END",-1);`,
`            const int legacyNpcIndex=m_Index;const DWORD legacyEntity=m_dwID,legacyScript=m_DropScriptID;
            LegacyDropLua(legacyNpcIndex,legacyScript,"DropRate_BEGIN",-1);
\t\t\tPlayer[nPlayer].ExecuteScript(legacyScript, "DropRate", legacyNpcIndex);
            LegacyDropLua(legacyNpcIndex,legacyScript,"DropRate_END",-1);
            if(Npc[legacyNpcIndex].m_dwID!=legacyEntity || Npc[legacyNpcIndex].m_RegionIndex<0)return;`);
edit('KNpc.cpp',`\t\t\tif (nPlayer)
\t\t\t\tPlayer[nPlayer].ExecuteScript(m_ActionScriptID, "LastDamage", m_Index);`,
`\t\t\tif (nPlayer){
                const int legacyNpcIndex=m_Index;const DWORD legacyEntity=m_dwID,legacyScript=m_ActionScriptID;
                LegacyDropLua(legacyNpcIndex,legacyScript,"LastDamage_BEGIN",-1);
\t\t\t\tPlayer[nPlayer].ExecuteScript(legacyScript, "LastDamage", legacyNpcIndex);
                LegacyDropLua(legacyNpcIndex,legacyScript,"LastDamage_END",-1);
                if(Npc[legacyNpcIndex].m_dwID!=legacyEntity || Npc[legacyNpcIndex].m_RegionIndex<0)return;
            }`);
console.log('CAPTURED_LUA_CALLBACK_IDENTITIES_AND_NATIVE_PICKUP_SCOPE');

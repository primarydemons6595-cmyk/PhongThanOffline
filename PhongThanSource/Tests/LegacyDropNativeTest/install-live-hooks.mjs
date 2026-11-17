import fs from 'node:fs';
import path from 'node:path';
const root=path.resolve(import.meta.dirname,'../../..'),src=path.join(root,'PhongThanSource/Sources/Core');
const backup=path.join(root,'analysis/legacy_drop_20261018_live/before');fs.mkdirSync(backup,{recursive:true});
function edit(relative,changes){const file=path.join(src,relative),bytes=fs.readFileSync(file);let s=bytes.toString('latin1');
  for(let [before,after] of changes){if(s.includes(after)||s.includes(after.replaceAll('\r\n','\n')))continue;
    if(!s.includes(before)){before=before.replaceAll('\r\n','\n');after=after.replaceAll('\r\n','\n');}
    const count=s.split(before).length-1;if(count!==1)throw Error(`${relative}: expected one anchor, got ${count}: ${before}`);s=s.replace(before,after);}
  if(s===bytes.toString('latin1'))return;const saved=path.join(backup,relative.replaceAll('/','__'));if(!fs.existsSync(saved))fs.writeFileSync(saved,bytes);fs.writeFileSync(file,Buffer.from(s,'latin1'));
}
edit('Src/KNpcTemplate.h',[["\tint\t\tm_AiMode;","\tint m_LegacyTreasure, m_LegacyTreasure1;\r\n\tchar m_LegacyDropFiles[4][1024];\r\n\tint\t\tm_AiMode;"]]);
edit('Src/KNpcTemplate.cpp',[
 ['#ifdef _SERVER\r\nstatic void LoadNpcTemplateScript','#ifdef _SERVER\r\n#include "LegacyDropEngine.h"\r\nstatic void LoadNpcTemplateScript'],
 ['\tg_NpcSetting.GetInteger(nNpcTempRow, "AIMode",\t0, &m_AiMode);',`\t// Preserve raw drop fields from the active native registry. Blank/invalid Treasure stays unresolved.
\tchar legacyNumber[64];
\tg_NpcSetting.GetString(nNpcTempRow,"Treasure","",legacyNumber,sizeof(legacyNumber));
\tif(!LegacyDrop::Integer(legacyNumber,m_LegacyTreasure))m_LegacyTreasure=-1;
\tg_NpcSetting.GetString(nNpcTempRow,"Treasure1","",legacyNumber,sizeof(legacyNumber));
\tif(!LegacyDrop::Integer(legacyNumber,m_LegacyTreasure1))m_LegacyTreasure1=-1;
\tfor(int legacySlot=0;legacySlot<4;++legacySlot){char key[32];if(legacySlot)sprintf(key,"DropRateFile%d",legacySlot);else strcpy(key,"DropRateFile");
\t\tg_NpcSetting.GetString(nNpcTempRow,key,"",m_LegacyDropFiles[legacySlot],sizeof(m_LegacyDropFiles[legacySlot]));}
\tg_NpcSetting.GetInteger(nNpcTempRow, "AIMode", 0, &m_AiMode);`.replaceAll('\n','\r\n')]
]);
edit('Src/KNpc.cpp',[
 ['#include "KCore.h"','#include "KCore.h"\r\n#include "LegacyDropRuntime.h"'],
 ['    PtDropNpc(*this, nPlayer); // NEW_PROJECT_RULES_V1; quest-script drops stay separate.','    if (!LegacyDropOnDeath(*this, nPlayer, nAttacker))\r\n        PtDropNpc(*this, nPlayer); // OFF/SHADOW/rejected preflight retain existing normal engine.'],
 ['\t\tm_CurrentLife -= nDamage;','#ifdef _SERVER\r\n\t\tLegacyDropHit(m_Index,nAttacker,nDamage);\r\n#endif\r\n\t\tm_CurrentLife -= nDamage;'],
 ['void KNpc::Revive()\r\n{','void KNpc::Revive()\r\n{\r\n#ifdef _SERVER\r\n    LegacyDropResetNpc(m_Index);\r\n#endif'],
 ['\t\t\tPlayer[nPlayer].ExecuteScript(m_DropScriptID, "DropRate", m_Index);','\t\t{\r\n            LegacyDropLua(m_Index,m_DropScriptID,"DropRate_BEGIN",-1);\r\n\t\t\tPlayer[nPlayer].ExecuteScript(m_DropScriptID, "DropRate", m_Index);\r\n            LegacyDropLua(m_Index,m_DropScriptID,"DropRate_END",-1);\r\n\t\t}']
]);
// Death-script BEGIN/END uses captured scalar values; after the callback the NPC
// might have been removed. The logger reads only the sidecar, never freed NPC data.
edit('Src/PhongThanQuestDeathContext.inl',[
 ['    BOOL bResult = pScript->CallFunction("OnDeath", 0, "d", nNpcIndex);','#ifdef PT_LEGACY_DROP_RUNTIME_H\n    LegacyDropLua(nNpcIndex,dwScriptId,"OnDeath_BEGIN",-1);\n#endif\n    BOOL bResult = pScript->CallFunction("OnDeath", 0, "d", nNpcIndex);\n#ifdef PT_LEGACY_DROP_RUNTIME_H\n    LegacyDropLua(nNpcIndex,dwScriptId,"OnDeath_END",bResult);\n#endif']
]);
edit('Src/KNpcSet.cpp',[
 ['#include "KCore.h"','#include "KCore.h"\r\n#include "LegacyDropRuntime.h"'],
 ['\tNpc[m_nIndex].m_dwID = m_dwIDCreator;','#ifdef _SERVER\r\n\tLegacyDropResetNpc(m_nIndex);\r\n#endif\r\n\tNpc[m_nIndex].m_dwID = m_dwIDCreator;']
]);
edit('Src/KNpc.h',[
 ['\tfriend class KNpcAttribModify;','#ifdef _SERVER\r\n    friend void LegacyDropTick(); // Loose-only bounded in-process spawn/death diagnostic.\r\n#endif\r\n\tfriend class KNpcAttribModify;']
]);
edit('Src/KSubWorldSet.cpp',[
 ['#include "KCore.h"','#include "KCore.h"\r\n#include "LegacyDropRuntime.h"'],
 ['\tm_nLoopRate++;','\tm_nLoopRate++;\r\n#ifdef _SERVER\r\n    LegacyDropTick();\r\n#endif']
]);
edit('Src/KPlayer.cpp',[
 ['#include "KCore.h"','#include "KCore.h"\r\n#include "LegacyDropRuntime.h"'],
 ['\tif (nObjIndex == 0)\r\n\t\treturn FALSE;\r\n\r\n\t/*if (Object[nObjIndex].m_nBelong != -1)',`\tif (nObjIndex == 0)\r\n\t\treturn FALSE;\r\n    unsigned long legacyPickupSerial=0;\r\n    if(Object[nObjIndex].m_nKind==Obj_Kind_Item && Object[nObjIndex].m_nItemDataID>0 && Object[nObjIndex].m_nItemDataID<MAX_ITEM)\r\n        legacyPickupSerial=Item[Object[nObjIndex].m_nItemDataID].GetID();\r\n    LegacyDropPickup(legacyPickupSerial,m_nPlayerIndex,nObjIndex,"ATTEMPT");\r\n\r\n\t/*if (Object[nObjIndex].m_nBelong != -1)`],
 ['\t\t\tObject[nObjIndex].m_dwScriptID = 0;\r\n\t\t\tObject[nObjIndex].m_nItemDataID = 0;','            LegacyDropPickup(legacyPickupSerial,m_nPlayerIndex,nObjIndex,"PICKED_UP");\r\n\t\t\tObject[nObjIndex].m_dwScriptID = 0;\r\n\t\t\tObject[nObjIndex].m_nItemDataID = 0;']
]);
edit('Core.dsp',[
 ['SOURCE=.\\Src\\LegacyDropPreflight.cpp\r\n# End Source File','SOURCE=.\\Src\\LegacyDropPreflight.cpp\r\n# End Source File\r\n# Begin Source File\r\n\r\nSOURCE=.\\Src\\LegacyDropRuntime.cpp\r\n# End Source File']
]);
edit('Src/ScriptFuns.cpp',[
 ['#include "KCore.h"','#include "KCore.h"\r\n#include "LegacyDropRuntime.h"'],
 ['\treturn nIndex;\r\n}\r\n\r\n#include "PhongThanQuestExchange.inl"','    LegacyDropLuaItem("CreateVngNormalItem",nIndex,0);\r\n\treturn nIndex;\r\n}\r\n\r\n#include "PhongThanQuestExchange.inl"'],
 ['\tObject[nObjectIndex].SetItemBelong(nOwnerPlayer);','\tObject[nObjectIndex].SetItemBelong(nOwnerPlayer);\r\n    LegacyDropLuaItem("ThrowItem",nItemIndex,nObjectIndex);']
]);
console.log('LIVE_HOOKS_INSTALLED_WITH_BYTE_PRESERVING_BACKUPS');

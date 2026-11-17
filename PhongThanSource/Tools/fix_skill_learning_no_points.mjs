import fs from 'node:fs';import path from 'node:path';const root=path.resolve(import.meta.dirname,'../..');
const src=path.join(root,'PhongThanSource/Sources/Core/Src');
function edit(name,fn){const p=path.join(src,name);const s=fs.readFileSync(p,'latin1'),next=fn(s);if(s!==next)fs.writeFileSync(p,Buffer.from(next,'latin1'));}
edit('KPlayer.cpp',s=>{
 s=s.replace('m_nSkillPoint += PLAYER_LEVEL_ADD_SKILL;','m_nSkillPoint = 0; // PT trains learned skills; no combat skill-point currency.');
 s=s.replace('m_nSkillPoint += Npc[m_nIndex].m_SkillList.RollBackSkills(false) - (Npc[m_nIndex].m_Level - nLevel) * PLAYER_LEVEL_ADD_SKILL;','m_nSkillPoint = 0; // Preserve trained skill levels/experience when setting character level.');
 const start=s.indexOf('void\tKPlayer::AddSkillPoint(BYTE* pProtocol)');if(start<0)throw Error('AddSkillPoint anchor');const open=s.indexOf('{',start);let depth=1,end=open+1;for(;depth&&end<s.length;end++){if(s[end]==='{')depth++;if(s[end]==='}')depth--;}
 s=s.slice(0,open)+'{\n\t// Legacy packet retained for wire compatibility; points cannot train PT skills.\n\tm_nSkillPoint=0;SendSyncData_Skill();\n}'+s.slice(end);return s;
});
edit('KPlayerDBFuns.cpp',s=>s.replace('m_nSkillPoint = pState->RemainingSkillPoints;','m_nSkillPoint = 0; // Ignore obsolete legacy combat points; ABI field remains.').replace('pState->RemainingSkillPoints = m_nSkillPoint;','pState->RemainingSkillPoints = 0;'));
edit('KItemList.cpp',s=>s.includes('#include "KPhongThanSkillLearning.h"')?s:s.replace('#include "KPhongThanProfessionSkills.h"','#include "KPhongThanProfessionSkills.h"\n#include "KPhongThanSkillLearning.h"').replace('if (!skills.Add(skill, 1, 0)) return FALSE;','if (!PhongThanNativeCanLearn(skill,skills,Npc[nNpcIdx].m_Level)) return FALSE;\n            if (!skills.Add(skill, 1, 0)) return FALSE;'));
edit('ScriptFuns.cpp',s=>{
 const inc='#include "KPlayer.h"';if(!s.includes(inc))throw Error('Player include missing');if(!s.includes('#include "KPhongThanSkillLearning.h"'))s=s.replace(inc,inc+'\n#include "KPhongThanProfessionSkills.h"\n#include "KPhongThanSkillLearning.h"');
 s=s.replace('int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel);','if (Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.GetLevel(nSkillId)<=0 &&\n        !PhongThanNativeCanLearn(nSkillId,Npc[Player[nPlayerIndex].m_nIndex].m_SkillList,Npc[Player[nPlayerIndex].m_nIndex].m_Level)){Lua_PushNumber(L,0);return 1;}\n    int nRet = Npc[Player[nPlayerIndex].m_nIndex].m_SkillList.Add(nSkillId, nSkillLevel);');
 s=s.replace('if (nOldLevel <= 0)\r\n\t\tnResult = SkillList.Add(nSkillId, nNewLevel);','if (nOldLevel <= 0)\r\n        {if(PhongThanNativeCanLearn(nSkillId,SkillList,Npc[Player[nPlayerIndex].m_nIndex].m_Level))nResult = SkillList.Add(nSkillId,nNewLevel);}');
 s=s.replace('Player[nPlayerIndex].m_nSkillPoint += (int)Lua_ValueToNumber(L, 1);','Player[nPlayerIndex].m_nSkillPoint = 0; // Obsolete point API is a compatibility no-op.');return s;
});
edit('P2TestCharacterSetup.h',s=>s.replace('if(player.m_nSkillPoint<20)player.m_nSkillPoint=20;','player.m_nSkillPoint=0;').replace('player.SendSyncData_Skill();','if(!npc.m_SkillList.GetLevel(27)&&player.m_cProfession.GetProfession()==0&&PhongThanNativeCanLearn(27,npc.m_SkillList,npc.m_Level))npc.m_SkillList.Add(27,1,0);player.SendSyncData_Skill();').replace('log["unspent_skill_points"]','log["starter_skill_27_level"]=P2Blue::Num(npc.m_SkillList.GetLevel(27));log["unspent_skill_points"]'));
edit('LegacyDropRuntime.cpp',s=>s.replace('#include "P2TestCharacterSetup.h"','#include "KPhongThanProfessionSkills.h"\n#include "KPhongThanSkillLearning.h"\n#include "P2TestCharacterSetup.h"'));

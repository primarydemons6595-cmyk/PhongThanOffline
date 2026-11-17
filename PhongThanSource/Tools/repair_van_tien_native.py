from pathlib import Path
root=Path(__file__).resolve().parents[1]
for name in ('ScriptFuns.cpp','KMission.cpp'):
 p=root/'Sources/Core/Src'/name; data=p.read_bytes()
 inc=b'#include "../../../Headers/PhongThanVanTienMission.h"'
 if inc not in data: data=data.replace(b'#include "KCore.h"',b'#include "KCore.h"\r\n'+inc,1)
 if name=='ScriptFuns.cpp':
  data=data.replace(b'sprintf(szScript, MISSIONTASK_SCRIPTFILE, nMissionId);',b'PhongThanMissionScriptPath(szScript, nMissionId, SubWorld[nSubWorldIndex].m_SubWorldID);')
  # Resolve before allocating: missing Lua must not leave a half-open mission.
  marker=b'\tKMission Mission;\r\n\tMission.SetMissionId(nMissionId);'
  start=data.index(b'int LuaInitMission('); end=data.index(b'int LuaRunMission(',start)
  part=data[start:end]
  if b'// Preflight mission' not in part:
   pre=b'\t// Preflight mission before publishing it to the world.\r\n\tchar preflightPath[MAX_PATH];\r\n\tPhongThanMissionScriptPath(preflightPath,nMissionId,SubWorld[nSubWorldIndex].m_SubWorldID);\r\n\tKLuaScript* preflight=(KLuaScript*)g_GetScript(preflightPath);\r\n\tif (!preflight || !preflight->m_LuaState) return 0;\r\n\r\n'
   part=part.replace(marker,pre+marker,1); data=data[:start]+part+data[end:]
  # All three lifecycle dispatches validate loaded state.
  start=data.index(b'int LuaInitMission(');end=data.index(b'int LuaStopMissionTimer(',start)
  part=data[start:end].replace(b'KLuaScript * pScript =(KLuaScript*) g_GetScript(szScript);\r\n\t\t\tLua_PushNumber',b'KLuaScript * pScript =(KLuaScript*) g_GetScript(szScript);\r\n\t\t\tif (!pScript || !pScript->m_LuaState) return 0;\r\n\t\t\tLua_PushNumber')
  data=data[:start]+part+data[end:]
  if b'int LuaSetNpcDeathScriptChecked(' not in data:
   pos=data.index(b'int LuaAddMissionNpc(')
   func=b'''int LuaSetNpcDeathScriptChecked(Lua_State* L)
{
    int ok=0;
    if (Lua_GetTopIndex(L)==2 && Lua_IsNumber(L,1) && Lua_IsString(L,2)) {
        int npc=(int)Lua_ValueToNumber(L,1);
        const char* path=Lua_ValueToString(L,2);
        int world=GetSubWorldIndex(L);
        if (npc>0 && npc<MAX_NPC && Npc[npc].m_dwID && !Npc[npc].IsPlayer() &&
            world>=0 && world<MAX_SUBWORLD && Npc[npc].m_SubWorldIndex==world &&
            path && strlen(path)<MAX_PATH && !strnicmp(path,"\\\\script\\\\",8) && !strstr(path,"..") && g_GetScript(path)) {
            Npc[npc].m_DeathScriptID=g_FileName2Id(path);ok=1;
        }
    }
    Lua_PushNumber(L,ok);return 1;
}

'''.replace(b'\n',b'\r\n')
   data=data[:pos]+func+data[pos:]
   data=data.replace(b'{"AddMSNpc",',b'{"SetNpcDeathScript", LuaSetNpcDeathScriptChecked},\r\n\t{"AddMSNpc",',1)
 else:
  data=data.replace(b'sprintf(szScript, MISSIONTASK_SCRIPTFILE, m_ulMissionId);',b'KSubWorld* owner=(KSubWorld*)GetOwner();\r\n\t\t\tPhongThanMissionScriptPath(szScript,m_ulMissionId,owner?owner->m_SubWorldID:0);')
 data=data.replace(b'g_FileName2Id(path);ok=1;',b'g_FileName2Id((char*)path);ok=1;')
 if name=='KMission.cpp' and b'// Match persistent identity before deleting an owned NPC.' not in data:
  start=data.index(b'BOOL KMission::StopMission()');end=data.index(b'unsigned long\tKMission::AddPlayer',start)
  part=data[start:end]
  part=part.replace(b'\t\tNpc[nTargetIndex].m_nMissionGroup = -1;',b'\t\t// Match persistent identity before deleting an owned NPC.\r\n\t\tif (nTargetIndex<=0 || nTargetIndex>=MAX_NPC || Npc[nTargetIndex].m_dwID!=m_MissionNpc.m_Data[nIdx].m_ulNpcID) { nIdx=GetNextNpc(nIdx,nTargetIndex);continue; }\r\n\t\tNpc[nTargetIndex].m_nMissionGroup = -1;',1)
  part=part.replace(b'\t\tNpc[Player[nTargetIndex].m_nIndex].m_nMissionGroup = -1;',b'\t\tif (nTargetIndex<=0 || nTargetIndex>=MAX_PLAYER || Player[nTargetIndex].m_dwID!=m_MissionPlayer.m_Data[nIdx].m_ulPlayerID || Player[nTargetIndex].m_nIndex<=0) { nIdx=GetNextPlayer(nIdx,nTargetIndex);continue; }\r\n\t\tNpc[Player[nTargetIndex].m_nIndex].m_nMissionGroup = -1;',1)
  data=data[:start]+part+data[end:]
 if name=='KMission.cpp' and b'// Rejoining the same mission must not duplicate membership.' not in data:
  start=data.index(b'unsigned long\tKMission::AddPlayer');end=data.index(b'unsigned long\tKMission::AddNpc',start)
  part=data[start:end];marker=b'\tunsigned long i = 0;'
  code=b'\t// Rejoining the same mission must not duplicate membership.\r\n\tunsigned long existing=0;\r\n\twhile ((existing=m_MissionPlayer.m_UseIdx.GetNext(existing))!=0) {\r\n\t\tif (m_MissionPlayer.m_Data[existing].m_ulPlayerIndex==ulPlayerIndex && m_MissionPlayer.m_Data[existing].m_ulPlayerID==ulPlayerID) return existing;\r\n\t}\r\n'
  part=part.replace(marker,code+marker,1);data=data[:start]+part+data[end:]
 p.write_bytes(data)
print('VTT mission routing and checked NPC death binding installed.')

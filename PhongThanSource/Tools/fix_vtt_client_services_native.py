"""Bounded native countdown API, mission diagnostics and safe news parsing."""
from restore_original_world import ROOT
import re
p=ROOT/'PhongThanSource/Sources/Core/Src/ScriptFuns.cpp';data=p.read_bytes()
marker=b'int LuaRunMission('
if b'int LuaShowMissionCountdown(' not in data:
 code=b'''// Only the requesting member receives the current native mission timer.
int LuaShowMissionCountdown(Lua_State* L)
{
#ifdef _SERVER
    const int player=GetPlayerIndex(L), world=GetSubWorldIndex(L);
    if(player<=0 || player>=MAX_PLAYER || world<0 || world>=MAX_SUBWORLD) return 0;
    const int id=(int)Lua_ValueToNumber(L,1), timer=(int)Lua_ValueToNumber(L,2);
    if(SubWorld[world].m_SubWorldID!=1066+id || id<1 || id>4 ||
       Player[player].m_nIndex<=0 || Npc[Player[player].m_nIndex].m_SubWorldIndex!=world) return 0;
    KMission* mission=SubWorld[world].m_MissionArray.FindByMissionId(id);
    if(!mission) return 0;
    unsigned long member=0;
    while((member=mission->m_MissionPlayer.m_UseIdx.GetNext(member))!=0)
        if(mission->m_MissionPlayer.m_Data[member].m_ulPlayerIndex==(unsigned long)player &&
           mission->m_MissionPlayer.m_Data[member].m_ulPlayerID==Player[player].m_dwID) break;
    if(!member) return 0;
    const int seconds=(mission->GetTimerRestTimer(timer)+GAME_FPS-1)/GAME_FPS;
    if(seconds<=0 || seconds>3600) return 0;
    KPhongThanScriptAction action;ZeroMemory(&action,sizeof(action));
    action.View=UI_NEWSINFO1;action.OptionCount=NEWSMESSAGE_COUNTING;
    action.Operation=PHONGTHAN_SCRIPT_SHOW;action.ServerOwned=1;
    const char* text=g_TaskGlobalValue[id]==1 ? "[VTT] Chuan bi chien dau: %d giay" : "[VTT] Thoi gian con lai: %d giay";
    strcpy(action.Content,text);const int length=strlen(text);
    memcpy(action.Content+length,&seconds,sizeof(seconds));action.ContentLength=length+sizeof(seconds);
    Player[player].DoScriptAction(&action);
#endif
    return 0;
}

'''.replace(b'\n',b'\r\n')
 data=data.replace(marker,code+marker,1)
 data=data.replace(b'{"GetMSRestTime",',b'{"ShowMissionCountdown", LuaShowMissionCountdown},\r\n\t{"GetMSRestTime",',1)
 p.write_bytes(data)
# Preserve legacy encoding. Both channels must reject absent/out-of-range %d.
for name in ('UiNewsMessage.cpp','UiNewsMessage2.cpp'):
 p=ROOT/'PhongThanSource/Sources/GameClient/Ui/UiCase'/name;data=p.read_bytes()
 data=data.replace(b'm_nInsertPlace < 0 &&',b'm_nInsertPlace < 0 ||')
 data=re.sub(rb'if \(m_pSelf && pMsg &&\s*pMsg->nMsgLen > 0 && pMsg->nMsgLen <= sizeof\(pMsg->sMsg\)\) \{',b'if (pMsg && pMsg->nMsgLen > 0 && pMsg->nMsgLen <= sizeof(pMsg->sMsg)) {\n        if (!m_pSelf) OpenWindow();\n        if (!m_pSelf) return;\n        m_pSelf->Show();',data,count=1)
 assert b'if (!m_pSelf) OpenWindow();' in data,name
 # ResourceText normal packets include a trailing binary repetition count.
 p.write_bytes(data)
p=ROOT/'PhongThanSource/Sources/Core/Src/KMission.cpp';data=p.read_bytes()
if b'VTT_TIMER utc=' not in data:
 needle=b'\t\tpMission->ExecuteScript(szScriptFile, "OnMissionTimer", dwTimerTaskId);'
 replacement=b'''        KSubWorld* world=(KSubWorld*)pMission->GetOwner();
        if(world && world->m_SubWorldID>=1067 && world->m_SubWorldID<=1070){
            FILE* f=fopen("vtt_mission_lifecycle.log","a");if(f){fprintf(f,"VTT_TIMER utc=%ld map=%d mission=%lu timer=%lu tick=%d\\n",(long)time(NULL),world->m_SubWorldID,pMission->GetMissionId(),dwTimerTaskId,g_SubWorldSet.GetGameTime());fclose(f);}
        }
'''.replace(b'\n',b'\r\n')+needle
 assert needle in data;data=data.replace(needle,replacement,1)
 data=data.replace(b'#include "KCore.h"',b'#include "KCore.h"\r\n#include <time.h>',1);p.write_bytes(data)
print('Patched countdown member validation, timer diagnostics, two safe news parsers.')

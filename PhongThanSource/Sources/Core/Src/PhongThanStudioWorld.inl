// Read-only Studio view, captured on the authoritative world thread.
// Opt-in file prevents disk activity when no editor is attached.
#include <stdio.h>
#include <time.h>
static void PhongThanStudioWorldTick()
{
    static DWORD last=0;
    DWORD now=GetTickCount();if(now-last<1500)return;last=now;
    if(GetFileAttributesA("studio_live_world.enabled")==0xffffffffUL)return;
    FILE* file=fopen("studio_live_world.json.tmp","wb");if(!file)return;
    fprintf(file,"{\"Version\":1,\"Pid\":%lu,\"UtcSeconds\":%lu,\"Npcs\":[",
        GetCurrentProcessId(),(unsigned long)time(NULL));
    int index=0,count=0;
    while((index=NpcSet.GetNextIdx(index))!=0) {
        KNpc& npc=Npc[index];
        // Player sessions are deliberately excluded from an NPC editing feed.
        if(npc.m_Kind==kind_player || npc.m_SubWorldIndex<0 || npc.m_SubWorldIndex>=MAX_SUBWORLD || npc.m_RegionIndex<0)continue;
        int x=0,y=0;npc.GetMpsPos(&x,&y);
        fprintf(file,"%s{\"Index\":%d,\"Template\":%d,\"Map\":%d,\"X\":%d,\"Y\":%d,\"Kind\":%d,\"Level\":%d,\"Life\":%d,\"MaxLife\":%d,\"Id\":%lu,\"OriginX\":%d,\"OriginY\":%d,\"AI\":%d,\"Target\":%d,\"Camp\":%d,\"Doing\":%d,\"ActiveSkill\":%d,\"Skill1\":%d,\"SkillLevel1\":%d,\"MissionGroup\":%d,\"FightMode\":%d}",
            count?",":"",index,npc.m_NpcSettingIdx,SubWorld[npc.m_SubWorldIndex].m_SubWorldID,
            x,y,npc.m_Kind,npc.m_Level,npc.m_CurrentLife,npc.m_CurrentLifeMax,npc.m_dwID,npc.m_OriginX,npc.m_OriginY,npc.m_AiMode,npc.m_nPeopleIdx,npc.m_CurrentCamp,npc.m_Doing,npc.m_ActiveSkillID,npc.m_SkillList.m_Skills[1].SkillId,npc.m_SkillList.m_Skills[1].CurrentSkillLevel,npc.m_nMissionGroup,npc.m_FightMode);
        ++count;
    }
    fprintf(file,"]}");int error=ferror(file);if(fclose(file)!=0)error=1;
    if(!error)MoveFileExA("studio_live_world.json.tmp","studio_live_world.json",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
}

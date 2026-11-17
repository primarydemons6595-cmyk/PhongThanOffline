#include <stdio.h>
#include "../../../Headers/PhongThanMovementTrace.h"

// Opt-in bounded local diagnostic. Buffered writes avoid a per-frame open/close.
static FILE* PtMovementFile()
{
    static bool checked=false;
    static FILE* log=0;
    static char buffer[65536];
    if(!checked){
        checked=true;
        FILE* flag=fopen("movement_trace.flag","rb");
        if(flag){
            fclose(flag);char path[100];
            sprintf(path,"movement_trace_%lu.jsonl",GetCurrentProcessId());
            log=fopen(path,"w");if(log)setvbuf(log,buffer,_IOFBF,sizeof(buffer));
        }
    }
    return log;
}
static int s_PtMovementFrames=0;
static unsigned int s_PtMovementLast=0;
static unsigned int s_PtMovementLogicMs=0;
void PhongThanMovementFrame(unsigned int now,unsigned int prepared,unsigned int finished,int count)
{
    FILE* log=PtMovementFile();if(!log||s_PtMovementFrames>=3600)return;
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
    if(actor<=0||actor>=MAX_NPC||Npc[actor].m_RegionIndex<0)return;
    int x,y,rx=0,ry=0,cx,cy,cz;Npc[actor].GetMpsPos(&x,&y);
    Npc[actor].GetNpcRes()->GetRenderPosition(&rx,&ry);
    g_ScenePlace.GetFocusPosition(cx,cy,cz);
    fprintf(log,"{\"event\":\"FRAME\",\"t\":%u,\"gap\":%u,\"pre\":%u,\"draw\":%u,\"logic_ms\":%u,\"x\":%d,\"y\":%d,\"rx\":%d,\"ry\":%d,\"cx\":%d,\"cy\":%d,\"map\":%d,\"region\":%lu,\"doing\":%d,\"mounted\":%d,\"speed\":%d,\"npcs\":%d}\n",
        now,s_PtMovementLast?now-s_PtMovementLast:0,prepared-now,finished-prepared,s_PtMovementLogicMs,
        x,y,rx,ry,cx,cy,SubWorld[0].m_SubWorldID,Npc[actor].m_dwRegionID,
        Npc[actor].m_Doing,Npc[actor].m_bRideHorse,(int)Npc[actor].m_CurrentRunSpeed,count);
    s_PtMovementLast=now;++s_PtMovementFrames;
    if(!(s_PtMovementFrames%30)||s_PtMovementFrames==3600)fflush(log);
}
void PhongThanMovementPosition(int mode,int action,int x,int y,bool ignored,unsigned int stamp,bool sameStep)
{
    FILE* log=PtMovementFile();if(!log||s_PtMovementFrames>=3600)return;
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC)return;
    int lx,ly;Npc[actor].GetMpsPos(&lx,&ly);
    fprintf(log,"{\"event\":\"POSITION\",\"t\":%lu,\"mode\":%d,\"action\":%d,\"x\":%d,\"y\":%d,\"sx\":%d,\"sy\":%d,\"ignored\":%d,\"ack_command\":%u,\"ack_step\":%u,\"local_command\":%u,\"local_step\":%u,\"same_step\":%d}\n",timeGetTime(),mode,action,lx,ly,x,y,ignored,stamp>>16,stamp&65535,(unsigned)Npc[actor].m_PtMoveSequence,(unsigned)Npc[actor].m_PtMoveStep,sameStep);
}
void PhongThanMovementRequest(int mode,int x,int y)
{
    FILE* log=PtMovementFile();if(!log||s_PtMovementFrames>=3600)return;
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC)return;
    int lx,ly;Npc[actor].GetMpsPos(&lx,&ly);
    fprintf(log,"{\"event\":\"MOVE\",\"t\":%lu,\"mode\":%d,\"x\":%d,\"y\":%d,\"tx\":%d,\"ty\":%d}\n",timeGetTime(),mode,lx,ly,x,y);
}

void PhongThanMovementSkill(int id,int x,int y,int incoming,int direct)
{
    FILE* log=PtMovementFile();if(!log||s_PtMovementFrames>=3600)return;
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC)return;
    fprintf(log,"{\"event\":\"SKILL\",\"t\":%lu,\"id\":%d,\"x\":%d,\"y\":%d,\"incoming\":%d,\"direct\":%d,\"doing\":%d,\"fight\":%d}\n",timeGetTime(),id,x,y,incoming,direct,Npc[actor].m_Doing,Npc[actor].m_FightMode);
}

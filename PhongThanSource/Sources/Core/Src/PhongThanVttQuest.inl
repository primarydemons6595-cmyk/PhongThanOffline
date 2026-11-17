// Included after the common authoritative quest payout helper.
#include "PhongThanVttText.h"
namespace PtVtt {
static int Base(int id){return 4600+id*12;}
static int Get(int p,int id,int offset){return Player[p].m_cTask.GetSaveVal(Base(id)+offset);}
static void Set(int p,int id,int offset,int value){Player[p].m_cTask.SetSaveVal(Base(id)+offset,value,TRUE);}
static bool Live(int p){return PhongThanIsLivePlayer(p)&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC;}
static KMission* Mission(int id,int* world=0){
    int w=g_SubWorldSet.SearchWorld(1066+id);if(world)*world=w;
    return w>=0?SubWorld[w].m_MissionArray.FindByMissionId(id):0;
}
static int Generation(KMission* mission){return mission?mission->GetMissionValue(1):0;}
static int Count(int mask){int count=0;for(int n=0;n<4;++n)if(mask&(1<<n))++count;return count;}
static bool Anchor(int p,int id){
    if(!Live(p)||id<1||id>4)return false;
    int npc=Player[p].m_nLastNpcIndex,actor=Player[p].m_nIndex,w=Npc[actor].m_SubWorldIndex;
    if(npc<=0||npc>=MAX_NPC||w<0||SubWorld[w].m_SubWorldID!=1066+id||
        Npc[npc].m_SubWorldIndex!=w||Npc[npc].m_NpcSettingIdx!=766||!Npc[npc].IsAlive())return false;
    int x,y,nx,ny;Npc[actor].GetMpsPos(&x,&y);Npc[npc].GetMpsPos(&nx,&ny);
    return PtEscortNear(x,y,nx,ny,512);
}
static int Stage(int p,int id){
    if(!Get(p,id,0))return 0;
    if(Get(p,id,2))return 3;
    KMission* mission=Mission(id);
    if(!mission||Get(p,id,0)!=Generation(mission)||g_TaskGlobalValue[id]==0)return 4;
    return Get(p,id,1)==15?2:1;
}
static void Journal(int p,int id,int finish=0){
    int stage=Stage(p,id);if(!stage&&!finish)return;
    char text[512];
    if(finish)strcpy(text,PT_VTT_FINISHED);
    else if(stage==1)sprintf(text,PT_VTT_KILL_FOUR,Count(Get(p,id,1)));
    else strcpy(text,stage==2?PT_VTT_KILL_BOSS:stage==3?PT_VTT_CLAIM:PT_VTT_EXPIRED);
    SendPhongThanTaskRecord(p,4000+id,stage,finish?2:1,text);
}
static void Clear(int p,int id){for(int i=0;i<12;++i)Set(p,id,i,0);}
static int Claim(int p,int id){
    if(!Get(p,id,0)||Get(p,id,1)!=15||!Get(p,id,2))return 0;
    PhongThanQuestExchangeRow reward;
    if(!PhongThanDecodeQuestItemTuple(Get(p,id,3),Get(p,id,4),Get(p,id,5),Get(p,id,6),Get(p,id,7),0,reward.item))return 0;
    reward.count=Get(p,id,8);if(reward.count<=0||reward.count>64)return 0;
    int generation=Get(p,id,0);
    int paid=PhongThanQuestPayout(p,Base(id),generation,0,&reward,1,Get(p,id,9),Get(p,id,10),Get(p,id,11));
    if(paid==1){Clear(p,id);Journal(p,id,1);}
    return paid;
}
static int Accept(int p,int id){
    KMission* mission=Mission(id);
    if(!mission||Generation(mission)<=0||(g_TaskGlobalValue[id]!=1&&g_TaskGlobalValue[id]!=2))return -2;
    if(Get(p,id,0))return -3;
    for(int i=11;i<=14;++i)if(mission->GetMissionValue(i))return -5;
    // Pin the configured reward at acceptance. Studio changes apply next run.
    int detail=id==1?78:id==4?41:80,count=id==3?2:1;
    int reward[9]={3,detail,0,0,0,count,0,0,0};
    if(PhongThanEventOwns(id)&&PhongThanEventValue(id,18)>0)
        for(int f=0;f<9;++f)reward[f]=PhongThanEventValue(id,13+f);
    PhongThanQuestItemTuple tuple;
    if(!PhongThanDecodeQuestItemTuple(reward[0],reward[1],reward[2],reward[3],reward[4],0,tuple)||reward[5]<=0||reward[5]>64)return -4;
    Clear(p,id);for(int f=0;f<9;++f)Set(p,id,3+f,reward[f]);
    Set(p,id,0,Generation(mission));Journal(p,id);return 1;
}
static int Kill(int npc){
    if(npc<=0||npc>=MAX_NPC||Npc[npc].IsPlayer()||Npc[npc].m_CurrentLife>0)return 0;
    int world=Npc[npc].m_SubWorldIndex;if(world<0)return 0;
    int id=SubWorld[world].m_SubWorldID-1066;if(id<1||id>4||g_TaskGlobalValue[id]!=2)return 0;
    KMission* mission=Mission(id);if(!mission||!mission->GetMissionNpc_DataIndex(npc,Npc[npc].m_dwID))return 0;
    int first=129+(id-1)*5,slot=Npc[npc].m_NpcSettingIdx-first;
    if(slot<0||slot>4)return 0;
    if(slot<4){
        if(mission->GetMissionValue(21+slot)!=npc||mission->GetMissionValue(11+slot))return 0;
        char one[]="1";mission->SetMission(11+slot,one);
    }else{
        if(mission->GetMissionValue(30)!=npc||mission->GetMissionValue(31)||mission->GetMissionValue(34)!=1)return 0;
        for(int n=11;n<=14;++n)if(!mission->GetMissionValue(n))return 0;
        mission->SetMission(31,"1");
    }
    unsigned long cursor=0;int generation=Generation(mission);
    while((cursor=mission->m_MissionPlayer.m_UseIdx.GetNext(cursor))!=0){
        TMissionPlayerInfo& member=mission->m_MissionPlayer.m_Data[cursor];int p=member.m_ulPlayerIndex;
        if(!Live(p)||Player[p].m_dwID!=member.m_ulPlayerID||Npc[Player[p].m_nIndex].m_SubWorldIndex!=world||Get(p,id,0)!=generation)continue;
        if(slot<4)Set(p,id,1,Get(p,id,1)|(1<<slot));
        else if(Get(p,id,1)==15)Set(p,id,2,1);
        Journal(p,id);
        if(slot==4 && Get(p,id,2))Claim(p,id);
    }
    return 1;
}
}
void PhongThanVttRestore(int player){if(PtVtt::Live(player))for(int id=1;id<=4;++id)PtVtt::Journal(player,id);}
int LuaEventVttQuest(Lua_State* L){
    int p=GetPlayerIndex(L),id=Lua_GetTopIndex(L)>=1?(int)Lua_ValueToNumber(L,1):0;
    int op=Lua_GetTopIndex(L)>=2?(int)Lua_ValueToNumber(L,2):0,result=-1;
    // Read-only book projection for this doctor's exact map, without the
    // interaction distance gate used by accept/cancel/claim mutations.
    if(op==5 && PtVtt::Live(p) && id>=1 && id<=4){
        int actor=Player[p].m_nIndex,npc=Player[p].m_nLastNpcIndex,w=Npc[actor].m_SubWorldIndex;result=0;
        if(w>=0 && w<MAX_SUBWORLD && SubWorld[w].m_SubWorldID==1066+id && npc>0 && npc<MAX_NPC &&
            Npc[npc].m_SubWorldIndex==w && Npc[npc].m_NpcSettingIdx==766){
            int stage=PtVtt::Stage(p,id);
            if(stage)result=stage==3?3:2;
            else {KMission* mission=PtVtt::Mission(id);
                if(mission && PtVtt::Generation(mission)>0 && (g_TaskGlobalValue[id]==1 || g_TaskGlobalValue[id]==2)){
                    result=1;for(int flag=11;flag<=14;++flag)if(mission->GetMissionValue(flag))result=0;
                }
            }
        }
    }
    else if(PtVtt::Anchor(p,id)){
        if(op==0)result=PtVtt::Stage(p,id);
        else if(op==1)result=PtVtt::Accept(p,id);
        else if(op==2){PtVtt::Clear(p,id);PtVtt::Journal(p,id,1);result=1;}
        else if(op==3)result=PtVtt::Claim(p,id);
        else if(op==4)result=PtVtt::Count(PtVtt::Get(p,id,1));
    }
    Lua_PushNumber(L,result);return 1;
}
static void (*g_PtVttGuardCredit)(int,int)=NULL;
int LuaEventVttKill(Lua_State* L){int npc=(int)Lua_ValueToNumber(L,1),result=PtVtt::Kill(npc);if(result&&g_PtVttGuardCredit)g_PtVttGuardCredit(GetPlayerIndex(L),npc);Lua_PushNumber(L,result);return 1;}
int LuaEventVttRefresh(Lua_State* L){
    int p=GetPlayerIndex(L);if(PtVtt::Live(p)){
        int world=Npc[Player[p].m_nIndex].m_SubWorldIndex;
        int id=world>=0?SubWorld[world].m_SubWorldID-1066:0;
        if(id>=1&&id<=4)PtVtt::Journal(p,id);
    }
    return 0;
}
int LuaEventVttPulse(Lua_State* L){
    int world=GetSubWorldIndex(L);
    if(world<0||world>=MAX_SUBWORLD)return 0;
    int id=SubWorld[world].m_SubWorldID-1066;
    if(id<1||id>4||g_TaskGlobalValue[id]!=2)return 0;
    KMission* mission=PtVtt::Mission(id);
    if(mission){char path[96];sprintf(path,"\\script\\phongthan\\van_tien\\mission%02d.lua",id);mission->ExecuteScript(path,"pt_tick",id);}
    return 0;
}

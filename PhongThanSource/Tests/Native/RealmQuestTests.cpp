#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define PT_REALM_QUEST_TEST
enum {MAX_PLAYER=3,MAX_NPC=12,MAX_SUBWORLD=4,kind_dialoger=3,kind_normal=0,
 TASKVALUE_PT_REALM_SIDE=4868,TASKVALUE_PT_REALM_LEVEL=4869};
struct Task{int v[5000];int GetSaveVal(int n){return v[n];}void SetSaveVal(int n,int value,int){v[n]=value;}};
struct Lock{int bLock;};struct Profession{int value;int GetProfession(){return value;}};
struct Actor{int m_SubWorldIndex,m_Kind,m_DialogRadius,m_NpcSettingIdx,m_Level,m_CurrentLife,m_nSex;DWORD m_dwID,m_ActionScriptID;bool IsPlayer(){return m_Kind==1;}} Npc[MAX_NPC];
struct World{int m_SubWorldID;} SubWorld[MAX_SUBWORLD];
int distance=10;struct NpcList{int GetDistance(int,int){return distance;}}NpcSet;
struct KPlayer{int m_PlayerDBLoad,m_nIndex,m_nLastNpcIndex,m_nPaceBarTime,trading,locked,xp,warps;Task m_cTask;Lock lock;Profession m_cProfession;
 bool CheckTrading(){return trading!=0;}int GetLockState(){return locked;}Lock* GetLockMove(){return &lock;}void TobeExchangeServer(int map,int x,int y){assert(map==1073&&x==205*256&&y==242*512);++warps;}} Player[MAX_PLAYER];
DWORD g_FileName2Id(const char* text){DWORD h=2166136261U;for(;*text;++text)h=(h^(unsigned char)*text)*16777619U;return h;}
struct PhongThanQuestItemTuple{int genre,detail,part,level,series;};
struct PhongThanQuestExchangeRow{PhongThanQuestItemTuple item;int count;};
bool room=true;int gifts=0,lastPart=0,lastGenre=0;int records=0;
bool PhongThanDecodeQuestItemTuple(int g,int d,int p,int l,int s,int,PhongThanQuestItemTuple& t){t.genre=g;t.detail=d;t.part=p;t.level=l;t.series=s;return true;}
int PhongThanQuestExchange(int p,int task,int expected,int next,void*,int,PhongThanQuestExchangeRow* reward,int count){
 if(!room||Player[p].m_cTask.GetSaveVal(task)!=expected)return 0;Player[p].m_cTask.SetSaveVal(task,next,TRUE);gifts+=count;lastPart=reward[0].item.part;lastGenre=reward[0].item.genre;return 1;}
int PhongThanQuestPayout(int p,int task,int expected,int next,PhongThanQuestExchangeRow* reward,int count,int xp,int,int){if(!PhongThanQuestExchange(p,task,expected,next,0,0,reward,count))return 0;Player[p].xp+=xp;return 1;}
void SendPhongThanTaskRecord(int,int,int,int,const char*){++records;}
bool PhongThanRealmDestination(int p,unsigned long map){return map==1073&&Player[p].m_cTask.GetSaveVal(4871)>=11;}
#include "../../Sources/Core/Src/PhongThanRealmQuest.inl"
static void Near(int r){
 Npc[1].m_SubWorldIndex=0;Npc[2].m_SubWorldIndex=0;Npc[2].m_dwID=20;Npc[2].m_Kind=3;Npc[2].m_DialogRadius=100;Player[1].m_nLastNpcIndex=2;
 const int maps[]={0,1061,1061,1003,1004,1073,1052},templates[]={0,202,164,764,178,779,780};
 const char* paths[]={"","\\script\\phongthan\\giapsi\\npc_khuong_young.lua","\\script\\phongthan\\giapsi\\npc_dac_young.lua","","\\script\\phongthan\\npc_restore\\1004_07.lua","\\script\\phongthan\\npc_restore\\1073_01.lua","\\script\\phongthan\\npc_restore\\1052_04.lua"};
 SubWorld[0].m_SubWorldID=maps[r];Npc[2].m_NpcSettingIdx=templates[r];Npc[2].m_ActionScriptID=r==3?0x005A11EAUL:g_FileName2Id(paths[r]);assert(PtRealmQuest::Role(1)==r);
}
static void Kill(int tid,int map){SubWorld[0].m_SubWorldID=map;Npc[3].m_SubWorldIndex=0;Npc[3].m_NpcSettingIdx=tid;Npc[3].m_CurrentLife=0;Npc[3].m_Kind=0;PhongThanRealmQuestKill(3,1);}
static int Stage(){return Player[1].m_cTask.GetSaveVal(PtRealmQuest::Stage);}
int main(){for(int profession=0;profession<4;++profession)for(int sex=0;sex<2;++sex)for(int side=1;side<=2;++side){
 ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));gifts=0;room=true;distance=10;
 Player[1].m_PlayerDBLoad=1;Player[1].m_nIndex=1;Player[1].m_cProfession.value=profession;Npc[1].m_Level=99;Npc[1].m_CurrentLife=100;Npc[1].m_nSex=sex;
 Near(1);assert(!PtRealmQuest::Apply(1,1));Npc[1].m_Level=100;assert(!PtRealmQuest::Apply(1,1));
 int mainTask=profession==1?1:profession==2?2:3;
 Player[1].m_cTask.v[mainTask==3?1:3]=90;assert(!PtRealmQuest::Apply(1,1));Player[1].m_cTask.v[mainTask]=90;
 DWORD good=Npc[2].m_ActionScriptID;Npc[2].m_ActionScriptID=1;assert(!PtRealmQuest::Apply(1,1));Npc[2].m_ActionScriptID=good;
 distance=201;assert(!PtRealmQuest::Apply(1,1));distance=10;assert(PtRealmQuest::Apply(1,1)&&Stage()==1);
 assert(!Player[1].m_cTask.v[TASKVALUE_PT_REALM_SIDE]);assert(!PtRealmQuest::Apply(1,11));Near(2);assert(PtRealmQuest::Apply(1,2)&&Stage()==2);
 Near(1);assert(PtRealmQuest::Apply(1,3)&&Stage()==3);Kill(97,1065);assert(Stage()==3);Kill(96,1055);assert(Stage()==3);Kill(96,1065);assert(Stage()==4);
 Near(1);room=false;assert(!PtRealmQuest::Apply(1,4)&&Stage()==4);room=true;assert(PtRealmQuest::Apply(1,4)&&Stage()==5&&gifts==1&&lastPart==431&&lastGenre==6&&Player[1].xp==20000000);
 assert(!PtRealmQuest::Apply(1,4)&&gifts==1);Npc[1].m_Level=109;assert(!PtRealmQuest::Apply(1,5));Npc[1].m_Level=110;assert(PtRealmQuest::Apply(1,5)&&Stage()==6);
 Near(3);assert(!PtRealmQuest::Apply(1,7));assert(PtRealmQuest::Apply(1,6)&&Stage()==6);assert(PtRealmQuest::Apply(1,6)&&Stage()==6);Near(4);assert(PtRealmQuest::Apply(1,6)&&Stage()==7);
 Near(side+2);assert(PtRealmQuest::Apply(1,7)&&Stage()==8);Kill(97,1065);assert(Stage()==8);Kill(98,1055);assert(Stage()==8);Kill(97,1055);assert(Stage()==9);
 Near(5-side);assert(!PtRealmQuest::Apply(1,8));Near(side+2);assert(PtRealmQuest::Apply(1,8)&&Stage()==10);
 Npc[1].m_Level=120;assert(!PtRealmQuest::Apply(1,9));Npc[1].m_Level=121;assert(PtRealmQuest::Apply(1,9)&&Stage()==11);
 Near(6);assert(PtRealmQuest::Apply(1,12)&&Player[1].warps==1);Near(5);assert(PtRealmQuest::Apply(1,10)&&Stage()==12);
 Near(5-side);assert(!PtRealmQuest::Apply(1,11));Near(side+2);room=false;assert(!PtRealmQuest::Apply(1,11)&&Stage()==12&&!Player[1].m_cTask.v[TASKVALUE_PT_REALM_SIDE]);room=true;
 assert(PtRealmQuest::Apply(1,11)&&Stage()==13&&gifts==2);assert(lastPart==(profession==0?(sex?62:35):profession==1?36:profession==2?37:324));
 assert(Player[1].m_cTask.v[TASKVALUE_PT_REALM_SIDE]==side&&Player[1].m_cTask.v[TASKVALUE_PT_REALM_LEVEL]==1);assert(!PtRealmQuest::Apply(1,11)&&gifts==2);
 }
 puts("PASS actual personal realm progression: 16 class/sex/side paths; prerequisites, NPC identity/distance, kill map/type, bag rollback, exact reward and duplicate prevention");return 0;
}

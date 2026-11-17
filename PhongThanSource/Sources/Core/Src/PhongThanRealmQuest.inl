// Personal ascension chain, reconstructed from the VNG quest descriptions.
// The old NPC functions remain intact. No handbook action grants this state.
#include "../../../Headers/PhongThanRealmQuestText.h"
#include "PhongThanRealm.h"
#ifndef PT_REALM_QUEST_TEST
#include "PhongThanRealmWorld.inl"
namespace PtRealmDaily {static void Tick();static void Kill(int,int);}
void PhongThanRealmIslandKill(int,int);
void PhongThanRealmIslandTick();
void PhongThanRealmServicesTick();
#endif
namespace PtRealmQuest {
enum {Stage=4871,Choice=4872,Visits=4873};
static int Get(int p,int t){return Player[p].m_cTask.GetSaveVal(t);}
static void Set(int p,int t,int v){Player[p].m_cTask.SetSaveVal(t,v,TRUE);}
static void Journal(int p){int s=Get(p,Stage);if(s<1||s>13)return;
 int header=s<=5?3086:s<=10?3087:3088;
 SendPhongThanTaskRecord(p,header,s,(s==5||s==10||s==13)?2:1,PT_REALM_STEPS[s][0]?PT_REALM_STEPS[s]:" ");
}
static int Role(int p){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad)return 0;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex;
 if(a<=0||a>=MAX_NPC||n<=0||n>=MAX_NPC||!Npc[n].m_dwID||Npc[n].m_Kind!=kind_dialoger||
  Npc[a].m_SubWorldIndex<0||Npc[a].m_SubWorldIndex>=MAX_SUBWORLD||Npc[a].m_SubWorldIndex!=Npc[n].m_SubWorldIndex||
  Npc[n].m_DialogRadius<=0||NpcSet.GetDistance(a,n)>Npc[n].m_DialogRadius*2)return 0;
 int map=SubWorld[Npc[a].m_SubWorldIndex].m_SubWorldID;DWORD script=Npc[n].m_ActionScriptID;int t=Npc[n].m_NpcSettingIdx;
 if(map==1061&&t==202&&script==g_FileName2Id("\\script\\phongthan\\giapsi\\npc_khuong_young.lua"))return 1;
 if(map==1061&&t==164&&script==g_FileName2Id("\\script\\phongthan\\giapsi\\npc_dac_young.lua"))return 2;
 if(map==1003&&t==764&&script==0x005A11EAUL)return 3;
 if(map==1004&&t==178&&script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1004_07.lua"))return 4;
 if(map==1073&&t==779&&script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1073_01.lua"))return 5;
 if(map==1052&&t==780&&script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1052_04.lua"))return 6;
 if(map==1052&&t==513&&script==g_FileName2Id("\\script\\phongthan\\realm\\portal.lua"))return 6;
 if(map==1052&&t==757&&script==g_FileName2Id("\\script\\phongthan\\realm\\linh_quang.lua"))return 7;
 if(map==1052&&t==165&&script==0xB64562B0UL)return 8;
 if(map==1065&&t==1453&&script==g_FileName2Id("\\script\\phongthan\\realm\\boat_tran_duong.lua"))return 9;
 if(map==1052&&t==165&&script==g_FileName2Id("\\script\\\xD1\xFE\xB3\xD8\\\xC7\xED\xCF\xF6\xC4\xEF\xC4\xEF.lua"))return 11;
 if(map==1055&&t==1453&&script==g_FileName2Id("\\script\\phongthan\\realm\\boat_dong_doanh.lua"))return 10;
 return 0;
}
static int Apply(int p,int operation){
 int role=Role(p);if(!role||operation<1||operation>12)return 0;
 KPlayer& owner=Player[p];int actor=owner.m_nIndex;
 if(owner.CheckTrading()||owner.GetLockState()||owner.GetLockMove()->bLock||owner.m_nPaceBarTime||Npc[actor].m_CurrentLife<=0)return 0;
 int s=Get(p,Stage),side=Get(p,Choice),level=Npc[actor].m_Level,next=-1;
 int profession=owner.m_cProfession.GetProfession(),mainTask=profession==1?1:profession==2?2:3;
 if(operation==1&&role==1&&s==0&&level>=100&&Get(p,mainTask)>=90)next=1;
 if(operation==2&&role==2&&s==1)next=2;
 if(operation==3&&role==1&&s==2)next=3;
 if(operation==4&&role==1&&s==4){
  PhongThanQuestExchangeRow gift;ZeroMemory(&gift,sizeof(gift));
  if(!PhongThanDecodeQuestItemTuple(6,1,431,0,0,0,gift.item))return 0;gift.count=1;
  return PhongThanQuestPayout(p,Stage,4,5,&gift,1,20000000,0,0);
 }
 if(operation==5&&role==1&&s==5&&level>=110)next=6;
 if(operation==6&&(role==3||role==4)&&s==6){int visits=Get(p,Visits)|(role==3?1:2);Set(p,Visits,visits);next=visits==3?7:6;}
 if(operation==7&&(role==3||role==4)&&s==7){Set(p,Choice,role-2);next=8;}
 if(operation==8&&role==side+2&&s==9)next=10;
 if(operation==9&&role==side+2&&s==10&&level>=121)next=11;
 if(operation==10&&role==5&&s==11&&PhongThanRealmDestination(p,1073))next=12;
 if(operation==11&&role==side+2&&s==12&&(side==1||side==2)&&PhongThanRealmDestination(p,1073)){
  int part=-1;
  if(profession==0)part=Npc[actor].m_nSex?62:35;
  else if(profession==1)part=36;else if(profession==2)part=37;else if(profession==3)part=324;
  if(part<0)return 0;
  PhongThanQuestExchangeRow gift;ZeroMemory(&gift,sizeof(gift));
  if(!PhongThanDecodeQuestItemTuple(0,0,part,1,profession,0,gift.item))return 0;gift.count=1;
  if(PhongThanQuestExchange(p,Stage,12,13,0,0,&gift,1)!=1)return 0;
  Set(p,TASKVALUE_PT_REALM_SIDE,side);Set(p,TASKVALUE_PT_REALM_LEVEL,1);return 1;
 }
 if(operation==12&&role==6&&s>=11&&PhongThanRealmDestination(p,1073)){owner.TobeExchangeServer(1073,205*256,242*512);return 1;}
 if(next<0)return 0;Set(p,Stage,next);
#ifndef PT_REALM_QUEST_TEST
 if(next==10)PtRealmWorldRuntime::GrantTitle(p,side==1?2:5);
#endif
 return 1;
}
}
#ifndef PT_REALM_QUEST_TEST
int LuaPtRealmQuest(Lua_State* L){
 int p=GetPlayerIndex(L),op=Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)?(int)Lua_ValueToNumber(L,1):-1;
 int role=PtRealmQuest::Role(p),result=op==0?1:PtRealmQuest::Apply(p,op);
 if(result&&role&&op>0)PtRealmQuest::Journal(p);
 Lua_PushNumber(L,result&&role?1:0);Lua_PushNumber(L,role?PtRealmQuest::Get(p,PtRealmQuest::Stage):0);
 Lua_PushNumber(L,role);Lua_PushNumber(L,role?PtRealmQuest::Get(p,PtRealmQuest::Choice):0);return 4;
}
#endif
void PhongThanRealmQuestKill(int dead,int player){
 using namespace PtRealmQuest;
 if(player<=0||player>=MAX_PLAYER||!Player[player].m_PlayerDBLoad||dead<=0||dead>=MAX_NPC||
    Npc[dead].IsPlayer()||Npc[dead].m_Kind!=kind_normal||Npc[dead].m_CurrentLife>0)return;
 int actor=Player[player].m_nIndex,world=Npc[dead].m_SubWorldIndex;
 if(actor<=0||actor>=MAX_NPC||world<0||world>=MAX_SUBWORLD||Npc[actor].m_SubWorldIndex!=world)return;
#ifndef PT_REALM_QUEST_TEST
 // A personal arrow hunt cannot settle realm/world boss progress.
 if(PhongThanRealmIsHuntNpc(dead)){PhongThanRealmIslandKill(dead,player);return;}
#endif
 int t=Npc[dead].m_NpcSettingIdx,map=SubWorld[world].m_SubWorldID,s=Get(player,Stage);
 if(s==3&&map==1065&&(t==96||t==447)){Set(player,Stage,4);Journal(player);}
 if(s==8&&map==1055&&(t==97||t==448)){Set(player,Stage,9);Journal(player);}
#ifndef PT_REALM_QUEST_TEST
 if((t==97||t==448)&&map==1055)PtRealmWorldRuntime::Dragon(0,player,dead);
 if((t==98||t==449)&&map==1056)PtRealmWorldRuntime::Dragon(1,player,dead);
 if((t==99||t==450)&&map==1054)PtRealmWorldRuntime::Dragon(2,player,dead);
 PtRealmDaily::Kill(dead,player);PhongThanRealmIslandKill(dead,player);
#endif
}
#ifndef PT_REALM_QUEST_TEST
void PhongThanRealmQuestTick(){
 using namespace PtRealmQuest;static DWORD tick=0,spawnTick=0,actors[MAX_PLAYER];DWORD now=GetTickCount();
 if(now-tick<1000)return;tick=now;bool needed[4]={false,false,false,false};
 PtRealmDaily::Tick();PhongThanRealmIslandTick();PhongThanRealmServicesTick();
 if(PtRealmWorldRuntime::Load()){
  PtRealmWorld::State old=PtRealmWorldRuntime::state;int day=PtRealmWorldRuntime::Today();
  if(PtRealmWorld::AdvanceDay(PtRealmWorldRuntime::state,day)&&!PtRealmWorldRuntime::Save())PtRealmWorldRuntime::state=old;
  const PtRealmWorld::State& state=PtRealmWorldRuntime::state;
  int mask=state.Phase==0?(state.Day==day?state.Mask:0):state.Phase==2?(state.FinalDay==day?state.FinalMask:0):7;
  for(int dragon=0;dragon<3;++dragon)if(!(mask&(1<<dragon)))needed[dragon+1]=true;
 }
 for(int p=1;p<MAX_PLAYER;++p){int actor=Player[p].m_nIndex;
  if(!Player[p].m_PlayerDBLoad||actor<=0||actor>=MAX_NPC){actors[p]=0;continue;}
  int s=Get(p,Stage);if(actors[p]!=Npc[actor].m_dwID){actors[p]=Npc[actor].m_dwID;Journal(p);}
  Player[p].m_ItemList.ReleaseUnqualifiedRealmGear();
  int currentWorld=Npc[actor].m_SubWorldIndex;
  if(currentWorld>=0&&currentWorld<MAX_SUBWORLD&&!PhongThanRealmDestination(p,SubWorld[currentWorld].m_SubWorldID)){
   POINT point;if(g_SubWorldSet.GetRevivalPosFromId(1052,0,&point))Npc[actor].ChangeWorld(1052,point.x,point.y);
  }
  if(PtRealmWorldRuntime::Load()){int receipt=PtRealmWorldRuntime::ReceiptIndex(p);if(receipt>=0&&(PtRealmWorldRuntime::receipts[receipt].Flags&1))PtRealmWorldRuntime::GrantTitle(p,1);}
  if(s==3)needed[0]=true;if(s==8)needed[1]=true;
 }
 if(now-spawnTick<30000)return;spawnTick=now;
 for(int target=0;target<4;++target){
  if(!needed[target])continue;int maps[4]={1065,1055,1056,1054};int map=maps[target],tid=96+target,world=g_SubWorldSet.SearchWorld(map);
  if(world<0)continue;bool found=false;
  for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==world&&
    (Npc[n].m_NpcSettingIdx==tid||Npc[n].m_NpcSettingIdx==tid+351)&&Npc[n].m_CurrentLife>0&&Npc[n].IsAlive()&&!PhongThanRealmIsHuntNpc(n))found=true;
  if(found)continue;
  // Authored quest placement: use a walkable point away from service NPCs.
  int x=0,y=0;
  for(int ring=0;ring<=80&&!x;ring+=4)for(int dx=-ring;dx<=ring&&!x;dx+=4)for(int dy=-ring;dy<=ring&&!x;dy+=4){
   int px=200*256+dx*32,py=207*512+dy*32;
   if(SubWorld[world].TestBarrier(px,py)!=Obstacle_NULL)continue;bool safe=true;
   for(int d=NpcSet.GetNextIdx(0);d;d=NpcSet.GetNextIdx(d))if(Npc[d].m_SubWorldIndex==world&&Npc[d].m_Kind==kind_dialoger){
    int nx,ny;Npc[d].GetMpsPos(&nx,&ny);if(abs(nx-px)<800&&abs(ny-py)<1000){safe=false;break;}}
   if(safe){x=px;y=py;}
  }
  if(!x)continue;int spawned=PhongThanSpawnNpc(tid+351,target?110:100,world,x,y,FALSE);
  if(spawned>0){Npc[spawned].m_Kind=kind_normal;Npc[spawned].m_Camp=Npc[spawned].m_CurrentCamp=camp_animal;Npc[spawned].m_AiMode=1;
   Npc[spawned].m_DeathScriptID=0;Npc[spawned].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+18*1800;Npc[spawned].SendSyncData(0,TRUE);}
 }
}
#endif
#ifndef PT_REALM_QUEST_TEST
#include "PhongThanRealmDaily.inl"
#include "PhongThanRealmIsland.inl"
#include "PhongThanIslandFame.inl"
#include "PhongThanRealmServices.inl"
#endif

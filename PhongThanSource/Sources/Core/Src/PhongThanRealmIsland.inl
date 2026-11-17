#include "../../../Headers/PhongThanBoatLanding.h"
#include "../../../Headers/PhongThanIslandContract.h"
#include "../../../Headers/PhongThanIslandHuntPoints.h"
namespace PtRealmIsland {
enum {Lure=PtIslandTask::Lure,Receipt=PtIslandTask::Receipt,HuntX=PtIslandTask::HuntX,HuntY=PtIslandTask::HuntY,Pass=PtIslandTask::Pass};
struct Hunt {DWORD owner,actor,entity;int npc;Hunt():owner(0),actor(0),entity(0),npc(0){}};
static Hunt hunts[MAX_PLAYER];static DWORD questEntities[MAX_NPC];
static int Get(int p,int task){return Player[p].m_cTask.GetSaveVal(task);}
static void Set(int p,int task,int value){Player[p].m_cTask.SetSaveVal(task,value,TRUE);Player[p].m_uMustSave=SAVE_REQUEST;}
static bool Has(int p,int detail,int count){int have=0;for(PlayerItem* item=Player[p].m_ItemList.GetFirstItem();item;item=Player[p].m_ItemList.GetNextItem()){
 int n=item->nIdx;if(n<=0||n>=MAX_ITEM||item->nPlace!=pos_equiproom&&item->nPlace!=pos_equiproomex&&item->nPlace!=pos_ptbag2&&item->nPlace!=pos_ptbag3)continue;
 if(Item[n].GetGenre()==item_materials&&Item[n].GetDetailType()==detail&&!Item[n].GetLock()->IsLock())have+=Item[n].GetStackNum();if(have>=count)return true;}return false;}
static void Migrate(int p){if(Get(p,Receipt))return;int old=Player[p].m_cTask.GetSaveVal(4762);
 if(Get(p,Lure)==0){if(old>=2&&old<=4)Set(p,Lure,old);else if(Has(p,180,1))Set(p,Lure,4);}
 Set(p,Receipt,1);
}
static bool Entity(const Hunt& h){return h.npc>0&&h.npc<MAX_NPC&&Npc[h.npc].m_dwID==h.entity&&Npc[h.npc].m_NpcSettingIdx==97;}
static void Journal(int p){int state=Get(p,Lure);if(state<1||state>5)return;char text[512];
 const char* fixed=state==1?"\x47\x69\x61\x6f\x20\x31\x30\x20\x43\x68\xca\x6e\x20\x54\x68\x69\xaa\x6e\x20\x54\x69\xd4\x6e\x20\x63\x68\x6f\x20\x54\x68\x75\x79\xd2\x6e\x20\x50\x68\x75\x20\xa7\xab\x6e\x67\x20\x44\x6f\x61\x6e\x68\x20\x28\x32\x32\x33\x2c\x31\x39\x37\x29\x20\xae\xd3\x20\x64\xf4\x20\x47\x69\x61\x6f\x20\x4c\x6f\x6e\x67\x2e":state==3?"\xa7\xb7\x20\x74\x69\xaa\x75\x20\x64\x69\xd6\x74\x20\x47\x69\x61\x6f\x20\x4c\x6f\x6e\x67\x2e\x20\x48\xb5\x6e\x68\x20\x74\x72\x61\x6e\x67\x20\xae\xc7\x79\x3b\x20\x68\xb7\x79\x20\x63\x68\xf5\x61\x20\x63\x68\xe7\x20\x72\xe5\x69\x20\x67\xc6\x70\x20\x54\x68\x75\x79\xd2\x6e\x20\x50\x68\x75\x20\xa7\xab\x6e\x67\x20\x44\x6f\x61\x6e\x68\x20\x28\x32\x32\x33\x2c\x31\x39\x37\x29\x20\xae\xd3\x20\x6e\x68\xcb\x6e\x20\x52\xa9\x75\x20\x54\x68\xc7\x6e\x20\x4c\x6f\x6e\x67\x2e":state==4?"\x4d\x61\x6e\x67\x20\x52\xa9\x75\x20\x54\x68\xc7\x6e\x20\x4c\x6f\x6e\x67\x20\x76\xd2\x20\x54\x68\x75\x79\xd2\x6e\x20\x50\x68\x75\x20\xa7\xab\x6e\x67\x20\x44\x6f\x61\x6e\x68\x20\x28\x32\x32\x33\x2c\x31\x39\x37\x29\x20\xae\xd3\x20\x6e\x68\xcb\x6e\x20\x71\x75\x79\xd2\x6e\x20\xae\xd5\x6e\x20\x50\x68\xad\xac\x6e\x67\x20\x54\x72\xad\xee\x6e\x67\x2e":"\xa7\xb7\x20\x6d\xeb\x20\xae\xad\xea\x6e\x67\x20\xae\xd5\x6e\x20\x50\x68\xad\xac\x6e\x67\x20\x54\x72\xad\xee\x6e\x67\x20\x28\x56\xa9\x6e\x20\x4c\x6f\x6e\x67\x20\xa7\xb6\x6f\x29\x2e";
 if(state==2)_snprintf(text,sizeof(text)-1,"\xa7\xb7\x20\x67\x69\x61\x6f\x20\x31\x30\x20\x43\x68\xca\x6e\x20\x54\x68\x69\xaa\x6e\x20\x54\x69\xd4\x6e\x2e\x20\x47\x69\x61\x6f\x20\x4c\x6f\x6e\x67\x20\x78\x75\xca\x74\x20\x68\x69\xd6\x6e\x20\x74\xb9\x69\x20\xa7\xab\x6e\x67\x20\x44\x6f\x61\x6e\x68\x20\x28\x25\x64\x2c\x25\x64\x29\x2e\x20\x48\xb7\x79\x20\xae\xd5\x6e\x20\xae\xe3\x20\x74\x69\xaa\x75\x20\x64\x69\xd6\x74\x20\xae\xd3\x20\x6c\xca\x79\x20\x52\xa9\x75\x20\x54\x68\xc7\x6e\x20\x4c\x6f\x6e\x67\x2e",Get(p,HuntX)/256,Get(p,HuntY)/512);else g_StrCpyLen(text,fixed,sizeof(text));text[sizeof(text)-1]=0;
 SendPhongThanTaskRecord(p,3083,state,state==5?2:1,text);}
static bool ClearLanding(int world,int x,int y){for(int dx=-64;dx<=64;dx+=32)for(int dy=-64;dy<=64;dy+=32)if(SubWorld[world].TestBarrier(x+dx,y+dy)!=Obstacle_NULL)return false;return true;}
static bool Landing(int map,int tx,int ty,int& x,int& y){int world=g_SubWorldSet.SearchWorld(map);if(world<0)return false;
 for(int radius=0;radius<=768;radius+=64)for(int dx=-radius;dx<=radius;dx+=64)for(int dy=-radius;dy<=radius;dy+=64){int px=tx+dx,py=ty+dy;if(ClearLanding(world,px,py)){x=px;y=py;return true;}}return false;}
static bool HuntPoint(int world,int x,int y){for(int dx=-32;dx<=32;dx+=32)for(int dy=-32;dy<=32;dy+=32)if(SubWorld[world].TestBarrier(x+dx,y+dy)!=Obstacle_NULL)return false;return true;}
static bool Summon(int p){int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD||SubWorld[world].m_SubWorldID!=1055||Npc[actor].m_CurrentLife<=0)return false;
 Hunt& h=hunts[p];if(Entity(h)&&Npc[h.npc].m_CurrentLife>0)return true;
 if(Entity(h))PhongThanRemoveNpc(h.npc);h=Hunt();int x=Get(p,HuntX),y=Get(p,HuntY);
 if(!x||!y||!HuntPoint(world,x,y)){
  int start=g_Random(PT_ISLAND_HUNT_POINT_COUNT);bool found=false;
  for(unsigned i=0;i<PT_ISLAND_HUNT_POINT_COUNT;++i){unsigned at=(start+i)%PT_ISLAND_HUNT_POINT_COUNT;int px=ptIslandHuntPoints[at][0],py=ptIslandHuntPoints[at][1];if(HuntPoint(world,px,py)){x=px;y=py;found=true;break;}}
  if(!found)return false;
 }
 int spawned=PhongThanSpawnNpc(97,120,world,x,y,FALSE);if(spawned<=0)return false;
 if(!Npc[spawned].SetPos(x,y)){PhongThanRemoveNpc(spawned);return false;}
 Npc[spawned].m_OriginX=x;Npc[spawned].m_OriginY=y;Npc[spawned].m_Kind=kind_normal;
 Npc[spawned].m_Camp=Npc[spawned].m_CurrentCamp=camp_animal;Npc[spawned].m_AiMode=1;
 Npc[spawned].m_ActionScriptID=Npc[spawned].m_DeathScriptID=Npc[spawned].m_TimerScriptID=0;Npc[spawned].m_nNpcTimeout=0;
 h.owner=Player[p].m_dwID;h.actor=Npc[actor].m_dwID;h.npc=spawned;h.entity=Npc[spawned].m_dwID;questEntities[spawned]=h.entity;
 Set(p,HuntX,x);Set(p,HuntY,y);return true;
}
static void ShowHunt(int p){Hunt& h=hunts[p];if(Entity(h))Npc[h.npc].SendSyncData(0,TRUE);}
static int Claim(int p,int from){PhongThanQuestExchangeRow gift;ZeroMemory(&gift,sizeof(gift));PhongThanDecodeQuestItemTuple(3,180,0,0,0,0,gift.item);gift.count=1;
 int result=PhongThanQuestExchange(p,Lure,from,4,0,0,&gift,1,true);if(result==1)Player[p].m_uMustSave=SAVE_REQUEST;return result==1?1:0;}
static void Combat(int p){Npc[Player[p].m_nIndex].SetFightMode(enumFightActive);Npc[Player[p].m_nIndex].SendSyncData(Player[p].m_nNetConnectIdx,FALSE);}
static int Apply(int p,int op,int role){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].CheckTrading()||Player[p].GetLockState())return 0;
 Migrate(p);int actor=Player[p].m_nIndex,state=Get(p,Lure);if(actor<=0||actor>=MAX_NPC||Npc[actor].m_CurrentLife<=0)return 0;
 if(op==1&&(role==8||role==10)&&Npc[actor].m_Level>=120&&state==0&&!Has(p,180,1)){Set(p,Lure,1);return 1;}
 if(op==2&&role==10&&(state==1||(state==0&&Npc[actor].m_Level>=120&&!Has(p,180,1)))){
  PhongThanQuestExchangeRow need;ZeroMemory(&need,sizeof(need));PhongThanDecodeQuestItemTuple(3,177,0,0,0,0,need.item);need.count=10;
  if(!Has(p,177,10)||!Summon(p))return 0;
  if(PhongThanQuestExchange(p,Lure,state,2,&need,1,0,0,true,0,true)!=1){if(Entity(hunts[p]))PhongThanRemoveNpc(hunts[p].npc);hunts[p]=Hunt();return 0;}
  Player[p].m_uMustSave=SAVE_REQUEST;Combat(p);ShowHunt(p);return 1;
 }
 if(op==3&&(role==8||role==10)&&state==3)return Claim(p,3);
 if(op==7&&role==10&&state==4){
  PhongThanQuestExchangeRow need;ZeroMemory(&need,sizeof(need));PhongThanDecodeQuestItemTuple(3,180,0,0,0,0,need.item);need.count=1;
  if(PhongThanQuestExchange(p,Lure,4,5,&need,1,0,0,true,0,true)!=1)return 0;
  Set(p,Pass,1);Player[p].DirectAddExp(1200000);return 1;
 }
 if(op==4&&role==9){const PtBoatLanding* dock=PtBoatLandingFor(1055);int x,y;if(!Landing(1055,dock->x,dock->y,x,y))return 0;
  if(Player[p].m_ItemList.GetEquipmentMoney()<2000000||!Player[p].Pay(2000000))return 0;
  int result=Npc[actor].ChangeWorld(1055,x,y);if(!result)Player[p].Earn(2000000);else Combat(p);return result?1:0;}
 if(op==5&&role==10&&Npc[actor].m_Level>=120&&(Get(p,Pass)==1||state==5)){int x,y;if(!Landing(1056,200*256,200*512,x,y)||!Npc[actor].ChangeWorld(1056,x,y))return 0;Combat(p);return 1;}
 if(op==6&&role==10){const PtBoatLanding* dock=PtBoatLandingFor(1065);int x,y;if(!Landing(1065,dock->x,dock->y,x,y)||!Npc[actor].ChangeWorld(1065,x,y))return 0;Combat(p);return 1;}
 return 0;
}
}
#ifndef PT_REALM_ISLAND_TEST
static bool PtIslandGoldenPosition(int& x,int& y){for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n)){
 int world=Npc[n].m_SubWorldIndex;if(Npc[n].m_NpcSettingIdx==97&&Npc[n].m_CurrentLife>0&&Npc[n].IsAlive()&&!PhongThanRealmIsHuntNpc(n)&&world>=0&&world<MAX_SUBWORLD&&SubWorld[world].m_SubWorldID==1055){Npc[n].GetMpsPos(&x,&y);return true;}}return false;}
int LuaPtRealmIsland(Lua_State* L){int p=GetPlayerIndex(L),role=PtRealmQuest::Role(p),op=Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)?(int)Lua_ValueToNumber(L,1):-1;
 if(role>=8&&role<=10)PtRealmIsland::Migrate(p);
 int result=op==0?1:PtRealmIsland::Apply(p,op,role);if(role<8||role>10)result=0;if(result&&op>0)PtRealmIsland::Journal(p);
 int x=role>=8&&role<=10?PtRealmIsland::Get(p,PtRealmIsland::HuntX):0,y=role>=8&&role<=10?PtRealmIsland::Get(p,PtRealmIsland::HuntY):0;
 if(op==8){x=y=0;result=role==10&&PtIslandGoldenPosition(x,y)?1:0;}
 Lua_PushNumber(L,result);Lua_PushNumber(L,role>=8&&role<=10?PtRealmIsland::Get(p,PtRealmIsland::Lure):0);
 Lua_PushNumber(L,role>=8&&role<=10&&PtRealmIsland::Has(p,180,1)?1:0);
 Lua_PushNumber(L,x/256);
 Lua_PushNumber(L,y/512);return 5;}
#endif
bool PhongThanRealmIsHuntNpc(int n){return n>0&&n<MAX_NPC&&PtRealmIsland::questEntities[n]==Npc[n].m_dwID&&Npc[n].m_dwID!=0;}
void PhongThanRealmIslandKill(int dead,int player){using namespace PtRealmIsland;
 if(player<=0||player>=MAX_PLAYER||Get(player,Lure)!=2)return;Hunt& h=hunts[player];
 if(h.owner==Player[player].m_dwID&&h.actor==Npc[Player[player].m_nIndex].m_dwID&&Entity(h)&&h.npc==dead){if(!Claim(player,2))Set(player,Lure,3);Journal(player);}}
void PhongThanRealmIslandTick(){using namespace PtRealmIsland;
 for(int p=1;p<MAX_PLAYER;++p){Hunt& h=hunts[p];int actor=Player[p].m_nIndex;bool live=Player[p].m_PlayerDBLoad&&actor>0&&actor<MAX_NPC;
  if(live)Migrate(p);
  if(h.owner&&(!live||h.owner!=Player[p].m_dwID||h.actor!=Npc[actor].m_dwID||Get(p,Lure)!=2)){if(Entity(h))PhongThanRemoveNpc(h.npc);h=Hunt();}
  if(live&&Get(p,Lure)==2&&Npc[actor].m_SubWorldIndex>=0&&SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID==1055){if(!Entity(h)||Npc[h.npc].m_CurrentLife<=0)if(Summon(p))ShowHunt(p);}
  if(live)Journal(p);
 }
}

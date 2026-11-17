#include "../../../Headers/PhongThanIslandContract.h"
namespace PtRealmDaily {
enum {Day=4874,Runs=4875,State=4876,Sequence=4877,Map=4878,End=PtIslandTask::DailyEnd,Kills=PtIslandTask::DailyKills,ToolReceipt=PtIslandTask::DailyToolReceipt,TienCredit=PtIslandTask::TienCredit,MaCredit=PtIslandTask::MaCredit,Goal=5};
struct Session {DWORD owner,actor,circleId,soulId,published,deadIds[Goal];int circle,soul,world;Session(){memset(this,0,sizeof(*this));}};
static Session sessions[MAX_PLAYER];
static int Get(int p,int t){return Player[p].m_cTask.GetSaveVal(t);}
static void Set(int p,int t,int v){Player[p].m_cTask.SetSaveVal(t,v,TRUE);}
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC;}
static void Migrate(int p){if(Get(p,PtIslandTask::DailyMigration))return;
 int old[5]={4764,4765,4766,4767,4768},now[5]={End,Kills,ToolReceipt,TienCredit,MaCredit};
 for(int i=0;i<5;++i){int value=Get(p,old[i]);if(value<0||i==0&&value<1000000000)continue;if(!Get(p,now[i])&&value)Set(p,now[i],value);}
 Set(p,PtIslandTask::DailyMigration,1);Player[p].m_uMustSave=SAVE_REQUEST;
}
static bool Entity(int n,DWORD id,int model){return n>0&&n<MAX_NPC&&Npc[n].m_dwID==id&&Npc[n].m_NpcSettingIdx==model;}
static void Clear(int p){Session& s=sessions[p];if(Entity(s.circle,s.circleId,755))PhongThanRemoveNpc(s.circle);
 if(s.soul>0&&s.soul<MAX_NPC&&Npc[s.soul].m_dwID==s.soulId&&(Npc[s.soul].m_NpcSettingIdx==756||Npc[s.soul].m_NpcSettingIdx==782))PhongThanRemoveNpc(s.soul);s=Session();}
static bool OwnedTool(int p,int id){for(PlayerItem* item=Player[p].m_ItemList.GetFirstItem();item;item=Player[p].m_ItemList.GetNextItem()){
 int n=item->nIdx;if(n<=0||n>=MAX_ITEM)continue;if(item->nPlace!=pos_equiproom&&item->nPlace!=pos_equiproomex&&item->nPlace!=pos_ptbag2&&item->nPlace!=pos_ptbag3&&item->nPlace!=pos_immediacy)continue;
 if(Item[n].GetGenre()==item_magicscript&&Item[n].GetDetailType()==id&&Item[n].GetStackNum()>0&&!Item[n].GetLock()->IsLock())return true;}return false;}
static void ToolRow(int id,PhongThanQuestExchangeRow& row){ZeroMemory(&row,sizeof(row));PhongThanDecodeQuestItemTuple(6,1,id,0,0,0,row.item);row.count=1;}
static bool Tools(int p,int side,int task,int expected,int next,int fee){
 PhongThanQuestExchangeRow gifts[2],needs;int count=0,first=side==1?410:411,second=side==1?412:413;
 if(!OwnedTool(p,first))ToolRow(first,gifts[count++]);if(!OwnedTool(p,second))ToolRow(second,gifts[count++]);
 ZeroMemory(&needs,sizeof(needs));PhongThanDecodeQuestItemTuple(4,47,0,0,0,0,needs.item);needs.count=fee;
 return PhongThanQuestExchange(p,task,expected,next,fee?&needs:0,fee?1:0,gifts,count,true,0,true)==1;
}
static int Doctor(int world){int best=0,score=-2147483647;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==world&&Npc[n].m_Kind==kind_dialoger&&(Npc[n].m_NpcSettingIdx==149||Npc[n].m_NpcSettingIdx==766)){
  int x,y;Npc[n].GetMpsPos(&x,&y);int value=x*2-y;if(value>score){best=n;score=value;}}
 return best;
}
static void Journal(int p,int side){
 if(side<1||side>2)return;int state=Get(p,State),phase=state%10;
 if(!state){SendPhongThanTaskRecord(p,3083+side,0,2," ");return;}
 int world=g_SubWorldSet.SearchWorld(Get(p,Map)),doctor=world>=0?Doctor(world):0,x=0,y=0;
 if(doctor)Npc[doctor].GetMpsPos(&x,&y);char text[512];
 if(phase==5)g_StrCpyLen(text,PT_REALM_REP_RETURN,sizeof(text));else _snprintf(text,sizeof(text)-1,PT_REALM_REP_PROGRESS,x/256,y/512,Get(p,Kills));text[sizeof(text)-1]=0;
 SendPhongThanTaskRecord(p,3083+side,phase,1,text);
}
static int Apply(int p,int operation,int role){
 if(!Live(p)||role<3||role>4||Player[p].CheckTrading()||Player[p].GetLockState()||Get(p,4871)<10)return 0;
 Migrate(p);int side=role-2,state=Get(p,State),today=PtRealmWorldRuntime::Today(),runs=Get(p,Day)==today?Get(p,Runs):0;
 if(operation==1&&state==0){
  int last=PtRealmWorldRuntime::LastReceipt(p),seq=Get(p,Sequence);if(last<0||seq<0||seq>=1000000000||last>=1000000000||runs<0||runs>=4)return 0;
  int world=g_SubWorldSet.SearchWorld(1047);if(world<0||!Doctor(world))return 0;
  if(!Tools(p,side,State,0,side*10+1,runs))return 0;
  Set(p,Day,today);Set(p,Runs,runs+1);Set(p,Sequence,(last>seq?last:seq)+1);Set(p,Map,1047);Set(p,Kills,0);Set(p,End,0);return 1;
 }
 if(operation==2&&state==side*10+5){
  int credit=Get(p,side==1?TienCredit:MaCredit);if(credit<0||credit>1000000000-2)return 0;
  if(!PtRealmWorldRuntime::CompleteReputation(p,Get(p,Sequence)))return 0;
  // Durable world receipt is idempotent if the character save was interrupted.
  Set(p,side==1?TienCredit:MaCredit,credit+2);Set(p,State,0);Set(p,Kills,0);Set(p,End,0);Clear(p);return 1;
 }
 if(operation==3&&state/10==side){int old=Get(p,ToolReceipt);return Tools(p,side,ToolReceipt,old,old==1?0:1,0)?1:0;}
 return 0;
}
static int Use(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_magicscript||Item[item].GetDetailType()<410||Item[item].GetDetailType()>413)return -1;
 if(Live(p))Migrate(p);
 if(!Live(p)||Player[p].CheckTrading()||Player[p].GetLockState()||Item[item].GetLock()->IsLock())return 0;
 int owned=Player[p].m_ItemList.FindSame(item);if(owned<=0)return 0;
 int place=Player[p].m_ItemList.m_Items[owned].nPlace;if(place!=pos_equiproom&&place!=pos_equiproomex&&place!=pos_ptbag2&&place!=pos_ptbag3&&place!=pos_immediacy)return 0;
 int state=Get(p,State),side=state/10,phase=state%10,id=Item[item].GetDetailType();
 if(side<1||side>2||Get(p,4871)<10)return 0;
 KNpc& actor=Npc[Player[p].m_nIndex];int world=actor.m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD||SubWorld[world].m_SubWorldID!=Get(p,Map)||actor.m_CurrentLife<=0)return 0;
 Session& s=sessions[p];
 if(id==(side==1?410:411)){
  if(phase!=1&&phase!=2)return 0;int doctor=Doctor(world);if(!doctor)return 0;
  int x,y,dx,dy;actor.GetMpsPos(&x,&y);Npc[doctor].GetMpsPos(&dx,&dy);
  if(abs(x-dx)>2500||abs(y-dy)>2500||abs(x-dx)<400&&abs(y-dy)<400||SubWorld[world].TestBarrier(x,y)!=Obstacle_NULL)return 0;
  Clear(p);int n=PhongThanSpawnNpc(755,1,world,x,y,FALSE);if(n<=0)return 0;
  s.owner=Player[p].m_dwID;s.actor=actor.m_dwID;s.world=world;s.circle=n;s.circleId=Npc[n].m_dwID;
  Npc[n].m_Kind=kind_dialoger;Npc[n].m_AiMode=0;Npc[n].m_DeathScriptID=0;Npc[n].m_ActionScriptID=0;Npc[n].m_TimerScriptID=0;
  g_StrCpyLen(Npc[n].Name,PT_REALM_CIRCLE_NAME,sizeof(Npc[n].Name));Npc[n].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+300*18;Npc[n].SendSyncData(0,TRUE);s.published=GetTickCount();
  Set(p,State,side*10+2);Set(p,Kills,0);Set(p,End,(int)time(NULL)+300);return 1;
 }
 if(id==(side==1?412:413)&&phase==3&&s.owner==Player[p].m_dwID&&s.actor==actor.m_dwID&&s.world==world&&
   Entity(s.soul,s.soulId,side==1?756:782)&&Get(p,End)>time(NULL)&&NpcSet.GetDistance(Player[p].m_nIndex,s.soul)<=800){Set(p,State,side*10+4);Set(p,End,(int)time(NULL)+300);Npc[s.soul].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+300*18;return 1;}
 return 0;
}
static void Kill(int dead,int p){
 if(!Live(p)||dead<=0||dead>=MAX_NPC)return;int state=Get(p,State),side=state/10;Session& s=sessions[p];
 if(state%10!=2||side<1||side>2||s.owner!=Player[p].m_dwID||s.actor!=Npc[Player[p].m_nIndex].m_dwID||
  !Entity(s.circle,s.circleId,755)||Get(p,End)<=time(NULL)||Npc[Player[p].m_nIndex].m_CurrentLife<=0||Npc[Player[p].m_nIndex].m_SubWorldIndex!=s.world||Npc[dead].m_SubWorldIndex!=s.world||Npc[dead].m_Kind!=kind_normal||Npc[dead].m_CurrentLife>0)return;
 int x,y,cx,cy;Npc[dead].GetMpsPos(&x,&y);Npc[s.circle].GetMpsPos(&cx,&cy);int dx=x-cx,dy=y-cy;
 if(abs(dx)>600||abs(dy)>600||dx*dx+dy*dy>600*600)return;
 int kills=Get(p,Kills);if(kills<0||kills>=Goal||!Npc[dead].m_dwID)return;
 for(int seen=0;seen<kills;++seen)if(s.deadIds[seen]==Npc[dead].m_dwID)return;
 s.deadIds[kills]=Npc[dead].m_dwID;Set(p,Kills,kills+1);
 if(kills+1==Goal){int n=PhongThanSpawnNpc(side==1?756:782,1,s.world,cx,cy,FALSE);if(n<=0){Set(p,Kills,Goal-1);return;}
  s.soul=n;s.soulId=Npc[n].m_dwID;Npc[n].m_Kind=kind_dialoger;Npc[n].m_AiMode=0;Npc[n].m_DeathScriptID=0;Npc[n].m_ActionScriptID=0;Npc[n].m_TimerScriptID=0;
  g_StrCpyLen(Npc[n].Name,side==1?PT_REALM_TIEN_SOUL:PT_REALM_MA_SOUL,sizeof(Npc[n].Name));Npc[n].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+300*18;Npc[n].SendSyncData(0,TRUE);Set(p,State,side*10+3);
 }
}
static void Tick(){
 for(int p=1;p<MAX_PLAYER;++p){Session& s=sessions[p];int state=Live(p)?Get(p,State):0,side=state/10,phase=state%10;
  if(!Live(p)||!state||s.owner&&s.owner!=Player[p].m_dwID){if(s.owner)Clear(p);continue;}
  int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;
  if(phase>=2&&phase<=4&&(s.owner!=Player[p].m_dwID||s.actor!=Npc[actor].m_dwID||world!=s.world||Get(p,End)<=time(NULL)||Npc[actor].m_CurrentLife<=0)){
   Clear(p);Set(p,State,side*10+1);Set(p,Kills,0);Set(p,End,0);continue;
  }
  if(phase==2&&!Entity(s.circle,s.circleId,755)){Clear(p);Set(p,State,side*10+1);Set(p,Kills,0);Set(p,End,0);continue;}
  if(phase>=2&&phase<=4&&GetTickCount()-s.published>=3000){if(Entity(s.circle,s.circleId,755))Npc[s.circle].SendSyncData(0,TRUE);if(Entity(s.soul,s.soulId,side==1?756:782))Npc[s.soul].SendSyncData(0,TRUE);s.published=GetTickCount();}
  Journal(p,side);
  if(phase!=4)continue;
  if(!Entity(s.soul,s.soulId,side==1?756:782)){Clear(p);Set(p,State,side*10+1);Set(p,Kills,0);continue;}
  if(NpcSet.GetDistance(actor,s.soul)>1400)continue;
  int x,y;Npc[actor].GetMpsPos(&x,&y);if(NpcSet.GetDistance(actor,s.soul)>96)Npc[s.soul].SendCommand(do_walk,x,y);
  int doctor=Doctor(world);if(doctor&&NpcSet.GetDistance(doctor,s.soul)<=500&&NpcSet.GetDistance(doctor,actor)<=600){Set(p,State,side*10+5);Clear(p);}
 }
}
}
int PhongThanRealmUseItem(int player,int item){return PtRealmDaily::Use(player,item);}
#ifndef PT_REALM_DAILY_TEST
int LuaPtRealmReputation(Lua_State* L){int p=GetPlayerIndex(L),role=PtRealmQuest::Role(p),op=Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)?(int)Lua_ValueToNumber(L,1):-1;
 if(role>=3&&role<=4&&PtRealmDaily::Live(p))PtRealmDaily::Migrate(p);
 int result=op==0?1:PtRealmDaily::Apply(p,op,role);if(role<3||role>4)result=0;
 if(result&&op>0)PtRealmDaily::Journal(p,role-2);
 Lua_PushNumber(L,result);Lua_PushNumber(L,role>=3&&role<=4?PtRealmDaily::Get(p,PtRealmDaily::State):0);
 int runs=role>=3&&role<=4&&PtRealmDaily::Get(p,PtRealmDaily::Day)==PtRealmWorldRuntime::Today()?PtRealmDaily::Get(p,PtRealmDaily::Runs):0;
 Lua_PushNumber(L,runs);Lua_PushNumber(L,role>=3&&role<=4?PtRealmDaily::Get(p,4871):0);
 int state=role>=3&&role<=4?PtRealmDaily::Get(p,PtRealmDaily::State):0,side=state/10;
 Lua_PushNumber(L,state&&(side==1||side==2)&&(!PtRealmDaily::OwnedTool(p,side==1?410:411)||!PtRealmDaily::OwnedTool(p,side==1?412:413))?1:0);return 5;
}
#endif

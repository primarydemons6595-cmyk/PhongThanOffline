// Server-only progress for the explicitly staged Dao Si/Di Nhan branches.
#include "../../../Headers/PhongThanTwoFactionData.h"
#include <time.h>
#include "../../../Headers/PhongThanNpcDialogBinding.h"
namespace PtTwoFaction {
enum {WolfState=4770,WolfCount=4771,WolfKings=4772};
struct Hunt {DWORD owner,actor,entity;int npc;Hunt(){memset(this,0,sizeof(*this));}};
static Hunt hunts[MAX_PLAYER];
struct Seen {DWORD owner,actor,ids[16];int next;Seen(){memset(this,0,sizeof(*this));}};
static Seen seen[MAX_PLAYER];
static int Get(int p,int t){return Player[p].m_cTask.GetSaveVal(t);}
static void Set(int p,int t,int v){Player[p].m_cTask.SetSaveVal(t,v,TRUE);}
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC;}
static int CountGenre(int p,int detail,int genre){int count=0;for(PlayerItem* it=Player[p].m_ItemList.GetFirstItem();it;it=Player[p].m_ItemList.GetNextItem()){
 if(it->nPlace!=pos_equiproom&&it->nPlace!=pos_equiproomex&&it->nPlace!=pos_ptbag2&&it->nPlace!=pos_ptbag3)continue;
 int n=it->nIdx;if(n>0&&n<MAX_ITEM&&Item[n].GetGenre()==genre&&Item[n].GetDetailType()==detail)count+=Item[n].GetStackNum();}return count;}
static int Count(int p,int detail){return CountGenre(p,detail,item_task);}
static bool Advance(int p,int task,int from,int to,int item,int limit){
 PhongThanQuestExchangeRow gift;ZeroMemory(&gift,sizeof(gift));
 if(item>=0){if(Count(p,item)>=limit)return false;PhongThanDecodeQuestItemTuple(4,item,0,0,0,0,gift.item);gift.count=1;}
 return PhongThanQuestExchange(p,task,from,to,0,0,item>=0?&gift:0,item>=0?1:0,true,0,true)==1;
}
static bool Entity(const Hunt& h){return h.npc>0&&h.npc<MAX_NPC&&Npc[h.npc].m_dwID==h.entity&&Npc[h.npc].m_NpcSettingIdx==606;}
static void Kill(int dead,int p){
 if(!Live(p)||dead<=0||dead>=MAX_NPC||Npc[dead].m_Kind!=kind_normal||Npc[dead].m_CurrentLife>0||Player[p].CheckTrading()||Player[p].GetLockState())return;
 int f=Player[p].m_cProfession.GetProfession(),actor=Player[p].m_nIndex,world=Npc[dead].m_SubWorldIndex;
 if((f!=1&&f!=2)||world<0||world>=MAX_SUBWORLD||Npc[actor].m_SubWorldIndex!=world||Npc[actor].m_CurrentLife<=0)return;
 if(!Npc[dead].m_dwID)return;Seen& record=seen[p];
 if(record.owner!=Player[p].m_dwID||record.actor!=Npc[actor].m_dwID){record=Seen();record.owner=Player[p].m_dwID;record.actor=Npc[actor].m_dwID;}
 for(int visited=0;visited<16;++visited)if(record.ids[visited]==Npc[dead].m_dwID)return;
 record.ids[record.next]=Npc[dead].m_dwID;record.next=(record.next+1)%16;
 int map=SubWorld[world].m_SubWorldID,tid=Npc[dead].m_NpcSettingIdx,task=f==1?1:2;
 if(map==1014&&tid==12&&Get(p,4550)==5&&CountGenre(p,277,item_magicscript)<1){
  PhongThanQuestExchangeRow gift;ZeroMemory(&gift,sizeof(gift));
  PhongThanDecodeQuestItemTuple(6,1,277,1,0,0,gift.item);gift.count=1;
  PhongThanQuestExchange(p,4550,5,5,0,0,&gift,1,true,0,true);
 }
 if(map==(f==1?1008:1011)&&Get(p,4554)==tid&&Get(p,4555)>=0&&Get(p,4555)<40)Set(p,4555,Get(p,4555)+1);
 for(int i=0;i<PT_TWO_KILL_COUNT;++i){const int* row=PT_TWO_KILLS[i];
  if((row[0]==0||row[0]==f)&&tid==row[1]&&map==row[2]){int t=row[3]==0?task:row[3];if(Get(p,t)==row[4])Advance(p,t,row[4],row[5],row[6],row[7]);}}
 if(f==2&&tid==105&&map==1012&&Get(p,35)>=3&&Get(p,35)<=5)Advance(p,35,Get(p,35),Get(p,35)+1,28,3);
 if(map==1070&&Get(p,task)==87&&tid>=541&&tid<=543){
  int bit=1<<(tid-541),mask=Get(p,4562);if(!(mask&bit))Set(p,4562,mask|bit);
  if(Get(p,4562)==7){Set(p,task,88);Set(p,4563,(int)time(NULL)+180);}
 }
 if(map==1070&&tid==2703&&Get(p,task)==88&&Get(p,4563)>=(int)time(NULL))Advance(p,task,88,89,174,1);
 if(f==1&&map==1009&&Get(p,WolfState)==1){
  Hunt& h=hunts[p];
  if(tid==4&&Get(p,WolfCount)<20)Set(p,WolfCount,Get(p,WolfCount)+1);
  if(tid==606&&h.owner==Player[p].m_dwID&&h.actor==Npc[actor].m_dwID&&h.npc==dead&&Entity(h)){
   int kings=Get(p,WolfKings);if(kings>=0&&kings<3){Set(p,WolfKings,kings+1);Set(p,WolfCount,0);if(kings+1==3)Set(p,WolfState,2);}h=Hunt();
  }
 }
}
#ifndef PT_TWO_FACTION_TEST
static bool Point(int world,int tx,int ty,int& x,int& y,int radius=256,bool avoidPeople=false){
 for(int r=0;r<=radius;r+=32)for(int dx=-r;dx<=r;dx+=32)for(int dy=-r;dy<=r;dy+=32){
  int px=tx+dx,py=ty+dy;if(SubWorld[world].TestBarrier(px,py)!=Obstacle_NULL)continue;bool clear=true;
  if(avoidPeople)for(int npc=NpcSet.GetNextIdx(0);npc;npc=NpcSet.GetNextIdx(npc))if(Npc[npc].m_SubWorldIndex==world&&Npc[npc].m_Kind==kind_dialoger){int nx,ny;Npc[npc].GetMpsPos(&nx,&ny);if(abs(nx-px)<192&&abs(ny-py)<256){clear=false;break;}}
  if(clear){x=px;y=py;return true;}}
 return false;
}
static void Services(){
 for(int i=0;i<PT_TWO_SERVICE_COUNT;++i){const PtTwoService& row=PT_TWO_SERVICES[i];int world=g_SubWorldSet.SearchWorld(row.map);if(world<0||!g_GetScript((char*)row.script))continue;
  DWORD hash=g_FileName2Id((char*)row.script);bool exists=false;int found=0;
  for(int existing=NpcSet.GetNextIdx(0);existing;existing=NpcSet.GetNextIdx(existing))if(Npc[existing].m_SubWorldIndex==world&&Npc[existing].m_Kind==kind_dialoger&&Npc[existing].m_NpcSettingIdx==row.tid){int x,y;Npc[existing].GetMpsPos(&x,&y);if((Npc[existing].m_ActionScriptID==hash&&abs(x-row.x*256)<768&&abs(y-row.y*512)<1024)||(abs(x-row.x*256)<256&&abs(y-row.y*512)<512)){exists=true;found=existing;}}
  if(exists){
   // Repair both halves of an already present service binding as well.
   if(PhongThanBindNpcDialogScript(Npc[found],row.script,hash)){
    Npc[found].m_DialogRadius=256;
   }
   continue;
  }
  int x,y;if(!Point(world,row.x*256+128,row.y*512+256,x,y,512,true))continue;
  int n=PhongThanSpawnNpc(row.tid,1,world,x,y,FALSE);if(n<=0)continue;
  Npc[n].m_Kind=kind_dialoger;Npc[n].m_AiMode=0;PhongThanBindNpcDialogScript(Npc[n],row.script,hash);Npc[n].m_DialogRadius=256;Npc[n].m_nNpcTimeout=0;
  Npc[n].m_DeathScriptID=Npc[n].m_TimerScriptID=0;g_StrCpyLen(Npc[n].Name,row.name,sizeof(Npc[n].Name));Npc[n].SendSyncData(0,TRUE);
 }
}
static void Tick(){
 static DWORD last=0,spawnTick=0;DWORD now=GetTickCount();if(now-last<1000)return;last=now;
 bool needed[PT_TWO_SPAWN_COUNT];ZeroMemory(needed,sizeof(needed));
 for(int p=1;p<MAX_PLAYER;++p){Hunt& h=hunts[p];int actor=Player[p].m_nIndex;bool live=Live(p);int f=live?Player[p].m_cProfession.GetProfession():-1;
  if(h.owner&&(!live||f!=1||h.owner!=Player[p].m_dwID||h.actor!=Npc[actor].m_dwID||Get(p,WolfState)!=1||Npc[actor].m_CurrentLife<=0||Npc[actor].m_SubWorldIndex<0||SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID!=1009)){
   if(Entity(h))PhongThanRemoveNpc(h.npc);h=Hunt();}
  if(!live||(f!=1&&f!=2))continue;
  Player[p].ExecuteScript("\\script\\phongthan\\two_faction\\journal.lua","PT2Journal",0);
  for(int q=0;q<PT_TWO_SPAWN_COUNT;++q){const int* row=PT_TWO_SPAWNS[q];if((row[0]==0||row[0]==f)&&Get(p,row[1])==row[2])needed[q]=true;}
  int world=Npc[actor].m_SubWorldIndex;
  if(f!=1||Get(p,WolfState)!=1||Get(p,WolfCount)<20||Npc[actor].m_CurrentLife<=0||world<0||SubWorld[world].m_SubWorldID!=1009||Entity(h))continue;
  int px,py,x,y;Npc[actor].GetMpsPos(&px,&py);if(!Point(world,px+128,py+128,x,y,384))continue;
  int n=PhongThanSpawnNpc(606,20,world,x,y,FALSE);if(n<=0)continue;
  h.owner=Player[p].m_dwID;h.actor=Npc[actor].m_dwID;h.npc=n;h.entity=Npc[n].m_dwID;
  Npc[n].m_Kind=kind_normal;Npc[n].m_Camp=Npc[n].m_CurrentCamp=camp_animal;Npc[n].m_AiMode=1;Npc[n].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+1800*18;
  Npc[n].m_DeathScriptID=0;g_StrCpyLen(Npc[n].Name,PT_TWO_WOLF_KING,sizeof(Npc[n].Name));Npc[n].SendSyncData(0,TRUE);
 }
 if(now-spawnTick<30000)return;spawnTick=now;Services();
 for(int s=0;s<PT_TWO_SPAWN_COUNT;++s){if(!needed[s])continue;const int* row=PT_TWO_SPAWNS[s];int world=g_SubWorldSet.SearchWorld(row[3]);if(world<0)continue;bool exists=false;
  for(int existing=NpcSet.GetNextIdx(0);existing;existing=NpcSet.GetNextIdx(existing))if(Npc[existing].m_SubWorldIndex==world&&Npc[existing].m_NpcSettingIdx==row[4]&&Npc[existing].m_CurrentLife>0){exists=true;g_StrCpyLen(Npc[existing].Name,PT_TWO_SPAWN_NAMES[s],sizeof(Npc[existing].Name));}
  int x,y;if(exists||!Point(world,row[5]*256+128,row[6]*512+256,x,y,512,true))continue;
  int n=PhongThanSpawnNpc(row[4],row[7],world,x,y,FALSE);if(n<=0)continue;
  Npc[n].m_Kind=kind_normal;Npc[n].m_Camp=Npc[n].m_CurrentCamp=camp_animal;Npc[n].m_AiMode=1;Npc[n].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+1800*18;Npc[n].m_DeathScriptID=0;g_StrCpyLen(Npc[n].Name,PT_TWO_SPAWN_NAMES[s],sizeof(Npc[n].Name));Npc[n].SendSyncData(0,TRUE);
 }
}
#endif
}
void PhongThanTwoFactionQuestKill(int dead,int p){PtTwoFaction::Kill(dead,p);}
#ifndef PT_TWO_FACTION_TEST
void PhongThanTwoFactionQuestTick(){PtTwoFaction::Tick();}
int LuaPtTwoFactionNote(Lua_State* L){
 int p=GetPlayerIndex(L);if(!PtTwoFaction::Live(p)||Lua_GetTopIndex(L)!=4||!Lua_IsNumber(L,1)||!Lua_IsNumber(L,2)||!Lua_IsString(L,3)||!Lua_IsNumber(L,4))return 0;
 int f=Player[p].m_cProfession.GetProfession(),id=(int)Lua_ValueToNumber(L,1),action=(int)Lua_ValueToNumber(L,2),step=(int)Lua_ValueToNumber(L,4);const char* text=Lua_ValueToString(L,3);
 if((f!=1&&f!=2)||id<3040||id>3053||(action!=1&&action!=2)||step<0||step>1000||!text||!text[0]||strlen(text)>480)return 0;
 SendPhongThanTaskRecord(p,id,step,action,text);return 0;
}
#endif

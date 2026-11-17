#include "PhongThanGiapGather.h"
#include "../../../Headers/PhongThanGinsengPositions.h"
namespace PtGinseng {
static const char* script="\\script\\phongthan\\giapsi\\ginseng.lua";
enum {Seconds=3,Distance=192};
struct Plant {int index,id;Plant():index(0),id(0){}};
struct Session {DWORD owner,actor;int plant,objectId,world,x,y,deadline,nextSync;Session(){memset(this,0,sizeof(*this));}};
static Plant plants[15];static Session sessions[MAX_PLAYER];
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].IsPlayer();}
static bool PlantLive(int i){int n=plants[i].index;return n>0&&n<MAX_OBJECT&&Object[n].m_nID==plants[i].id&&Object[n].m_nDataID==PT_GINSENG_POSITIONS[i][3]&&Object[n].m_dwScriptID==g_FileName2Id((char*)script);}
static void Clear(int p){
 if(Live(p)&&Player[p].m_dwTaskExcuteScriptId==g_FileName2Id((char*)script)&&!strcmp(Player[p].m_szTaskExcuteFun,"pt_ginseng_finish")){Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=0;Player[p].m_szTaskExcuteFun[0]=0;}
 if(Live(p)&&sessions[p].owner==Player[p].m_dwID&&sessions[p].actor==Npc[Player[p].m_nIndex].m_dwID)Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);
 sessions[p]=Session();
}
static bool Blessed(int p){return FindIBBuffSlot(GetPlayerIBBuffStore(p),336)>=0;}
static bool Valid(int p){
 if(!Live(p))return false;Session& s=sessions[p];int a=Player[p].m_nIndex;
 if(!s.owner||s.owner!=Player[p].m_dwID||s.actor!=Npc[a].m_dwID||s.plant<0||s.plant>=15||!PlantLive(s.plant)||plants[s.plant].id!=s.objectId||
  Npc[a].m_SubWorldIndex!=s.world||Npc[a].m_CurrentLife<=0||Player[p].GetLockState()||Player[p].CheckTrading()||
  Player[p].m_cTask.GetSaveVal(4550)!=(s.plant<10?3:5)||(s.plant>=10&&!Blessed(p))||Player[p].m_dwTaskExcuteScriptId!=g_FileName2Id((char*)script)||strcmp(Player[p].m_szTaskExcuteFun,"pt_ginseng_finish"))return false;
 int x,y;Npc[a].GetMpsPos(&x,&y);return x==s.x&&y==s.y&&Object[plants[s.plant].index].m_nState==OBJ_PROP_STATE_DISPLAY;
}
static int Start(int p,int object){
 if(!Live(p)||Player[p].m_nPaceBarTime||Player[p].GetLockState()||Player[p].CheckTrading())return 0;
 int at=-1;for(int i=0;i<15;++i)if(PlantLive(i)&&plants[i].index==object)at=i;if(at<0||Player[p].m_cTask.GetSaveVal(4550)!=(at<10?3:5)||(at>=10&&!Blessed(p)))return 0;
 KNpc& n=Npc[Player[p].m_nIndex];KObj& obj=Object[object];
 if(n.m_CurrentLife<=0||(n.m_Series<0||n.m_Series>2)||n.m_SubWorldIndex!=obj.m_nSubWorldID||SubWorld[n.m_SubWorldIndex].m_SubWorldID!=PT_GINSENG_POSITIONS[at][2]||obj.m_nState!=OBJ_PROP_STATE_DISPLAY)return 0;
 int x,y,px,py;n.GetMpsPos(&x,&y);obj.GetMpsPos(&px,&py);if(!PtEscortNear(x,y,px,py,Distance))return 0;
 Session s;s.owner=Player[p].m_dwID;s.actor=n.m_dwID;s.plant=at;s.objectId=obj.m_nID;s.world=n.m_SubWorldIndex;s.x=x;s.y=y;s.deadline=g_SubWorldSet.GetGameTime()+Seconds*GAME_FPS;
 sessions[p]=s;Player[p].m_dwTaskExcuteScriptId=g_FileName2Id((char*)script);g_StrCpyLen(Player[p].m_szTaskExcuteFun,"pt_ginseng_finish",sizeof(Player[p].m_szTaskExcuteFun));
 Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=Seconds*GAME_FPS;sessions[p].nextSync=g_SubWorldSet.GetGameTime()+3;Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);return 1;
}
static int Finish(int p){
 if(!Live(p)||!Valid(p)||g_SubWorldSet.GetGameTime()+1<sessions[p].deadline||Player[p].m_nPaceBarTime!=0){if(p>0&&p<MAX_PLAYER&&sessions[p].owner&&!Valid(p))Clear(p);return 0;}
 int at=sessions[p].plant,bit=1<<at,mask=Player[p].m_cTask.GetSaveVal(4553),object=plants[sessions[p].plant].index,result=0;
 {PhongThanQuestExchangeRow reward;ZeroMemory(&reward,sizeof(reward));PhongThanDecodeQuestItemTuple(4,at<10?192:201,0,0,0,0,reward.item);reward.count=1;
  result=PhongThanQuestExchange(p,4553,mask,mask^bit,0,0,&reward,1,true);
  // The task word is a transaction receipt, not permanent plant ownership.
  // Native Prop state synchronizes disappearance and restores the plant
  // after its authored LifeTime (180 game ticks for both herb templates).
  if(result==1)Object[object].SetState(OBJ_PROP_STATE_HIDE);
 }
 Clear(p);return result==1&&at>=10?2:result;
}
static void Plants(){
 for(int i=0;i<15;++i)if(!PlantLive(i)){
  int world=g_SubWorldSet.SearchWorld(PT_GINSENG_POSITIONS[i][2]);if(world<0)continue;
  KMapPos pos;ZeroMemory(&pos,sizeof(pos));pos.nSubWorld=world;SubWorld[world].Mps2Map(PT_GINSENG_POSITIONS[i][0],PT_GINSENG_POSITIONS[i][1],&pos.nRegion,&pos.nMapX,&pos.nMapY,&pos.nOffX,&pos.nOffY);if(pos.nRegion<0)continue;
  KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nMovieFlag=1;int object=ObjSet.Add(PT_GINSENG_POSITIONS[i][3],pos,info);
  if(object>0){Object[object].SetScriptFile((char*)script);plants[i].index=object;plants[i].id=Object[object].m_nID;}
 }
 int world=g_SubWorldSet.SearchWorld(1016);if(world<0)return;
 // Only the two authored guardians beside these plants; leave other monsters
 // and all shared template stats alone. Their old 300px vision missed the row.
 for(int n=NpcSet.GetNextIdx(0);n>0;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==world&&Npc[n].m_NpcSettingIdx==608&&Npc[n].m_Kind==kind_normal){
  int x,y;Npc[n].GetMpsPos(&x,&y);if(x>=195*256&&x<=206*256&&y>=202*512&&y<=206*512){Npc[n].m_CurrentVisionRadius=800;Npc[n].m_AiMode=1;}
 }
}
static void Snapshot(){
 if(GetFileAttributesA("studio_live_world.enabled")==0xffffffffUL)return;
 FILE* f=fopen("studio_live_gather.json.tmp","wb");if(!f)return;fprintf(f,"{\"Version\":1,\"Pid\":%lu,\"ObjectTemplates\":[345,350],\"Plants\":[",GetCurrentProcessId());
 for(int i=0;i<15;++i){int x=0,y=0;if(PlantLive(i))Object[plants[i].index].GetMpsPos(&x,&y);fprintf(f,"%s{\"Slot\":%d,\"Id\":%d,\"Live\":%s,\"Template\":%d,\"Map\":%d,\"X\":%d,\"Y\":%d}",i?",":"",i,plants[i].id,PlantLive(i)?"true":"false",PT_GINSENG_POSITIONS[i][3],PT_GINSENG_POSITIONS[i][2],x,y);}
 fprintf(f,"]}");fclose(f);MoveFileExA("studio_live_gather.json.tmp","studio_live_gather.json",MOVEFILE_REPLACE_EXISTING);
}
}
void PhongThanGiapGatherDamaged(int npc){if(npc<=0||npc>=MAX_NPC)return;for(int p=1;p<MAX_PLAYER;++p)if(PtGinseng::sessions[p].owner&&Player[p].m_nIndex==npc)PtGinseng::Clear(p);}
void PhongThanGiapGatherTick(){
 static DWORD last=0;DWORD now=GetTickCount();if(now-last>=1000){last=now;PtGinseng::Plants();PtGinseng::Snapshot();}
 for(int p=1;p<MAX_PLAYER;++p)if(PtGinseng::sessions[p].owner){
  if(!PtGinseng::Valid(p)||Player[p].m_nPaceBarTime==0)PtGinseng::Clear(p);
  else if(g_SubWorldSet.GetGameTime()>=PtGinseng::sessions[p].nextSync){
   PtGinseng::sessions[p].nextSync=g_SubWorldSet.GetGameTime()+3;
   Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);
  }
 }
}
int LuaGiapGather(Lua_State* L){int p=GetPlayerIndex(L),result=0;if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){double op=Lua_ValueToNumber(L,1);if(op==0)result=PtGinseng::Start(p,GetObjIndex(L));else if(op==1)result=PtGinseng::Finish(p);}Lua_PushNumber(L,result);return 1;}

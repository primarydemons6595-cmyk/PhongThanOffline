// Item-specific travel context: choices never select another copy in a bag.
#include "../../../Headers/PhongThanTravelDestinations.h"
#include "../../../Headers/PhongThanTravelItemPolicy.h"
namespace PtTravel {
struct Pending {DWORD actor,serial,until;int item,world,profile;};
static Pending pending[MAX_PLAYER];
static bool Owned(int p,int item){
 if(p<=0||p>=MAX_PLAYER||item<=0||item>=MAX_ITEM||Player[p].m_nIndex<=0||Player[p].m_nIndex>=MAX_NPC)return false;
 int at=Player[p].m_ItemList.FindSame(item);if(at<=0)return false;
 int place=Player[p].m_ItemList.m_Items[at].nPlace;
 return (place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3||place==pos_immediacy)&&
 !Player[p].GetLockState()&&!Player[p].CheckTrading()&&
 Item[item].GetLock()->nState==LOCK_STATE_NORMAL&&
 (!Item[item].GetExpireTime()||Item[item].GetExpireTime()>KSG_GetCurSec());
}
static bool Allowed(int p,bool commerce=false){
 KNpc& n=Npc[Player[p].m_nIndex];int world=n.m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD||n.m_CurrentLife<=0||n.m_Doing==do_death||n.m_Doing==do_revive||
 (Player[p].GetLockMove()->bLock && Player[p].GetLockMove()->nPlace<=0)||Player[p].m_bForbidUseTownP||!PhongThanEscortMayChangeMap(p))return false;
 PHONGTHAN_PLAYER_NPC_STATE* morph=PhongThanPlayerState(p);if(morph&&morph->nMorphTemplate==364&&!commerce)return false;
 int map=SubWorld[world].m_SubWorldID;
 return map!=1064&&map!=1066&&map!=1071&&map!=1072;
}
static int Profile(KItem& item){return PhongThanTravelProfile(item.GetGenre(),item.GetDetailType(),item.GetParticular());}
static bool Charge(int p,int item){
 KItemList& bag=Player[p].m_ItemList;
 // An IB use-count is a reservoir, even if its original row forbids stacking.
 if(Item[item].GetGenre()==item_ibitem&&Item[item].GetStackNum()>1){
  Item[item].SetStackNum(Item[item].GetStackNum()-1);bag.SyncItem(item);return true;
 }
 return bag.RemoveItem(item,1)!=0;
}
}
int LuaTravelItem(Lua_State* L){
 int p=GetPlayerIndex(L),result=0;int op=(int)Lua_ValueToNumber(L,1),arg=(int)Lua_ValueToNumber(L,2);
 if(p>0&&p<MAX_PLAYER){using namespace PtTravel;Pending& q=pending[p];
  if(op==0){q.serial=0;
   FILE* diag=fopen("travel_item_diag.log","a");if(diag){fprintf(diag,"OPEN player=%d item=%d owned=%d allowed=%d moveLock=%d place=%d forbidden=%d\n",p,arg,Owned(p,arg),Allowed(p),Player[p].GetLockMove()->bLock,Player[p].GetLockMove()->nPlace,Player[p].m_bForbidUseTownP);fclose(diag);}
   if(Owned(p,arg)&&Allowed(p,Profile(Item[arg])==11)){
   int profile=Profile(Item[arg]);if(profile){q.actor=Npc[Player[p].m_nIndex].m_dwID;q.serial=Item[arg].GetID();q.item=arg;
    q.world=Npc[Player[p].m_nIndex].m_SubWorldIndex;q.profile=profile;q.until=GetTickCount()+120000;result=profile;}
  }}else if(op==1&&q.serial&&Owned(p,q.item)&&Allowed(p,q.profile==11)&&q.actor==Npc[Player[p].m_nIndex].m_dwID&&
   q.world==Npc[Player[p].m_nIndex].m_SubWorldIndex&&q.serial==Item[q.item].GetID()&&(int)(q.until-GetTickCount())>=0){
   int map=0,x=0,y=0,fight=1;
   if(q.profile==8){PLAYER_REVIVAL_POS& v=*Player[p].GetDeathRevivalPos();map=v.m_nSubWorldID;x=v.m_nMpsX;y=v.m_nMpsY;fight=0;}
   else if(arg>=1&&arg<=PT_TRAVEL_DESTINATION_COUNT){const PtTravelDestination& d=ptTravelDestinations[arg-1];
    bool match=d.profile==q.profile||(q.profile==9&&d.profile>=2&&d.profile<=5)||(q.profile==10&&d.profile>=6&&d.profile<=7);
    if(q.profile==11){static const int maps[9]={1002,1003,1004,1021,1020,1024,1038,1029,1035};int at=Player[p].m_cTask.GetSaveVal(333);match=at>=1&&at<=9&&d.map==maps[at-1];}
    if(match){map=d.map;x=d.x*32;y=d.y*32;fight=d.fight;}
   }
   if(map&&g_SubWorldSet.SearchWorld(map)>=0){DWORD serial=q.serial;q.serial=0;
    if(Npc[Player[p].m_nIndex].ChangeWorld(map,x,y)){
     if(Item[q.item].GetID()==serial&&Charge(p,q.item)){Npc[Player[p].m_nIndex].SetFightMode(fight);Player[p].m_uMustSave=SAVE_REQUEST;result=1;}
    }
   }
  }
 }
 Lua_PushNumber(L,result);return 1;
}
int PhongThanOpenTravelItem(int player,int item){
 if(!PtTravel::Profile(Item[item]))return -1;
 return Player[player].ExecuteScript(PtTravel::Profile(Item[item])==12?"\\script\\phongthan\\items\\compass.lua":"\\script\\phongthan\\travel_items.lua","main",item)?1:0;
}

namespace PtTravel {
static const int compassTasks[5][3]={{714,715,716},{837,838,839},{840,841,842},{843,844,845},{846,847,848}};
static bool CompassMap(int map){
 if(map<1001||map>1099)return false;
 int c=map-1000;return !(c>=53&&c<=56)&&c!=58&&c!=59&&!(c>=60&&c<=61)&&!(c>=66&&c<=72)&&c!=97&&c!=98;
}
static bool Context(int p){
 if(p<=0||p>=MAX_PLAYER)return false;Pending& q=pending[p];
 return q.profile==12&&q.serial&&Owned(p,q.item)&&Allowed(p)&&q.actor==Npc[Player[p].m_nIndex].m_dwID&&
 q.world==Npc[Player[p].m_nIndex].m_SubWorldIndex&&q.serial==Item[q.item].GetID()&&(int)(q.until-GetTickCount())>=0;
}
// Terrain-only check: do not treat the player standing on the saved cell
// as an obstacle. Closed VTT gates and authored terrain remain blocked.
static bool CompassTerrain(int world,int x,int y){
 if(world<0||world>=MAX_SUBWORLD||x<=0||y<=0)return false;
 int region,cx,cy,ox,oy;SubWorld[world].Mps2Map(x,y,&region,&cx,&cy,&ox,&oy);
 if(region<0)return false;
 return SubWorld[world].m_Region[region].GetBarrierMin(cx,cy,ox,oy,FALSE,TRUE)==0;
}
static int Compass(int p,int op,int slot,int& map,int& x,int& y){
 map=x=y=0;if(slot<1||slot>5||!Context(p))return 0;
 const int* task=compassTasks[slot-1];int a=Player[p].m_nIndex,w=Npc[a].m_SubWorldIndex;
 if(op==1){
  map=SubWorld[w].m_SubWorldID;if(!CompassMap(map))return 0;
  Npc[a].GetMpsPos(&x,&y);if(!CompassTerrain(w,x,y))return 0;
  Player[p].m_cTask.SetSaveVal(task[1],x,TRUE);Player[p].m_cTask.SetSaveVal(task[2],y,TRUE);
  Player[p].m_cTask.SetSaveVal(task[0],map,TRUE);Player[p].m_uMustSave=SAVE_REQUEST;return 1;
 }
 map=Player[p].m_cTask.GetSaveVal(task[0]);x=Player[p].m_cTask.GetSaveVal(task[1]);y=Player[p].m_cTask.GetSaveVal(task[2]);
 if(map>0&&map<100)map+=1000;
 if(op==3){for(int i=0;i<3;++i)Player[p].m_cTask.SetSaveVal(task[i],0,TRUE);Player[p].m_uMustSave=SAVE_REQUEST;map=x=y=0;return 1;}
 if(!CompassMap(map)||x<=0||y<=0)return 0;
 if(op==0)return 1;if(op!=2)return 0;
 int target=g_SubWorldSet.SearchWorld(map);if(target<0||target>=MAX_SUBWORLD||!CompassTerrain(target,x,y))return 0;
 int r,cx,cy,ox,oy;SubWorld[target].Mps2Map(x,y,&r,&cx,&cy,&ox,&oy);if(r<0)return 0;
 Pending& q=pending[p];if(!Npc[a].ChangeWorld(map,x,y))return 0;
 if(!Charge(p,q.item))return 0;
 q.serial=0;Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}
}
int LuaCompassItem(Lua_State* L){
 int p=GetPlayerIndex(L),map=0,x=0,y=0,result=0;
 if(Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,2)){
  double op=Lua_ValueToNumber(L,1),slot=Lua_ValueToNumber(L,2);
  if(op>=0&&op<=3&&op==(int)op&&slot>=1&&slot<=5&&slot==(int)slot)result=PtTravel::Compass(p,(int)op,(int)slot,map,x,y);
 }
 Lua_PushNumber(L,result);Lua_PushNumber(L,map);Lua_PushNumber(L,x/256);Lua_PushNumber(L,y/512);return 4;
}

#include "PhongThanLifeRuntime.h"
#include "PhongThanMiningRuntime.h"
#include "../../../Headers/PhongThanLifeSkills.h"
#include "../../../Headers/PhongThanLifeData.h"
#include "../../../Headers/PhongThanMiningCrater.h"
#include "../../../Headers/PhongThanFishing.h"
#include <time.h>
namespace PtLife {
static const char* HarvestScript="\\script\\phongthan\\life\\harvest.lua";
static const char* MenuScript="\\script\\phongthan\\life\\skills.lua";
struct Site {int index,id;Site():index(0),id(0){}};
struct Session {DWORD owner,actor;int site,objectId,world,x,y,deadline,nextSync;Session(){memset(this,0,sizeof(*this));}};
static Site sites[PT_LIFE_NODE_COUNT];static Session sessions[MAX_PLAYER];
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].IsPlayer();}
static int Get(int p,int key){return Player[p].m_cTask.GetSaveVal(key);}
static void Set(int p,int key,int value){Player[p].m_cTask.SetSaveVal(key,value,TRUE);}
static int Rank(int p,int skill){int rank=Get(p,PT_LIFE_LEVEL_BEGIN+skill);if(skill==3)rank=max(rank,Npc[Player[p].m_nIndex].m_SkillList.GetLevel(62));return max(0,min(10,rank));}
static const int* Limits(int p){return PT_LIFE_LIMITS[max(0,min(200,Npc[Player[p].m_nIndex].m_Level))];}
static void Refresh(int p){
 if(!Live(p))return;const int* caps=Limits(p);int now=(int)time(NULL),last=Get(p,PT_LIFE_REFRESH);if(now<=0)return;
 if(last<=0){Set(p,PT_LIFE_ENERGY,caps[0]);Set(p,PT_LIFE_STRENGTH,caps[1]);Set(p,PT_LIFE_REFRESH,now);return;}
 if(last>now){Set(p,PT_LIFE_REFRESH,now);return;}
 int steps=(now-last)/600;
 if(steps>0){
  int energy=max(0,min(caps[0],Get(p,PT_LIFE_ENERGY))),strength=max(0,Get(p,PT_LIFE_STRENGTH));
  Set(p,PT_LIFE_ENERGY,(int)min((__int64)caps[0],(__int64)energy+(__int64)steps*caps[2]));
  if(strength<caps[1])Set(p,PT_LIFE_STRENGTH,(int)min((__int64)caps[1],(__int64)strength+(__int64)steps*caps[3]));
  Set(p,PT_LIFE_REFRESH,last+steps*600);
 }
}
static bool Busy(int p){return !Live(p)||Player[p].CheckTrading()||Player[p].GetLockState()||Npc[Player[p].m_nIndex].m_CurrentLife<=0||Player[p].m_nPaceBarTime>0;}
static bool Near(int x,int y,int px,int py,int distance){__int64 dx=(__int64)x-px,dy=(__int64)y-py;return dx*dx+dy*dy<=(__int64)distance*distance;}
static bool MiningPick(int p){int item=Player[p].m_ItemList.GetEquipment(itempart_weapon);return item>0&&item<MAX_ITEM&&Item[item].GetID()&&Item[item].GetGenre()==item_equip&&Item[item].GetDetailType()==0&&Item[item].GetParticular()==8;}
static bool FishingRod(int p){int item=Player[p].m_ItemList.GetEquipment(itempart_weapon);return item>0&&item<MAX_ITEM&&Item[item].GetID()&&Item[item].GetGenre()==item_equip&&Item[item].GetDetailType()==0&&Item[item].GetParticular()==PT_FISHING_ROD_PARTICULAR;}
static bool Trainer(int p,int skill=0){
 if(!Live(p))return false;int n=Player[p].m_nLastNpcIndex,a=Player[p].m_nIndex;
 if(n<=0||n>=MAX_NPC||Npc[n].m_Kind!=kind_dialoger||Npc[n].m_SubWorldIndex!=Npc[a].m_SubWorldIndex)return false;
 int world=Npc[a].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD)return false;
 int map=SubWorld[world].m_SubWorldID,tid=Npc[n].m_NpcSettingIdx;
 if(!PhongThanLifeTrainerAllows(map,tid,skill))return false;
 if(Npc[a].m_ActionScriptID!=Npc[n].m_ActionScriptID)return false;
 int x,y,px,py;Npc[a].GetMpsPos(&x,&y);Npc[n].GetMpsPos(&px,&py);return Near(x,y,px,py,256);
}
static bool SiteLive(unsigned int at){int n=sites[at].index;return n>0&&n<MAX_OBJECT&&Object[n].m_nID==sites[at].id&&Object[n].m_nDataID==PT_LIFE_NODES[at].object&&Object[n].m_dwScriptID==g_FileName2Id((char*)HarvestScript);}
static void Clear(int p){
 if(Live(p)&&sessions[p].owner==Player[p].m_dwID&&sessions[p].actor==Npc[Player[p].m_nIndex].m_dwID&&Player[p].m_dwTaskExcuteScriptId==g_FileName2Id((char*)HarvestScript)&&!strcmp(Player[p].m_szTaskExcuteFun,"pt_life_gather_finish")){
  Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=0;Player[p].m_szTaskExcuteFun[0]=0;
  Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);
 }
 sessions[p]=Session();
}
static bool Valid(int p){
 if(!Live(p))return false;Session& s=sessions[p];int a=Player[p].m_nIndex;
 if(!s.owner||s.owner!=Player[p].m_dwID||s.actor!=Npc[a].m_dwID||s.site<0||s.site>=(int)PT_LIFE_NODE_COUNT||!SiteLive(s.site)||sites[s.site].id!=s.objectId||
  Npc[a].m_SubWorldIndex!=s.world||Npc[a].m_CurrentLife<=0||Player[p].CheckTrading()||Player[p].GetLockState()||
  Player[p].m_dwTaskExcuteScriptId!=g_FileName2Id((char*)HarvestScript)||strcmp(Player[p].m_szTaskExcuteFun,"pt_life_gather_finish"))return false;
 if(PT_LIFE_NODES[s.site].skill==4&&!FishingRod(p))return false;
 int x,y;Npc[a].GetMpsPos(&x,&y);return x==s.x&&y==s.y&&Object[sites[s.site].index].m_nState==OBJ_PROP_STATE_DISPLAY;
}
static int Start(int p,int object){
 if(Busy(p))return -5;int at=-1;for(unsigned int i=0;i<PT_LIFE_NODE_COUNT;++i)if(SiteLive(i)&&sites[i].index==object)at=i;if(at<0)return 0;
 const PT_LIFE_NODE& node=PT_LIFE_NODES[at];if(Rank(p,node.skill)<node.rank)return -2;
 if(node.skill==4&&!FishingRod(p))return -8;
 if(node.detail==3&&(!MiningPick(p)||Npc[Player[p].m_nIndex].m_SkillList.GetLevel(62)<=0))return -7;
 Refresh(p);int cost=10+node.rank*2;if(Get(p,PT_LIFE_STRENGTH)<cost)return -3;
 KNpc& actor=Npc[Player[p].m_nIndex];KObj& obj=Object[object];
 if(actor.m_SubWorldIndex!=obj.m_nSubWorldID||obj.m_nState!=OBJ_PROP_STATE_DISPLAY)return 0;
 int x,y,px,py;actor.GetMpsPos(&x,&y);obj.GetMpsPos(&px,&py);if(!Near(x,y,px,py,node.skill==4?PT_FISHING_CAST_REACH:PhongThanIsMiningCrater(node.object)?PT_MINING_CRATER_REACH:192))return -4;
 Session s;s.owner=Player[p].m_dwID;s.actor=actor.m_dwID;s.site=at;s.objectId=obj.m_nID;s.world=actor.m_SubWorldIndex;s.x=x;s.y=y;
 s.deadline=g_SubWorldSet.GetGameTime()+3*GAME_FPS;s.nextSync=g_SubWorldSet.GetGameTime()+3;sessions[p]=s;
 Player[p].m_dwTaskExcuteScriptId=g_FileName2Id((char*)HarvestScript);
 g_StrCpyLen(Player[p].m_szTaskExcuteFun,"pt_life_gather_finish",sizeof(Player[p].m_szTaskExcuteFun));
 Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=3*GAME_FPS;actor.SendSyncData(0,TRUE);return 1;
}
static int Finish(int p){
 if(!Live(p)||!Valid(p)||g_SubWorldSet.GetGameTime()+1<sessions[p].deadline||Player[p].m_nPaceBarTime!=0)return 0;
 int at=sessions[p].site,object=sites[at].index;const PT_LIFE_NODE& node=PT_LIFE_NODES[at];
 int rank=Rank(p,node.skill),cost=10+node.rank*2;if(rank<node.rank||Get(p,PT_LIFE_STRENGTH)<cost){Clear(p);return -3;}
 if(node.detail==3&&(!MiningPick(p)||Npc[Player[p].m_nIndex].m_SkillList.GetLevel(62)<=0)){Clear(p);return -7;}
 PhongThanQuestExchangeRow reward[2];ZeroMemory(reward,sizeof(reward));reward[0].count=1;int count=1;
 if(!PhongThanDecodeQuestItemTuple(3,node.detail,0,0,0,0,reward[0].item)){Clear(p);return 0;}
 if(node.detail==3){
  // The original teacher names these three extra metals. The chance is an
  // explicit Loose policy, not presented as a recovered VNG probability.
  static int extraChance=-1;if(extraChance<0){KIniFile config;extraChance=0;if(config.Load("\\settings\\phongthan\\LifeMiningLoot.ini"))config.GetInteger("Mining","ExtraChancePer10000",0,&extraChance);extraChance=max(0,min(10000,extraChance));}
  if(g_Random(10000)<extraChance){int detail=928+g_Random(3);if(PhongThanDecodeQuestItemTuple(3,detail,0,0,0,0,reward[1].item)){reward[1].count=1;count=2;}}
 }
 int serial=Get(p,PT_LIFE_RECEIPT),next=serial==0?1:0;
 int result=PhongThanQuestExchange(p,PT_LIFE_RECEIPT,serial,next,0,0,reward,count,true,0,true);
 if(result==1){
  Set(p,PT_LIFE_STRENGTH,Get(p,PT_LIFE_STRENGTH)-cost);
  // Authored harvest gain mirrors the existing 250/170/90 recipe falloff.
  int gain=max(0,250-80*(rank-node.rank));Set(p,PT_LIFE_EXP_BEGIN+node.skill,min(10000,Get(p,PT_LIFE_EXP_BEGIN+node.skill)+gain));
  if(node.detail==3)Object[object].m_nLifeTime=PhongThanIsMiningCrater(node.object)?PT_MINING_CRATER_RESPAWN_TICKS:3*GAME_FPS;
  Object[object].SetState(OBJ_PROP_STATE_HIDE);
 }
 Clear(p);return result==1?1:-6;
}
static void Spawn(){
 if(!g_GetScript((char*)HarvestScript))ReLoadScript((char*)HarvestScript);
 for(unsigned int i=0;i<PT_LIFE_NODE_COUNT;++i)if(!SiteLive(i)){
  const PT_LIFE_NODE& node=PT_LIFE_NODES[i];if(PhongThanIsMiningCrater(node.object))continue;int world=PhongThanGatherSpawnWorld(node.map);if(world<0)continue;
  KMapPos pos;ZeroMemory(&pos,sizeof(pos));pos.nSubWorld=world;
  SubWorld[world].Mps2Map(node.x,node.y,&pos.nRegion,&pos.nMapX,&pos.nMapY,&pos.nOffX,&pos.nOffY);if(pos.nRegion<0)continue;
  KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nMovieFlag=1;
  int n=ObjSet.Add(node.object,pos,info);if(n<=0)continue;
  Object[n].SetScriptFile((char*)HarvestScript);Object[n].m_nLifeTime=PhongThanIsMiningCrater(node.object)?PT_MINING_CRATER_RESPAWN_TICKS:2600;
  g_StrCpyLen(Object[n].m_szName,node.name,sizeof(Object[n].m_szName));sites[i].index=n;sites[i].id=Object[n].m_nID;
 }
}
}
bool PhongThanLifeGatherDamaged(int actor){PhongThanMiningDamaged(actor);bool handled=false;if(actor<=0||actor>=MAX_NPC)return false;
 for(int p=1;p<MAX_PLAYER;++p)if(PtLife::sessions[p].owner&&Player[p].m_nIndex==actor){
  if(PtLife::sessions[p].owner==Player[p].m_dwID&&PtLife::sessions[p].actor==Npc[actor].m_dwID)handled=true;
  PtLife::Clear(p);
 }return handled;
}
void PhongThanLifeTick(){
 PhongThanMiningTick();
 static DWORD last=0;DWORD now=GetTickCount();if(now-last>=1000){last=now;PtLife::Spawn();for(int p=1;p<MAX_PLAYER;++p)if(PtLife::Live(p))PtLife::Refresh(p);}
 for(int p=1;p<MAX_PLAYER;++p)if(PtLife::sessions[p].owner){
  if(!PtLife::Valid(p)||Player[p].m_nPaceBarTime==0)PtLife::Clear(p);
  else if(g_SubWorldSet.GetGameTime()>=PtLife::sessions[p].nextSync){PtLife::sessions[p].nextSync=g_SubWorldSet.GetGameTime()+3;Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);}
 }
}
bool PhongThanLifeOpenMenu(int p,int skill){
 if(PtLife::Busy(p)||!PhongThanIsLifeProfession(skill))return false;PtLife::Refresh(p);
 if(!g_GetScript((char*)PtLife::MenuScript)&&!ReLoadScript((char*)PtLife::MenuScript))return false;
 Player[p].m_nLastNpcIndex=0;return Player[p].ExecuteScript((char*)PtLife::MenuScript,"pt_life_open",skill)!=FALSE;
}
bool PhongThanLifeMake(int p,int recipe){
 if(recipe<1||recipe>128||PtLife::Busy(p))return false;
 if(!g_GetScript((char*)PtLife::MenuScript)&&!ReLoadScript((char*)PtLife::MenuScript))return false;
 return Player[p].ExecuteScript((char*)PtLife::MenuScript,"pt_life_gui_make",recipe)!=FALSE;
}
int LuaLifeBusy(Lua_State* L){Lua_PushNumber(L,PtLife::Busy(GetPlayerIndex(L))?1:0);return 1;}
int LuaLifeTrainerNear(Lua_State* L){int skill=0;if(Lua_GetTopIndex(L)>0){double v=Lua_ValueToNumber(L,1);if(!Lua_IsNumber(L,1)||v<0||v>10||v!=(int)v){Lua_PushNumber(L,0);return 1;}skill=(int)v;}Lua_PushNumber(L,PtLife::Trainer(GetPlayerIndex(L),skill)?1:0);return 1;}
int LuaLifeGather(Lua_State* L){int p=GetPlayerIndex(L),result=0;if(PtLife::Live(p)&&Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){
 double op=Lua_ValueToNumber(L,1);if(op==0)result=PtLife::Start(p,GetObjIndex(L));else if(op==1)result=PtLife::Finish(p);
 }Lua_PushNumber(L,result);return 1;}
int LuaGetLiveSkillProficiencyCompat(Lua_State* L){int p=GetPlayerIndex(L),skill=(int)Lua_ValueToNumber(L,1),v=0;
 if(PtLife::Live(p)&&PhongThanIsLifeProfession(skill))v=max(0,min(10000,PtLife::Get(p,PT_LIFE_EXP_BEGIN+skill)));Lua_PushNumber(L,v);return 1;
}
int LuaAddLiveSkillCompat(Lua_State* L){
 int p=GetPlayerIndex(L),result=0;double sd=Lua_ValueToNumber(L,1),rd=Lua_ValueToNumber(L,2);int skill=(int)sd,rank=(int)rd;
 if(Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,2)&&sd==skill&&rd==rank&&PhongThanIsLifeProfession(skill)&&rank>=1&&rank<=10&&
  !PtLife::Busy(p)&&PtLife::Trainer(p,skill)&&rank==PtLife::Rank(p,skill)+1&&Npc[Player[p].m_nIndex].m_Level>=PhongThanLifeRequiredLevel(rank)&&
  (rank==1||PtLife::Get(p,PT_LIFE_EXP_BEGIN+skill)>=10000)){
  PtLife::Set(p,PT_LIFE_LEVEL_BEGIN+skill,rank);PtLife::Set(p,PT_LIFE_EXP_BEGIN+skill,0);result=1;
 }Lua_PushNumber(L,result);return 1;
}
int LuaLifeState(Lua_State* L){
 int p=GetPlayerIndex(L),result=0;double kd=Lua_ValueToNumber(L,1);int key=(int)kd;
 bool allowed=(key>PT_LIFE_EXP_BEGIN&&key<=PT_LIFE_EXP_BEGIN+10)||(key>=PT_LIFE_RECEIPT&&key<=PT_LIFE_EXPECTED_RANK)||(key>PT_LIFE_FORMULA_BEGIN&&key<=PT_LIFE_FORMULA_BEGIN+128);
 if(PtLife::Live(p)&&Lua_IsNumber(L,1)&&kd==key&&allowed){
  PtLife::Refresh(p);
  if(Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,2)){
   double vd=Lua_ValueToNumber(L,2);if(vd>=-1&&vd<=2147483647.0&&vd==(int)vd){int v=(int)vd;
    if(key>PT_LIFE_EXP_BEGIN&&key<=PT_LIFE_EXP_BEGIN+10)v=max(0,min(10000,v));
    else if(key==PT_LIFE_ENERGY)v=max(0,min(PtLife::Limits(p)[0],v));
    else if(key==PT_LIFE_STRENGTH)v=max(0,min(PtLife::Limits(p)[1],v));
    else if(key==PT_LIFE_EXPECTED_RANK)v=max(-1,min(10,v));
    else v=max(0,v);
    PtLife::Set(p,key,v);
   }
  }
  result=PtLife::Get(p,key);
 }Lua_PushNumber(L,result);return 1;
}

bool PhongThanLifeGrantStrength(int p,int amount){
 if(PtLife::Busy(p)||(amount!=100&&amount!=1000))return false;
 PtLife::Refresh(p);int old=PtLife::Get(p,PT_LIFE_STRENGTH);
 if(old<0||old>1000000000-amount)return false;
 PtLife::Set(p,PT_LIFE_STRENGTH,old+amount);Player[p].m_uMustSave=SAVE_REQUEST;return true;
}

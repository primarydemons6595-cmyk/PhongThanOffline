#include "PhongThanTgptVtt.h"
#include "../../../Headers/PhongThanTgptVttData.h"
namespace PtTgpt {
enum{Receipt=5030,Cooldown=5031,ExtraMask=5032,ExtraPending=5033,GreenMask=5034,LotteryDraw=5035};
static const char* Script="\\script\\phongthan\\van_tien\\pearl_chest.lua";
struct Chest{PtArena::Entity npc;int player,pearls;DWORD owner;bool claimed;Chest():player(0),pearls(0),owner(0),claimed(false){}};
struct Loot{int generation,world,x,y;bool active;Chest chests[5];Loot():generation(0),world(-1),x(0),y(0),active(false){}};
struct Session{DWORD owner,actor;int arena,slot,x,y,deadline,nextSync;Session(){memset(this,0,sizeof(*this));}};
static Loot loot[4];static Session sessions[MAX_PLAYER];
static bool Live(int p){return PtVtt::Live(p)&&Player[p].m_PlayerDBLoad;}
static int Get(int p,int key){return Player[p].m_cTask.GetSaveVal(key);}
static void Set(int p,int key,int value){Player[p].m_cTask.SetSaveVal(key,value,TRUE);}
static bool Merchant(int p,int type){
 if(!Live(p)||Player[p].GetLockState()||Player[p].CheckTrading())return false;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex,w=Npc[a].m_SubWorldIndex;
 if(n<=0||n>=MAX_NPC||w<0||w>=MAX_SUBWORLD||Npc[n].m_SubWorldIndex!=w||Npc[n].m_Kind!=kind_dialoger)return false;
 int map=SubWorld[w].m_SubWorldID;
 if(type==0&&!(Npc[n].m_NpcSettingIdx==155&&(map==1021||(map>=1067&&map<=1070))))return false;
 if(type==1&&!PhongThanNearXichTungTu(p))return false;
 if(type==2&&!(map==1021&&Npc[n].m_ActionScriptID==g_FileName2Id("\\script\\phongthan\\van_tien\\ung_tiem_thuong.lua")))return false;
 int x,y,nx,ny;Npc[a].GetMpsPos(&x,&y);Npc[n].GetMpsPos(&nx,&ny);return Npc[a].m_CurrentLife>0&&PtEscortNear(x,y,nx,ny,512);
}
static void Clear(int p){
 Session& s=sessions[p];if(s.owner&&s.arena>=1&&s.arena<=4&&s.slot>=0&&s.slot<5){Chest& c=loot[s.arena-1].chests[s.slot];if(c.player==p&&c.owner==s.owner){c.player=0;c.owner=0;}}
 if(Live(p)&&s.owner==Player[p].m_dwID&&s.actor==Npc[Player[p].m_nIndex].m_dwID&&Player[p].m_dwTaskExcuteScriptId==g_FileName2Id((char*)Script)&&!strcmp(Player[p].m_szTaskExcuteFun,"pt_vtt_loot_finish")){
  Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=0;Player[p].m_szTaskExcuteFun[0]=0;Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);
 }s=Session();
}
static bool Valid(int p){
 if(!Live(p))return false;Session& s=sessions[p];if(!s.owner||s.owner!=Player[p].m_dwID||s.arena<1||s.arena>4||s.slot<0||s.slot>=5)return false;
 int a=Player[p].m_nIndex,w=-1;KMission* m=PtVtt::Mission(s.arena,&w);Loot& l=loot[s.arena-1];Chest& c=l.chests[s.slot];
 if(!m||l.generation!=PtVtt::Generation(m)||g_TaskGlobalValue[s.arena]!=2||!l.active||s.actor!=Npc[a].m_dwID||Npc[a].m_SubWorldIndex!=w||
  !PtArena::Member(p,m)||c.claimed||!PtArena::Live(c.npc)||c.player!=p||c.owner!=s.owner||Npc[a].m_CurrentLife<=0||!Npc[a].m_FightMode||Player[p].m_cPK.GetNormalPKState()<=0||
  Player[p].GetLockState()||Player[p].CheckTrading()||Player[p].m_dwTaskExcuteScriptId!=g_FileName2Id((char*)Script)||strcmp(Player[p].m_szTaskExcuteFun,"pt_vtt_loot_finish"))return false;
 int x,y;Npc[a].GetMpsPos(&x,&y);return x==s.x&&y==s.y;
}
static int Start(int p){
 if(!Live(p)||Player[p].m_nPaceBarTime>0||Player[p].GetLockState()||Player[p].CheckTrading())return 0;
 if(Get(p,Cooldown)>(int)time(NULL))return -2;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex,w=Npc[a].m_SubWorldIndex;
 if(w<0||w>=MAX_SUBWORLD||!Npc[a].m_FightMode||Player[p].m_cPK.GetNormalPKState()<=0)return 0;
 int id=SubWorld[w].m_SubWorldID-1066;if(id<1||id>4)return 0;
 KMission* m=PtVtt::Mission(id);if(!m||!PtArena::Member(p,m)||g_TaskGlobalValue[id]!=2)return 0;
 Loot& l=loot[id-1];if(!l.active||l.generation!=PtVtt::Generation(m))return 0;
 for(int i=0;i<5;++i){Chest& c=l.chests[i];if(c.claimed||!PtArena::Live(c.npc)||c.npc.index!=n)continue;
  if(c.player)return -3;int x,y,nx,ny;Npc[a].GetMpsPos(&x,&y);Npc[n].GetMpsPos(&nx,&ny);if(!PtEscortNear(x,y,nx,ny,192))return 0;
  Session s;s.owner=Player[p].m_dwID;s.actor=Npc[a].m_dwID;s.arena=id;s.slot=i;s.x=x;s.y=y;s.deadline=g_SubWorldSet.GetGameTime()+20*GAME_FPS;s.nextSync=g_SubWorldSet.GetGameTime()+3;
  sessions[p]=s;c.player=p;c.owner=s.owner;Player[p].m_dwTaskExcuteScriptId=g_FileName2Id((char*)Script);g_StrCpyLen(Player[p].m_szTaskExcuteFun,"pt_vtt_loot_finish",sizeof(Player[p].m_szTaskExcuteFun));
  Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=20*GAME_FPS;Npc[a].SendSyncData(0,TRUE);return 1;
 }return 0;
}
static int Finish(int p){
 if(!Valid(p)){if(p>0&&p<MAX_PLAYER)Clear(p);return 0;}
 Session s=sessions[p];if(g_SubWorldSet.GetGameTime()+1<s.deadline||Player[p].m_nPaceBarTime!=0)return 0;
 PhongThanQuestExchangeRow reward;reward.count=loot[s.arena-1].chests[s.slot].pearls;if(reward.count<1||reward.count>5){Clear(p);return 0;}if(!PhongThanDecodeQuestItemTuple(3,PT_TGPT_PEARLS[s.arena-1],0,0,0,0,reward.item)){Clear(p);return 0;}
 int old=Get(p,Receipt);int result=PhongThanQuestExchange(p,Receipt,old,old==0?1:0,0,0,&reward,1,true,0,true);
 if(result==1){Chest& c=loot[s.arena-1].chests[s.slot];c.claimed=true;PtArena::Remove(c.npc);Set(p,Cooldown,(int)time(NULL)+60);}
 Clear(p);return result==1?1:-4;
}
static bool Spawn(Chest& c,int world,int tid,int x,int y,KMission* m){
 if(c.claimed||PtArena::Live(c.npc))return true;
 int r,cx,cy,ox,oy;SubWorld[world].Mps2Map(x,y,&r,&cx,&cy,&ox,&oy);
 if(r<0||SubWorld[world].TestBarrier(x,y)!=0)return false;
 int n=NpcSet.Add(MAKELONG(1,tid),world,x,y,FALSE);if(n<=0)return false;
 if(m->AddNpc(n,Npc[n].m_dwID,0)<=0){PhongThanRemoveNpc(n);return false;}
 c.npc.index=n;c.npc.serial=Npc[n].m_dwID;if(!c.pearls)c.pearls=g_Random(5)+1;Npc[n].m_Kind=kind_dialoger;Npc[n].m_AiMode=0;Npc[n].m_DeathScriptID=0;
 g_StrCpyLen(Npc[n].Name,PT_TGPT_CHEST_NAME,sizeof(Npc[n].Name));g_StrCpyLen(Npc[n].ActionScript,Script,sizeof(Npc[n].ActionScript));Npc[n].m_ActionScriptID=g_FileName2Id((char*)Script);return true;
}
static void SpawnAll(int id,KMission* m){
 Loot& l=loot[id-1];if(!l.active)return;static const int templates[4]={410,413,414,415};
 for(int i=0;i<5;++i){Chest& c=l.chests[i];if(c.claimed||PtArena::Live(c.npc))continue;
  for(int attempt=0;attempt<9;++attempt){int x=PT_TGPT_CHEST_POS[id-1][i][attempt][0],y=PT_TGPT_CHEST_POS[id-1][i][attempt][1];bool occupied=false;
   for(int j=0;j<5;++j)if(PtArena::Live(l.chests[j].npc)){int nx,ny;Npc[l.chests[j].npc.index].GetMpsPos(&nx,&ny);if(abs(nx-x)<48&&abs(ny-y)<48)occupied=true;}
   if(!occupied&&Spawn(c,l.world,templates[id-1],x,y,m))break;
  }
 }
}
static int Weapon(int p,int id,int profession){
 if(!Merchant(p,0)||id<1||id>4||profession<0||profession>2)return 0;
 const PTTgptWeapon& def=PT_TGPT_WEAPONS[id-1][profession];int magic[MAX_ITEM_MAGICLEVEL];ZeroMemory(magic,sizeof(magic));
 int item=ItemSet.Add(0,profession,def.level,0,0,def.part,magic,g_SubWorldSet.GetGameVersion(),0,def.row);
 if(item<=0)return 0;KItem prototype=Item[item];ItemSet.Remove(item);
 PhongThanQuestExchangeRow cost,reward;cost.count=PT_TGPT_COSTS[id-1];reward.count=1;
 if(!PhongThanDecodeQuestItemTuple(3,PT_TGPT_PEARLS[id-1],0,0,0,0,cost.item))return 0;
 reward.item.genre=0;reward.item.detail=prototype.GetDetailType();reward.item.particular=prototype.GetParticular();reward.item.level=prototype.GetLevel();reward.item.series=prototype.GetSeries();reward.item.luck=0;
 int old=Get(p,Receipt);return PhongThanQuestExchange(p,Receipt,old,old==0?1:0,&cost,1,&reward,1,true,&prototype,true);
}
static int Merge(int p,int tier){
 if(!Merchant(p,1)||tier<1||tier>3)return 0;PhongThanQuestExchangeRow cost,reward;cost.count=2;reward.count=1;
 if(!PhongThanDecodeQuestItemTuple(3,61+tier,0,0,0,0,cost.item)||!PhongThanDecodeQuestItemTuple(3,62+tier,0,0,0,0,reward.item))return 0;
 int old=Get(p,Receipt);return PhongThanQuestExchange(p,Receipt,old,old==0?1:0,&cost,1,&reward,1,true,0,true);
}
static int Extra(int p,int operation){
 if(!Merchant(p,0))return 0;int rank=Get(p,405),left=Get(p,406),pending=Get(p,ExtraPending);
 if(operation==0&&rank>=7&&rank<=9&&left==1&&Get(p,408)==100000&&!(Get(p,ExtraMask)&(1<<rank))){Set(p,ExtraPending,pending|(1<<rank));return 1;}
 int award=0;for(int level=7;level<=9;++level)if((pending&(1<<level))&&rank>level&&!(Get(p,ExtraMask)&(1<<level))){award=level;break;}if(!award)return 0;
 int profession=Player[p].m_cProfession.GetProfession();if(profession<0||profession>2)return 0;
 PhongThanQuestExchangeRow reward;reward.count=1;if(!PhongThanDecodeQuestItemTuple(6,1,PT_TGPT_BLUEPRINTS[profession],1,0,0,reward.item))return 0;
 int old=Get(p,ExtraMask),result=PhongThanQuestExchange(p,ExtraMask,old,old|(1<<award),0,0,&reward,1,true);
 if(result==1)Set(p,ExtraPending,pending&~(1<<award));return result;
}
static int Green(int p,int slot){
 if(!Merchant(p,0)||slot<1||slot>5||Get(p,408)!=100000||Get(p,406)!=1)return 0;
 int rank=Get(p,405),level=rank==2?3:rank==5?5:rank==8?7:rank==10?9:0;
 int profession=Player[p].m_cProfession.GetProfession();if(!level||profession<0||profession>2)return 0;
 int mask=Get(p,GreenMask),bit=1<<rank;if(mask&bit)return 1;
 static const int parts[5]={2,5,6,7,9};PhongThanQuestExchangeRow reward;reward.count=1;
 if(!PhongThanDecodeQuestItemTuple(0,parts[slot-1],profession+5,level,0,0,reward.item))return 0;
 return PhongThanQuestExchange(p,GreenMask,mask,mask|bit,0,0,&reward,1);
}
static int BuyLottery(int p,int count){
 if(!Merchant(p,2)||(count!=1&&count!=5)||Npc[Player[p].m_nIndex].m_Level<5)return 0;
 PhongThanQuestExchangeRow reward;reward.count=count;if(!PhongThanDecodeQuestItemTuple(6,1,12,1,0,0,reward.item))return 0;
 int cost=count*1000;if(!Player[p].Pay(cost))return 0;
 int old=Get(p,Receipt),result=PhongThanQuestExchange(p,Receipt,old,old==0?1:0,0,0,&reward,1,true);
 if(result!=1)Player[p].Earn(cost);else Set(p,136,min(MAX_INT-count,Get(p,136))+count);return result;
}
}
void PhongThanTgptVttTick(){
 for(int p=1;p<MAX_PLAYER;++p)if(PtTgpt::sessions[p].owner){
  if(!PtTgpt::Valid(p)||Player[p].m_nPaceBarTime==0)PtTgpt::Clear(p);
  else if(g_SubWorldSet.GetGameTime()>=PtTgpt::sessions[p].nextSync){PtTgpt::sessions[p].nextSync=g_SubWorldSet.GetGameTime()+3;Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);}
 }
 static DWORD last=0;DWORD now=GetTickCount();if(now-last<1000)return;last=now;
 for(int id=1;id<=4;++id){PtTgpt::Loot& l=PtTgpt::loot[id-1];int w=-1;KMission* m=PtVtt::Mission(id,&w);
  if(l.active&&(!m||g_TaskGlobalValue[id]!=2||l.generation!=PtVtt::Generation(m))){for(int c=0;c<5;++c)PtArena::Remove(l.chests[c].npc);l=PtTgpt::Loot();}
  if(m&&l.active)PtTgpt::SpawnAll(id,m);
 }
}
bool PhongThanTgptVttDamaged(int actor){bool handled=false;for(int p=1;p<MAX_PLAYER;++p)if(PtTgpt::sessions[p].owner&&Player[p].m_nIndex==actor){PtTgpt::Clear(p);handled=true;}return handled;}
int LuaEventVttBossLoot(Lua_State* L){
 int n=(int)Lua_ValueToNumber(L,1),result=0;if(n>0&&n<MAX_NPC&&Npc[n].m_CurrentLife<=0){int w=Npc[n].m_SubWorldIndex,id=w>=0?SubWorld[w].m_SubWorldID-1066:0;
  if(id>=1&&id<=4){KMission* m=PtVtt::Mission(id);if(m&&g_TaskGlobalValue[id]==2&&m->GetMissionValue(30)==n&&m->GetMissionValue(31)==1&&m->GetMissionNpc_DataIndex(n,Npc[n].m_dwID)){
   PtTgpt::Loot& l=PtTgpt::loot[id-1];if(!l.active){l.active=true;l.generation=PtVtt::Generation(m);l.world=w;Npc[n].GetMpsPos(&l.x,&l.y);if(!g_GetScript((char*)PtTgpt::Script))ReLoadScript((char*)PtTgpt::Script);PtTgpt::SpawnAll(id,m);}result=1;
  }}
 }Lua_PushNumber(L,result);return 1;
}
int LuaEventVttLoot(Lua_State* L){int p=GetPlayerIndex(L),result=0;if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){double op=Lua_ValueToNumber(L,1);if(op==0)result=PtTgpt::Start(p);else if(op==1)result=PtTgpt::Finish(p);}Lua_PushNumber(L,result);return 1;}
int LuaEventVttExchange(Lua_State* L){int p=GetPlayerIndex(L),result=0;if(Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,2)){double a=Lua_ValueToNumber(L,1),b=Lua_ValueToNumber(L,2);if(a==(int)a&&b==(int)b)result=PtTgpt::Weapon(p,(int)a,(int)b);}Lua_PushNumber(L,result);return 1;}
int LuaEventVttMerge(Lua_State* L){int result=0;if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){double v=Lua_ValueToNumber(L,1);if(v==(int)v)result=PtTgpt::Merge(GetPlayerIndex(L),(int)v);}Lua_PushNumber(L,result);return 1;}
int LuaEventVttExtra(Lua_State* L){Lua_PushNumber(L,PtTgpt::Extra(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1)));return 1;}
int LuaEventVttExpMultiplier(Lua_State* L){time_t vn=time(NULL)+7*3600;tm* d=gmtime(&vn);Lua_PushNumber(L,d&&d->tm_wday==2?2:1);return 1;}
int LuaEventVttGreen(Lua_State* L){Lua_PushNumber(L,PtTgpt::Green(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1)));return 1;}
int LuaEventVttBuyLottery(Lua_State* L){int result=0;if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){double v=Lua_ValueToNumber(L,1);if(v==1||v==5)result=PtTgpt::BuyLottery(GetPlayerIndex(L),(int)v);}Lua_PushNumber(L,result);return 1;}
int PhongThanUseTgptLottery(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_magicscript||Item[item].GetDetailType()!=12)return -1;
 if(!PtTgpt::Live(p)||Player[p].GetLockState()||Player[p].CheckTrading()||!PtPetCarried(p,item)||Npc[Player[p].m_nIndex].m_CurrentLife<=0)return 0;
 if(Item[item].m_CommonAttrib.nExpireTime>0&&Item[item].m_CommonAttrib.nExpireTime<=(int)KSG_GetCurSec())return 0;
 int draw=PtTgpt::Get(p,PtTgpt::LotteryDraw);if(draw<1||draw>1000){draw=g_Random(1000)+1;PtTgpt::Set(p,PtTgpt::LotteryDraw,draw);}
 PhongThanQuestExchangeRow cost,reward;cost.count=reward.count=1;
 if(!PhongThanDecodeQuestItemTuple(6,1,12,1,0,0,cost.item))return 0;
 int money=draw==1?100000:draw<=6?20000:0,count=0;
 if(draw>=7&&draw<=106){reward.item=cost.item;count=1;}
 else if(draw>=107&&draw<=256){if(!PhongThanDecodeQuestItemTuple(3,draw<=206?62:63,0,0,0,0,reward.item))return 0;count=1;}
 if(money&&!Player[p].Earn(money))return 0;
 int result=PhongThanQuestExchange(p,PtTgpt::LotteryDraw,draw,0,&cost,1,&reward,count,true,0,true);
 if(result!=1){if(money)Player[p].Pay(money);return 0;}
 if(money)PtTgpt::Set(p,137,min(MAX_INT-money,PtTgpt::Get(p,137))+money);
 Player[p].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,money?PT_TGPT_LOTTERY_MONEY:count?PT_TGPT_LOTTERY_ITEM:PT_TGPT_LOTTERY_NONE);return 1;
}

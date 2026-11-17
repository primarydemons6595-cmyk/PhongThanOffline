#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <map>
#define _SERVER
#include "../../Sources/Core/Src/PhongThanThuongChu.h"
enum {MAX_PLAYER=64,MAX_NPC=256,MAX_SUBWORLD=8,TRUE_SAVE=1,enumFightActive=1,npc_gold=2,camp_free=4,camp_justice=1,camp_evil=2,enumPKWar=1,relation_none=0,relation_enemy=1,relation_ally=2,obj_npc=1,kind_dialoger=3,TASKVALUE_PT_COPPER_CASH=5100};
struct lua_State {int a[3],result;};
struct PHONGTHAN_CHARACTER_STATE_HEADER {unsigned long StateSize,RoleTime;};
static PHONGTHAN_CHARACTER_STATE_HEADER states[MAX_PLAYER];
static bool PhongThanValidateCharacterState(const PHONGTHAN_CHARACTER_STATE_HEADER* p,unsigned long n){return p&&n==sizeof(*p);}
struct PLAYER_REVIVAL_POS {int m_nSubWorldID,m_nMpsX,m_nMpsY;};
namespace PtEvents {static bool Write(const char* path,const std::string& text){FILE* f=fopen(path,"wb");if(!f)return false;bool ok=fwrite(text.data(),1,text.size(),f)==text.size();fclose(f);return ok;}}
static int caller=1,clockSeconds=10000,certificates=0,certSpace=1,warMin=15;
int GetPlayerIndex(Lua_State*){return caller;}static double Lua_ValueToNumber(Lua_State* l,int i){return l->a[i-1];}static void Lua_PushNumber(Lua_State* l,int n){l->result=n;}
static time_t FixtureTime(time_t*){return clockSeconds;}
#define time FixtureTime
static int GetRandomNumber(int,int){return 2;}
static int PhongThanEventValue(int,int){return 120;}
struct Task {int v[5400];Task(){ZeroMemory(v,sizeof(v));}int GetSaveVal(int n){return v[n];}void SetSaveVal(int n,int val,BOOL){v[n]=val;}};
struct Bag {int money;Bag():money(0){}int GetEquipmentMoney(){return money;}bool CostMoney(int n){if(money<n)return false;money-=n;return true;}};
struct PK {int pk,locked;PK():pk(0),locked(0){}int GetNormalPKState(){return pk;}void SetLockPKState(BYTE p,BYTE l){pk=p;locked=l;}};
struct Team {int m_nFlag,m_nID;Team():m_nFlag(0),m_nID(-1){}};
struct KPlayer {int m_nIndex,m_PlayerDBLoad;DWORD m_dwID;Task m_cTask;Bag m_ItemList;PK m_cPK;Team m_cTeam;int xp,saves;bool trading;
 char Name[32],AccountName[32];void* m_SaveBuffer;PLAYER_REVIVAL_POS revive;
 KPlayer():m_nIndex(0),m_PlayerDBLoad(0),m_dwID(0),xp(0),saves(0),trading(false),m_SaveBuffer(0){Name[0]=AccountName[0]=0;revive.m_nSubWorldID=1001;revive.m_nMpsX=50000;revive.m_nMpsY=100000;}bool CheckTrading(){return trading;}int GetLockState(){return 0;}BOOL Save(){++saves;return TRUE;}void DirectAddExp(int n){xp+=n;}void Earn(int n){m_ItemList.money+=n;}PLAYER_REVIVAL_POS* GetDeathRevivalPos(){return &revive;}}Player[MAX_PLAYER];
struct KNpc {int m_Kind;char ActionScript[160];DWORD m_ActionScriptID;int m_SubWorldIndex,m_RegionIndex,m_MapX,m_MapY,m_CurrentCamp,m_nPKFlag,m_Level,m_FightMode,m_CurrentLife,m_btSpecial,x,y,player;DWORD m_dwID;char Name[32];
 KNpc():m_SubWorldIndex(-1),m_RegionIndex(-1),m_MapX(0),m_MapY(0),m_CurrentCamp(0),m_nPKFlag(0),m_Level(120),m_FightMode(0),m_CurrentLife(100),m_btSpecial(0),x(0),y(0),player(0),m_dwID(0){Name[0]=0;}
 bool IsPlayer(){return player>0;}int GetPlayerIdx(){return player;}bool IsAlive(){return m_CurrentLife>0;}void SetCurrentCamp(int c){m_CurrentCamp=c;}void SetFightMode(int f){m_FightMode=f;}
 bool ChangeWorld(int map,int ax,int ay);void GetMpsPos(int* a,int* b){*a=x;*b=y;}}Npc[MAX_NPC];
struct Region {void RemoveNpc(int){}void DecRef(int,int,int){}};
struct FixtureWorld {int m_SubWorldID,m_nTotalRegion;Region m_Region[1];FixtureWorld():m_SubWorldID(0),m_nTotalRegion(1){}}SubWorld[MAX_SUBWORLD];
struct Worlds {int SearchWorld(int map){for(int i=0;i<MAX_SUBWORLD;++i)if(SubWorld[i].m_SubWorldID==map)return i;return -1;}}g_SubWorldSet;
bool KNpc::ChangeWorld(int map,int ax,int ay){int w=g_SubWorldSet.SearchWorld(map);if(w<0)return false;m_SubWorldIndex=w;m_RegionIndex=0;x=ax;y=ay;return true;}
struct NpcSetType {int next;NpcSetType():next(100){}int Add(int,int w,int x,int y,BOOL){int n=100;while(n<MAX_NPC&&Npc[n].m_dwID)++n;if(n>=MAX_NPC)return 0;Npc[n]=KNpc();Npc[n].m_SubWorldIndex=w;Npc[n].m_RegionIndex=0;Npc[n].m_dwID=5000+n;Npc[n].x=x;Npc[n].y=y;return n;}void Remove(int n){Npc[n].m_dwID=0;Npc[n].m_RegionIndex=-1;}int GetDistance(int a,int b){return abs(Npc[a].x-Npc[b].x)+abs(Npc[a].y-Npc[b].y);}}NpcSet;
int PhongThanThuongChuEconomy(int,int,int){return certSpace;}
int PhongThanThuongChuQuestPayment(int p,int silver,int copper){if(Player[p].m_ItemList.money<silver||Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)<copper)return 0;Player[p].m_ItemList.money-=silver;Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)-copper,TRUE);return 1;}
void PhongThanThuongChuBuffTick(int,unsigned long){}
static DWORD g_FileName2Id(char*){return 123;}
#include "../../Sources/Core/Src/PhongThanThuongChu.inl"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line%d %s\n",__LINE__,#x);return 1;}}while(0)
static int Call(int p,int op,int arg=0){caller=p;lua_State l;ZeroMemory(&l,sizeof(l));l.a[0]=op;l.a[1]=arg;LuaThuongChu(&l);return l.result;}
static void Role(int p,int side){Player[p]=KPlayer();Player[p].m_nIndex=p;Player[p].m_PlayerDBLoad=1;Player[p].m_dwID=1000+p;sprintf(Player[p].Name,"role%d",p);strcpy(Player[p].AccountName,"account");states[p].StateSize=sizeof(states[p]);states[p].RoleTime=p+1;Player[p].m_SaveBuffer=&states[p];Npc[p]=KNpc();Npc[p].player=p;Npc[p].m_dwID=2000+p;Npc[p].m_CurrentCamp=7;Npc[p].ChangeWorld(1001,(side==1?189:212)*256,(side==1?191:215)*512);}
static void Mode(int n){CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);char value[10];sprintf(value,"%d",n);WritePrivateProfileStringA("Battle","Mode",value,"settings\\phongthan\\ThuongChu.ini");}
static bool JoinRole(int p,int side){if(!Player[p].m_PlayerDBLoad)Role(p,side);Npc[p].ChangeWorld(1001,(side==1?189:212)*256,(side==1?191:205)*512);return Call(p,1,side)!=0;}
static void AtCamp(int p){int x,y;PtThuongChu::Base(PtThuongChu::members[p].side,x,y);Npc[p].ChangeWorld(PtThuongChu::Map,x*256,y*512);}
int main(){using namespace PtThuongChu;
 SubWorld[0].m_SubWorldID=1001;SubWorld[1].m_SubWorldID=1071;SubWorld[2].m_SubWorldID=1020;SubWorld[3].m_SubWorldID=1021;SubWorld[4].m_SubWorldID=1064;
 Mode(3);CHECK(PhongThanThuongChuOpen(600,1800));CHECK(match.mode==3&&Map==1064&&match.fightAt==10600&&match.endAt==12400);CHECK(!PhongThanThuongChuOpen(600,1800));
 Role(1,1);Npc[1].m_Level=119;CHECK(!Call(1,1,1));Npc[1].m_Level=120;CHECK(Call(1,1,1));CHECK(!Call(1,1,2));CHECK(!Call(1,2));
 for(int p=2;p<=10;++p)CHECK(JoinRole(p,p%2?1:2));CHECK(Count(1)==5&&Count(2)==5);
 Mode(1);clockSeconds+=600;PhongThanThuongChuTick();CHECK(match.phase==2&&match.mode==3&&Map==1064);CHECK(units[30].kind==UnitService&&units[31].kind==UnitService);
 CHECK(Call(1,2)&&Call(2,2)&&Call(3,2));CHECK(World(1)==1064&&World(2)==1064&&World(3)==1064);CHECK(Player[1].revive.m_nSubWorldID==1064&&Player[1].revive.m_nMpsX==176*256);CHECK(PhongThanThuongChuRelation(1,2)==relation_enemy);CHECK(PhongThanThuongChuRelation(1,3)==relation_ally);
 CHECK(!Call(1,4,1));Player[1].m_ItemList.money=1000000;AtCamp(1);CHECK(Call(1,4,1));CHECK(Player[1].m_ItemList.money==960000&&Call(1,21)==1);CHECK(!Call(1,4,2));CHECK(!Call(1,5));
 for(int kill=0;kill<10;++kill)CHECK(PhongThanThuongChuDeath(2,1));CHECK(Call(1,20)==10);CHECK(Call(1,5)&&Call(1,14,1)==15&&Call(1,14,2)==0&&members[1].done==1);CHECK(!Call(1,5));
 CHECK(Call(1,4,4));Player[1].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,10,TRUE);int oldMoney=Player[1].m_ItemList.money,oldRuns=Call(1,21);CHECK(Call(1,17,3));CHECK(Call(1,21)==oldRuns&&Player[1].m_ItemList.money==oldMoney&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)==9);
 CHECK(Call(1,16)&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)==4&&Call(1,14,1)==30);CHECK(!Call(1,16));CHECK(Call(1,4,2));CHECK(Call(1,21)==3);CHECK(!Call(1,5));
 CHECK(PhongThanThuongChuDeath(units[21].index,1));CHECK(members[1].carried==1&&Call(1,27)==1);CHECK(Call(1,18));CHECK(Call(1,20)==1&&Call(1,26,1)==0);CHECK(!Call(1,18));
 CHECK(PhongThanThuongChuDeath(units[20].index,2));CHECK(members[2].carried==1);CHECK(PhongThanThuongChuDeath(2,1));CHECK(!members[2].carried&&units[20].respawn>0);
 CHECK(!units[6].index);clockSeconds+=599;PhongThanThuongChuTick();CHECK(!units[6].index);++clockSeconds;PhongThanThuongChuTick();CHECK(units[6].kind==UnitCommander&&units[11].kind==UnitCommander);
 CHECK(PhongThanThuongChuDeath(units[9].index,1));CHECK(units[15].kind==UnitCommanderFlag&&match.phase==2&&Call(1,26,1)==0);CHECK(PhongThanThuongChuDeath(units[15].index,1));CHECK(members[1].carried==2&&match.phase==2);
 Npc[1].ChangeWorld(1064,190*256,202*512);CHECK(!Call(1,18));CHECK(PhongThanThuongChuDeath(1,2));CHECK(!members[1].carried&&units[15].kind==UnitCommanderFlag&&!units[15].dead);CHECK(PhongThanThuongChuDeath(units[15].index,1));AtCamp(1);CHECK(Call(1,18)&&Call(1,26,1)==1&&match.phase==2);
 CHECK(PhongThanThuongChuDeath(units[10].index,1));CHECK(PhongThanThuongChuDeath(units[16].index,1));CHECK(Call(1,18)&&Call(1,26,1)==2&&match.phase==2);
 CHECK(PhongThanThuongChuDeath(units[11].index,1));CHECK(PhongThanThuongChuDeath(units[17].index,1));CHECK(Call(1,18));CHECK(match.phase==3&&match.winner==1&&Call(1,26,1)==3);CHECK(Player[1].m_cTask.GetSaveVal(TaskReward)==2000000&&Player[2].m_cTask.GetSaveVal(TaskReward)==700000);CHECK(!Call(1,8));
 clockSeconds+=30;PhongThanThuongChuTick();CHECK(match.phase==0&&World(1)==1021&&Npc[1].m_CurrentCamp==7&&!Player[1].m_cPK.locked&&Player[1].revive.m_nSubWorldID==1001);CHECK(Call(1,8)==2000000&&!Call(1,8));Call(2,8);Call(3,8);CHECK(Player[1].m_cTask.GetSaveVal(TaskMerit)>0);
 // A new session globally chooses the new mode, retaining separate reputation.
 Mode(2);++clockSeconds;CHECK(PhongThanThuongChuOpen(600,1800));for(p=1;p<=10;++p)CHECK(JoinRole(p,p%2?1:2));clockSeconds+=600;PhongThanThuongChuTick();CHECK(match.mode==2&&Map==1064&&match.phase==2);CHECK(Call(1,2));AtCamp(1);Player[1].m_ItemList.money=1000000;
 Player[1].m_cTask.SetSaveVal(TaskQuestDay,Day(),TRUE);Player[1].m_cTask.SetSaveVal(TaskQuestRuns,3,TRUE);Player[1].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,2,TRUE);CHECK(!Call(1,4,4));CHECK(Player[1].m_ItemList.money==1000000&&Call(1,21)==3);
 Player[1].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,3,TRUE);CHECK(Call(1,4,4));CHECK(Player[1].m_ItemList.money==960000&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)==0&&Call(1,21)==4);
 for(int i=0;i<3;++i){CHECK(PhongThanThuongChuDeath(units[19].index,1));if(i<2){clockSeconds+=60;PhongThanThuongChuTick();}}CHECK(Call(1,20)==3);AtCamp(1);CHECK(Call(1,5)&&Call(1,14,1)==45&&Call(1,14,2)==0);
 Player[1].m_cTask.SetSaveVal(TaskQuestRuns,10,TRUE);CHECK(!Call(1,4,1));Player[1].m_cTask.SetSaveVal(TaskQuestDay,Day()-1,TRUE);CHECK(Call(1,4,1)&&Call(1,21)==1);
 clockSeconds=match.endAt;PhongThanThuongChuTick();CHECK(match.phase==3&&match.winner==1);clockSeconds+=30;PhongThanThuongChuTick();CHECK(match.phase==0);
 Mode(1);++clockSeconds;CHECK(PhongThanThuongChuOpen(600,1800));CHECK(JoinRole(11,1));clockSeconds+=600;PhongThanThuongChuTick();CHECK(match.phase==3&&match.winner==0&&Map==1071);clockSeconds+=30;PhongThanThuongChuTick();CHECK(match.phase==0);
 printf("PASS %d production TGPT controller checks: one session mode for both sides, four quest gates, daily payments, no double awards, carrying/dropping/submitting commander flags, rewards and revival restoration\n",checks);return 0;
}


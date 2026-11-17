"""Build a fixture around the production mining runtime, without game data writes."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];source=ROOT/'PhongThanSource'
s=(source/'Tests/Native/LifeRuntimeTests.cpp').read_text('utf8').split('#define time LifeTestTime')[0]
s=s.replace('void PhongThanMiningTick(){}\nvoid PhongThanMiningDamaged(int){}\n','')
s=s.replace('int g_Random(int){return 0;}','int randomRoll=0;int g_Random(int range){return range>0?randomRoll%range:0;}')
s=s.replace('struct KIniFile{BOOL Load(const char*){return TRUE;}void GetInteger(const char*,const char*,int,int* v){*v=1000;}};','struct KIniFile{BOOL Load(const char*){return FALSE;}void GetInteger(const char*,const char*,int v,int* out){*out=v;}void GetString(const char*,const char*,const char*,char* out,int){out[0]=0;}};')
s=s.replace('int GetParticular(){return particular?particular:8;}','int GetParticular(){return particular?particular:8;}char* GetName(){return "Copper";}')
s=s.replace('struct Bag{','struct PlayerItem {int nIdx;};PlayerItem inventory={1};\nstruct Bag{PlayerItem* GetFirstItem(){return &inventory;}PlayerItem* GetNextItem(){return 0;}')
s=s.replace('m_CurrentLife,x,y;', 'm_CurrentLife,x,y,m_bRideHorse,m_CurrentLucky,lastCommand,m_Doing;')
s=s.replace('bool IsPlayer(){', 'int GetPlayerIdx(){return 1;}void BeginMiningSit(){m_Doing=9;}void SendCommand(int command,int,int,int){lastCommand=command;}bool IsPlayer(){')
s=s.replace('Bag m_ItemList;', 'void SendSystemMessage(const char*,const char* text);Bag m_ItemList;')
s=s.replace('gift,int n,bool,int,bool){','gift,int n,bool,int,bool,int=0,bool=false,const int* =0,bool=false){')
s+='''
#define MESSAGE_SYSTEM_ANNOUCE_HEAD ""
enum {do_skill=6,do_sit=9};DWORD fakeTick=1000;DWORD MiningTestTick(){return fakeTick;}char lastMessage[512];
void PlayerType::SendSystemMessage(const char*,const char* text){strcpy(lastMessage,text);}
bool PhongThanQuestItemMatches(const PickItem&,const Tuple&){return true;}
#define time LifeTestTime
#define GetTickCount MiningTestTick
#include "../../Sources/Core/Src/PhongThanMiningRuntime.inl"
#undef time
int main(){
 assert(PtMiningChance(2000,10,0)==2000&&PtMiningChance(2000,10,1000)==10000&&PtMiningChance(2000,10,-5)==2000);
 assert(PtMiningNear(0,0,512,0,512)&&!PtMiningNear(0,0,513,0,512));
 PtMining::loaded=PtMining::enabled=PtMining::stateValid=true;PtMining::siteCount=PtMining::lootCount=1;PtMining::chance=10000;
 PTMiningSite& s=PtMining::sites[0];ZeroMemory(&s,sizeof(s));s.id=1;s.map=1057;s.x=1000;s.y=2000;s.capacity=3;s.respawn=600;s.reach=512;s.enabled=1;
 assert(PtMiningSiteValid(s));PTMiningLoot& l=PtMining::loot[0];ZeroMemory(&l,sizeof(l));l.genre=3;l.detail=3;l.weight=1000;l.minimum=l.maximum=1;assert(PtMiningLootValid(l));
 strcpy(PtMining::prefix,"You mined ");strcpy(PtMining::between," ");strcpy(PtMining::suffix,".");
 Player[1].m_PlayerDBLoad=true;Player[1].m_dwID=20;Player[1].m_nIndex=1;Player[1].m_ItemList.pick=1;Item[1].id=30;
 Npc[1].m_dwID=21;Npc[1].m_Kind=1;Npc[1].m_CurrentLife=100;Npc[1].m_SubWorldIndex=57;Npc[1].m_SkillList.mine=1;Npc[1].x=1000;Npc[1].y=2000;
 SubWorld[57].m_SubWorldID=1057;PtMining::Update();assert(PtMining::state.count==1&&PtMining::state.mines[0].remaining==3);
 assert(PhongThanMiningCanCast(1,1000,2000));assert(PhongThanMiningClick(1,PtMining::objects[0].index)&&Npc[1].lastCommand==do_skill&&Player[1].m_nPaceBarTime==0);
 Npc[1].x+=512;assert(PtMiningRoll(1,1000,2000)&&awards==1&&PtMining::state.mines[0].remaining==2&&Player[1].m_nPaceBarTime==0);
 assert(strstr(lastMessage,"<color=red>1<color>")&&strstr(lastMessage,"<color=yellow>Copper<color>"));
 Npc[1].x++;assert(!PtMiningRoll(1,1000,2000)&&awards==1);Npc[1].x--;
 bagFailure=1;assert(!PtMiningRoll(1,1000,2000)&&PtMining::state.mines[0].remaining==2);bagFailure=0;
 Item[1].particular=71;assert(!PtMiningRoll(1,1000,2000));Item[1].particular=8;
 PtMining::chance=0;assert(PtMiningRoll(1,1000,2000)&&awards==1&&PtMining::state.mines[0].remaining==2);PtMining::chance=10000;
 assert(PtMiningRoll(1,1000,2000)&&PtMiningRoll(1,1000,2000)&&PtMining::state.mines[0].remaining==0);
 assert(!PtMiningRoll(1,1000,2000)&&awards==3);PtMining::LoadState();assert(PtMining::stateValid&&PtMining::state.mines[0].remaining==0);
 fakeTime+=599;PtMining::Update();assert(PtMining::state.mines[0].remaining==0);++fakeTime;PtMining::Update();assert(PtMining::state.mines[0].remaining==3);
 PtMining::state.mines[0].remaining=10;PtMining::sites[0].capacity=10;Npc[1].x=1000;int oldAwards=awards;
 assert(PhongThanMiningCast(1,1000,2000)&&Npc[1].m_Doing==do_sit&&PtMining::sessions[1].owner&&awards==oldAwards+1);
 assert(PhongThanMiningCast(1,1000,2000)&&awards==oldAwards+1);
 fakeTick+=1000;PhongThanMiningTick();assert(awards==oldAwards+2&&Npc[1].m_Doing==do_sit&&Player[1].m_nPaceBarTime==0);
 Npc[1].x++;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards+2);
 Npc[1].x--;assert(PhongThanMiningCast(1,1000,2000)&&PtMining::sessions[1].owner);PhongThanMiningDamaged(1);assert(!PtMining::sessions[1].owner);
 PtMining::chance=0;fakeTick+=1000;assert(PhongThanMiningCast(1,1000,2000)&&Npc[1].m_Doing==do_sit);oldAwards=awards;fakeTick+=1000;PhongThanMiningTick();assert(awards==oldAwards&&PtMining::sessions[1].owner);Npc[1].m_Doing=0;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner);
 fakeTick+=1000;PtMining::chance=10000;assert(PhongThanMiningCast(1,1000,2000));oldAwards=awards;
 Item[1].particular=71;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards);Item[1].particular=8;
 fakeTick+=1000;assert(PhongThanMiningCast(1,1000,2000));oldAwards=awards;Player[1].m_dwID++;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards);
 fakeTick+=1000;assert(PhongThanMiningCast(1,1000,2000));oldAwards=awards;Player[1].trading=1;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards);Player[1].trading=0;
 puts("PASS actual mining runtime: active seated sessions, repeated random rolls, movement/action/damage cancellation, rim range, chance/luck, no progress, shared reserves, full-bag rollback, exhaustion/restart/refill, colored messages");return 0;
}
'''
(source/'Tests/Native/MiningRuntimeTests.generated.cpp').write_text(s,'utf8')

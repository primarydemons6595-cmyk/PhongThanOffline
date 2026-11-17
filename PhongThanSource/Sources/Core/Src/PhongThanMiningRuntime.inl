#include "PhongThanMiningRuntime.h"
#include "../../../Headers/PhongThanMiningPolicy.h"
#include "../../../Headers/PhongThanMiningCrater.h"
#include "../../../Headers/PhongThanLifeSkills.h"
#include <time.h>
namespace PtMining {
static PTMiningSite sites[PT_MINING_MAX_SITES];static PTMiningLoot loot[PT_MINING_MAX_LOOT];
static int siteCount=0,lootCount=0,chance=0,luckBonus=0;static bool enabled=false,loaded=false,stateValid=true;
static char prefix[128],between[128],suffix[128];
struct MiningSession {DWORD owner,actor,nextAttempt;int site,world,x,y;};
static MiningSession sessions[MAX_PLAYER];static DWORD lastAttempt[MAX_PLAYER],lastOwner[MAX_PLAYER];
struct ObjectRef {int index,id;};static ObjectRef objects[PT_MINING_MAX_SITES];
struct Reserve {int id,remaining;DWORD refill;};
struct State {DWORD magic,version,count;Reserve mines[PT_MINING_MAX_SITES];DWORD checksum;};static State state;
static DWORD Hash(const void* data,int size){const unsigned char* b=(const unsigned char*)data;DWORD h=2166136261UL;for(int i=0;i<size;++i)h=(h^b[i])*16777619UL;return h;}
static bool Save(){
 state.magic=0x4d4e5450;state.version=1;state.checksum=Hash(&state,sizeof(state)-sizeof(DWORD));
 FILE* f=fopen("MiningState.bin.tmp","wb");if(!f)return false;bool ok=fwrite(&state,1,sizeof(state),f)==sizeof(state);if(fflush(f))ok=false;if(fclose(f))ok=false;
 if(!ok)return false;return MoveFileExA("MiningState.bin.tmp","MiningState.bin",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=FALSE;
}
static void LoadState(){
 ZeroMemory(&state,sizeof(state));FILE* f=fopen("MiningState.bin","rb");if(!f){DWORD attr=GetFileAttributesA("MiningState.bin");DWORD error=GetLastError();stateValid=attr==0xffffffffUL&&(error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND);return;}
 bool ok=fread(&state,1,sizeof(state),f)==sizeof(state)&&fgetc(f)==EOF;fclose(f);
 if(!ok||state.magic!=0x4d4e5450||state.version!=1||state.count>PT_MINING_MAX_SITES||state.checksum!=Hash(&state,sizeof(state)-sizeof(DWORD))){stateValid=false;return;}
 for(unsigned i=0;i<state.count;++i){if(state.mines[i].id<=0||state.mines[i].remaining<0||state.mines[i].remaining>1000000){stateValid=false;return;}for(unsigned j=0;j<i;++j)if(state.mines[i].id==state.mines[j].id){stateValid=false;return;}}
}
static bool Hex(KIniFile& ini,const char* section,const char* key,char* out,int size){
 char text[512];ini.GetString(section,key,"",text,sizeof(text));int n=strlen(text);if(n%2||n/2>=size)return false;
 for(int i=0;i<n;i+=2){char pair[3]={text[i],text[i+1],0},*end=0;int v=strtol(pair,&end,16);if(*end||v<32||v==127||v=='<'||v=='>')return false;out[i/2]=(char)v;}out[n/2]=0;return true;
}
static int Read(KIniFile& ini,const char* section,const char* key,int fallback=0){int v=fallback;ini.GetInteger(section,key,fallback,&v);return v;}
static void Load(){
 enabled=false;siteCount=lootCount=0;KIniFile ini;
 if(!ini.Load("\\settings\\phongthan\\MiningPolicy.ini")||Read(ini,"Main","Version")!=1)return;
 int count=Read(ini,"Main","SiteCount"),items=Read(ini,"Main","LootCount");chance=Read(ini,"Main","ChancePer10000");luckBonus=Read(ini,"Main","LuckBonusPerPoint");
 if(count<1||count>PT_MINING_MAX_SITES||items<1||items>PT_MINING_MAX_LOOT||chance<0||chance>10000||luckBonus<0||luckBonus>1000)return;
 if(!Hex(ini,"Messages","PrefixHex",prefix,sizeof(prefix))||!Hex(ini,"Messages","BetweenHex",between,sizeof(between))||!Hex(ini,"Messages","SuffixHex",suffix,sizeof(suffix)))return;
 for(int i=0;i<count;++i){char section[32];sprintf(section,"Mine%d",i+1);PTMiningSite& s=sites[i];ZeroMemory(&s,sizeof(s));
  s.id=Read(ini,section,"Id");s.map=Read(ini,section,"Map");s.x=Read(ini,section,"X");s.y=Read(ini,section,"Y");s.capacity=Read(ini,section,"Capacity");s.respawn=Read(ini,section,"RespawnSeconds");s.reach=Read(ini,section,"Reach");s.table=Read(ini,section,"Table");s.enabled=Read(ini,section,"Enabled");
  if(!PtMiningSiteValid(s)||!Hex(ini,section,"NameHex",s.name,sizeof(s.name)))return;for(int j=0;j<i;++j)if(sites[j].id==s.id)return;
 }
 for(i=0;i<items;++i){char section[32];sprintf(section,"Loot%d",i+1);PTMiningLoot& l=loot[i];l.table=Read(ini,section,"Table");l.genre=Read(ini,section,"Genre");l.detail=Read(ini,section,"Detail");l.part=Read(ini,section,"Particular");l.level=Read(ini,section,"Level");l.series=Read(ini,section,"Series");l.weight=Read(ini,section,"Weight");l.minimum=Read(ini,section,"Minimum");l.maximum=Read(ini,section,"Maximum");
  if(!PtMiningLootValid(l))return;PhongThanQuestExchangeRow row;ZeroMemory(&row,sizeof(row));if(!PhongThanDecodeQuestItemTuple(l.genre,l.detail,l.part,l.level,l.series,0,row.item))return;
 }
 for(i=0;i<count;++i)if(sites[i].enabled){bool found=false;for(int j=0;j<items;++j)if(loot[j].table==sites[i].table)found=true;if(!found)return;}
 siteCount=count;lootCount=items;enabled=Read(ini,"Main","Enabled")==1;
}
static Reserve* ReserveFor(int id){for(unsigned i=0;i<state.count;++i)if(state.mines[i].id==id)return &state.mines[i];return 0;}
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].IsPlayer();}
static bool Pick(int p){int item=Player[p].m_ItemList.GetEquipment(itempart_weapon);return item>0&&item<MAX_ITEM&&Item[item].GetID()&&Item[item].GetGenre()==item_equip&&Item[item].GetDetailType()==0&&Item[item].GetParticular()==PT_MINING_PICK;}
static bool Actor(int p){return Live(p)&&!Player[p].CheckTrading()&&!Player[p].GetLockState()&&Player[p].m_nPaceBarTime==0&&Npc[Player[p].m_nIndex].m_CurrentLife>0&&!Npc[Player[p].m_nIndex].m_bRideHorse&&Npc[Player[p].m_nIndex].m_SkillList.GetLevel(PT_MINING_SKILL)>0&&Pick(p);}
static bool ObjectLive(int i){int n=objects[i].index;return n>0&&n<MAX_OBJECT&&Object[n].m_nID==objects[i].id&&Object[n].m_nDataID==PT_MINING_CRATER_OBJECT;}
static void Update(){
 if(!loaded){loaded=true;LoadState();Load();}if(!enabled||!stateValid)return;
 bool changed=false;DWORD now=(DWORD)time(0);
 for(int i=0;i<siteCount;++i){PTMiningSite& s=sites[i];if(!s.enabled)continue;Reserve* r=ReserveFor(s.id);
  if(!r){if(state.count>=PT_MINING_MAX_SITES)continue;r=&state.mines[state.count++];r->id=s.id;r->remaining=s.capacity;r->refill=0;changed=true;}
  if(r->remaining>s.capacity){r->remaining=s.capacity;changed=true;}
  if(!r->remaining&&r->refill&&now>=r->refill){r->remaining=s.capacity;r->refill=0;changed=true;}
 }
 if(changed&&!Save()){stateValid=false;return;}
 for(i=0;i<siteCount;++i){PTMiningSite& s=sites[i];Reserve* r=ReserveFor(s.id);if(!s.enabled||!r)continue;
  if(!ObjectLive(i)){int world=PhongThanGatherSpawnWorld(s.map);if(world<0)continue;KMapPos pos;ZeroMemory(&pos,sizeof(pos));pos.nSubWorld=world;SubWorld[world].Mps2Map(s.x,s.y,&pos.nRegion,&pos.nMapX,&pos.nMapY,&pos.nOffX,&pos.nOffY);if(pos.nRegion<0)continue;
   KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nMovieFlag=1;int n=ObjSet.Add(PT_MINING_CRATER_OBJECT,pos,info);if(n<=0)continue;objects[i].index=n;objects[i].id=Object[n].m_nID;g_StrCpyLen(Object[n].m_szName,s.name,sizeof(Object[n].m_szName));
  }
  int desired=r->remaining>0?OBJ_PROP_STATE_DISPLAY:OBJ_PROP_STATE_HIDE;if(Object[objects[i].index].m_nState!=desired)Object[objects[i].index].SetState(desired);
 }
}
static int SiteNear(int actor,int x,int y){
 if(actor<=0||actor>=MAX_NPC)return -1;int world=Npc[actor].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD)return -1;
 int px,py;Npc[actor].GetMpsPos(&px,&py);int best=-1;__int64 distance=0;
 for(int i=0;i<siteCount;++i){const PTMiningSite& s=sites[i];if(!s.enabled||s.map!=SubWorld[world].m_SubWorldID||!PtMiningNear(x,y,s.x,s.y,96)||!PtMiningNear(px,py,s.x,s.y,s.reach))continue;
  __int64 dx=(__int64)x-s.x,dy=(__int64)y-s.y,d=dx*dx+dy*dy;if(best<0||d<distance){best=i;distance=d;}}
 return best;
}
}
static bool PtMiningRoll(int actor,int x,int y);
void PhongThanMiningTick(){
 static DWORD last=0;DWORD now=GetTickCount();if(now-last>=1000){last=now;PtMining::Update();}
 for(int p=1;p<MAX_PLAYER;++p){PtMining::MiningSession& s=PtMining::sessions[p];if(!s.owner)continue;
  bool valid=PtMining::Actor(p)&&Player[p].m_dwID==s.owner&&Npc[Player[p].m_nIndex].m_dwID==s.actor&&Npc[Player[p].m_nIndex].m_SubWorldIndex==s.world&&Npc[Player[p].m_nIndex].m_Doing==do_sit;
  int x=0,y=0;if(valid){Npc[Player[p].m_nIndex].GetMpsPos(&x,&y);valid=x==s.x&&y==s.y&&s.site>=0&&s.site<PtMining::siteCount;}
  PtMining::Reserve* reserve=valid?PtMining::ReserveFor(PtMining::sites[s.site].id):0;if(!valid||!reserve||reserve->remaining<=0){ZeroMemory(&s,sizeof(s));continue;}
  if((int)(now-s.nextAttempt)>=0){s.nextAttempt=now+1000;PtMining::lastAttempt[p]=now;PtMining::lastOwner[p]=s.owner;
   if(!PtMiningRoll(Player[p].m_nIndex,PtMining::sites[s.site].x,PtMining::sites[s.site].y))ZeroMemory(&s,sizeof(s));
  }
 }
}
void PhongThanMiningDamaged(int actor){for(int p=1;p<MAX_PLAYER;++p)if(PtMining::sessions[p].owner&&Player[p].m_nIndex==actor)ZeroMemory(&PtMining::sessions[p],sizeof(PtMining::sessions[p]));}
bool PhongThanMiningClick(int p,int object){
 PtMining::Update();if(!PtMining::Actor(p)||!PtMining::enabled||!PtMining::stateValid)return false;
 for(int i=0;i<PtMining::siteCount;++i)if(PtMining::ObjectLive(i)&&PtMining::objects[i].index==object){
  const PTMiningSite& s=PtMining::sites[i];int actor=Player[p].m_nIndex;if(PtMining::SiteNear(actor,s.x,s.y)!=i)return false;
  PtMining::Reserve* r=PtMining::ReserveFor(s.id);if(!r||r->remaining<=0)return false;Npc[actor].SendCommand(do_skill,PT_MINING_SKILL,s.x,s.y);return true;
 }return false;
}
static bool PtMiningRoll(int actor,int x,int y){
 PtMining::Update();if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsPlayer()||!PtMining::enabled||!PtMining::stateValid)return false;int p=Npc[actor].GetPlayerIdx();if(!PtMining::Actor(p))return false;
 int at=PtMining::SiteNear(actor,x,y);if(at<0)return false;const PTMiningSite& s=PtMining::sites[at];PtMining::Reserve* r=PtMining::ReserveFor(s.id);if(!r||r->remaining<=0)return false;
 // Each seated mining attempt rolls independently; elapsed time never guarantees loot.
 if(g_Random(10000)>=PtMiningChance(PtMining::chance,PtMining::luckBonus,Npc[actor].m_CurrentLucky))return true;
 int total=0;for(int i=0;i<PtMining::lootCount;++i)if(PtMining::loot[i].table==s.table)total+=PtMining::loot[i].weight;if(total<=0)return false;
 int roll=g_Random(total),chosen=-1;for(i=0;i<PtMining::lootCount;++i)if(PtMining::loot[i].table==s.table){roll-=PtMining::loot[i].weight;if(roll<0){chosen=i;break;}}if(chosen<0)return false;
 const PTMiningLoot& l=PtMining::loot[chosen];int count=l.minimum+g_Random(l.maximum-l.minimum+1);if(count>r->remaining)count=r->remaining;
 PhongThanQuestExchangeRow reward;ZeroMemory(&reward,sizeof(reward));if(!PhongThanDecodeQuestItemTuple(l.genre,l.detail,l.part,l.level,l.series,0,reward.item))return false;reward.count=count;
 // Reserve first; rejected inventory transactions restore the reserve. A disk
 // write failure disables mining rather than repeatedly granting free ore.
 PtMining::Reserve before=*r;r->remaining-=count;if(!r->remaining)r->refill=(DWORD)time(0)+s.respawn;
 if(!PtMining::Save()){*r=before;PtMining::stateValid=false;return false;}
 int serial=Player[p].m_cTask.GetSaveVal(PT_LIFE_RECEIPT),next=serial==0?1:0;
 if(PhongThanQuestExchange(p,PT_LIFE_RECEIPT,serial,next,0,0,&reward,1,true,0,true,0,false,0,true)!=1){*r=before;if(!PtMining::Save())PtMining::stateValid=false;return false;}
 char name[128];name[0]=0;for(PlayerItem* it=Player[p].m_ItemList.GetFirstItem();it;it=Player[p].m_ItemList.GetNextItem())if(it->nIdx>0&&it->nIdx<MAX_ITEM&&PhongThanQuestItemMatches(Item[it->nIdx],reward.item)){g_StrCpyLen(name,Item[it->nIdx].GetName(),sizeof(name));break;}if(!name[0])g_StrCpyLen(name,"item",sizeof(name));const char* label=name[0]=='#'?name+1:name;
 char message[512];_snprintf(message,sizeof(message)-1,"%s<color=red>%d<color>%s<color=yellow>%s<color>%s",PtMining::prefix,count,PtMining::between,label,PtMining::suffix);message[sizeof(message)-1]=0;Player[p].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,message);
 PtMining::Update();return true;
}
bool PhongThanMiningCanCast(int actor,int x,int y){
 PtMining::Update();if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsPlayer()||!PtMining::enabled||!PtMining::stateValid)return false;
 int p=Npc[actor].GetPlayerIdx();if(!PtMining::Actor(p))return false;int at=PtMining::SiteNear(actor,x,y);if(at<0)return false;
 PtMining::Reserve* r=PtMining::ReserveFor(PtMining::sites[at].id);return r&&r->remaining>0;
}
bool PhongThanMiningCast(int actor,int x,int y){
 PtMining::Update();if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsPlayer()||!PtMining::enabled||!PtMining::stateValid)return false;
 int p=Npc[actor].GetPlayerIdx();if(!PtMining::Actor(p))return false;int at=PtMining::SiteNear(actor,x,y);if(at<0)return false;
 PtMining::Reserve* r=PtMining::ReserveFor(PtMining::sites[at].id);if(!r||r->remaining<=0)return false;
 DWORD now=GetTickCount();PtMining::MiningSession& session=PtMining::sessions[p];
 if(session.owner==Player[p].m_dwID&&session.actor==Npc[actor].m_dwID&&session.site==at){Npc[actor].BeginMiningSit();return true;}
 ZeroMemory(&session,sizeof(session));session.owner=Player[p].m_dwID;session.actor=Npc[actor].m_dwID;session.site=at;session.world=Npc[actor].m_SubWorldIndex;Npc[actor].GetMpsPos(&session.x,&session.y);
 // Sit is the active digging posture. Rolls continue while seated; changing
 // position, action, tool, world, identity, or taking damage cancels mining.
 Npc[actor].BeginMiningSit();session.nextAttempt=now+1000;
 if(PtMining::lastOwner[p]!=session.owner||(int)(now-PtMining::lastAttempt[p])>=1000){PtMining::lastOwner[p]=session.owner;PtMining::lastAttempt[p]=now;if(!PtMiningRoll(actor,x,y)){ZeroMemory(&session,sizeof(session));return false;}}
 return true;
}

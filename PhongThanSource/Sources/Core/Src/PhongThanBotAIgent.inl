#include "../../../Headers/PhongThanBotPvpPolicy.h"
#include "../../../Headers/PhongThanBotNation.h"
#include "../../../Headers/PhongThanTerritoryPaths.h"
#include "PhongThanTerritory.h"
#include "../../../Headers/PhongThanBank.h"
#include "../../../Headers/PhongThanMagicRetention.h"
#include "../../../Headers/PhongThanCharacterBundle.h"
// BotAIgent: real KPlayer instances, no socket and no client process.
// Called only on the authoritative world thread. Control is leased by Event Service.
#include <time.h>
#include "../../../Headers/PhongThanItemLink.h"
#include "../../../Headers/PhongThanBotCommand.h"
#include "../../../Headers/PhongThanBotMarketPolicy.h"
#include "../../../Headers/PhongThanBotAdPolicy.h"
#include "../../../Headers/PhongThanMapIdentity.h"
#include <vector>
#include "../../../Headers/PhongThanCharacter.h"
#include "../../../Headers/PhongThanStarterWeapon.h"
#include "../../../Headers/PhongThanStarterSpawn.h"
#include "../../../Headers/PhongThanRoute.h"
#include "../../../Headers/PhongThanBotTraining.h"
#include "../../../Headers/PhongThanBotSurvival.h"
#include "../../../Headers/PhongThanBotName.h"
#include "../../../Headers/PhongThanBotEquipment.h"
#include "../../../Headers/PhongThanBotGearRating.h"
#include "../../../Headers/PhongThanBotMarketQuality.h"
#include "../../../Headers/PhongThanBotBossPolicy.h"
#include "../../../Headers/PhongThanBotSpeech.h"
#include "../../../Headers/PhongThanBotPortal.h"
#include "../../../Headers/PhongThanBotPartySpeech.h"
#include "../../../Headers/PhongThanBotQuestNpcs.h"
#include "../../../Headers/PhongThanAccountStorage.h"
#include "KItem.h"
#include "PhongThanQuestItemTuple.h"
#include "../../../Headers/PhongThanBotObjectives.h"
#include "../../../Headers/PhongThanBotNpcVisit.h"
#include "PhongThanCarry.h"
#include "PhongThanBotInitialState.h"
#include "../../../Headers/PhongThanBotRecovery.h"
#include "../../../Headers/PhongThanLifeSkills.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "KProtocol.h"
#include "../../../Headers/PhongThanStall.h"
#include "KObjSet.h"
#include "KObj.h"
#include "KBuySell.h"
#include "KPhongThanProfessionSkills.h"
#include "KPhongThanSkillLearning.h"
bool PhongThanBotRequestPk(int,int);
bool PhongThanBotRestorePk(int,int);
#include "PhongThanRevival.h"
#include "../../../Headers/PhongThanBotProgression.h"
#include "../../../Headers/PhongThanCompose.h"
#include "KNpcTemplate.h"
bool PhongThanEventNumberInput(KPlayer&,const PHONGTHAN_UI_NUMBER_REQUEST&);
int PhongThanBotWhisper(int,const char*,const char*,int);
namespace PtBotAIgent {
#include "../../../Headers/PhongThanBotAutonomyPolicy.h"
enum {Capacity=4096,ActiveCapacity=500}; // Stable lifetime identities; archives do not consume active capacity.
struct Profile {char Id[33],Name[25];int Enabled,Profession,Gender,Map,X,Y,Train,Auto;};
struct Runtime {Profile Config;int Player,GoalMap,GoalLevel;DWORD GoalEntity,LastSave,LastAction,Retry,MobKills;int SpawnFailures;char State[32],Error[128];};

static int botDecisionMilliseconds=0,botAutoSavesPerTick=0;
static Runtime bots[Capacity];static int count=0,listedCount=0,listedSlots[ActiveCapacity];static unsigned long revision=0,expires=0;static DWORD lastPoll=0;
static bool BotMarketReserved(int player,int index);
static int botDeclutterPlayer=0;
static int BotProgressRequired(KPlayer&,KItem&);
static int BotProgressMissing(KPlayer&,KItem&);
static unsigned long Hash(const void* data,int size){unsigned long h=2166136261UL;const BYTE* p=(const BYTE*)data;for(int i=0;i<size;++i)h=(h^p[i])*16777619UL;return h;}
static void Log(const char* level,const char* id,const char* text){
 CreateDirectoryA("BotAIgent",0);WIN32_FILE_ATTRIBUTE_DATA a;
 if(GetFileAttributesExA("BotAIgent\\activity.log",GetFileExInfoStandard,&a)&&a.nFileSizeLow>2097152)MoveFileExA("BotAIgent\\activity.log","BotAIgent\\activity.log.1",MOVEFILE_REPLACE_EXISTING);
 FILE* f=fopen("BotAIgent\\activity.log","ab");if(f){time_t local=time(0)+25200;struct tm* t=gmtime(&local);char stamp[32];strftime(stamp,sizeof(stamp),"%Y-%m-%d %H:%M:%S +07",t);fprintf(f,"[%s] [%s] [%s] %s\n",stamp,level,id,text);fclose(f);}
}
#include "PhongThanBotAutonomy.inl"
static void BotInventoryEvent(const char* bot,const char* event,DWORD item,int from,int to,int detail,int particular){
 const char* path="BotAIgent\\inventory-changes.tsv";WIN32_FILE_ATTRIBUTE_DATA a;
 if(GetFileAttributesExA(path,GetFileExInfoStandard,&a)&&(a.nFileSizeHigh||a.nFileSizeLow>2097152))MoveFileExA(path,"BotAIgent\\inventory-changes.tsv.1",MOVEFILE_REPLACE_EXISTING);
 FILE* f=fopen(path,"ab");if(f){fprintf(f,"%lu\t%s\t%s\t%lu\t%d\t%d\t%d\t%d\n",(unsigned long)time(0),bot,event,item,from,to,detail,particular);fclose(f);}
}
// Read-model projections may skip durability; character/ownership writes keep it.
static bool Atomic(const char* path,const void* data,int size,bool durable=true){
 std::string next=std::string(path)+".tmp";HANDLE f=CreateFileA(next.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
 DWORD n=0;bool ok=WriteFile(f,data,size,&n,0)&&n==(DWORD)size&&(!durable||FlushFileBuffers(f));CloseHandle(f);
 if(ok)ok=MoveFileExA(next.c_str(),path,MOVEFILE_REPLACE_EXISTING|(durable?MOVEFILE_WRITE_THROUGH:0))!=0;if(!ok)DeleteFileA(next.c_str());return ok;
}
static bool WriteState(const char* id,const PHONGTHAN_CHARACTER_STATE_HEADER* state){
 if(!PhongThanValidateCharacterState(state,state->StateSize))return false;
 CreateDirectoryA("BotAIgent",0);CreateDirectoryA("BotAIgent\\characters",0);
 char path[128];sprintf(path,"BotAIgent\\characters\\%s.pthc",id);std::string error;
 bool written=PtCharacterBundle::CommitPath("CharacterStore",path,state,true,error);
 if(written&&state->ItemCount){char before[180],done[180];sprintf(before,"%s.before-starter-v1",path);sprintf(done,"%s.starter-live-v1.done",path);if(GetFileAttributesA(before)!=(DWORD)-1&&GetFileAttributesA(done)==(DWORD)-1)Atomic(done,"1\n",2);}
 return written;
}
static bool NewState(const Profile& c,std::vector<BYTE>& bytes,const char*& error){
 if(!PhongThanBuildBotInitialState(c.Id,c.Name,c.Profession,c.Gender,c.Map,c.X,c.Y,bytes,error))return false;
 if(!WriteState(c.Id,(const PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0])){error="character_state_write_failed";return false;}return true;
}
static bool ReadState(const Profile& c,std::vector<BYTE>& state,const char*& error){
 error="character_state_invalid";
 char path[128];sprintf(path,"BotAIgent\\characters\\%s.pthc",c.Id);
 if(GetFileAttributesA(path)==(DWORD)-1){DWORD code=GetLastError();if(code==ERROR_FILE_NOT_FOUND||code==ERROR_PATH_NOT_FOUND)return NewState(c,state,error);return false;}
 PtCharacterBundle::Bundle bundle;if(!PtCharacterBundle::Read(path,bundle)){error="character_state_checksum_invalid";return false;}
 const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)bundle.state.data();char account[32];sprintf(account,"bot%.12s",c.Id);
 if(!PhongThanValidateCharacterState(s,bundle.state.size()))return false;
 if(strcmp((const char*)s->RoleName,c.Name)||strcmp((const char*)s->AccountName,account)||s->Profession!=c.Profession||s->Gender!=c.Gender){error="character_identity_mismatch";return false;}
 state.assign((const BYTE*)bundle.state.data(),(const BYTE*)bundle.state.data()+bundle.state.size());error=0;return true;
}
static bool ReservedName(const char* name){
 std::string file;if(!PtRoleFileName(name,file))return true;
 if(GetFileAttributesA((std::string("CharacterStore\\")+file).c_str())!=(DWORD)-1)return true;
 WIN32_FIND_DATAA data;HANDLE h=FindFirstFileA("CharacterStore\\*",&data);if(h==INVALID_HANDLE_VALUE)return false;bool found=false;
 do{if((data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)&&data.cFileName[0]!='.'&&GetFileAttributesA((std::string("CharacterStore\\")+data.cFileName+"\\"+file).c_str())!=(DWORD)-1){found=true;break;}}while(FindNextFileA(h,&data));FindClose(h);return found;
}
static void ResetCompanion(Runtime& r);
static void Stop(Runtime& r){
 if(!r.Player)return;if(!Player[r.Player].Save()){lstrcpyA(r.Error,"save_failed_kept_online");Log("ERROR",r.Config.Id,r.Error);return;}
 ResetCompanion(r);PlayerSet.RemoveServerBot(r.Player);r.Player=0;lstrcpyA(r.State,"offline");r.Error[0]=0;Log("INFO",r.Config.Id,"offline_saved");
}
static void Poll(){
 FILE* f=fopen("BotAIgent\\control.tsv","rb");if(!f){expires=0;return;}
 char line[512],magic[40];unsigned long version=0,rev=0,lease=0;bool ok=fgets(line,sizeof(line),f)!=0&&sscanf(line,"%39s\t%lu\t%lu\t%lu",magic,&version,&rev,&lease)==4&&!strcmp(magic,"PT_BOTAIGENT")&&(version==1||version==2);
 Profile next[ActiveCapacity];ZeroMemory(next,sizeof(next));int total=0;
 while(ok&&fgets(line,sizeof(line),f)){
  if(total>=ActiveCapacity){ok=false;break;}Profile& p=next[total];
  char nameField[49];int fields=sscanf(line,"%32s\t%48s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",p.Id,nameField,&p.Enabled,&p.Profession,&p.Gender,&p.Map,&p.X,&p.Y,&p.Train,&p.Auto);ok=(fields==9||fields==10)&&p.Auto>=0&&p.Auto<=1&&PTBotNameField(nameField,version,p.Name);
  if(!ok||strlen(p.Id)!=32||strlen(p.Name)<3||p.Enabled<0||p.Enabled>1||p.Profession<0||p.Profession>3||p.Gender<0||p.Gender>1||p.Map<=0||p.X<=0||p.Y<=0||p.Train<0||p.Train>1){ok=false;break;}
  for(int j=0;j<32;++j)if(!((p.Id[j]>='0'&&p.Id[j]<='9')||(p.Id[j]>='a'&&p.Id[j]<='f')))ok=false;
  for(j=0;j<total;++j)if(!strcmp(p.Id,next[j].Id)||!stricmp(p.Name,next[j].Name))ok=false;++total;
 }
 if(ferror(f))ok=false;fclose(f);if(!ok){expires=0;Log("ERROR","service","invalid_control_snapshot");return;}
 // Keep all runtime identities stable; removals must first save and leave the world.
 for(int i=0;i<count;++i){bool found=false;for(int j=0;j<total;++j)if(!strcmp(bots[i].Config.Id,next[j].Id))found=true;if(!found){bots[i].Config.Enabled=0;Stop(bots[i]);}}
 listedCount=0;
 for(i=0;i<total;++i){int slot=-1;for(int j=0;j<count;++j)if(!strcmp(bots[j].Config.Id,next[i].Id))slot=j;
  if(slot<0){if(count>=Capacity){Log("ERROR",next[i].Id,"runtime_identity_history_full");continue;}slot=count++;ZeroMemory(&bots[slot],sizeof(Runtime));lstrcpyA(bots[slot].State,"offline");}
  if(bots[slot].Player&&(strcmp(bots[slot].Config.Name,next[i].Name)||bots[slot].Config.Profession!=next[i].Profession||bots[slot].Config.Gender!=next[i].Gender)){Log("ERROR",next[i].Id,"identity_change_rejected");continue;}
  bots[slot].Config=next[i];listedSlots[listedCount++]=slot;
 }
 revision=rev;expires=lease;
}
static bool PlayerKey(int player,std::string& key);
static void MemoryPath(const Runtime& r,char* path);
static bool Remember(Runtime& r,int player,int hostility,int trust,const char* kind);
#include "PhongThanBotSocial.inl"
static void AppendCompanionStatus(FILE* file,Runtime& r);
static void AppendAutonomyStatus(FILE*,Runtime&);
static void AppendProgressionStatus(FILE*,Runtime&);
static void Status(){
 FILE* f=fopen("BotAIgent\\status.json.tmp","wb");if(!f)return;
 fprintf(f,"{\"Version\":1,\"CommandProtocol\":2,\"Pid\":%lu,\"UtcSeconds\":%lu,\"Revision\":%lu,\"Bots\":[",GetCurrentProcessId(),(unsigned long)time(0),revision);
 for(int i=0;i<count;++i){Runtime& r=bots[i];int n=r.Player?Player[r.Player].m_nIndex:0,map=0,x=0,y=0,life=0,maxlife=0,level=0;
  if(n>0&&n<MAX_NPC){KNpc& npc=Npc[n];if(npc.m_SubWorldIndex>=0&&npc.m_SubWorldIndex<MAX_SUBWORLD)map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;npc.GetMpsPos(&x,&y);life=npc.m_CurrentLife;maxlife=npc.m_CurrentLifeMax;level=npc.m_Level;}
  int team=r.Player&&Player[r.Player].m_cTeam.m_nFlag?Player[r.Player].m_cTeam.m_nID:-1;
  fprintf(f,"%s{\"Id\":\"%s\",\"Player\":%d,\"Entity\":%d,\"Map\":%d,\"X\":%d,\"Y\":%d,\"Life\":%d,\"MaxLife\":%d,\"Level\":%d,\"State\":\"%s\",\"Error\":\"%s\",\"Team\":%d,\"TargetMap\":%d,\"TargetLevel\":%d,\"TargetEntity\":%lu,\"Sleeping\":%s,\"MobKills\":%lu",i?",":"",r.Config.Id,r.Player,n,map,x,y,life,maxlife,level,r.State,r.Error,team,r.GoalMap,r.GoalLevel,r.GoalEntity,r.Player&&Player[r.Player].m_bSleepMode?"true":"false",r.MobKills);AppendCompanionStatus(f,r);AppendAutonomyStatus(f,r);AppendProgressionStatus(f,r);fputc('}',f);
 }fprintf(f,"]");SocialObservers(f);fprintf(f,"}");bool ok=!ferror(f);if(fclose(f))ok=false;if(ok)MoveFileExA("BotAIgent\\status.json.tmp","BotAIgent\\status.json",MOVEFILE_REPLACE_EXISTING);
}
static bool BotStoreHand(KPlayer&);
#include "PhongThanBotTraining.inl"
static bool PlayerKey(int player,std::string& key);
#include "PhongThanBotChatQueue.inl"
#include "PhongThanBotEquipment.inl"
#include "PhongThanBotCombat.inl"
static void UpgradeLegacyStarter(Runtime& r,const PHONGTHAN_CHARACTER_STATE_HEADER* state){
 if(state->FightLevel!=1||state->ItemCount||state->Profession>=3)return;
 char marker[180];sprintf(marker,"BotAIgent\\characters\\%s.pthc.before-starter-v1",r.Config.Id);
 if(GetFileAttributesA(marker)==(DWORD)-1)return;
 sprintf(marker,"BotAIgent\\characters\\%s.pthc.starter-live-v1.done",r.Config.Id);if(GetFileAttributesA(marker)!=(DWORD)-1)return;
 // An old role has managed magic snapshots. Add through the live item list,
 // then let normal Save persist both the item snapshot and the character.
 PHONGTHAN_CHARACTER_ITEM_RECORD record;if(!PhongThanInitializeStarterWeapon(record,state->Profession))return;
 KItem item;ZeroMemory(&item,sizeof(item));int levels[PHONGTHAN_ITEM_PROPERTY_VALUE_COUNT];ZeroMemory(levels,sizeof(levels));
 if(!ItemGen.Gen_ExistEquipmentByTemplateRow(record.DetailType,record.TemplateRow,0,levels,0,1,&item))return;
 item.SetParam(record.ScriptParam);item.SetStackNum(1);item.GetBackLocal()->Release();
 int index=ItemSet.Add(&item);if(index<=0)return;
 if(!Player[r.Player].m_ItemList.Add(index,record.Container,record.SlotX,record.SlotY,false)){ItemSet.Remove(index);return;}
 Log("INFO",r.Config.Id,"legacy_starter_weapon_added_through_inventory");
 if(!Player[r.Player].Save())Log("ERROR",r.Config.Id,"legacy_starter_save_pending");
}
static void CompanionTradeTick(Runtime& r,DWORD now);
#include "PhongThanBotDurability.inl"
#include "PhongThanBotWarriorCleanup.inl"
#include "../../../Headers/PhongThanBotWorkBudget.h"

static void BotWorkBudgetSettings(DWORD now){
 static DWORD checked=0;if(checked&&now-checked<1000)return;checked=now;
 int ms=GetPrivateProfileIntA("Budget","DecisionMilliseconds",0,"BotAIgent\\performance.ini");
 int saves=GetPrivateProfileIntA("Budget","MaxAutoSavesPerTick",0,"BotAIgent\\performance.ini");
 botDecisionMilliseconds=ms>=0&&ms<=50?ms:0;botAutoSavesPerTick=saves>=0&&saves<=64?saves:0;
}
static void Tick(){
 DWORD now=GetTickCount();static DWORD lastMovement=0;if(now-lastMovement>=100){lastMovement=now;PTPerfScope movementPerformance(PTPERF_BOT_MOVEMENT);MovementTick();}
 static DWORD lastChatPoll=0,lastStatus=0;
 if(now-lastPoll>=1000){lastPoll=now;if(GetFileAttributesA("BotAIgent")==((DWORD)-1))return;{PTPerfScope controlPerformance(PTPERF_BOT_CONTROL);BotLoadAutonomy(now);Poll();}{PTPerfScope socialPerformance(PTPERF_BOT_SOCIAL);SocialTick();}{PTPerfScope indexesPerformance(PTPERF_BOT_INDEXES);RefreshTrainingIndex();BotIndexDrops();}routeBudget=4;}
 if(now-lastChatPoll>=100){lastChatPoll=now;PTPerfScope chatPerformance(PTPERF_BOT_CHAT);PollChat();}
 if(count>0)BotLoadStory();
 bool leased=expires>=(unsigned long)time(0)&&expires<=(unsigned long)time(0)+30;
 BotWorkBudgetSettings(now);
 // Bound path work per world tick, rather than four paths per second for all bots.
 if(botDecisionMilliseconds>0)routeBudget=2;
 static int turn=0;int batch=PTBotDecisionBatch(listedCount),processed=0,saves=0;
 LARGE_INTEGER decisionStarted;QueryPerformanceCounter(&decisionStarted);
 {PTPerfScope decisionsPerformance(PTPERF_BOT_DECISIONS);
 for(int visit=0;visit<batch;++visit){
  if(botDecisionMilliseconds>0){LARGE_INTEGER current;QueryPerformanceCounter(&current);double elapsed=(current.QuadPart-decisionStarted.QuadPart)*1000.0/g_PTServerPerformance.frequency.QuadPart;if(!PTBotMayStartDecision(processed,elapsed,botDecisionMilliseconds))break;}
  ++processed;int i=listedSlots[(visit+turn)%listedCount];Runtime& r=bots[i];
  if(!leased||!r.Config.Enabled){Stop(r);continue;}
  if(!r.Player){if(r.Retry&&now-r.Retry<PTBotRetryDelay(r.SpawnFailures))continue;r.Retry=now;std::vector<BYTE> state;
   if(ReservedName(r.Config.Name)){lstrcpyA(r.State,"error");lstrcpyA(r.Error,"name_reserved_by_human_character");Log("ERROR",r.Config.Id,r.Error);continue;}
   const char* readError=0;if(!ReadState(r.Config,state,readError)){lstrcpyA(r.State,"error");lstrcpynA(r.Error,readError?readError:"character_state_invalid",sizeof(r.Error));Log("ERROR",r.Config.Id,r.Error);continue;}
   char spawnError[100]={0};r.Player=PlayerSet.AddServerBot((const PHONGTHAN_CHARACTER_STATE_HEADER*)&state[0],spawnError,sizeof(spawnError));
   if(!r.Player){if(r.SpawnFailures<20)++r.SpawnFailures;lstrcpyA(r.State,"retry_spawn");lstrcpynA(r.Error,spawnError[0]?spawnError:"spawn_or_load_failed",sizeof(r.Error));Log("ERROR",r.Config.Id,r.Error);continue;}
   r.SpawnFailures=0;
   UpgradeLegacyStarter(r,(const PHONGTHAN_CHARACTER_STATE_HEADER*)&state[0]);
   r.LastSave=now-Hash(r.Config.Id,32)%30000;r.Error[0]=0;lstrcpyA(r.State,"idle");Log("INFO",r.Config.Id,"online_server_player");
  }
  if(now-r.LastSave>=30000&&PTBotMayAutoSave(saves,botAutoSavesPerTick)){++saves;PTPerfScope savePerformance(PTPERF_SAVE);if(!Player[r.Player].Save()){lstrcpyA(r.Error,"autosave_failed");Log("ERROR",r.Config.Id,r.Error);}else r.Error[0]=0;r.LastSave=now;}
  int n=Player[r.Player].m_nIndex;if(n<=0||n>=MAX_NPC)continue;KNpc& npc=Npc[n];
  CompanionTradeTick(r,now);BotCleanupWarriorLoot(r);BotMaintainDurability(r,now);BotLearnOwned(r,now);BotManageEquipment(r,now);BotWatchProgress(r,now);Train(r,now);BotCombatSnapshot(r,training[i],now);

 }
 }
 if(listedCount)turn=PTBotNextDecisionTurn(turn,processed,listedCount);if(now-lastStatus>=1000){lastStatus=now;PTPerfScope statusPerformance(PTPERF_BOT_STATUS);Status();}
}
static bool PlayerKey(int player,std::string& key){
 if(player<=0||player>=MAX_PLAYER||!Player[player].m_PlayerDBLoad||Player[player].m_bIsBot)return false;
 const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)Player[player].m_SaveBuffer;
 if(!PhongThanValidateCharacterState(s,s->StateSize))return false;
 key="";for(int part=0;part<2;++part){const char* name=part?Player[player].Name:Player[player].AccountName;
  for(int i=0;name[i];++i){char h[3];sprintf(h,"%02x",(unsigned char)name[i]);key+=h;}key+='-';}
 char epoch[16];sprintf(epoch,"%lu",s->RoleTime);key+=epoch;return true;
}
static void MemoryPath(const Runtime& r,char* path){sprintf(path,"BotAIgent\\memory\\%s.ini",r.Config.Id);}
static bool Remember(Runtime& r,int player,int hostility,int trust,const char* kind){
 std::string key;if(!PlayerKey(player,key))return false;CreateDirectoryA("BotAIgent\\memory",0);
 char path[128],temp[144];MemoryPath(r,path);sprintf(temp,"%s.tmp",path);
 int h=GetPrivateProfileIntA(key.c_str(),"Hostility",0,path)+hostility,t=GetPrivateProfileIntA(key.c_str(),"Trust",0,path)+trust;
 if(h<0)h=0;if(h>100)h=100;if(t< -100)t=-100;if(t>100)t=100;
 if(GetFileAttributesA(path)!=(DWORD)-1){if(!CopyFileA(path,temp,FALSE))return false;}else{FILE* f=fopen(temp,"wb");if(!f)return false;fclose(f);}
 char value[32];sprintf(value,"%d",h);bool ok=WritePrivateProfileStringA(key.c_str(),"Hostility",value,temp)!=0;
 sprintf(value,"%d",t);ok=WritePrivateProfileStringA(key.c_str(),"Trust",value,temp)!=0&&ok;
 sprintf(value,"%lu",(unsigned long)time(0));ok=WritePrivateProfileStringA(key.c_str(),"LastUtc",value,temp)!=0&&ok;
 ok=WritePrivateProfileStringA(key.c_str(),"LastKind",kind,temp)!=0&&ok;WritePrivateProfileStringA(0,0,0,temp);
 if(ok)ok=MoveFileExA(temp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;if(!ok)DeleteFileA(temp);return ok;
}
#include "PhongThanBotCompanionChat.inl"
#include "PhongThanBotPartyChat.inl"
}
void PhongThanBotTakingHit(int victim,int attacker,int damage){PtBotAIgent::BotPvpIncident(victim,attacker,damage);}
int PhongThanBotTradeInvite(int from,int bot,int rps){
 using namespace PtBotAIgent;for(int i=0;i<count;++i)if(bots[i].Player==bot){
  Runtime& r=bots[i];CompanionState& c=companions[i];DWORD now=GetTickCount();if(rps&&botPolicy.rps&&expires>=(unsigned long)time(0)){
   std::string key;char memory[128],answer[256]={0};MemoryPath(r,memory);int hostility=PlayerKey(from,key)?GetPrivateProfileIntA(key.c_str(),"Hostility",0,memory):80;
   BotMarketChat(r,from,"rps invitation",PTBC_RPS,hostility,answer,sizeof(answer));if(answer[0])CompanionTell(r,from,answer);
  }
  if(!rps&&expires>=(unsigned long)time(0)&&(!c.Mode||(CompanionOwns(r,from)&&(c.Mode<PTBC_SALE_QUOTE||c.Mode==PTBC_GOTO||c.Mode==PTBC_VTT||c.Mode==PTBC_TRADE)))){
   std::string key;char memory[128],answer[256]={0};MemoryPath(r,memory);int hostility=PlayerKey(from,key)?GetPrivateProfileIntA(key.c_str(),"Hostility",0,memory):80;BotMarketChat(r,from,"giao dich",PTBC_TRADE,hostility,answer,sizeof(answer));if(answer[0])CompanionTell(r,from,answer);
  }
  bool consent=CompanionOwns(r,from)&&(int)(c.GiftUntil-now)>0&&expires>=(unsigned long)time(0)&&((rps&&botPolicy.rps&&c.Mode==PTBC_RPS)||(!rps&&(c.Mode==PTBC_GIFT||c.Mode==PTBC_TRADE||(c.Mode==PTBC_SALE_MEET&&botMarket[i].arrived))));
  if(consent)Npc[Player[bot].m_nIndex].SetFightMode(0);
  TRADE_REPLY_START_COMMAND reply;ZeroMemory(&reply,sizeof(reply));reply.m_nDestIdx=from;reply.m_bFolkGame=rps?TRUE:FALSE;reply.m_bDecision=consent?1:0;Player[bot].c2sTradeReplyStart((BYTE*)&reply);
  Log("TRADE",r.Config.Id,Player[bot].CheckTrading()?"normal_trade_session_opened":"trade_declined_or_engine_rejected");return 1;
 }return 0;
}
int PhongThanBotTeamInvite(int captain,int member){
 using namespace PtBotAIgent;
 if(captain<=0||captain>=MAX_PLAYER||member<=0||member>=MAX_PLAYER||!Player[member].m_bIsBot)return 0;
 for(int i=0;i<count;++i)if(bots[i].Player==member){
  std::string key;char file[128];MemoryPath(bots[i],file);
  int hostile=PlayerKey(captain,key)?GetPrivateProfileIntA(key.c_str(),"Hostility",0,file):0;
  int npc=Player[member].m_nIndex,leader=Player[captain].m_nIndex;
  bool accept=npc>0&&npc<MAX_NPC&&leader>0&&leader<MAX_NPC&&!Player[member].m_cTeam.m_nFlag&&hostile<40&&Npc[npc].m_CurrentLife>0&&abs(Npc[npc].m_Level-Npc[leader].m_Level)<=30;
  Player[captain].m_cTeam.GetInviteReply(captain,member,accept?1:0);
  Log("PARTY",bots[i].Config.Id,accept?"party_invite_accepted":"party_invite_declined_relationship_or_level");return 1;
 }return 0;
}
BOOL PhongThanBotPersist(int player,const PHONGTHAN_CHARACTER_STATE_HEADER* state){
 for(int i=0;i<PtBotAIgent::count;++i)if(PtBotAIgent::bots[i].Player==player){bool saved=PtBotAIgent::WriteState(PtBotAIgent::bots[i].Config.Id,state);if(saved)PtBotAIgent::bots[i].LastSave=GetTickCount();return saved?TRUE:FALSE;}return FALSE;
}
void PhongThanBotKilled(int victim,int attacker){
 if(victim>0&&victim<MAX_NPC&&attacker>0&&attacker<MAX_NPC&&Npc[victim].m_Kind==kind_normal&&Npc[attacker].IsPlayer()){
  int killer=Npc[attacker].GetPlayerIdx();for(int k=0;k<PtBotAIgent::count;++k)if(PtBotAIgent::bots[k].Player==killer){PtBotAIgent::Runtime& owner=PtBotAIgent::bots[k];++owner.MobKills;char record[160];sprintf(record,"monster_kill_confirmed entity=%lu template=%d monster_level=%d total=%lu",Npc[victim].m_dwID,Npc[victim].m_NpcSettingIdx,Npc[victim].m_Level,owner.MobKills);PtBotAIgent::Log("COMBAT",owner.Config.Id,record);break;}return;
 }
 if(victim<=0||victim>=MAX_NPC||attacker<=0||attacker>=MAX_NPC||!Npc[victim].IsPlayer()||!Npc[attacker].IsPlayer())return;
 int bot=Npc[victim].GetPlayerIdx(),killer=Npc[attacker].GetPlayerIdx();
 for(int hit=0;hit<PtBotAIgent::listedCount;++hit){int slot=PtBotAIgent::listedSlots[hit];if(PtBotAIgent::bots[slot].Player==killer){++PtBotAIgent::botPvp[slot].kills;PtBotAIgent::Log("PVP",PtBotAIgent::bots[slot].Config.Id,"player_kill_confirmed_by_death_callback");break;}}
 for(int i=0;i<PtBotAIgent::count;++i){PtBotAIgent::Runtime& r=PtBotAIgent::bots[i];if(r.Player==bot){
  if(Player[killer].m_bIsBot){PtBotAIgent::Log("PVP",r.Config.Id,"bot_attacker_retaliation_recorded");break;}
  if(PtBotAIgent::Remember(r,killer,40,-25,"killed_by_player"))PtBotAIgent::Log("MEMORY",r.Config.Id,"remembered_killer");
  else PtBotAIgent::Log("ERROR",r.Config.Id,"memory_write_failed");break;
 }}
}
int PhongThanBotWhisper(int from,const char* target,const char* text,int length){
 using namespace PtBotAIgent;
 if(from<=0||from>=MAX_PLAYER||Player[from].m_nNetConnectIdx<0||length<1||length>255)return 0;
 Runtime* bot=0;for(int i=0;i<count;++i)if(bots[i].Player&&!stricmp(bots[i].Config.Name,target)){bot=&bots[i];break;}if(!bot)return 0;
 static DWORD last[MAX_PLAYER],owner[MAX_PLAYER];DWORD now=GetTickCount();if(!forcedIntent&&owner[from]==Player[from].m_dwID&&now-last[from]<600)return 1;if(!forcedIntent){owner[from]=Player[from].m_dwID;last[from]=now;}
 char commandQuery[256],commandRaw[256];memcpy(commandRaw,text,length);commandRaw[length]=0;PTBotFoldPartySpeech(commandRaw,commandQuery,sizeof(commandQuery));int command=PTBotReleaseCommand(commandQuery)?PTBC_CANCEL:forcedSemantic?forcedCommand:PTBotCommandMode(commandQuery);if(command==PTBC_CANCEL)CancelQueuedCompanionChats(from,bot-bots);

 if(!forcedIntent){Log("CHAT",bot->Config.Id,chatScope?"nearby_message_received":"private_message_received");if(command!=PTBC_CANCEL&&expires>=(unsigned long)time(0)&&QueueChat(from,bot-bots,text,length))return 1;}
 char raw[256],query[256];memcpy(raw,text,length);raw[length]=0;memcpy(query,raw,length+1);CharLowerBuffA(query,length);
 if(forcedIntent==2)lstrcpyA(query,"giup");else if(forcedIntent==3)lstrcpyA(query,"mua");else if(forcedIntent==4)lstrcpyA(query,"xin loi");else if(forcedIntent==5)lstrcpyA(query,"party");
 char answer[256];lstrcpyA(answer,"\103\150\265\157\040\142\271\156\056\040\102\271\156\040\155\165\350\156\040\150\341\151\040\155\343\156\040\256\345\040\156\265\157\054\040\150\141\171\040\162\361\040\155\327\156\150\040\256\151\040\256\251\165\077");
 std::string key;char memory[128];MemoryPath(*bot,memory);int hostility=PlayerKey(from,key)?GetPrivateProfileIntA(key.c_str(),"Hostility",0,memory):0;
 bool commandHandled=false;if(command==PTBC_PARTY){int previous=forcedPartyAction;forcedPartyAction=1;PartyChat(*bot,from,raw,hostility,answer,sizeof(answer));forcedPartyAction=previous;commandHandled=true;}else commandHandled=CompanionChat(*bot,from,raw,command,hostility,answer,sizeof(answer));
 if(commandHandled){}else if((!forcedSemantic||forcedIntent==5)&&(strstr(query,"party")||strstr(query,"to doi"))){
  PartyChat(*bot,from,raw,hostility,answer,sizeof(answer));
 }else if(strstr(query,"giup")||strstr(query,"boss")||strstr(query,"van luong")){
  if(hostility>=40)lstrcpyA(answer,"\114\307\156\040\164\162\255\355\143\040\143\337\156\040\256\270\156\150\040\155\327\156\150\054\040\147\151\352\040\154\271\151\040\156\150\352\040\147\151\363\160\040\265\077\040\115\327\156\150\040\143\150\255\141\040\256\345\156\147\040\375\040\256\251\165\056");
  else if(bot->Config.Train)lstrcpyA(answer,"\115\327\156\150\040\256\141\156\147\040\164\162\141\151\156\056\040\110\151\326\156\040\164\271\151\040\143\150\255\141\040\156\150\313\156\040\256\151\040\150\351\040\164\350\156\147\040\150\141\171\040\256\270\156\150\040\142\157\163\163\040\256\255\356\143\056");
  else lstrcpyA(answer,"\115\327\156\150\040\143\150\255\141\040\156\150\313\156\040\256\151\040\150\351\040\164\350\156\147\040\150\141\171\040\256\270\156\150\040\142\157\163\163\040\256\255\356\143\040\154\363\143\040\156\265\171\056");
 }else if(strstr(query,"xin loi")){
  lstrcpyA(answer,hostility>0?"\115\327\156\150\040\156\147\150\145\040\162\345\151\054\040\156\150\255\156\147\040\143\307\156\040\164\150\352\151\040\147\151\141\156\040\256\323\040\164\151\156\040\142\271\156\040\154\271\151\056":"\113\150\253\156\147\040\163\141\157\056\040\102\271\156\040\143\307\156\040\147\327\077");
 }else if(strstr(query,"mua")||strstr(query,"ban do")){
  lstrcpyA(answer,"\107\366\151\040\256\363\156\147\040\164\252\156\040\155\343\156\040\256\345\040\142\271\156\040\143\307\156\056\040\115\327\156\150\040\163\317\040\153\151\323\155\040\164\162\141\040\164\363\151\040\166\265\040\153\150\157\073\040\143\150\255\141\040\143\150\350\164\040\147\151\141\157\040\144\336\143\150\056");
  KPlayer& p=Player[bot->Player];char folded[256];PTBotFoldPartySpeech(raw,folded,sizeof(folded));bool worn=strstr(folded,"dang mac")||strstr(folded,"dang deo")||strstr(folded,"set do");bool weapon=strstr(folded,"vu khi");PlayerItem* item=p.m_ItemList.GetFirstItem();bool found=false;
  while(item){int idx=item->nIdx;if(idx>0&&idx<MAX_ITEM&&Item[idx].GetID()&&Item[idx].GetName()[0]&&(strstr(raw,Item[idx].GetName()[0]=='#'?Item[idx].GetName()+1:Item[idx].GetName())||(worn&&item->nPlace==pos_equip&&(!weapon||item->nX==itempart_weapon)))){
    found=true;if(item->nPlace==pos_equip||item->nPlace==pos_ptinactiveequip||item->nPlace==pos_ptrealmescrow)lstrcpyA(answer,"M\343n n\265y tui \256ang d\357ng, ch\255a b\270n \253ng \254i. \247\345 d\255 tui m\355i rao.");
    else if(Item[idx].m_CommonAttrib.bLockTrade||Item[idx].GetLock()->nState!=0)lstrcpyA(answer,"\115\343\156\040\256\345\040\256\343\040\256\141\156\147\040\153\150\343\141\054\040\153\150\253\156\147\040\147\151\141\157\040\144\336\143\150\040\256\255\356\143\056");
    else lstrcpyA(answer,"\115\327\156\150\040\143\343\040\155\343\156\040\256\345\040\256\343\054\040\156\150\255\156\147\040\143\150\255\141\040\155\353\040\143\150\370\143\040\156\250\156\147\040\143\150\350\164\040\147\151\270\040\166\265\040\147\151\141\157\040\144\336\143\150\040\164\371\040\256\351\156\147\056");break;
   }item=p.m_ItemList.GetNextItem();}
  if(!found&&length>8)lstrcpyA(answer,"\115\327\156\150\040\143\150\255\141\040\170\270\143\040\256\336\156\150\040\256\255\356\143\040\155\343\156\040\142\271\156\040\150\341\151\056\040\107\366\151\040\256\363\156\147\040\164\252\156\040\150\157\306\143\040\154\151\252\156\040\153\325\164\040\166\313\164\040\160\150\310\155\040\156\150\320\056");
 }
 if(!commandHandled&&(forcedIntent==1||forcedIntent==2)&&generatedReply[0])lstrcpynA(answer,generatedReply,sizeof(answer));
 AdaptChatStyle(answer);if(commandHandled)RememberDirectTurn(*bot,from,raw,answer);else RememberChatTurn(raw,length,answer);
 if(chatScope){char addressed[256];_snprintf(addressed,sizeof(addressed)-1,"%s: %s",Npc[Player[from].m_nIndex].Name,answer);addressed[255]=0;lstrcpyA(answer,addressed);}
 if(chatScope!=0&&chatScope!=1){bool sent=::PhongThanBotReplyPublic(bot->Player,from,(DWORD)chatScope,answer,strlen(answer));Log("CHAT",bot->Config.Id,sent?"addressed_channel_reply_published":"channel_reply_rate_limited");return 1;}
 if(chatScope){
  char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+255];ZeroMemory(packet,sizeof(packet));int len=strlen(answer),total=sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+len;
  tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendchat;h->wLength=total-1;
  CHAT_CHANNELCHAT_SYNC* msg=(CHAT_CHANNELCHAT_SYNC*)(h+1);msg->ProtocolType=chat_channelchat;msg->wSize=sizeof(*msg)+len-1;msg->packageID=(DWORD)-1;msg->channelid=1;msg->sentlen=(BYTE)len;lstrcpynA(msg->someone,bot->Config.Name,sizeof(msg->someone));memcpy(msg+1,answer,len);
  int actor=Player[bot->Player].m_nIndex;for(int p=1;p<MAX_PLAYER;++p)if(Player[p].m_PlayerDBLoad&&!Player[p].m_bIsBot&&Player[p].m_nNetConnectIdx>=0&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].m_SubWorldIndex==Npc[actor].m_SubWorldIndex&&NpcSet.GetDistance(actor,Player[p].m_nIndex)<=1024)g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,packet,total);
  Log("CHAT",bot->Config.Id,"nearby_reply_published");return 1;
 }
 char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_SOMEONECHAT_SYNC)+255];ZeroMemory(packet,sizeof(packet));int len=strlen(answer),total=sizeof(tagExtendProtoHeader)+sizeof(CHAT_SOMEONECHAT_SYNC)+len;
 tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendchat;h->wLength=total-1;
 CHAT_SOMEONECHAT_SYNC* msg=(CHAT_SOMEONECHAT_SYNC*)(h+1);msg->ProtocolType=chat_someonechat;msg->wSize=sizeof(*msg)+len-1;msg->sentlen=(BYTE)len;lstrcpynA(msg->someone,bot->Config.Name,sizeof(msg->someone));memcpy(msg+1,answer,len);
 g_pServer->PackDataToClient(Player[from].m_nNetConnectIdx,packet,total);Log("CHAT",bot->Config.Id,"whisper_replied_from_authoritative_state");return 1;
}
int PhongThanBotNearby(int from,const char* text,int length){
 using namespace PtBotAIgent;if(from<=0||from>=MAX_PLAYER||length<1||length>255)return 0;
 int actor=Player[from].m_nIndex;if(actor<=0||actor>=MAX_NPC)return 0;int selected=-1,best=769,named=-1;
 char query[256];memcpy(query,text,length);query[length]=0;CharLowerBuffA(query,length);
 for(int i=0;i<count;++i)if(bots[i].Player){int npc=Player[bots[i].Player].m_nIndex;if(npc<=0||npc>=MAX_NPC||Npc[npc].m_SubWorldIndex!=Npc[actor].m_SubWorldIndex||Npc[npc].m_CurrentLife<=0)continue;
  int distance=NpcSet.GetDistance(actor,npc);if(distance<0||distance>768)continue;
  char name[25];lstrcpyA(name,bots[i].Config.Name);CharLowerBuffA(name,strlen(name));if(strstr(query,name)){selected=named=i;break;}
  if(distance<best){best=distance;selected=i;}
 }
 char folded[256];PTBotFoldPartySpeech(query,folded,sizeof(folded));int mode=PTBotCommandMode(folded);
 struct NearbyThread {DWORD Owner,Until;int Bot;};static NearbyThread threads[MAX_PLAYER];DWORD threadNow=GetTickCount();
 if(named<0&&threads[from].Owner==Player[from].m_dwID&&(int)(threads[from].Until-threadNow)>0&&threads[from].Bot>=0&&threads[from].Bot<count&&bots[threads[from].Bot].Player)selected=threads[from].Bot;
 if(named<0&&mode!=PTBC_NONE&&!(threads[from].Owner==Player[from].m_dwID&&(int)(threads[from].Until-threadNow)>0))for(int ownerBot=0;ownerBot<count;++ownerBot)if(bots[ownerBot].Player&&CompanionOwns(bots[ownerBot],from)&&(companions[ownerBot].Mode||companions[ownerBot].Pending)){selected=ownerBot;break;}
 if(selected<0)return 0;static DWORD last[MAX_PLAYER],owner[MAX_PLAYER];DWORD now=GetTickCount();if(owner[from]==Player[from].m_dwID&&now-last[from]<600)return 1;owner[from]=Player[from].m_dwID;last[from]=now;threads[from].Owner=Player[from].m_dwID;threads[from].Until=now+120000;threads[from].Bot=selected;
 int previous=chatScope;chatScope=1;int result=PhongThanBotWhisper(from,bots[selected].Config.Name,text,length);chatScope=previous;return result;
}

int PhongThanBotPublicChat(int from,DWORD channel,const char* text,int length){
 using namespace PtBotAIgent;if(from<=0||from>=MAX_PLAYER||!SocialAlive(from)||Player[from].m_bIsBot||Player[from].m_nNetConnectIdx<0||length<1||length>255||(channel!=PT_CHAT_WORLD&&channel!=PT_CHAT_TRADE&&channel!=PT_CHAT_AREA))return 0;
 struct Thread {DWORD Owner,Channel,Until;int Bot;};static Thread threads[64];static DWORD last=0;DWORD now=GetTickCount();if(now-last<3000)return 0;
 int actor=Player[from].m_nIndex,selected=-1,entry=from%64;char query[256];memcpy(query,text,length);query[length]=0;CharLowerBuffA(query,length);
 for(int i=0;i<count;++i)if(bots[i].Player&&SocialAlive(bots[i].Player)){int n=Player[bots[i].Player].m_nIndex;if(channel==PT_CHAT_AREA&&Npc[n].m_SubWorldIndex!=Npc[actor].m_SubWorldIndex)continue;char name[25];lstrcpyA(name,bots[i].Config.Name);CharLowerBuffA(name,strlen(name));if(strstr(query,name)){selected=i;break;}}
 if(selected<0&&threads[entry].Owner==Player[from].m_dwID&&threads[entry].Channel==channel&&(int)(threads[entry].Until-now)>0){int i=threads[entry].Bot;if(i>=0&&i<count&&bots[i].Player&&SocialAlive(bots[i].Player)&&(channel!=PT_CHAT_AREA||Npc[Player[bots[i].Player].m_nIndex].m_SubWorldIndex==Npc[actor].m_SubWorldIndex))selected=i;}
 if(selected<0){unsigned best=0;for(int i=0;i<count;++i)if(bots[i].Player&&SocialAlive(bots[i].Player)){KNpc& npc=Npc[Player[bots[i].Player].m_nIndex];if(!PTBotPublicChannelAllowed(npc.m_Level,10,channel)||(channel==PT_CHAT_AREA&&npc.m_SubWorldIndex!=Npc[actor].m_SubWorldIndex))continue;unsigned score=Hash(bots[i].Config.Id,32)^Player[from].m_dwID^(now/60000);if(selected<0||score>best){selected=i;best=score;}}}
 if(selected<0)return 0;threads[entry].Owner=Player[from].m_dwID;threads[entry].Channel=channel;threads[entry].Until=now+120000;threads[entry].Bot=selected;last=now;
 int previous=chatScope;chatScope=(int)channel;int result=PhongThanBotWhisper(from,bots[selected].Config.Name,text,length);chatScope=previous;return result;
}

static void PhongThanBotItemOffer(int from,const char* recipient,int item){
 using namespace PtBotAIgent;if(!recipient||from<=0||from>=MAX_PLAYER||Player[from].m_bIsBot)return;
 for(int b=0;b<count;++b)if(bots[b].Player&&!strcmp(bots[b].Config.Name,recipient)){BotMarketIncomingOffer(bots[b],from,item);return;}
}

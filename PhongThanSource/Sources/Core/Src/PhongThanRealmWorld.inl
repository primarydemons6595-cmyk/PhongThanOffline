#include "../../../Headers/PhongThanRealmWorld.h"
#include <vector>
#include <time.h>
namespace PtRealmWorldRuntime {
struct Receipt {char Account[32],Role[32];int Sequence,Flags;};
struct FileHeader {DWORD Magic,Version,Count;PtRealmWorld::State State;DWORD Checksum;};
static PtRealmWorld::State state;static std::vector<Receipt> receipts;static bool loaded=false,good=false;
static const char* path="CharacterStore\\realm_world.dat";
static DWORD Hash(const void* bytes,int size,DWORD h=2166136261U){const BYTE* p=(const BYTE*)bytes;for(int i=0;i<size;++i)h=(h^p[i])*16777619U;return h;}
static bool Load(){
 if(loaded)return good;loaded=true;ZeroMemory(&state,sizeof(state));receipts.clear();
 HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(f==INVALID_HANDLE_VALUE){good=GetLastError()==ERROR_FILE_NOT_FOUND;return good;}
 FileHeader h;ZeroMemory(&h,sizeof(h));DWORD got=0;bool ok=ReadFile(f,&h,sizeof(h),&got,0)&&got==sizeof(h)&&h.Magic==0x57525450&&h.Version==1&&h.Count<=100000&&PtRealmWorld::Valid(h.State)&&GetFileSize(f,0)==sizeof(h)+h.Count*sizeof(Receipt);
 if(ok){receipts.resize(h.Count);if(h.Count)ok=ReadFile(f,&receipts[0],h.Count*sizeof(Receipt),&got,0)&&got==h.Count*sizeof(Receipt);}
 CloseHandle(f);DWORD crc=Hash(&h.State,sizeof(h.State));
 if(ok&&h.Count)crc=Hash(&receipts[0],h.Count*sizeof(Receipt),crc);
 if(!ok||crc!=h.Checksum){g_DebugLog("[RealmWorld] invalid durable state; progression locked");return false;}
 for(unsigned i=0;i<receipts.size();++i)if(!receipts[i].Account[0]||!receipts[i].Role[0]||receipts[i].Account[31]||receipts[i].Role[31]||receipts[i].Sequence<0||(receipts[i].Flags&~1))return false;
 state=h.State;good=true;return true;
}
static bool Save(){
 if(!Load()||!PtRealmWorld::Valid(state)||receipts.size()>100000)return false;
 FileHeader h;ZeroMemory(&h,sizeof(h));h.Magic=0x57525450;h.Version=1;h.Count=receipts.size();h.State=state;
 h.Checksum=Hash(&h.State,sizeof(h.State));if(h.Count)h.Checksum=Hash(&receipts[0],h.Count*sizeof(Receipt),h.Checksum);
 std::string temp=std::string(path)+".tmp";HANDLE f=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
 DWORD wrote=0;bool ok=WriteFile(f,&h,sizeof(h),&wrote,0)&&wrote==sizeof(h);
 if(ok&&h.Count)ok=WriteFile(f,&receipts[0],h.Count*sizeof(Receipt),&wrote,0)&&wrote==h.Count*sizeof(Receipt);
 if(ok)ok=FlushFileBuffers(f)!=0;CloseHandle(f);
 if(ok)ok=MoveFileExA(temp.c_str(),path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temp.c_str());return ok;
}
static int Today(){return (int)((time(NULL)+25200)/86400);}
static int ReceiptIndex(int p){for(unsigned i=0;i<receipts.size();++i)if(!_stricmp(receipts[i].Account,Player[p].AccountName)&&!strcmp(receipts[i].Role,Player[p].Name))return i;return -1;}
static int LastReceipt(int p){if(!Load())return -1;int index=ReceiptIndex(p);return index<0?0:receipts[index].Sequence;}
static void GrantTitle(int p,int title){int word=title/32;unsigned long value=GetTitleQualifyWord(p,word);if(value&(1UL<<(title%32)))return;
 char text[16];sprintf(text,"%08lx",value|(1UL<<(title%32)));Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_TITLE_QUALIFY_BEGIN+word,text,TRUE);}
static bool Hero(int p){int i=ReceiptIndex(p);if(i<0){if(receipts.size()>=100000)return false;Receipt r;ZeroMemory(&r,sizeof(r));g_StrCpyLen(r.Account,Player[p].AccountName,32);g_StrCpyLen(r.Role,Player[p].Name,32);r.Flags=1;receipts.push_back(r);}else receipts[i].Flags|=1;return true;}
static void Announce(const char* text){KPhongThanScriptAction action;ZeroMemory(&action,sizeof(action));action.View=UI_NEWSINFO;action.OptionCount=NEWSMESSAGE_NORMAL;action.Operation=PHONGTHAN_SCRIPT_SHOW;action.ServerOwned=1;
 g_StrCpyLen(action.Content,text,512);int length=strlen(action.Content);int times=1;memcpy(action.Content+length,&times,sizeof(times));action.ContentLength=length+sizeof(times);PhongThanBroadcastScriptAction(action,false);}
static bool CompleteReputation(int p,int sequence){
 if(!Load()||sequence<1)return false;int i=ReceiptIndex(p);
 if(i>=0&&receipts[i].Sequence>=sequence)return true;
 if(i<0&&receipts.size()>=100000)return false;
 PtRealmWorld::State old=state;Receipt saved;bool added=i<0;
 if(added){Receipt r;ZeroMemory(&r,sizeof(r));g_StrCpyLen(r.Account,Player[p].AccountName,32);g_StrCpyLen(r.Role,Player[p].Name,32);r.Sequence=sequence;receipts.push_back(r);i=receipts.size()-1;}
 else{saved=receipts[i];receipts[i].Sequence=sequence;}
 PtRealmWorld::Reputation(state);
 if(Save())return true;state=old;if(added)receipts.pop_back();else receipts[i]=saved;return false;
}
static void Dragon(int dragon,int player,int dead){
 if(!Load())return;PtRealmWorld::State old=state;
 bool registered=player>0&&player<MAX_PLAYER&&Player[player].m_cTask.GetSaveVal(4879)==1;
 if(!PtRealmWorld::Dragon(state,Today(),dragon,registered))return;
 std::vector<Receipt> prior=receipts;
 bool heroes=true;
 if(old.Phase==2&&state.Phase==3&&registered){heroes=Hero(player);if(Player[player].m_cTeam.m_nFlag){int team=Player[player].m_cTeam.m_nID;
  for(int ordinal=1;ordinal<=MAX_TEAM_MEMBER+1;++ordinal){int mate=PhongThanQuestTeamMember(team,ordinal);if(mate<=0||mate>=MAX_PLAYER||Player[mate].m_cTask.GetSaveVal(4879)!=1)continue;
   int actor=Player[mate].m_nIndex;if(actor>0&&actor<MAX_NPC&&Npc[actor].m_SubWorldIndex==Npc[dead].m_SubWorldIndex&&NpcSet.GetDistance(actor,dead)<=1800)if(!Hero(mate))heroes=false;}
 }}
 if(!heroes||!Save()){state=old;receipts=prior;return;}
 if(old.Phase==0&&state.Phase==1)Announce(PT_REALM_RESCUED);
 if(old.Phase==2&&state.Phase==3)Announce(PT_REALM_GATE_OPEN);
}
}
bool PhongThanRealmDestination(int player,unsigned long map){
 using namespace PtRealmWorldRuntime;
 if(!PtRealmWorld::IsRealmMap(map))return true;if(player<=0||player>=MAX_PLAYER||!Load())return false;
 return PtRealmWorld::CanEnter(state,map,Player[player].m_cTask.GetSaveVal(4871),Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE),Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_LEVEL));
}
bool PhongThanRealmStoredDestination(const PHONGTHAN_CHARACTER_STATE_HEADER* role,int map){
 if(!PtRealmWorld::IsRealmMap(map))return true;if(!role||!PtRealmWorldRuntime::Load())return false;
 int stage=0,side=0,level=0;const PHONGTHAN_CHARACTER_TASK_RECORD* tasks=PhongThanCharacterTasks(role);
 for(unsigned i=0;i<role->TaskCount;++i){int* value=tasks[i].TaskId==4871?&stage:tasks[i].TaskId==TASKVALUE_PT_REALM_SIDE?&side:tasks[i].TaskId==TASKVALUE_PT_REALM_LEVEL?&level:0;
  if(value){char text[17];memcpy(text,tasks[i].Value,16);text[16]=0;*value=atoi(text);}}
 return PtRealmWorld::CanEnter(PtRealmWorldRuntime::state,map,stage,side,level);
}

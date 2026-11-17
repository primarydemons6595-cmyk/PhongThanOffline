#include "PhongThanLiveCampaigns.h"
#include "../../../Headers/PhongThanLiveCampaignPolicy.h"
#include "KSortScript.h"
#include <fstream>
#include <sstream>
namespace PtLive {
PtCampaignPolicy::Snapshot policy;
std::string applied, rejected, error, command, state="idle";
std::vector<std::string> scripts;unsigned cursor=0,failures=0;
DWORD seen[MAX_NPC]={0};
int mapJob=0,mapCursor=0;DWORD last=0,mapRetry=0;
bool Read(const char* path,std::string& bytes){std::ifstream f(path,std::ios::binary);if(!f)return false;f.seekg(0,std::ios::end);long size=(long)f.tellg();if(size<0||size>1024*1024)return false;f.seekg(0);bytes.resize(size);if(size)f.read(&bytes[0],size);return !!f;}
void Status(){char head[600];sprintf(head,"{\"pid\":%lu,\"utc\":%ld,\"revision\":%ld,\"error\":\"%s\",\"command\":\"%s\",\"state\":\"%s\",\"processed\":%u,\"total\":%u,\"failed\":%u}\n",GetCurrentProcessId(),(long)time(NULL),policy.revision,error.c_str(),command.c_str(),state.c_str(),cursor,scripts.size(),failures);FILE*f=fopen("EventService\\campaigns.status.tmp","wb");if(f){fwrite(head,1,strlen(head),f);fclose(f);MoveFileExA("EventService\\campaigns.status.tmp","EventService\\campaigns.status.json",MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);}}
void Load(){std::string bytes;if(!Read("EventService\\campaigns.snapshot",bytes)||bytes==applied||bytes==rejected)return;PtCampaignPolicy::Snapshot next;std::string why;if(PtCampaignPolicy::Parse(bytes,next,why)){policy=next;applied=bytes;rejected="";error="";}else{rejected=bytes;error=why;}}
bool ScriptPath(const std::string& s){return s.size()<100&&s.size()>8&&s.substr(0,8)=="\\script\\"&&s.find("..") == std::string::npos&&s.find(':')==std::string::npos&&s.find('"')==std::string::npos&&s.find('\0')==std::string::npos&&s.substr(s.size()-4)==".lua";}
void Commands(){std::string bytes;if(!Read("EventService\\reload.command",bytes))return;std::vector<std::string> f=PtCampaignPolicy::Split(bytes,'\t');if(f.size()!=5||!PtCampaignPolicy::Id(f[0])||f[0]==command)return;int pid,utc;if(!PtCampaignPolicy::Number(f[1],pid,1,2147483647)||pid!=(int)GetCurrentProcessId()||!PtCampaignPolicy::Number(f[2],utc,1,2147483647)||abs((long)time(NULL)-utc)>60)return;
 if(f[3]=="cancel"){command=f[0];scripts.clear();mapJob=0;state="cancelled";Status();return;}
 if(state=="running"||state=="waiting_for_players_to_leave")return;
 command=f[0];state="running";cursor=failures=0;scripts.clear();mapJob=mapCursor=0;mapRetry=0;
 if(f[3]=="script"&&ScriptPath(f[4]))scripts.push_back(f[4]);
 else if(f[3]=="scripts"||f[3]=="all_supported"){for(unsigned i=0;i<PhongThanLoadedScriptCount();++i){const char*p=PhongThanLoadedScriptPath(i);if(p&&ScriptPath(p))scripts.push_back(p);}if(f[3]=="all_supported")mapJob=-1;Load();}
 else if(f[3]=="txt"){Load();state="completed";}
 else if(f[3]=="map"){if(!PtCampaignPolicy::Number(f[4],mapJob,1,65535))state="invalid_command";}
 else state="invalid_command";
 Status();
}
void Jobs(){if(state!="running"&&state!="waiting_for_players_to_leave")return;DWORD begin=GetTickCount();char previous[MAX_PATH];g_GetFilePath(previous);g_SetFilePath("\\");
 while(cursor<scripts.size()){if(!PhongThanReloadScriptSafe(scripts[cursor].c_str())){++failures;error="script_reload_failed_see_reload_log";FILE*f=fopen("EventService\\reload.errors.log","a");if(f){fprintf(f,"%ld %s %s\n",(long)time(NULL),command.c_str(),scripts[cursor].c_str());fclose(f);}}++cursor;if(GetTickCount()-begin>=4)break;}g_SetFilePath(previous);
 if(cursor<scripts.size())return;
 if(mapJob){if(GetTickCount()-mapRetry<1000)return;mapRetry=GetTickCount();char why[100];bool found=false;for(;mapCursor<MAX_SUBWORLD;++mapCursor){int w=mapCursor;if(SubWorld[w].m_SubWorldID<=0||(mapJob!=-1&&SubWorld[w].m_SubWorldID!=mapJob))continue;found=true;if(!SubWorld[w].ReloadLiveGeometry(why,sizeof(why))){if(!strcmp(why,"waiting_for_players_to_leave")){state=why;return;}if(mapJob!=-1||strcmp(why,"protected_event_map_geometry")){++failures;error=why;}}state="running";++mapCursor;if(mapJob==-1&&mapCursor<MAX_SUBWORLD)return;break;}
 if(!found&&mapJob!=-1)++failures;mapJob=0;}
 state=failures?"completed_with_errors":"completed";Status();
}
}
int PhongThanLiveExperience(int map,int damage,int xp,int life){return PtCampaignPolicy::Experience(damage,xp,life,PtLive::policy.Rate(map,(long)time(NULL),true));}
int PhongThanLiveDropRate(int map){return PtLive::policy.Rate(map,(long)time(NULL),false);}
void PhongThanLiveCampaignTick(){PtLive::Jobs();DWORD now=GetTickCount();if(now-PtLive::last<1000)return;PtLive::last=now;PtLive::Load();PtLive::Commands();PtLive::Status();}
void PhongThanLiveCampaignReset(int index){if(index>0&&index<MAX_NPC)PtLive::seen[index]=0;}
void PhongThanLiveCampaignDrop(int index,int owner){
 if(index<=0||index>=MAX_NPC||owner<=0||owner>=MAX_PLAYER)return;KNpc& npc=Npc[index];
 if(npc.m_Kind!=kind_normal||npc.m_Experience<=0||npc.m_SubWorldIndex<0||npc.m_SubWorldIndex>=MAX_SUBWORLD||Player[owner].m_nNetConnectIdx<0)return;
 int pn=Player[owner].m_nIndex;if(pn<=0||pn>=MAX_NPC||!Npc[pn].IsPlayer()||Npc[pn].m_SubWorldIndex!=npc.m_SubWorldIndex)return;
 if(PtLive::seen[index]==npc.m_dwID)return;PtLive::seen[index]=npc.m_dwID;
 int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;long utc=(long)time(NULL);
 for(unsigned c=0;c<PtLive::policy.campaigns.size();++c){const PtCampaignPolicy::Campaign& campaign=PtLive::policy.campaigns[c];if(!campaign.extras||!campaign.Active(utc)||!campaign.scope.Matches(map))continue;
 for(unsigned d=0;d<campaign.items.size();++d){const PtCampaignPolicy::Drop& row=campaign.items[d];if(!row.enabled||!row.scope.Matches(map)||g_Random(1000000)>=row.probability)continue;
 PhongThanQuestItemTuple tuple;if(!PhongThanDecodeQuestItemTuple(row.genre,row.detail,row.part,row.level,row.series,0,tuple))continue;
 for(int q=0;q<row.quantity;++q){int magic[MAX_ITEM_MAGICLEVEL]={0};int item=ItemSet.Add(tuple.genre,tuple.series,tuple.level,tuple.luck,tuple.detail,tuple.particular,magic,g_SubWorldSet.GetGameVersion());if(item<=0)continue;
 if(!PhongThanQuestItemMatches(Item[item],tuple)||!Item[item].GetName()[0]||Item[item].GetWidth()<=0||Item[item].GetHeight()<=0||Item[item].GetObjIdx()<=0){ItemSet.Remove(item);continue;}
 char kind[32];if(!ObjSet.m_cTabFile.GetString(Item[item].GetObjIdx()+1,3,"",kind,sizeof(kind))||strcmp(kind,"Item")){ItemSet.Remove(item);continue;}
 int x,y;npc.GetMpsPos(&x,&y);POINT pos;pos.x=x;pos.y=y;SubWorld[npc.m_SubWorldIndex].GetFreeObjPos(pos);KMapPos at;at.nSubWorld=npc.m_SubWorldIndex;SubWorld[at.nSubWorld].Mps2Map(pos.x,pos.y,&at.nRegion,&at.nMapX,&at.nMapY,&at.nOffX,&at.nOffY);if(at.nRegion<0){ItemSet.Remove(item);continue;}
 KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nItemID=item;info.m_nItemWidth=Item[item].GetWidth();info.m_nItemHeight=Item[item].GetHeight();info.m_nColorID=Item[item].GetColorItem();info.m_nGenre=Item[item].GetGenre();info.m_nDetailType=Item[item].GetDetailType();info.m_nMovieFlag=info.m_nSoundFlag=1;g_StrCpyLen(info.m_szName,Item[item].GetName(),sizeof(info.m_szName));int obj=ObjSet.Add(Item[item].GetObjIdx(),at,info);if(obj<=0)ItemSet.Remove(item);else Object[obj].SetItemBelong(owner);
 }}}
}

// Included after IBBuff helpers in ScriptFuns.cpp. No protocol or role ABI changes.
#include "../../../Headers/PhongThanStall.h"
namespace PtStall {
struct License {int buff,detail,seconds,enabled;std::string maps;};
static std::vector<License> licenses;static KIniFile rules;static DWORD checked=0;static bool initialized=false;
static bool Contains(const std::string& maps,int map){char key[32];sprintf(key,",%d,",map);std::string normalized=","+maps+",";return normalized.find(key)!=std::string::npos;}
static void Defaults(){licenses.clear();for(int i=0;i<17;++i){License d;d.buff=d.detail=i<8?38+i:i<16?125+i-8:186;d.seconds=PTStallOriginalSeconds(d.detail);d.enabled=1;char map[32];sprintf(map,"%d",PTStallOriginalMap(d.detail));d.maps=d.detail==186?"1001,1002,1003,1004,1020,1021,1052":map;licenses.push_back(d);}}
static void Load(){
 DWORD now=GetTickCount();if(initialized&&now-checked<5000)return;checked=now;
 if(!initialized){Defaults();initialized=true;}
 KIniFile next;if(!next.Load("\\settings\\phongthan\\StallPolicy.ini"))return;
 int version=0,count=0;next.GetInteger("Main","Version",0,&version);next.GetInteger("Main","Count",-1,&count);if(version!=1||count<0||count>PT_STALL_MAX_LICENSES)return;
 std::vector<License> parsed;
 for(int i=0;i<count;++i){char section[32],maps[1024];sprintf(section,"License%d",i);License d;
  next.GetInteger(section,"Buff",0,&d.buff);next.GetInteger(section,"Detail",0,&d.detail);next.GetInteger(section,"Seconds",0,&d.seconds);next.GetInteger(section,"Enabled",0,&d.enabled);next.GetString(section,"Maps","",maps,sizeof(maps));d.maps=maps;
  if(!PTStallBuff(d.buff)||d.seconds<1||d.seconds>2592000||d.enabled<0||d.enabled>1||!maps[0])return;
  if(d.buff<9500?d.detail!=d.buff:d.detail!=62000+d.buff-9500)return;
  for(int j=0;j<(int)parsed.size();++j)if(parsed[j].buff==d.buff||parsed[j].detail==d.detail)return;
  parsed.push_back(d);
 }
 licenses.swap(parsed);rules.Load("\\settings\\phongthan\\StallPolicy.ini");
}
static bool Actor(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].m_SubWorldIndex>=0&&Npc[Player[p].m_nIndex].m_SubWorldIndex<MAX_SUBWORLD;}
}
bool PhongThanStallAllowed(int p){
 using namespace PtStall;if(!Actor(p))return false;Load();KNpc& actor=Npc[Player[p].m_nIndex];
 if(actor.m_CurrentLife<=0||actor.m_Doing==do_death||actor.m_bRideHorse||actor.m_FightMode)return false;
 int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,mode=PTStallDefaultMode(map);char key[32];sprintf(key,"%d",map);rules.GetInteger("Maps",key,mode,&mode);
 if(mode==2)return true;if(mode!=1)return false;
 PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);DWORD now=KSG_GetCurSec();
 for(int i=0;i<(int)licenses.size();++i){const License& d=licenses[i];if(!d.enabled||!Contains(d.maps,map))continue;int at=FindIBBuffSlot(store,d.buff);if(at>=0&&store->Entry[at].nTimes>0&&store->Entry[at].dwExpireTime>now)return true;}
 return false;
}
int PhongThanUseStallLicense(int p,int item){
 using namespace PtStall;if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_ibitem||Item[item].GetParticular()!=0)return -1;
 Load();int detail=Item[item].GetDetailType();const License* found=0;for(int i=0;i<(int)licenses.size();++i)if(licenses[i].detail==detail){found=&licenses[i];break;}
 if(!found)return PTStallBuff(detail)||(detail>=62000&&detail<62032)?0:-1;
 if(!found->enabled||!Actor(p)||Player[p].GetLockState()||Player[p].CheckTrading()||!PtPetCarried(p,item)||Item[item].GetLock()->IsLock()||(Item[item].GetExpireTime()&&Item[item].GetExpireTime()<=KSG_GetCurSec()))return 0;
 PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);if(!store||FindIBBuffSlot(store,found->buff)>=0)return 0;
 // A new permit needs two serialized buff values and (at most) one clock value.
 // Refuse before consumption if the existing role's task payload is already full.
 int taskCount=0;for(int task=0;task<MAX_TASK;++task)if(Player[p].m_cTask.GetSaveStr(task)[0])++taskCount;
 if(taskCount+3>254)return 0;
 PHONGTHAN_IBBUFF_STORE old=*store;if(!AddIBBuffToStore(store,Player[p].m_nIndex,found->buff,found->seconds,0))return 0;
 if(!Player[p].m_ItemList.RemoveItem(item,1)){*store=old;return 0;}
 PersistPlayerIBBuffStore(p);Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}
void PhongThanStallPersist(int p){if(PtStall::Actor(p))PersistPlayerIBBuffStore(p);}
void PhongThanStallTick(int p){
 if(!PtStall::Actor(p)||!Player[p].m_PTrade.nTrade)return;
 if(!PhongThanStallAllowed(p)){Player[p].SendTradeCancel();return;}
 KNpc& actor=Npc[Player[p].m_nIndex];if(actor.m_Doing!=do_sit)actor.SendCommand(do_sit);
}

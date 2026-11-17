#include "PhongThanPkPunishment.h"
#include "../../../Headers/PhongThanPkPunishment.h"
namespace PtPkRuntime {
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Player[p].m_nNetConnectIdx>=0&&!Player[p].m_pStatusLoadPlayerInfo;}
static int Get(int p,int id){return Player[p].m_cTask.GetSaveVal(id);}
static void Set(int p,int id,int value){if(Player[p].m_cTask.GetSaveVal(id)!=value)Player[p].m_cTask.SetSaveVal(id,value,FALSE);}
struct Clock{DWORD actor,last;int socket;Clock():actor(0),last(0),socket(-1){}};
static Clock clocks[MAX_PLAYER];
static int Map(int p){int w=Npc[Player[p].m_nIndex].m_SubWorldIndex;return w>=0&&w<MAX_SUBWORLD?SubWorld[w].m_SubWorldID:0;}
}
int PhongThanPkRaw(int p){int points=Player[p].m_cPK.GetPKValue(),raw=PtPkRuntime::Get(p,PtPkPunishment::Minutes);return PtPkPunishment::Points(raw)==points?raw:PtPkPunishment::Raw(points);}
void PhongThanPkValueChanged(int p,int before,int after){if(p<=0||p>=MAX_PLAYER)return;using namespace PtPkRuntime;
 if(before!=after){Set(p,PtPkPunishment::Minutes,PtPkPunishment::Raw(after));Set(p,PtPkPunishment::Seconds,0);}
 if(after>6)Set(p,PtPkPunishment::Sentence,1);else if(after==0){Set(p,PtPkPunishment::Sentence,0);Set(p,PtPkPunishment::Minutes,0);Set(p,PtPkPunishment::Seconds,0);}
}
void PhongThanPkSetRaw(int p,int raw){if(p<=0||p>=MAX_PLAYER)return;if(raw<0)raw=0;if(raw>6000)raw=6000;Player[p].m_cPK.SetPKValue(PtPkPunishment::Points(raw));PtPkRuntime::Set(p,PtPkPunishment::Minutes,raw);PtPkRuntime::Set(p,PtPkPunishment::Seconds,0);}
bool PhongThanPkDestination(int p,unsigned long* map,int* x,int* y){using namespace PtPkRuntime;
 if(!map||!x||!y||p<=0||p>=MAX_PLAYER)return false;if(!Live(p))return true;int pk=Player[p].m_cPK.GetPKValue();
 if(PtPkPunishment::MustStay(Map(p),pk,Get(p,PtPkPunishment::Sentence))&&*map!=PtPkPunishment::Island)return false;
 if((pk>6||(pk>0&&Get(p,PtPkPunishment::Sentence)))&&*map!=PtPkPunishment::Island){POINT pos;if(!g_SubWorldSet.GetRevivalPosFromId(PtPkPunishment::Island,0,&pos))return false;*map=PtPkPunishment::Island;*x=pos.x;*y=pos.y;Set(p,PtPkPunishment::Sentence,1);}
 return true;
}
void PhongThanPkRevivalPos(int p,int* map,int* x,int* y){using namespace PtPkRuntime;if(!Live(p)||!map||!x||!y)return;int pk=Player[p].m_cPK.GetPKValue();if(pk<=0||(pk<=6&&!Get(p,PtPkPunishment::Sentence)))return;POINT pos;if(g_SubWorldSet.GetRevivalPosFromId(PtPkPunishment::Island,0,&pos)){*map=PtPkPunishment::Island;*x=pos.x;*y=pos.y;}}
void PhongThanPkPunishmentTick(int p){using namespace PtPkRuntime;if(!Live(p))return;
 KNpc& n=Npc[Player[p].m_nIndex];Clock& clock=clocks[p];DWORD now=GetTickCount();
 int pk=Player[p].m_cPK.GetPKValue();if(pk>6)Set(p,PtPkPunishment::Sentence,1);
 // Enforce confinement on the first ready frame, before throttling the timer.
 if(pk>0&&Get(p,PtPkPunishment::Sentence)&&Map(p)!=PtPkPunishment::Island){POINT pos;if(g_SubWorldSet.GetRevivalPosFromId(PtPkPunishment::Island,0,&pos))n.ChangeWorld(PtPkPunishment::Island,pos.x,pos.y);clock.actor=n.m_dwID;clock.socket=Player[p].m_nNetConnectIdx;clock.last=now;return;}
 if(clock.actor!=n.m_dwID||clock.socket!=Player[p].m_nNetConnectIdx){clock.actor=n.m_dwID;clock.socket=Player[p].m_nNetConnectIdx;clock.last=now;return;}
 DWORD elapsed=now-clock.last;if(elapsed<1000)return;int credit=elapsed>2000?1:elapsed/1000;clock.last=elapsed>2000?now:clock.last+credit*1000;
 if(pk<=0){PhongThanPkValueChanged(p,0,0);return;}
 if(Map(p)!=PtPkPunishment::Island){if(Get(p,PtPkPunishment::Sentence)){POINT pos;if(g_SubWorldSet.GetRevivalPosFromId(PtPkPunishment::Island,0,&pos))n.ChangeWorld(PtPkPunishment::Island,pos.x,pos.y);}return;}
 int seconds=Get(p,PtPkPunishment::Seconds);if(seconds<0||seconds>59)seconds=0;seconds+=credit;
 int raw=PhongThanPkRaw(p);if(seconds>=60){raw-=seconds/60;seconds%=60;PhongThanPkSetRaw(p,raw);}Set(p,PtPkPunishment::Seconds,seconds);
}
int PhongThanUsePkWater(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=8||Item[item].GetDetailType()!=33||Item[item].GetParticular()!=2)return -1;
 if(!PtPkRuntime::Live(p)||Player[p].GetLockState()||Player[p].CheckTrading()||Npc[Player[p].m_nIndex].m_CurrentLife<=0||Item[item].GetLock()->nState!=LOCK_STATE_NORMAL)return 0;
 if(Item[item].GetExpireTime()&&Item[item].GetExpireTime()<=KSG_GetCurSec())return 0;
 KItemList& bag=Player[p].m_ItemList;int at=bag.FindSame(item);if(at<=0)return 0;int place=bag.m_Items[at].nPlace;
 if(place!=pos_equiproom&&place!=pos_equiproomex&&place!=pos_ptbag2&&place!=pos_ptbag3&&place!=pos_immediacy)return 0;
 int pk=Player[p].m_cPK.GetPKValue();if(pk<=0)return 0;
 if(!bag.RemoveItem(item,1))return 0;PhongThanPkSetRaw(p,PtPkPunishment::Raw(pk>2?pk-2:0));return 1;
}
void PhongThanPkDeathItems(int p,int mode){using namespace PtPkRuntime;if(!Live(p))return;KNpc& n=Npc[Player[p].m_nIndex];
 if(!PtPkPunishment::DeathDrops(n.m_CurrentCamp,mode,enumDEATH_MODE_NPC_KILL,enumDEATH_MODE_PLAYER_SPAR_NO_PUNISH,enumDEATH_MODE_PLAYER_NO_PUNISH,enumDEATH_MODE_PKBATTLE_PUNISH,enumDEATH_MODE_TOURNAMENTS_PUNISH))return;
 int pk=Player[p].m_cPK.GetPKValue();if(pk<0)pk=0;if(pk>MAX_DEATH_PUNISH_PK_VALUE)pk=MAX_DEATH_PUNISH_PK_VALUE;
 KItemList& bag=Player[p].m_ItemList;if(Player[p].CheckTrading())return;std::vector<PlayerItem> candidates;
 for(PlayerItem* entry=bag.GetFirstItem();entry;entry=bag.GetNextItem()){if(entry->nIdx<=0||entry->nIdx>=MAX_ITEM)continue;int place=entry->nPlace;
 if(place==pos_equip||place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3||place==pos_immediacy)candidates.push_back(*entry);}
 int equipmentDrops=0;
 for(unsigned i=0;i<candidates.size();++i){PlayerItem old=candidates[i];int item=old.nIdx;bool equipped=old.nPlace==pos_equip;
 if(Item[item].GetLockDrop()||Item[item].GetLock()->IsLock()||(equipped&&(old.nX==itempart_horse||equipmentDrops>=1||Map(p)==PtPkPunishment::Island)))continue;
 int rate=equipped?PlayerSet.m_sPKPunishParam[pk].m_nEquip:PlayerSet.m_sPKPunishParam[pk].m_nItem;if(rate<=0||g_Random(1000)>=rate)continue;
 int at=bag.FindSame(item);if(at<=0||bag.m_Items[at].nPlace!=old.nPlace)continue;
 KMapPos position;Player[p].GetAboutPos(&position);if(position.nRegion<0)continue;
 KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nItemID=item;info.m_nItemWidth=Item[item].GetWidth();info.m_nItemHeight=Item[item].GetHeight();info.m_nColorID=Item[item].GetColorItem();info.m_nGenre=Item[item].GetGenre();info.m_nDetailType=Item[item].GetDetailType();info.m_nMovieFlag=info.m_nSoundFlag=1;g_StrCpyLen(info.m_szName,Item[item].GetName(),sizeof(info.m_szName));
 if(!bag.Remove(item))continue;int object=ObjSet.Add(Item[item].GetObjIdx(),position,info);
 if(object<=0){if(!bag.Add(item,old.nPlace,old.nX,old.nY,false))g_DebugLog("[PK_DROP_ROLLBACK_FAILED] player=%d item=%d",p,item);continue;}
 Object[object].SetItemBelong(-1);if(equipped)++equipmentDrops;
 }
}

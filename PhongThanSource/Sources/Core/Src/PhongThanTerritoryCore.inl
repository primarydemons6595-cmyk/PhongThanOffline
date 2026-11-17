// Included by the native nation service: one store, one durable revision.
#include "PhongThanTerritory.h"
#include "../../../Headers/PhongThanTerritoryPaths.h"
#include "KWeatherMgr.h"
#include "KObj.h"
#include "KObjSet.h"
#include <limits.h>
namespace PTTerritory {
struct Policy {int enabled,seconds,decay,construction,cap,fee,credit,popularity,initial,perBuilding,admin;} policy;
static DWORD policyAt=0,tickAt=0,snapshotAt=0,saveAt=0;
static bool tablesReady=false,dirty=false;
static KTabFile create,upgrade,building,limits,positions,entry;
static unsigned int worldRevision[MAX_SUBWORLD];
static int visualIndex[MAX_SUBWORLD][PT_TERRITORY_BUILDINGS];
static DWORD visualId[MAX_SUBWORLD][PT_TERRITORY_BUILDINGS];
static int Integer(const char* key,int fallback,int low,int high){int n=GetPrivateProfileIntA("Main",key,fallback,".\\settings\\phongthan\\TerritoryPolicy.ini");return n>=low&&n<=high?n:fallback;}
static void Poll(){DWORD now=GetTickCount();if(policyAt&&now-policyAt<2000)return;policyAt=now;
 policy.enabled=Integer("Enabled",1,0,1);policy.seconds=Integer("OnlineSecondsPerPopularity",60,1,86400);policy.decay=Integer("PopularityDailyDecay",0,0,100000);
 policy.construction=Integer("ConstructionSeconds",60,1,604800);policy.cap=Integer("DailyContributionCap",20,0,100000);policy.fee=Integer("BasicCityFee",500000,0,1000000000);
 policy.credit=Integer("FounderCredit",5000,0,100000000);policy.popularity=Integer("RequiredPopularity",800,0,100000000);policy.initial=Integer("InitialProsperity",1000,0,100000000);
 policy.perBuilding=Integer("ProsperityPerCompletedBuilding",100,0,1000000);policy.admin=Integer("AllowStudioAdministration",1,0,1);
}
static bool Tables(){if(!tablesReady)tablesReady=create.Load((char*)PT_TERRITORY_CREATE)&&upgrade.Load((char*)PT_TERRITORY_UPGRADE)&&building.Load((char*)PT_TERRITORY_BUILDING)&&limits.Load((char*)PT_TERRITORY_LIMITS)&&positions.Load((char*)PT_TERRITORY_POSITIONS)&&entry.Load((char*)PT_TERRITORY_ENTRY);return tablesReady;}
static int Cell(KTabFile& t,int row,int column){int n=0;t.GetInteger(row,column,0,&n);return n;}
static int Row(KTabFile& t,int value){char key[32];sprintf(key,"%d",value);return t.FindRow(key);}
static PTNationDevelopment& Development(int nation){PTNationDevelopment& d=s_PtNation.State.Development[nation];if(d.NationId!=s_PtNation.State.Nations[nation].Id){memset(&d,0,sizeof(d));d.NationId=s_PtNation.State.Nations[nation].Id;}return d;}
static PTTerritoryContribution& Contribution(int member){PTTerritoryContribution& c=s_PtNation.State.Contributions[member];const PTNationMember& m=s_PtNation.State.Members[member];if(c.NationId!=m.NationId||strcmp(c.Name,m.Name)){memset(&c,0,sizeof(c));c.NationId=m.NationId;lstrcpynA(c.Name,m.Name,sizeof(c.Name));}return c;}
static int NationForPlayer(int p){int m=PTNationSelf(p);return m>=0&&s_PtNation.State.Members[m].Duty>0?s_PtNation.Nation(s_PtNation.State.Members[m].NationId):-1;}
static int NationForMap(int map){for(int n=0;n<PT_NATION_CAPACITY;++n)if(s_PtNation.State.Nations[n].Id&&s_PtNation.State.Development[n].CityLevel&&s_PtNation.State.Development[n].CityMapId==map)return n;return -1;}
static bool Save(){++s_PtNation.State.Revision;if(!s_PtNation.Save(s_PtNationFile)){--s_PtNation.State.Revision;return false;}dirty=false;return true;}
static bool Occupied(int world){for(int p=1;p<MAX_PLAYER;++p)if(Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].m_SubWorldIndex==world)return true;return false;}
static bool Preserve(int world){KSubWorld& map=SubWorld[world];if(Occupied(world)||map.m_MissionArray.GetUsedCount()>0||map.m_NoneRegionNpcList.GetNodeCount()>0)return true;
 for(int region=0;region<map.m_nTotalRegion;++region)for(KIndexNode* node=(KIndexNode*)map.m_Region[region].m_ObjList.GetHead();node;node=(KIndexNode*)node->GetNext()){
  int id=node->m_nIndex;if(Object[id].m_nKind==Obj_Kind_Item||Object[id].m_nKind==Obj_Kind_Money)return true;
 }return false;
}
static void ReleaseWorld(int world){KSubWorld& map=SubWorld[world];
 for(int n=1;n<MAX_NPC;++n)if(Npc[n].m_dwID&&!Npc[n].IsPlayer()&&Npc[n].m_SubWorldIndex==world)NpcSet.Remove(n);
 map.Close();delete[]map.m_Region;map.m_Region=0;map.m_nTotalRegion=0;
 if(map.m_pWeatherMgr){delete map.m_pWeatherMgr;map.m_pWeatherMgr=0;}
 memset(visualIndex[world],0,sizeof(visualIndex[world]));memset(visualId[world],0,sizeof(visualId[world]));worldRevision[world]=0;
}
static int Template(int type){char resource[32];sprintf(resource,"building%03d",type+1);
 for(int row=2;row<=g_NpcSetting.GetHeight();++row){char value[64];g_NpcSetting.GetString(row,"NpcResType","",value,sizeof(value));if(!strcmp(value,resource))return row-2;}return -1;
}
static bool Position(int world,int x,int y){KSubWorld& map=SubWorld[world];int region=-1,cx=0,cy=0,dx=0,dy=0;map.Mps2Map(x,y,&region,&cx,&cy,&dx,&dy);return region>=0&&region<map.m_nTotalRegion&&map.TestBarrier(x,y)==0;}
static void Visuals(int world,int nation){KSubWorld& map=SubWorld[world];PTNationDevelopment& d=Development(nation);map.m_bCheckTong=TRUE;map.m_dwTongName=d.NationId;lstrcpynA(map.m_szTongName,s_PtNation.State.Nations[nation].Name,sizeof(map.m_szTongName));
 unsigned int signature=PTNationStore::Hash(d.Buildings,d.BuildingCount*sizeof(PTTerritoryBuilding))^d.NationId;
 if(worldRevision[world]==signature)return;
 for(int b=0;b<PT_TERRITORY_BUILDINGS;++b){int old=visualIndex[world][b];if(old>0&&old<MAX_NPC&&Npc[old].m_dwID==visualId[world][b])NpcSet.Remove(old);visualIndex[world][b]=0;visualId[world][b]=0;}
 for(b=0;b<d.BuildingCount;++b){const PTTerritoryBuilding& item=d.Buildings[b];int type=Template(item.Type);if(type<0)continue;
  int npc=NpcSet.Add(MAKELONG(1,type),world,item.X,item.Y,FALSE);if(npc<=0||npc>=MAX_NPC)continue;
  Npc[npc].m_Kind=kind_dialoger;Npc[npc].m_CurrentCamp=camp_begin;Npc[npc].m_Level=item.CompleteAt?1:2;
  strcpy(Npc[npc].ActionScript,"\\script\\phongthan\\territory\\building.lua");Npc[npc].m_ActionScriptID=g_FileName2Id(Npc[npc].ActionScript);
  visualIndex[world][b]=npc;visualId[world][b]=Npc[npc].m_dwID;
 }worldRevision[world]=signature;
}
static bool PayResources(int nation,KTabFile& table,int row,int first){PTNationRecord& country=s_PtNation.State.Nations[nation];PTNationDevelopment& d=Development(nation);int money=Cell(table,row,first);if(money<0||country.Money<money)return false;
 for(int i=0;i<10;++i){int amount=Cell(table,row,first+1+i);if(amount<0||d.Resources[i]<amount)return false;}
 country.Money-=money;for(i=0;i<10;++i)d.Resources[i]-=Cell(table,row,first+1+i);return true;
}
static bool Create(int nation,int land,int totem,const char* name,bool admin,int reserved=-1){if(!Tables()||nation<0||land<0||land>4||totem<0||totem>2||!name||!name[0]||strlen(name)>31)return false;
 PTNationDevelopment& d=Development(nation);if(d.CityLevel)return false;bool used[25];memset(used,0,sizeof(used));
 for(int n=0;n<PT_NATION_CAPACITY;++n){const PTNationDevelopment& x=s_PtNation.State.Development[n];if(x.CityLevel&&x.CityMapId>=2100&&x.CityMapId<2225)used[(x.CityMapId-2100)/5]=true;}
 for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m){const PTTerritoryContribution& c=s_PtNation.State.Contributions[m];if(c.PendingAfter&&c.PendingResource==2&&c.PendingCitySlotPlusOne)used[c.PendingCitySlotPlusOne-1]=true;}
 int slot=reserved;if(slot<0){slot=0;while(slot<25&&used[slot])++slot;}if(slot>=25)return false;
 if(!admin){int money=Cell(create,land+2,2),bronze=Cell(create,land+2,6);if(d.Popularity<policy.popularity||money<0||bronze<0||s_PtNation.State.Nations[nation].Money<money||d.Resources[0]<bronze)return false;s_PtNation.State.Nations[nation].Money-=money;d.Resources[0]-=bronze;}
 d.CityMapId=2100+slot*5+land;d.CityLevel=1;d.Land=land;d.Totem=totem;d.Prosperity=policy.initial;d.ProsperityAt=(unsigned int)time(0);lstrcpynA(d.CityName,name,sizeof(d.CityName));d.BuildingCount=3;
 int kinds[]={totem==0?2:totem==1?17:18,6,0};for(int i=0;i<3;++i){PTTerritoryBuilding& b=d.Buildings[i];b.Type=kinds[i];b.X=Cell(positions,land+2,3+i*3)*32;b.Y=Cell(positions,land+2,4+i*3)*32;b.CompleteAt=0;}return true;
}
static bool Upgrade(int nation){PTNationDevelopment& d=Development(nation);if(!Tables()||d.CityLevel<1||d.CityLevel>=4)return false;int row=Row(upgrade,d.CityLevel+1);if(row<2||d.Prosperity<Cell(upgrade,row,2)||d.SustainedChecks<Cell(upgrade,row,3)||!PayResources(nation,upgrade,row,4))return false;++d.CityLevel;d.SustainedChecks=0;return true;}
static bool AddBuilding(int nation,int type,int x,int y){PTNationDevelopment& d=Development(nation);if(!Tables()||!d.CityLevel||type<0||type>=34||d.BuildingCount>=128)return false;
 if((type==2&&d.Totem!=0)||(type==17&&d.Totem!=1)||(type==18&&d.Totem!=2))return false;
 int count=0;for(int b=0;b<d.BuildingCount;++b){const PTTerritoryBuilding& old=d.Buildings[b];if(old.Type==type)++count;if(abs(old.X-x)<96&&abs(old.Y-y)<96)return false;}
 int row=Row(limits,d.CityLevel);if(row<2||count>=Cell(limits,row,type+2))return false;
 int world=PhongThanTerritoryWorld(d.CityMapId);if(world<0||!Position(world,x,y))return false;
 row=Row(building,type);if(row<2||!PayResources(nation,building,row,2))return false;
 PTTerritoryBuilding& added=d.Buildings[d.BuildingCount++];added.Type=type;added.X=x;added.Y=y;added.CompleteAt=(unsigned int)time(0)+policy.construction;return true;
}
static bool Near(int p,int type){int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD)return false;int x,y;Npc[actor].GetMpsPos(&x,&y);
 int npc=Player[p].m_nLastNpcIndex;if(npc<=0||npc>=MAX_NPC||!Npc[npc].m_dwID||Npc[npc].m_SubWorldIndex!=world)return false;int nx,ny;Npc[npc].GetMpsPos(&nx,&ny);if(abs(nx-x)>512||abs(ny-y)>512)return false;
 if(type==0)return SubWorld[world].m_SubWorldID==1001&&(Npc[npc].m_ActionScriptID==g_FileName2Id((char*)PT_TERRITORY_FOUNDER)||Npc[npc].m_ActionScriptID==g_FileName2Id("\\script\\phongthan\\territory\\founder.lua"));
 if(type==2)return SubWorld[world].m_SubWorldID==1001&&(Npc[npc].m_ActionScriptID==g_FileName2Id((char*)PT_TERRITORY_GATE)||Npc[npc].m_ActionScriptID==g_FileName2Id("\\script\\phongthan\\territory\\gate.lua"));
 return (SubWorld[world].m_SubWorldID==1001&&Npc[npc].m_ActionScriptID==g_FileName2Id("\\script\\phongthan\\territory\\warehouse.lua"))||(NationForMap(SubWorld[world].m_SubWorldID)>=0&&Npc[npc].m_NpcSettingIdx==375);
}
static void AddPoints(PTTerritoryContribution& c,int points,int day){if(c.Day!=day){c.Day=day;c.Today=0;}int week=day/7;if(c.Week!=week){c.LastWeek=c.Week+1==week?c.ThisWeek:0;c.ThisWeek=0;c.Week=week;}if(points>INT_MAX-c.Total||points>INT_MAX-c.Usable||points>INT_MAX-c.ThisWeek)return;c.Total+=points;c.Usable+=points;c.ThisWeek+=points;c.Today+=points;}
}
int PhongThanTerritoryCount(){if(!PTNationReady())return 0;int count=0;for(int n=0;n<PT_NATION_CAPACITY;++n)if(s_PtNation.State.Development[n].CityLevel)++count;return count;}
int PhongThanTerritoryPlayerValue(int p,int field){if(!PTNationLive(p)||!PTNationReady())return 0;int nation=PTTerritory::NationForPlayer(p),member=PTNationSelf(p);if(nation<0||member<0)return 0;PTNationDevelopment& d=PTTerritory::Development(nation);PTTerritoryContribution& c=PTTerritory::Contribution(member);
 switch(field){case 0:return d.Popularity;case 1:return d.CityLevel;case 2:return c.Total;case 3:return c.Usable;case 4:return c.ThisWeek;case 5:return c.LastWeek;case 6:return d.CityMapId;case 7:return d.Prosperity;default:return field>=10&&field<20?d.Resources[field-10]:0;}}
int PhongThanTerritoryBeginDonation(int p,int type,int tier,int before,int& after){PTTerritory::Poll();if(!PTNationLive(p)||!PTNationReady()||!PTTerritory::policy.enabled||Player[p].CheckTrading()||Player[p].GetLockState()||!PTTerritory::Near(p,1)||type<0||type>1||tier<0||tier>2||before<0||before>=1000000000)return 0;
 int member=PTNationSelf(p);if(member<0||s_PtNation.State.Members[member].Duty<1||s_PtNation.State.Members[member].Duty>4)return 0;int nation=PTTerritory::NationForPlayer(p);if(nation<0)return 0;PTTerritoryContribution& c=PTTerritory::Contribution(member);if(c.PendingAfter)return 0;
 int amount=type==0?(tier==0?50000:tier==1?500000:5000000):(tier==1?250:tier==2?2500:0);if(!amount)return 0;int day=(int)(time(0)/86400),points=tier==0?0:tier==1?1:10;
 int today=c.Day==day?c.Today:0,room=PTTerritory::policy.cap-today;if(room<0)room=0;if(points>room)points=room;
 PTNationDevelopment& d=PTTerritory::Development(nation);if((type==0&&s_PtNation.State.Nations[nation].Money>INT_MAX-amount)||(type==1&&d.Resources[0]>INT_MAX-amount)||c.Total>INT_MAX-points||c.Usable>INT_MAX-points||(c.Week==day/7&&c.ThisWeek>INT_MAX-points))return 0;
 PTTerritoryContribution old=c;c.PendingBefore=before;c.PendingAfter=before+1;c.PendingResource=type;c.PendingAmount=amount;c.PendingPoints=points;c.PendingDay=day;
 if(!PTTerritory::Save()){c=old;return 0;}after=c.PendingAfter;return amount;
}

#include "PhongThanTerritoryBooks.inl"
int PhongThanTerritoryBeginCreate(int p,int land,int totem,int before,int& after){PTTerritory::Poll();
 if(!PTNationLive(p)||!PTNationReady()||!PTTerritory::policy.enabled||!PTTerritory::Tables()||Player[p].CheckTrading()||Player[p].GetLockState()||!PTTerritory::Near(p,0)||land<0||land>4||totem<0||totem>2||before<0||before>=1000000000)return -1;
 int m=PTNationSelf(p),n=PTTerritory::NationForPlayer(p);if(n<0||m<0||!PTNationRight(m,14)||Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)<PTTerritory::policy.credit)return -1;
 PTNationDevelopment& d=PTTerritory::Development(n);PTTerritoryContribution& c=PTTerritory::Contribution(m);if(d.CityLevel||d.Popularity<PTTerritory::policy.popularity)return -1;
 bool used[25];memset(used,0,sizeof(used));for(int i=0;i<PT_NATION_CAPACITY;++i){const PTNationDevelopment& item=s_PtNation.State.Development[i];if(item.CityLevel)used[(item.CityMapId-2100)/5]=true;}
 for(i=0;i<PT_NATION_MEMBER_CAPACITY;++i){const PTTerritoryContribution& x=s_PtNation.State.Contributions[i];if(x.NationId==d.NationId&&x.PendingAfter)return -1;if(x.PendingAfter&&x.PendingResource==2)used[x.PendingCitySlotPlusOne-1]=true;}
 int slot=0;while(slot<25&&used[slot])++slot;if(slot==25)return -1;
 int cost=PTTerritory::Cell(PTTerritory::create,land+2,2),bronze=PTTerritory::Cell(PTTerritory::create,land+2,6);if(cost<0||bronze<0||s_PtNation.State.Nations[n].Money<cost||d.Resources[0]<bronze)return -1;
 PTTerritoryContribution old=c;c.PendingBefore=before;c.PendingAfter=before+1;c.PendingResource=2;c.PendingAmount=PTTerritory::policy.fee;c.PendingDay=(int)(time(0)/86400);c.PendingCitySlotPlusOne=slot+1;c.PendingLand=land;c.PendingTotem=totem;c.PendingMoney=cost;c.PendingBronze=bronze;
 s_PtNation.State.Nations[n].Money-=cost;d.Resources[0]-=bronze;
 if(!PTTerritory::Save()){s_PtNation.State.Nations[n].Money+=cost;d.Resources[0]+=bronze;c=old;return -1;}after=c.PendingAfter;return c.PendingAmount;
}
static void PTTerritoryClearPending(PTTerritoryContribution& c){memset(&c.PendingBefore,0,sizeof(c)-offsetof(PTTerritoryContribution,PendingBefore));}
void PhongThanTerritoryCancelDonation(int p,int after){if(!PTNationLive(p)||!PTNationReady())return;int m=PTNationSelf(p),n=PTTerritory::NationForPlayer(p);if(m<0||n<0)return;PTTerritoryContribution& c=PTTerritory::Contribution(m);if(c.PendingAfter!=after)return;
 PTNationState* old=new PTNationState;memcpy(old,&s_PtNation.State,sizeof(*old));
 if(c.PendingResource==2){s_PtNation.State.Nations[n].Money+=c.PendingMoney;PTTerritory::Development(n).Resources[0]+=c.PendingBronze;}else if(c.PendingResource>=5)PTTerritory::Books(n).Fragments[c.PendingResource-5]+=c.PendingAmount;PTTerritoryClearPending(c);if(!PTTerritory::Save())memcpy(&s_PtNation.State,old,sizeof(*old));delete old;
}
struct PTTerritoryRoleSave {BYTE Ticket[PHONGTHAN_SESSION_TICKET_SIZE];int After;bool Queued;};
static PTTerritoryRoleSave s_TerritoryRoleSave[MAX_PLAYER];
static bool PTTerritoryRoleCommitted(int p,int after){
 // Server bots persist synchronously to their own atomic character file;
 // they have no Goddess socket/ack. Require that real durable write before
 // crediting a contribution. Human characters keep the existing ack gate.
 if(Player[p].m_bIsBot)return Player[p].Save()!=FALSE;
 PTTerritoryRoleSave& save=s_TerritoryRoleSave[p];
 if(save.After!=after||memcmp(save.Ticket,Player[p].m_SessionTicket,sizeof(save.Ticket))){memset(&save,0,sizeof(save));save.After=after;memcpy(save.Ticket,Player[p].m_SessionTicket,sizeof(save.Ticket));}
 if(save.Queued)return Player[p].m_uMustSave==SAVE_IDLE;
 // Save() builds a RAM snapshot. Only Goddess' durable-save acknowledgement
 // changes SAVE_DOING to SAVE_IDLE. Never credit items from Save() alone.
 if(Player[p].m_uMustSave==SAVE_DOING||!Player[p].Save())return false;
 Player[p].m_uMustSave=SAVE_REQUEST;save.Queued=true;return false;
}
int PhongThanTerritoryFinishDonation(int p,bool loaded){if(!PTNationLive(p)||Player[p].m_pStatusLoadPlayerInfo||!PTNationReady())return 0;int m=PTNationSelf(p),n=PTTerritory::NationForPlayer(p);if(m<0||n<0)return 0;PTTerritoryContribution& c=PTTerritory::Contribution(m);if(!c.PendingAfter)return 1;
 int value=Player[p].m_cTask.GetSaveVal(PT_TERRITORY_RECEIPT_TASK);if(value==c.PendingBefore){if(loaded)PhongThanTerritoryCancelDonation(p,c.PendingAfter);return 0;}if(value!=c.PendingAfter||(!loaded&&!PTTerritoryRoleCommitted(p,c.PendingAfter)))return 0;
 if((c.PendingResource==0&&s_PtNation.State.Nations[n].Money>INT_MAX-c.PendingAmount)||(c.PendingResource==1&&PTTerritory::Development(n).Resources[0]>INT_MAX-c.PendingAmount))return 0;
 PTNationState* before=new PTNationState;memcpy(before,&s_PtNation.State,sizeof(*before));bool ok=true;
 if(c.PendingResource==2)ok=PTTerritory::Create(n,c.PendingLand,c.PendingTotem,s_PtNation.State.Nations[n].Name,true,c.PendingCitySlotPlusOne-1);
 else if(c.PendingResource<3){if(c.PendingResource==0)s_PtNation.State.Nations[n].Money+=c.PendingAmount;else PTTerritory::Development(n).Resources[0]+=c.PendingAmount;PTTerritory::AddPoints(c,c.PendingPoints,c.PendingDay);}
 else if(c.PendingResource<5){PTNationBook& b=PTTerritory::Books(n);if(b.Fragments[c.PendingResource-3]>INT_MAX-c.PendingAmount)ok=false;else {b.Fragments[c.PendingResource-3]+=c.PendingAmount;PTTerritory::AddPoints(c,c.PendingPoints,c.PendingDay);}}
 if(ok){PTTerritoryClearPending(c);ok=PTTerritory::Save();}if(!ok)memcpy(&s_PtNation.State,before,sizeof(*before));delete before;return ok?1:0;
}

int PhongThanTerritoryWorld(int map){if(!PTNationReady())return -1;int nation=PTTerritory::NationForMap(map);if(nation<0)return -1;
 for(int w=0;w<MAX_SUBWORLD;++w)if(SubWorld[w].m_SubWorldID==map){if(!SubWorld[w].EnsureContent())return -1;PTTerritory::Visuals(w,nation);return w;}
 int slot=-1;for(w=0;w<MAX_SUBWORLD;++w)if(SubWorld[w].m_SubWorldID<0){slot=w;break;}
 if(slot<0)for(w=0;w<MAX_SUBWORLD;++w)if(PTTerritory::NationForMap(SubWorld[w].m_SubWorldID)>=0&&!PTTerritory::Preserve(w)){PTTerritory::ReleaseWorld(w);slot=w;break;}
 if(slot<0)return -1;SubWorld[slot].m_nIndex=slot;if(!SubWorld[slot].LoadMap(map)||!SubWorld[slot].EnsureContent()){PTTerritory::ReleaseWorld(slot);return -1;}PTTerritory::Visuals(slot,nation);return slot;
}
bool PhongThanTerritoryCityInfo(int map,char* name,int& owner,int& level,int& land,char* country){if(!PTNationReady())return false;int n=PTTerritory::NationForMap(map);if(n<0)return false;PTNationDevelopment& d=PTTerritory::Development(n);lstrcpynA(name,d.CityName,32);lstrcpynA(country,s_PtNation.State.Nations[n].Name,32);owner=d.NationId;level=d.CityLevel;land=d.Land;return true;}

int PhongThanTerritoryCityAt(int index,char* name,int& level){if(!PTNationReady()||index<0)return 0;for(int n=0;n<PT_NATION_CAPACITY;++n){PTNationDevelopment& d=s_PtNation.State.Development[n];if(!d.CityLevel)continue;if(index--==0){lstrcpynA(name,d.CityName,32);level=d.CityLevel;return d.CityMapId;}}return 0;}
int PhongThanTerritoryAction(int p,int operation,int value,int second,const char* text){PTTerritory::Poll();if(!PTNationLive(p)||!PTNationReady()||!PTTerritory::policy.enabled||!PTTerritory::Tables()||Player[p].CheckTrading()||Player[p].GetLockState())return 0;
 int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD)return 0;
 if(operation==1){if(!PTTerritory::Near(p,2))return 0;int n=PTTerritory::NationForMap(value);if(n<0)return 0;int land=PTTerritory::Development(n).Land;int target=PhongThanTerritoryWorld(value);if(target<0)return 0;int x=PTTerritory::Cell(PTTerritory::entry,land+2,2)*32,y=PTTerritory::Cell(PTTerritory::entry,land+2,3)*32;if(!PTTerritory::Position(target,x,y))return 0;return Npc[actor].ChangeWorld(value,x,y);}
 int n=PTTerritory::NationForMap(SubWorld[world].m_SubWorldID),own=PTTerritory::NationForPlayer(p),m=PTNationSelf(p);if(n<0)return 0;
 if(operation==4)return Npc[actor].ChangeWorld(1001,196*256+128,196*512+256);
 if(n!=own||m<0||!PTNationRight(m,14))return 0;
 for(int i=0;i<PT_NATION_MEMBER_CAPACITY;++i)if(s_PtNation.State.Contributions[i].NationId==s_PtNation.State.Nations[n].Id&&s_PtNation.State.Contributions[i].PendingAfter)return 0;
 PTNationState* old=new PTNationState;memcpy(old,&s_PtNation.State,sizeof(*old));bool ok=false;
 if(operation==2)ok=PTTerritory::Upgrade(n);else if(operation==3){int x,y;Npc[actor].GetMpsPos(&x,&y);ok=PTTerritory::AddBuilding(n,value,x,y);}
 if(ok)ok=PTTerritory::Save();if(!ok)memcpy(&s_PtNation.State,old,sizeof(*old));delete old;return ok?1:0;
}
void PhongThanTerritoryOpen(int p,int operation){if(!PTNationLive(p)||!PTNationReady())return;const char* script=operation==0?"\\script\\phongthan\\territory\\founder.lua":operation==1?"\\script\\phongthan\\territory\\gate.lua":"\\script\\phongthan\\territory\\building.lua";Player[p].ExecuteScript((char*)script,"main",0);}

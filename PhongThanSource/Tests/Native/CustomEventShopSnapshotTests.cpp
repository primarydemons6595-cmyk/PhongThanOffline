#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#define _SERVER
#include "../../Sources/Core/Src/PhongThanCustomEvents.h"
#define MAX_PLAYER 3
#define MAX_NPC 16
#define MAX_SUBWORLD 2
#define kind_dialoger 3
#define camp_free 0
#define obj_npc 1
struct Region{void RemoveNpc(int){}void DecRef(int,int,int){}};
struct World{int m_nTotalRegion;Region m_Region[1];int GetBarrier(int,int){return 0;}} SubWorld[MAX_SUBWORLD];
struct KNpc{int m_SubWorldIndex,m_RegionIndex,m_MapX,m_MapY,m_Kind;DWORD m_dwID,m_ActionScriptID,m_DeathScriptID,m_TimerScriptID,m_DropScriptID;char Name[32],ActionScript[100];bool IsPlayer(){return false;}bool IsAlive(){return true;}void SetFightMode(int){}void SetCurrentCamp(int){}} Npc[MAX_NPC];
struct Actor{bool m_PlayerDBLoad;int m_nIndex,m_nLastNpcIndex;bool CheckTrading(){return false;}bool GetLockState(){return false;}} Player[MAX_PLAYER];
static int fixtureSpawned=0,fixtureRemoved=0;static std::string fixtureSnapshot;
struct Npcs{void Remove(int i){++fixtureRemoved;Npc[i].m_dwID=0;}int Add(int,int,int,int,BOOL){int i=++fixtureSpawned;if(i>=MAX_NPC)return 0;Npc[i].m_dwID=100+i;return i;}int GetDistance(int,int){return 0;}} NpcSet;
struct Worlds{int SearchWorld(int){return 0;}} g_SubWorldSet;
static DWORD g_FileName2Id(char*){return 1;}BOOL PhongThanReloadScriptSafe(const char*){return TRUE;}void PhongThanCustomPotsTick(){}
namespace PtEvents {
static std::vector<std::string> Split(const std::string& s,char separator){std::vector<std::string> v;unsigned int at=0,next;while((next=s.find(separator,at))!=std::string::npos){v.push_back(s.substr(at,next-at));at=next+1;}v.push_back(s.substr(at));return v;}
static bool Number(const std::string& s,long& n,long low,long high){char* end=0;n=strtol(s.c_str(),&end,10);return !s.empty()&&end&&!*end&&n>=low&&n<=high;}
static bool Hex(const std::string& s,char* out,int size){if(s.size()%2||s.size()/2>=(unsigned)size)return false;for(unsigned int i=0;i<s.size();i+=2){char raw[3]={s[i],s[i+1],0};char* end;long n=strtol(raw,&end,16);if(*end||n<=0||n>255)return false;out[i/2]=(char)n;}out[s.size()/2]=0;return true;}
static bool Read(const char*,std::string& out,int){out=fixtureSnapshot;return !out.empty();}static bool Write(const char*,const char*){return true;}
}
#include "../../Sources/Core/Src/PhongThanCustomEvents.inl"
#define CHECK(c) do{if(!(c)){printf("FAIL line %d %s\n",__LINE__,#c);return 1;}++checks;}while(0)
int main(int argc,char** argv){int checks=0;using namespace PtCustomEvents;
 if(argc==2&&strstr(argv[1],".tsv")){FILE* f=fopen(argv[1],"rb");if(!f)return 2;std::string bytes;char chunk[4096];unsigned int got;while((got=fread(chunk,1,sizeof(chunk),f))!=0)bytes.append(chunk,got);fclose(f);CHECK(Load(bytes));int choices=0,goods=0;for(std::map<int,PTCustomEvent>::iterator i=events.begin();i!=events.end();++i){for(unsigned int r=0;r<i->second.recipes.size();++r)choices+=i->second.recipes[r].rewards.size();for(unsigned int g=0;g<i->second.goods.size();++g){PTCustomGoods& row=i->second.goods[g];if(row.barter&&row.payment.genre==4&&row.payment.detail==47)CHECK(row.payment.part==0);++goods;}}printf("PASS live snapshot production parser revision=%ld events=%u rewards=%d goods=%d\n",revision,events.size(),choices,goods);return 0;}
 const char* coin="PT_CUSTOM_EVENTS\t1\t1\nE\t1\t0\t1\t54657374\nG\t1\t1\t0\t3,99,0,0,0\t1\nB\t1\t1\t4,47,0,0,0\t25\n";
 CHECK(Load(coin));CHECK(events[1].goods[0].barter&&events[1].goods[0].payment.genre==4&&events[1].goods[0].payment.detail==47&&events[1].goods[0].payment.count==25);
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t2\nE\t1\t0\t1\t54657374\nB\t1\t1\t4,47,0,0,0\t25\n"));CHECK(revision==1&&events[1].goods[0].payment.count==25);
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t2\nE\t1\t0\t1\t54657374\nG\t1\t1\t10\t3,99,0,0,0\t1\nB\t1\t1\t4,47,0,0,0\t25\n"));
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t2\nE\t1\t0\t1\t54657374\nG\t1\t1\t0\t3,99,0,0,0\t1\nB\t1\t1\t3,42,0,0,0\t0\n"));
 CHECK(Load("PT_CUSTOM_EVENTS\t1\t2\nE\t1\t0\t1\t54657374\nG\t1\t1\t0\t3,99,0,0,0\t1\nB\t1\t1\t3,42,0,0,0\t3\n"));CHECK(events[1].goods[0].payment.genre==3&&events[1].goods[0].payment.detail==42&&events[1].goods[0].payment.count==3);
 CHECK(Load("PT_CUSTOM_EVENTS\t1\t3\nE\t1\t0\t1\t54657374\nG\t1\t1\t10000\t3,99,0,0,0\t1\t546574206974656d\n"));CHECK(events[1].goods[0].name=="Tet item");
 CHECK(events[1].shopTheme==1&&!events[1].hasShopTheme);
 CHECK(Load("PT_CUSTOM_EVENTS\t1\t4\nE\t1\t0\t1\t54657374\nS\t1\t1\t4\n"));CHECK(events[1].shopTheme==4&&events[1].hasShopTheme);
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t5\nE\t1\t0\t1\t54657374\nS\t1\t1\t5\n"));CHECK(revision==4&&events[1].shopTheme==4);
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t5\nE\t1\t0\t1\t54657374\nS\t1\t1\t0\nS\t1\t1\t1\n"));
 CHECK(events[1].toolIdleSeconds==300&&!events[1].hasToolIdle);
 CHECK(Load("PT_CUSTOM_EVENTS\t1\t6\nE\t1\t0\t1\t54657374\nD\t1\t1\t60\n"));CHECK(events[1].toolIdleSeconds==60&&events[1].hasToolIdle);
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t7\nE\t1\t0\t1\t54657374\nD\t1\t1\t0\n"));CHECK(revision==6&&events[1].toolIdleSeconds==60);
 CHECK(!Load("PT_CUSTOM_EVENTS\t1\t7\nE\t1\t0\t1\t54657374\nD\t1\t1\t300\nD\t1\t1\t60\n"));
 printf("PASS CUSTOM_EVENT_SHOP_SNAPSHOT checks=%d production_parser=1\n",checks);return 0;
}

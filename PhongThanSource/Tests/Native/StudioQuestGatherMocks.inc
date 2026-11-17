#include "KStrBase.h"
enum {MAX_OBJECT=16,GAME_FPS=18,OBJ_PROP_STATE_DISPLAY=0,OBJ_PROP_STATE_HIDE=1};
struct KMapPos {int nSubWorld,nRegion,nMapX,nMapY,nOffX,nOffY;};
struct KObjItemInfo {int m_nMovieFlag;};
struct QuestObjectMock {
 int m_nID,m_nDataID,m_nSubWorldID,m_nState,m_nLifeTime,x,y;DWORD m_dwScriptID;char m_szName[80];
 void SetScriptFile(char* path){m_dwScriptID=g_FileName2Id(path);}
 void GetMpsPos(int* a,int* b){*a=x;*b=y;}
 void SetState(int state){m_nState=state;}
} Object[MAX_OBJECT];
struct QuestObjectSetMock {
 int serial;
 int Add(int type,KMapPos pos,KObjItemInfo){for(int i=1;i<MAX_OBJECT;++i)if(!Object[i].m_nID){Object[i].m_nID=++serial;Object[i].m_nDataID=type;Object[i].m_nSubWorldID=pos.nSubWorld;Object[i].x=pos.nMapX;Object[i].y=pos.nMapY;return i;}return 0;}
 void Remove(int i){ZeroMemory(&Object[i],sizeof(Object[i]));}
} ObjSet;
int questBossTestMap=1040;
int WorldSet::SearchWorld(int map){return map==SubWorld[0].m_SubWorldID?0:map==questBossTestMap?1:-1;}
int GetObjIndex(Lua_State*){return 1;}
static int questRoll=0;
static int QuestTestRandom(int range){return questRoll%range;}
#define g_Random QuestTestRandom
#include "StudioQuestSealMocks.inc"

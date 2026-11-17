// Execute the production drop .inl with isolated network/item/world adapters.
// No server/game process or runtime file is touched.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum { item_equip=0,item_medicine=1,item_materials=3,item_task=4,item_townportal=5,item_magicscript=6,item_skillbook=7,item_ibitem=8,equip_detailnum=13 };
enum { kind_normal=0,MAX_PLAYER=8,MAX_NPC=16,MAX_SUBWORLD=2,MAX_ITEM_MAGICLEVEL=6 };
static const char* policy;
static int rolls[10], rollAt, rollCount, allocated, removed, sent, failGeneration, wrongIdentity, wrongObject, mappedRegion=1;
class KIniFile
{
public:
    BOOL Load(const char*) { return GetFileAttributesA(policy)!=(DWORD)-1; }
    BOOL GetString(const char* section,const char* key,const char* fallback,char* output,int size)
    { char marker[2]; BOOL found=GetPrivateProfileStringA(section,key,"~",marker,2,policy)!=0 && marker[0]!='~'; GetPrivateProfileStringA(section,key,fallback,output,size,policy); return found; }
    BOOL GetInteger(const char* section,const char* key,int fallback,int* output)
    { char text[80]; BOOL found=GetString(section,key,"",text,sizeof(text)); *output=found?atoi(text):fallback; return found; }
};
void g_StrCpyLen(char* dest,const char* src,int size) { strncpy(dest,src,size-1); dest[size-1]=0; }
int g_Random(int range) { if(rollAt>=rollCount) return range-1; return rolls[rollAt++]; }
struct KNpc
{
    int m_Kind,m_SubWorldIndex,m_RegionIndex,m_NpcSettingIdx,m_Index; DWORD m_dwID;
    BOOL IsPlayer() { return m_Kind==1; }
    void GetMpsPos(int* x,int* y) { *x=100;*y=200; }
};
KNpc Npc[MAX_NPC];
struct TPlayer { int m_nNetConnectIdx,m_nIndex;bool m_bIsBot,m_PlayerDBLoad; } Player[MAX_PLAYER];
struct TItem
{
    int genre,detail,particular,level,series;
    int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return particular;} int GetLevel(){return level;} int GetSeries(){return series;}
    char* GetName(){return "REAL_TABLE_ITEM";} int GetWidth(){return 1;} int GetHeight(){return 1;}
    int GetQuality(){return 0;} int GetColorItem(){return 0;} int GetObjIdx(){return 38;}
} Item[256];
struct TItemSet
{
    int Add(int genre,int series,int level,int,int detail,int particular,int*,int)
    {
        if(failGeneration) return 0;
        int i=++allocated; Item[i].genre=genre;Item[i].detail=detail+(wrongIdentity?1:0);Item[i].particular=particular;
        Item[i].series=series; Item[i].level=genre==3?0:level; return i;
    }
    void Remove(int){++removed;}
} ItemSet;
struct TWorldSet { int GetGameVersion(){return 1;} } g_SubWorldSet;
struct KMapPos { int nSubWorld,nRegion,nMapX,nMapY,nOffX,nOffY; };
struct TWorld
{
    void GetFreeObjPos(POINT&){}
    void Mps2Map(int,int,int* r,int* x,int* y,int* ox,int* oy){*r=mappedRegion;*x=*y=*ox=*oy=0;}
} SubWorld[MAX_SUBWORLD];
struct KObjItemInfo
{
    int m_nItemID,m_nItemWidth,m_nItemHeight,m_nColorID,m_nGenre,m_nDetailType,m_nMovieFlag,m_nSoundFlag;
    char m_szName[80];
};
struct TTab
{
    BOOL GetString(int,int,const char*,char* output,int size) { g_StrCpyLen(output,wrongObject?"Body":"Item",size);return TRUE; }
};
struct TObjSet
{
    TTab m_cTabFile;
    int Add(int,KMapPos,KObjItemInfo info){ if(info.m_nItemID<=0 || !info.m_szName[0]) return -1; return ++sent; }
} ObjSet;
struct TObject { void SetItemBelong(int){} } Object[256];
#include "PhongThanNpcDrop.inl"
static int checks;
#define CHECK(x) do { ++checks; if(!(x)){printf("FAIL line=%d\n",__LINE__);return 1;} } while(0)
static void Reset(int a,int b)
{ allocated=removed=sent=rollAt=failGeneration=wrongIdentity=wrongObject=0; mappedRegion=1;rollCount=2;rolls[0]=a;rolls[1]=b; }
int main(int argc,char** argv)
{
    if(argc!=3 || !SetCurrentDirectoryA(argv[2]))return 2; policy=argv[1];
    KNpc& npc=Npc[1];npc.m_Index=1;npc.m_NpcSettingIdx=99;npc.m_dwID=123;npc.m_SubWorldIndex=0;npc.m_RegionIndex=1;
    Player[1].m_nIndex=2; Player[1].m_nNetConnectIdx=1;Npc[2].m_Kind=1;Npc[2].m_SubWorldIndex=0;
    Reset(0,120000); PtDropNpc(npc,1);CHECK(sent==2 && allocated==2 && removed==0);
    Reset(200000,250000); PtDropNpc(npc,1);CHECK(sent==1 && Item[1].genre==3 && Item[1].level==0);
    Reset(999999,999999); PtDropNpc(npc,1);CHECK(sent==0 && allocated==0);
    Player[1].m_nNetConnectIdx=-999;Player[1].m_bIsBot=true;Player[1].m_PlayerDBLoad=true;
    Reset(0,0);PtDropNpc(npc,1);CHECK(sent==2);
    Player[1].m_PlayerDBLoad=false;Reset(0,0);PtDropNpc(npc,1);CHECK(sent==0 && rollAt==0);
    Player[1].m_bIsBot=false;Player[1].m_PlayerDBLoad=true;Reset(0,0);PtDropNpc(npc,1);CHECK(sent==0 && rollAt==0);
    Player[1].m_nNetConnectIdx=1;
    Reset(0,0); PtDropNpc(npc,0);CHECK(sent==0 && rollAt==0);
    Reset(0,0);Npc[2].m_SubWorldIndex=1;PtDropNpc(npc,1);CHECK(sent==0);Npc[2].m_SubWorldIndex=0;
    Reset(0,0);failGeneration=1;PtDropNpc(npc,1);CHECK(sent==0 && removed==0);
    Reset(0,0);wrongIdentity=1;PtDropNpc(npc,1);CHECK(sent==0 && removed==2);
    Reset(0,0);wrongObject=1;PtDropNpc(npc,1);CHECK(sent==0 && removed==2);
    Reset(0,0);mappedRegion=-1;PtDropNpc(npc,1);CHECK(sent==0 && removed==2);
    Reset(0,0);npc.m_Kind=1;PtDropNpc(npc,1);CHECK(sent==0);
    printf("PASS NPC_DROP_PRODUCTION_INL checks=%d; network mocked; runtime untouched\n",checks);return 0;
}

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KLuaScript.h"
#include <limits.h>
#include "../../Headers/PhongThanReservoirs.h"
#ifdef PT_TEST_GIFTS
#include "KFilePath.h"
#include "KPakFile.h"
static int GiftMaxStack(int,int,int);
#endif
enum {MAX_PLAYER_ITEM=128,MAX_PLAYER=3,MAX_NPC=8,MAX_ITEM=128,MAX_TASK=5000,MAX_ITEM_MAGICLEVEL=40,MAX_SUBWORLD=2,MAX_ITEM_MAGICATTRIB=20,magic_normal_end=357,equip_shipin=12,itempart_cuff=5,TASKVALUE_PT_HANDBOOK_RECEIPT=4893,
      pos_immediacy=22,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=19,pos_ptbag3=20,item_equip=0,item_medicine=1,item_materials=3,item_task=4,item_townportal=5,item_magicscript=6,item_skillbook=7,item_ibitem=8,equip_detailnum=16};
#include "../../Sources/Core/Src/PhongThanQuestItemTuple.h"
struct Lock {bool locked;bool IsLock(){return locked;}};
struct KItemNormalAttrib{int nAttribType,nValue[3];};
struct KItem {
    int GetExpireTime(){return 0;}

    int id,genre,detail,part,level,series,stack,maxStack,row;bool sell,trade,drop;Lock lock;struct {int nGeneratorLevel[40];}m_GeneratorParam;KItemNormalAttrib m_aryMagicAttrib[20];
 struct {int nParam;}m_CommonAttrib;int GetParam(){return m_CommonAttrib.nParam;}int GetParticularType(){return part;}int GetRow(){return row;}bool IsTemp(){return false;}const char* GetName(){return "jade";}bool GetLockSell(){return sell;}bool GetLockTrade(){return trade;}bool GetLockDrop(){return drop;}void SetLockSell(bool b){sell=b;}void SetLockTrade(bool b){trade=b;}void SetLockDrop(bool b){drop=b;}
    int GetID(){return id;} int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return part;}int GetLevel(){return level;}int GetSeries(){return series;}
    int GetStackNum(){return stack;}int GetMaxStackNum(){return maxStack>0?maxStack:1;}int GetWidth(){return 1;}int GetHeight(){return 1;}
    bool IsStack(){return GetMaxStackNum()>1 && !PhongThanReservoirTooltip(genre,detail,part,GetMaxStackNum(),"NONE");}
    const char* GetScript(){return "NONE";}int GetExpirePoint(){return 0;}void SetExpireTime(int){}
    void SetStackNum(int value){stack=value;}Lock* GetLock(){return &lock;}
} Item[MAX_ITEM];
struct ItemPos {int nPlace,nX,nY;};
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct KItemList {
    PlayerItem m_Items[MAX_ITEM];int capacity,cursor,equipped;bool failSearch;int GetEquipment(int){return equipped;}int GetEquipmentMoney();void SyncItemMagicAttrib(int){}
    PlayerItem* GetFirstItem(){cursor=0;return GetNextItem();}
    PlayerItem* GetNextItem(){while(++cursor<MAX_ITEM)if(m_Items[cursor].nIdx)return &m_Items[cursor];return 0;}
    int FindSame(int index){return m_Items[index].nIdx?index:0;}
    bool Remove(int index){if(!FindSame(index))return false;m_Items[index].nIdx=0;return true;}
    void SyncItem(int){}
    bool SearchPosition(int,int,ItemPos* p,bool){
        if(failSearch)return false;
        for(int x=0;x<capacity;++x){bool used=false;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx && m_Items[i].nX==x)used=true;
            if(!used){p->nPlace=pos_equiproom;p->nX=x;p->nY=0;return true;}}
        return false;
    }
    int Add(int index,int place,int x,int y,bool){
        if(x<0 || x>=capacity || m_Items[index].nIdx)return 0;
        for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx && m_Items[i].nX==x)return 0;
        m_Items[index].nIdx=index;m_Items[index].nPlace=place;m_Items[index].nX=x;m_Items[index].nY=y;return index;
    }
    int Count(){int count=0;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx)++count;return count;}
};
struct Task {int value,slots[MAX_TASK];int GetSaveVal(int id){return id==20?value:slots[id];}void SetSaveVal(int id,int next,BOOL){if(id==20)value=next;else slots[id]=next;}};
struct PlayerMock {
    int m_nIndex,m_nLastNpcIndex;void* m_pStatusLoadPlayerInfo;bool locked,trading,payFail;void UpdataCurData(){}void SendCurNormalSyncData(){}Task m_cTask;KItemList m_ItemList;
    unsigned long m_dwID;int cash,xp;struct {int m_nFlag;} m_cTong;
    bool Earn(int money){if(money<0||cash>2000000000-money)return false;cash+=money;return true;}
    bool Pay(int money){if(payFail||money<0||cash<money)return false;cash-=money;return true;}
    void DirectAddExp(int amount){xp+=amount;}
    bool GetLockState(){return locked;}bool CheckTrading(){return trading;}
} Player[MAX_PLAYER];
int GetPlayerIndex(Lua_State*){return 1;}
struct WorldSet {int GetGameVersion(){return 1;}int SearchWorld(int map); } g_SubWorldSet;
struct Items {
    int allocated,failAt;
    int Add(KItem* p){int i=Add(p->genre,p->series,p->level,0,p->detail,p->part,0,1);if(i){int id=Item[i].id;Item[i]=*p;Item[i].id=id;}return i;}
    int Add(int genre,int series,int level,int,int detail,int particular,int* =0,int =0){
        if(++allocated==failAt)return 0;
        for(int i=2;i<MAX_ITEM;++i)if(!Item[i].id){Item[i].id=i+100;Item[i].genre=genre;Item[i].detail=detail;
            Item[i].part=particular;Item[i].level=level;Item[i].series=series;Item[i].stack=1;Item[i].maxStack=100;
#ifdef PT_TEST_GIFTS
            Item[i].maxStack=GiftMaxStack(genre,detail,particular);
#endif
            return i;}
        return 0;
    }
    void Remove(int index){Item[index].id=0;}
} ItemSet;

#include "KFilePath.h"
#include "KPakList.h"
static int g_Random(int n){return n>0?17%n:0;}
int KItemList::GetEquipmentMoney(){return Player[1].cash;}
struct NpcMock {int m_CurrentLife,m_SubWorldIndex,m_RegionIndex,m_NpcSettingIdx,m_DialogRadius;DWORD m_ActionScriptID;} Npc[MAX_NPC];
struct WorldMock {int m_SubWorldID;} SubWorld[2];
struct NpcSetMock {int GetDistance(int,int){return 2;}} NpcSet;
#define _SERVER
#define KMagicDescH
struct JadeTestMagicDesc {BOOL Init(){return TRUE;}const char* GetDesc(void*){return "Test attribute +7";}} g_MagicDesc;
#define PT_QUEST_ITEM_NOW() 0
int PhongThanConsumableInitialCharges(int,int,int,int,const char*){return 0;}
#include "../../Sources/Core/Src/PhongThanQuestExchange.inl"
#include "../../Sources/Core/Src/PhongThanJadeService.inl"
static void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(&ItemSet,sizeof(ItemSet));Player[1].m_nIndex=1;Player[1].m_nLastNpcIndex=2;Player[1].cash=100000;Player[1].m_ItemList.capacity=20;Npc[1].m_CurrentLife=100;Npc[2].m_DialogRadius=20;Npc[2].m_ActionScriptID=0xB89373F6;SubWorld[0].m_SubWorldID=1052;}
static void Seed(int i,int genre,int detail,int part,int stack,int place=pos_equiproom){Item[i].id=100+i;Item[i].genre=genre;Item[i].detail=detail;Item[i].part=part;Item[i].stack=stack;Item[i].maxStack=100;assert(Player[1].m_ItemList.Add(i,place,i-1,0,false));}
int main(int argc,char** argv){
 assert(argc==2);assert(SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 assert(PtJade::Tables());int cases=0;
 for(int row=0;row<51;++row){int cap=0,ql=0;assert(PtJade::Shape(row,cap,ql));for(int group=1;group<=5;++group){for(unsigned draw=0;draw<11000;draw+=127){int zero[40]={0},after[40];assert(PtJade::Roll(row,group,zero,after,draw,draw));assert(PtJade::Valid(row,after));KItem item;ZeroMemory(&item,sizeof(item));item.detail=12;item.row=row;memcpy(item.m_GeneratorParam.nGeneratorLevel,after,sizeof(after));assert(PtJade::Apply(item));assert(item.m_aryMagicAttrib[5+group].nAttribType>0);assert(item.m_aryMagicAttrib[5+group].nValue[0]==after[19+group]);int saved[40];memcpy(saved,after,sizeof(saved));KItem reload=item;ZeroMemory(reload.m_aryMagicAttrib,sizeof(reload.m_aryMagicAttrib));assert(PtJade::Apply(reload));assert(!memcmp(reload.m_aryMagicAttrib,item.m_aryMagicAttrib,sizeof(item.m_aryMagicAttrib)));++cases;}}}
 Reset();Seed(1,0,12,0,1);Item[1].row=0;Player[1].m_ItemList.equipped=1;Seed(2,6,739,0,1);Item[2].level=1;Seed(3,3,1045,0,5);
 assert(PtJadeRefine(1,1,999)==0&&Item[2].id&&Item[3].stack==5);assert(PtJadeRefine(1,1,101)==1&&!Item[2].id&&!Item[3].id);assert(PtJade::Valid(0,Item[1].m_GeneratorParam.nGeneratorLevel));int before[40];memcpy(before,Item[1].m_GeneratorParam.nGeneratorLevel,sizeof(before));
 Seed(2,6,739,0,1);Item[2].level=1;Seed(3,3,1045,0,5);assert(PtJadeRefine(1,1,101)==-3&&Item[2].id&&Item[3].stack==5);assert(!memcmp(before,Item[1].m_GeneratorParam.nGeneratorLevel,sizeof(before)));Seed(4,8,1081,2,1);assert(PtJadeRefine(1,1,101)==1&&!Item[4].id);assert(PtJadeRefine(1,2,101)==-2);
 Reset();Seed(1,0,12,0,1);Item[1].trade=true;Player[1].m_ItemList.failSearch=true;assert(PtJadeExtract(1,101)==-3&&Item[1].id&&Player[1].m_ItemList.FindSame(1));Player[1].m_ItemList.failSearch=false;assert(PtJadeExtract(1,101)==1&&!Item[1].id);bool found=false;for(int n=2;n<MAX_ITEM;++n)if(Player[1].m_ItemList.FindSame(n)){assert(Item[n].detail>=1045&&Item[n].detail<=1049&&Item[n].trade);found=true;}assert(found);assert(!PtJadeExtract(1,101));
 Reset();Seed(1,3,260,0,2);Player[1].cash=99999;assert(PtJadePolish(1,101)==-3&&Item[1].stack==2);Player[1].cash=100000;Player[1].payFail=true;assert(PtJadePolish(1,101)==-3&&Item[1].stack==2&&Player[1].cash==100000&&Player[1].m_ItemList.Count()==1);Player[1].payFail=false;assert(PtJadePolish(1,101)==1&&Item[1].stack==1&&Player[1].cash==0);assert(PtJadePolished(260,0)==288);for(int d=0;d<8;++d)assert(PtJadePolished(374,d)==375+d);assert(!PtJadePolished(5524,0));
 Reset();Seed(1,3,260,0,1);Npc[2].m_ActionScriptID=0;assert(!PtJadePolish(1,101)&&Item[1].id&&Player[1].cash==100000);
 Reset();Seed(1,3,254,0,1);Player[1].cash=99999;assert(PtJadePolish(1,101)==-3&&Item[1].id);Player[1].cash=100000;DWORD output=0;assert(PtJadePolish(1,101,&output)==1&&output&&Player[1].cash==0);
 int polished=0;for(int k=2;k<MAX_ITEM;++k)if(Item[k].id==(int)output)polished=k;assert(polished&&Item[polished].detail==282);
 int param=0;assert(PtGem::Roll(2,21,1,param)&&PtGem::Apply(Item[polished],param));Item[polished].trade=true;
 assert(PtJadeRevert(1,output)==-3&&Item[polished].GetParam()==param&&Player[1].m_ItemList.FindSame(polished));
 Seed(3,8,509,2,1);assert(PtJadeRevert(1,output)==1&&!Item[polished].id&&!Item[3].id);bool rawFound=false;
 for(k=2;k<MAX_ITEM;++k)if(Player[1].m_ItemList.FindSame(k)){assert(Item[k].detail==254&&Item[k].trade&&!Item[k].GetParam()&&!Item[k].m_aryMagicAttrib[0].nAttribType);rawFound=true;}assert(rawFound);assert(PtJadeRevert(1,output)==0);
 Reset();Seed(1,3,282,0,1);assert(PtGem::Apply(Item[1],param));Seed(2,4,47,0,17);assert(PtJadeRevert(1,101)==-3&&Item[2].stack==17&&Item[1].id);Item[2].stack=18;assert(PtJadeRevert(1,101)==1&&!Item[1].id&&!Item[2].id);
 Reset();Seed(1,3,282,0,1);assert(PtGem::Apply(Item[1],param));Seed(2,8,509,2,1);Player[1].m_ItemList.failSearch=true;assert(PtJadeRevert(1,101)==-3&&Item[1].id&&Item[2].id&&Item[1].GetParam()==param);Player[1].m_ItemList.failSearch=false;Npc[2].m_ActionScriptID=0;assert(!PtJadeRevert(1,101));
 Reset();Seed(1,3,282,0,1);assert(PtGem::Apply(Item[1],param));Seed(2,8,509,2,2,pos_ptbag2);Seed(3,4,47,0,18);
 assert(PtJadeRevert(1,101)==1&&Item[2].stack==1&&Item[3].stack==18);
 Reset();Seed(1,3,282,0,1);assert(PtGem::Apply(Item[1],param));Seed(2,8,509,2,1);Seed(3,4,47,0,18);Player[1].m_ItemList.failSearch=true;
 assert(PtJadeRevert(1,101)==-3&&Item[2].id&&Item[3].stack==18&&Item[1].GetParam()==param);
 for(int raw=253;raw<=401;++raw)if(PtJadePolished(raw,0))assert(PtJadePolishFee(raw)==100000);
 printf("PASS jade production rules: %d rolls, reload, stale serial, capacity, all-tier 10-van fee, mirror priority across bags, Xu fallback, atomic rollback, extraction bind, NPC scope\n",cases);g_pPakList=0;return 0;
}

#include "../../Headers/PhongThanStallMessages.h"
#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanStall.h"
enum {MAX_PLAYER=8,MAX_NPC=8,MAX_ITEM=32,MAX_SUBWORLD=4,MAX_TASK=5400,item_ibitem=8,pos_equiproom=1,pos_equiproomex=2,pos_ptbag2=3,pos_ptbag3=4,do_death=9,do_sit=10,SAVE_REQUEST=1,MESSAGE_SYSTEM_ANNOUCE_HEAD=1};
static DWORD seconds=1000;static DWORD KSG_GetCurSec(){return seconds;}
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct Lock {bool locked;bool IsLock(){return locked;}};
struct KItem {int id,genre,detail,part,expiry,price;bool lockedTrade,temp;Lock lock;
 int GetOwner(){return 102;}void SetOwner(DWORD){}int GetID(){return id;}int GetGenre(){return genre;}int GetParticular(){return part;}int GetDetailType(){return detail;}int GetExpireTime(){return expiry;}Lock* GetLock(){return &lock;}bool GetLockTrade(){return lockedTrade;}bool IsTemp(){return temp;}int GetTradePrice(){return price;}void SetTradePrice(int p){price=p;}int GetWidth(){return 1;}int GetHeight(){return 1;}
};static KItem Item[MAX_ITEM];
struct Inventory {PlayerItem m_Items[MAX_ITEM];int money;bool failAdd,failRemove,full;
 Inventory():money(0),failAdd(false),failRemove(false),full(false){ZeroMemory(m_Items,sizeof(m_Items));}
 int FindSame(int n){return n>0&&n<MAX_ITEM&&m_Items[n].nIdx==n?n:0;}
 int SearchID(int id){for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx&&Item[i].id==id)return i;return 0;}
 int Hand(){return 0;}int GetEquipmentMoney(){return money;}
 bool CheckCanPlaceInEquipment(int,int,int* p,int* x,int* y){*p=pos_equiproom;*x=*y=0;return !full;}
 bool Remove(int n){if(failRemove||!FindSame(n))return false;m_Items[n].nIdx=0;return true;}
 bool RemoveItem(int n,int){if(!Remove(n))return false;Item[n].id=0;return true;}
 int Add(int n,int p,int x,int y,bool){if(failAdd)return 0;m_Items[n].nIdx=n;m_Items[n].nPlace=p;m_Items[n].nX=x;m_Items[n].nY=y;return n;}
};
struct Tasks {std::map<int,int> values;const char* GetSaveStr(int key){return values[key]?"used":"";}int GetSaveVal(int key){return values[key];}void SetSaveVal(int key,int value,bool){values[key]=value;}};
struct Shop {int nTrade;};
struct KNpc {int m_SubWorldIndex,m_CurrentLife,m_Doing,m_bRideHorse,m_FightMode;void SendCommand(int cmd){m_Doing=cmd;}};static KNpc Npc[MAX_NPC];
struct KPlayer {int m_PlayerDBLoad,m_nIndex,m_dwID,m_uMustSave;bool busy,locked,failEarn;Inventory m_ItemList;Tasks m_cTask;Shop m_PTrade;
 KPlayer():m_PlayerDBLoad(0),m_nIndex(0),m_dwID(0),m_uMustSave(0),busy(false),locked(false),failEarn(false){m_PTrade.nTrade=0;}
 bool CheckTrading(){return busy||m_PTrade.nTrade;}bool GetLockState(){return locked;}bool Pay(int n){if(m_ItemList.money<n)return false;m_ItemList.money-=n;return true;}bool Earn(int n){if(failEarn)return false;m_ItemList.money+=n;return true;}
 void SendTradeCancel(){m_PTrade.nTrade=0;}void SendTradeCount(int){if(!m_ItemList.FindSame(5))m_PTrade.nTrade=0;}void SendSystemMessage(int,const char*){}
};static KPlayer Player[MAX_PLAYER];
struct World {int m_SubWorldID;};static World SubWorld[MAX_SUBWORLD];
struct NpcSetType {int distance;int GetDistance(int,int){return distance;}};static NpcSetType NpcSet;
enum{PHONGTHAN_MAX_IBBUFF=48,PHONGTHAN_IBBUFF_ID_MASK=0x3fff,PHONGTHAN_IBBUFF_LEVEL_MASK=0xff,PHONGTHAN_IBBUFF_TIMES_MASK=0x3ff,TASKVALUE_PT_IBBUFF_BEGIN=4900};
struct PHONGTHAN_IBBUFF_ENTRY {int nId,nLevel,nTimes;DWORD dwExpireTime;};struct PHONGTHAN_IBBUFF_STORE {int nOwnerIndex;DWORD dwOwnerId;PHONGTHAN_IBBUFF_ENTRY Entry[48];};static PHONGTHAN_IBBUFF_STORE buffs[MAX_PLAYER];
static PHONGTHAN_IBBUFF_STORE* GetPlayerIBBuffStore(int p){return &buffs[p];}
static int FindIBBuffSlot(PHONGTHAN_IBBUFF_STORE* b,int id){for(int i=0;i<48;++i)if(b->Entry[i].nId==id&&b->Entry[i].nTimes>0&&b->Entry[i].dwExpireTime>seconds)return i;return -1;}
static bool AddIBBuffToStore(PHONGTHAN_IBBUFF_STORE* b,int,int id,int duration,int){for(int i=0;i<48;++i)if(!b->Entry[i].nId){b->Entry[i].nId=id;b->Entry[i].nTimes=1;b->Entry[i].dwExpireTime=seconds+duration;return true;}return false;}
#define g_PhongThanPlayerIBBuff buffs
static BOOL IsValidIBBuffId(int nId)
{
    return nId > 0 && nId <= PHONGTHAN_IBBUFF_ID_MASK;
}
static DWORD PackIBBuffIdentity(const PHONGTHAN_IBBUFF_ENTRY *pEntry)
{
    if (!pEntry || !IsValidIBBuffId(pEntry->nId))
        return 0;
    int nLevel = pEntry->nLevel;
    int nTimes = pEntry->nTimes;
    if (nLevel < 0) nLevel = 0;
    if (nLevel > PHONGTHAN_IBBUFF_LEVEL_MASK) nLevel = PHONGTHAN_IBBUFF_LEVEL_MASK;
    if (nTimes < 1) nTimes = 1;
    if (nTimes > PHONGTHAN_IBBUFF_TIMES_MASK) nTimes = PHONGTHAN_IBBUFF_TIMES_MASK;
    return (DWORD)(pEntry->nId | (nLevel << 14) | (nTimes << 22));
}
static void UnpackIBBuffIdentity(DWORD dwValue, PHONGTHAN_IBBUFF_ENTRY *pEntry)
{
    if (!pEntry)
        return;
    pEntry->nId = (int)(dwValue & PHONGTHAN_IBBUFF_ID_MASK);
    pEntry->nLevel = (int)((dwValue >> 14) & PHONGTHAN_IBBUFF_LEVEL_MASK);
    pEntry->nTimes = (int)((dwValue >> 22) & PHONGTHAN_IBBUFF_TIMES_MASK);
}
static void PersistPlayerIBBuffStore(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
        return;
    PHONGTHAN_IBBUFF_STORE *pStore = &g_PhongThanPlayerIBBuff[nPlayerIndex];
    DWORD stallClock=KSG_GetCurSec();bool hasStallLicense=false;
    for(int clockSlot=0;clockSlot<PHONGTHAN_MAX_IBBUFF;++clockSlot)if(PTStallBuff(pStore->Entry[clockSlot].nId)&&pStore->Entry[clockSlot].nTimes>0&&pStore->Entry[clockSlot].dwExpireTime>stallClock){hasStallLicense=true;break;}
    if(hasStallLicense)Player[nPlayerIndex].m_cTask.SetSaveVal(PT_STALL_SAVED_TASK,(int)stallClock,TRUE);
    else if(Player[nPlayerIndex].m_cTask.GetSaveVal(PT_STALL_SAVED_TASK))Player[nPlayerIndex].m_cTask.SetSaveVal(PT_STALL_SAVED_TASK,0,TRUE);
    for (int i = 0; i < PHONGTHAN_MAX_IBBUFF; ++i)
    {
        int nTask = TASKVALUE_PT_IBBUFF_BEGIN + i * 2;
        DWORD dwPacked = PackIBBuffIdentity(&pStore->Entry[i]);
        int packedValue=(int)dwPacked,endValue=dwPacked?(int)pStore->Entry[i].dwExpireTime:0;
        if(Player[nPlayerIndex].m_cTask.GetSaveVal(nTask)!=packedValue)
            Player[nPlayerIndex].m_cTask.SetSaveVal(nTask,packedValue,TRUE);
        if(Player[nPlayerIndex].m_cTask.GetSaveVal(nTask+1)!=endValue)
            Player[nPlayerIndex].m_cTask.SetSaveVal(nTask+1,endValue,TRUE);
    }
}
static int GetIBBuffDefaultSeconds(int,int){return 3600;}
static void ApplyNativeIBBuffState(int,const PHONGTHAN_IBBUFF_ENTRY*){}
void RestorePhongThanIBBuffs(int nPlayerIndex)
{
    if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER ||
        Player[nPlayerIndex].m_nIndex <= 0 || Player[nPlayerIndex].m_nIndex >= MAX_NPC)
        return;
    PHONGTHAN_IBBUFF_STORE *pStore = &g_PhongThanPlayerIBBuff[nPlayerIndex];
    ZeroMemory(pStore, sizeof(*pStore));
    pStore->nOwnerIndex = nPlayerIndex;
    pStore->dwOwnerId = Player[nPlayerIndex].m_dwID;
    int nWrite = 0;
    DWORD dwNow = KSG_GetCurSec();
    for (int i = 0; i < PHONGTHAN_MAX_IBBUFF; ++i)
    {
        int nTask = TASKVALUE_PT_IBBUFF_BEGIN + i * 2;
        PHONGTHAN_IBBUFF_ENTRY Entry;
        ZeroMemory(&Entry, sizeof(Entry));
        UnpackIBBuffIdentity((DWORD)Player[nPlayerIndex].m_cTask.GetSaveVal(nTask), &Entry);
        Entry.dwExpireTime = (DWORD)Player[nPlayerIndex].m_cTask.GetSaveVal(nTask + 1);
        if(PTStallBuff(Entry.nId)){
            DWORD saved=(DWORD)Player[nPlayerIndex].m_cTask.GetSaveVal(PT_STALL_SAVED_TASK);
            Entry.dwExpireTime=PTStallResumeExpiry(Entry.dwExpireTime,saved,dwNow);
        }
        if(!Entry.dwExpireTime&&(Entry.nId==12||Entry.nId==13||Entry.nId==160||Entry.nId==175||Entry.nId==176||Entry.nId==1357||Entry.nId==898)){
            int seconds=GetIBBuffDefaultSeconds(Entry.nId,Entry.nLevel);
            if(seconds>0)Entry.dwExpireTime=dwNow+seconds;
        }
        if (!IsValidIBBuffId(Entry.nId) || Entry.nTimes <= 0 ||
            (Entry.dwExpireTime && Entry.dwExpireTime <= dwNow))
            continue;
        pStore->Entry[nWrite++] = Entry;
        ApplyNativeIBBuffState(Player[nPlayerIndex].m_nIndex, &Entry);
    }
    PersistPlayerIBBuffStore(nPlayerIndex);
}

static bool PtPetCarried(int p,int n){return Player[p].m_ItemList.FindSame(n)&&Player[p].m_ItemList.m_Items[n].nPlace==pos_equiproom;}
#include "../../Sources/Core/Src/PhongThanStall.inl"
#include "../../Sources/Core/Src/PhongThanStallPurchase.inl"
static void Reset(){
 for(int p=0;p<MAX_PLAYER;++p){Player[p]=KPlayer();ZeroMemory(&buffs[p],sizeof(buffs[p]));}ZeroMemory(Item,sizeof(Item));ZeroMemory(Npc,sizeof(Npc));
 for(int role=1;role<=2;++role){Player[role].m_PlayerDBLoad=1;Player[role].m_nIndex=role;Player[role].m_dwID=100+role;Npc[role].m_CurrentLife=100;Npc[role].m_SubWorldIndex=0;}
 SubWorld[0].m_SubWorldID=1002;NpcSet.distance=64;seconds=1000;
 Player[1].m_ItemList.money=10000;Player[2].m_ItemList.money=200;Player[2].m_PTrade.nTrade=1;
 Item[5].id=55;Item[5].price=1000;Player[2].m_ItemList.Add(5,pos_equiproom,1,1,false);
}
static void Reload(){PtStall::checked=GetTickCount()-6000;}
static void Policy(const char* body){FILE* f=fopen("settings\\phongthan\\StallPolicy.ini","wb");assert(f);fputs(body,f);fclose(f);Reload();}
int main(int argc,char**argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);DeleteFileA("settings\\phongthan\\StallPolicy.ini");CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);

 Reset();Player[1].m_cTask.SetSaveVal(42,765,true);AddIBBuffToStore(&buffs[1],1,38,172800,0);AddIBBuffToStore(&buffs[1],1,898,3600,0);
 PersistPlayerIBBuffStore(1);assert(Player[1].m_cTask.GetSaveVal(PT_STALL_SAVED_TASK)==1000);
 seconds+=3600;RestorePhongThanIBBuffs(1);assert(FindIBBuffSlot(&buffs[1],38)>=0&&buffs[1].Entry[0].dwExpireTime==seconds+172800);assert(FindIBBuffSlot(&buffs[1],898)<0);assert(Player[1].m_cTask.GetSaveVal(42)==765);
 seconds+=600;PersistPlayerIBBuffStore(1);seconds+=864000;RestorePhongThanIBBuffs(1);assert(buffs[1].Entry[0].dwExpireTime==seconds+172200);assert(Player[1].m_cTask.GetSaveVal(42)==765);
 puts("PASS actual production IBBuff persistence/restore: offline pause, online countdown, expired unrelated buff removal and unrelated task preservation");
 Reset();assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1003;assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1004;assert(PhongThanStallAllowed(1));
 SubWorld[0].m_SubWorldID=1005;assert(!PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1021;assert(!PhongThanStallAllowed(1));AddIBBuffToStore(&buffs[1],1,38,172800,0);assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1020;assert(!PhongThanStallAllowed(1));
 SubWorld[0].m_SubWorldID=1021;seconds+=172800;assert(!PhongThanStallAllowed(1));
 Reset();SubWorld[0].m_SubWorldID=1067;AddIBBuffToStore(&buffs[1],1,129,518400,0);assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1068;assert(!PhongThanStallAllowed(1));
 Reset();SubWorld[0].m_SubWorldID=1052;AddIBBuffToStore(&buffs[1],1,186,172800,0);assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1005;assert(!PhongThanStallAllowed(1));
 Reset();Item[6].id=66;Item[6].genre=8;Item[6].detail=38;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);assert(PhongThanUseStallLicense(1,6)==1);assert(!Item[6].id&&buffs[1].Entry[0].dwExpireTime==173800);
 Item[6].id=66;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);assert(PhongThanUseStallLicense(1,6)==0&&Item[6].id==66);
 Reset();Item[6].id=66;Item[6].genre=8;Item[6].detail=38;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);Player[1].m_ItemList.failRemove=true;assert(PhongThanUseStallLicense(1,6)==0&&!buffs[1].Entry[0].nId&&Item[6].id==66);
 Reset();Item[6].id=66;Item[6].genre=8;Item[6].detail=38;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);for(int taskSlot=0;taskSlot<253;++taskSlot)Player[1].m_cTask.SetSaveVal(taskSlot,1,true);assert(PhongThanUseStallLicense(1,6)==0&&Item[6].id==66&&!buffs[1].Entry[0].nId);
 Reset();assert(PhongThanStallPurchase(1,2,5));assert(Player[1].m_ItemList.SearchID(55)==5&&!Player[2].m_ItemList.SearchID(55));assert(Player[1].m_ItemList.money==9000&&Player[2].m_ItemList.money==1200);assert(Player[2].m_cTask.GetSaveVal(5351)==1&&Player[2].m_cTask.GetSaveVal(5352)==1000);assert(!PhongThanStallPurchase(1,2,5));
 for(int failure=0;failure<11;++failure){Reset();switch(failure){case 0:Item[5].price=0;break;case 1:Item[5].lock.locked=true;break;case 2:Player[1].m_ItemList.full=true;break;case 3:Player[1].m_ItemList.money=999;break;case 4:NpcSet.distance=500;break;case 5:Item[5].expiry=999;break;case 6:Player[2].locked=true;break;case 7:Player[1].m_ItemList.failAdd=true;break;case 8:Player[2].failEarn=true;break;case 9:Player[2].m_ItemList.failRemove=true;break;case 10:Player[2].m_ItemList.m_Items[5].nPlace=9;break;}
 int money=Player[1].m_ItemList.money;assert(!PhongThanStallPurchase(1,2,5));assert(Player[2].m_ItemList.SearchID(55)==5&&!Player[1].m_ItemList.SearchID(55));assert(Player[1].m_ItemList.money==money&&Player[2].m_ItemList.money==200);assert(!Player[2].m_cTask.GetSaveVal(5351));}
 Reset();SubWorld[0].m_SubWorldID=1021;assert(!PhongThanStallPurchase(1,2,5)&&!Player[2].m_PTrade.nTrade);
 Policy("[Main]\nVersion=1\nCount=0\n[Maps]\n1021=2\n1002=0\n");Reset();assert(!PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1021;assert(PhongThanStallAllowed(1));
 Policy("[Main]\nVersion=1\nCount=1\n[Maps]\n1005=1\n[License0]\nBuff=9500\nDetail=62000\nSeconds=7200\nEnabled=1\nMaps=1005\n");Reset();SubWorld[0].m_SubWorldID=1005;assert(!PhongThanStallAllowed(1));AddIBBuffToStore(&buffs[1],1,9500,7200,0);assert(PhongThanStallAllowed(1));Npc[1].m_bRideHorse=1;assert(!PhongThanStallAllowed(1));Npc[1].m_bRideHorse=0;Npc[1].m_FightMode=1;assert(!PhongThanStallAllowed(1));
 Policy("[Main]\nVersion=1\nCount=1\n[License0]\nBuff=1\nDetail=1\nSeconds=7200\nEnabled=1\nMaps=1002\n");Npc[1].m_FightMode=0;assert(PhongThanStallAllowed(1));
 Reset();SubWorld[0].m_SubWorldID=1005;Player[1].m_PTrade.nTrade=1;AddIBBuffToStore(&buffs[1],1,9500,1,0);PhongThanStallTick(1);assert(Npc[1].m_Doing==do_sit);++seconds;PhongThanStallTick(1);assert(!Player[1].m_PTrade.nTrade);
 assert(PTStallResumeExpiry(173800,1000,1000000)==1172800&&PTStallResumeExpiry(999,1000,2000)==999&&PTStallResumeExpiry(173800,0,1000000)==173800);
 assert(PTStallLevel(0)==1&&PTStallLevel(9)==1&&PTStallLevel(10)==2&&PTStallLevel(40)==3&&PTStallLevel(810)==10);
 puts("PASS production stall map gates, permit use/rollback, expiry/pose, custom policy reload, settlement conservation, failure rollback and progression");return 0;
}

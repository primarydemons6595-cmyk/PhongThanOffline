// Runs the actual consumable module with deterministic engine boundary doubles.
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef PT_TEST_CLIENT
#define CLIENT_PLAYER_INDEX 1
enum {GDCNI_PLAYER_RT_ATTRIBUTE=1};
static int uiNotifies=0;
static void CoreDataChanged(int,int,int){++uiNotifies;}
#else
#define _SERVER
#endif
#define KTabFile_H
#define MAX_PLAYER 4
#define MAX_NPC 4
#define MAX_ITEM 16
#define MAX_PLAYER_ITEM 16
enum {item_ibitem=8,pos_equiproom=1,pos_equiproomex=2,pos_immediacy=3,pos_repositoryroom=4,pos_ptbag2=5,pos_ptbag3=6,do_death=9,do_revive=10};
struct KMagicAttrib {int nAttribType,nValue[3];};
static DWORD clockMs=100;
static time_t utcSeconds=1700000000;
static time_t PTTestTime(time_t* out){if(out)*out=utcSeconds;return utcSeconds;}
#define time PTTestTime
static DWORD PTTestTick(){return clockMs;}
#define GetTickCount PTTestTick
static int KSG_GetCurSec(){return (int)utcSeconds;}
static const char* tablePath;
class KTabFile {
    int values[128][9],height;
public:
    BOOL Load(const char*) {
        height=0;FILE* f=fopen(tablePath,"r");if(!f)return FALSE;
        char line[512];if(!fgets(line,sizeof(line),f)){fclose(f);return FALSE;}
        while(fgets(line,sizeof(line),f) && height<128) {
            int* v=values[height];
            if(sscanf(line,"%d%d%d%d%d%d%d%d%d",v,v+1,v+2,v+3,v+4,v+5,v+6,v+7,v+8)!=9){fclose(f);return FALSE;}
            ++height;
        }
        fclose(f);return TRUE;
    }
    int GetHeight(){return height+1;}
    void GetInteger(int r,int c,int fallback,int* out){*out=(r>=2 && r<=height+1 && c>=1 && c<=9)?values[r-2][c-1]:fallback;}
};
struct TestItem {
    struct{int nExpireTime;}m_CommonAttrib;int genre,detail,part,count;DWORD id;char script[64];
    int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return part;} int GetStackNum(){return count;}
    void SetStackNum(int n){count=n;} DWORD GetID(){return id;}
    char* GetScript(){return script;}
    struct Lock{bool IsLock(){return false;}}lock;Lock* GetLock(){return &lock;}int GetExpireTime(){return 0;}
} Item[MAX_ITEM];
struct TestList {
    struct Entry {int nPlace;} m_Items[MAX_PLAYER_ITEM];
    int owned[MAX_ITEM],synced;
    int FindSame(int i){return owned[i]?i:0;}
    BOOL RemoveItem(int i,int){if(!owned[i])return FALSE;owned[i]=0;Item[i].count=0;return TRUE;}
    void SyncItem(int){++synced;}
};
struct KNpc {
    DWORD m_dwID;
    int m_CurrentLife,m_CurrentMana,m_CurrentLifeMax,m_CurrentManaMax,m_Doing;
    bool m_bRideHorse;float m_CurrentRunSpeed,m_CurrentWalkSpeed;
    struct {int LifePotionEffect,ManaPotionEffect;} m_PtCombat;
    int applied[387];
    void ModifyAttrib(int,KMagicAttrib* a){applied[a->nAttribType]+=a->nValue[0];}
} Npc[MAX_NPC];
class KPlayer {
public:
    DWORD m_dwID;int m_nIndex;TestList m_ItemList;
    char AccountName[32],Name[32];
    struct Trade {int m_nIsTrading;} m_cTrade;
    bool GetLockState(){return false;}
    void UpdataCurData();
} Player[MAX_PLAYER];
#ifdef PT_TEST_CLIENT
#include "../Sources/Core/Src/PhongThanConsumableClient.inl"
#else
#include "../Sources/Core/Src/PhongThanConsumables.inl"
#endif
void KPlayer::UpdataCurData(){Npc[m_nIndex].m_CurrentRunSpeed=100;Npc[m_nIndex].m_CurrentWalkSpeed=50;ZeroMemory(Npc[m_nIndex].applied,sizeof(Npc[m_nIndex].applied));PhongThanConsumableApply(*this);}
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d: %s\n",__LINE__,#x);return 1;}}while(0)
static void Give(int i,int detail,int part,int count){
    Item[i].genre=8;Item[i].detail=detail;Item[i].part=part;Item[i].count=count;Item[i].id=100+i;
    strcpy(Item[i].script,"NONE");Player[1].m_ItemList.owned[i]=1;Player[1].m_ItemList.m_Items[i].nPlace=pos_equiproom;
}
#ifdef PT_TEST_CLIENT
int main(int argc,char** argv){
    if(argc!=2)return 1;tablePath=argv[1];KPlayer& p=Player[1];p.m_nIndex=1;Npc[1].m_dwID=7;
    PHONGTHAN_SELF_CONSUMABLES packet;ZeroMemory(&packet,sizeof(packet));
    packet.Vitals.Header.PacketSize=sizeof(packet);packet.Vitals.EntityId=7;
    packet.RemainingSeconds[0]=3600;packet.RemainingSeconds[15]=3600;
    uiNotifies+=PhongThanConsumableReceive(p,&packet);CHECK(Npc[1].applied[85]==400 && Npc[1].applied[89]==800);
    CHECK(uiNotifies==1);uiNotifies+=PhongThanConsumableReceive(p,&packet);CHECK(uiNotifies==1);
    DWORD icons[PT_CONSUMABLE_SLOTS];CHECK(PhongThanConsumableUi(icons,sizeof(icons))==1);
    CHECK(icons[0]==3600 && icons[15]==3600 && icons[14]==0);
    clockMs+=1000;PhongThanConsumableUi(icons,sizeof(icons));CHECK(icons[0]==3599);
    CHECK(PhongThanConsumableUi(NULL,sizeof(icons))==0);
    CHECK(PhongThanConsumableUi(icons,sizeof(icons)-1)==0);
    packet.RemainingSeconds[PhongThanTimedItemSlot(233)]=2592000;
    PhongThanConsumableReceive(p,&packet);PhongThanConsumableUi(icons,sizeof(icons));
    CHECK(icons[PhongThanTimedItemSlot(233)]==2592000);
    CHECK(Npc[1].applied[196]==0 && Npc[1].applied[192]==0);
    p.UpdataCurData();CHECK(Npc[1].applied[85]==400);
    packet.RemainingSeconds[0]=0;PhongThanConsumableReceive(p,&packet);CHECK(Npc[1].applied[85]==0 && Npc[1].applied[89]==800);
    packet.RemainingSeconds[14]=3600;packet.Vitals.EntityId=99;PhongThanConsumableReceive(p,&packet);CHECK(Npc[1].applied[85]==0);
    packet.Vitals.EntityId=7;PhongThanConsumableReceive(p,&packet);CHECK(Npc[1].applied[85]==800);
    packet.Vitals.Header.PacketSize=sizeof(PHONGTHAN_SELF_VITALS);PhongThanConsumableReceive(p,&packet);
    CHECK(Npc[1].applied[85]==800 && Npc[1].applied[89]==800);
    PhongThanConsumableUi(icons,sizeof(icons));CHECK(icons[14]==3600 && icons[15]==3600);
    packet.Vitals.Header.PacketSize=sizeof(packet);PhongThanConsumableReceive(p,&packet);
    PhongThanConsumableClear(p);p.UpdataCurData();CHECK(Npc[1].applied[85]==0);
    PhongThanConsumableUi(icons,sizeof(icons));CHECK(icons[0]==0 && icons[15]==0);
    printf("PASS client checks=%d; server-authoritative add/remove/rebuild/identity/release\n",checks);return 0;
}
#else
int main(int argc,char** argv){
    if(argc!=2)return 1;tablePath=argv[1];
    KPlayer& p=Player[1];p.m_dwID=7;p.m_nIndex=1;
    strcpy(p.AccountName,"consumable_test");sprintf(p.Name,"fixture_%lu",GetCurrentProcessId());
    CHECK(CreateDirectoryA("CharacterStore",NULL) || GetLastError()==ERROR_ALREADY_EXISTS);
    KNpc& n=Npc[1];n.m_CurrentLife=900;n.m_CurrentLifeMax=1000;n.m_CurrentMana=500;n.m_CurrentManaMax=1000;
    CHECK(PTConsumableLoad());CHECK(ptConsumableCount==78 || ptConsumableCount==82);
    CHECK(PhongThanConsumableInitialCharges(8,28,3,50000,"NONE")==50000);
    CHECK(PhongThanConsumableInitialCharges(8,29,4,50000,"NONE")==50000);
    CHECK(PhongThanConsumableInitialCharges(8,0,0,10,"NONE")==10);
    CHECK(PhongThanConsumableInitialCharges(8,14,0,1,"NONE")==1);
    CHECK(PhongThanConsumableInitialCharges(8,36,0,5,"pill1.lua")==0);
    CHECK(PhongThanConsumableInitialCharges(8,99999,0,5,"NONE")==0);
    Give(8,330,0,5);Item[8].m_CommonAttrib.nExpireTime=(int)utcSeconds;CHECK(PhongThanUseConsumable(p,8)==0 && Item[8].count==5);Item[8].m_CommonAttrib.nExpireTime=0;
    Give(1,28,3,1000);CHECK(PhongThanUseConsumable(p,1)==1);CHECK(Item[1].count==1000);
    clockMs+=499;PhongThanConsumableTick(p);CHECK(n.m_CurrentLife==900);
    ++clockMs;PhongThanConsumableTick(p);CHECK(n.m_CurrentLife==950 && Item[1].count==950);
    clockMs+=500;PhongThanConsumableTick(p);CHECK(n.m_CurrentLife==1000 && Item[1].count==900);
    clockMs+=500;PhongThanConsumableTick(p);CHECK(Item[1].count==900);
    n.m_CurrentLife=975;clockMs+=500;PhongThanConsumableTick(p);CHECK(n.m_CurrentLife==1000 && Item[1].count==875);
    n.m_CurrentLife=800;p.m_ItemList.m_Items[1].nPlace=pos_repositoryroom;
    clockMs+=500;PhongThanConsumableTick(p);CHECK(n.m_CurrentLife==800 && Item[1].count==875);
    CHECK(PhongThanUseConsumable(p,1)==0);
    p.m_ItemList.m_Items[1].nPlace=pos_immediacy;CHECK(PhongThanUseConsumable(p,1)==1);
    n.m_Doing=do_death;clockMs+=500;PhongThanConsumableTick(p);CHECK(Item[1].count==875);CHECK(PhongThanUseConsumable(p,1)==0);
    n.m_Doing=0;p.m_cTrade.m_nIsTrading=1;clockMs+=500;PhongThanConsumableTick(p);CHECK(Item[1].count==875);
    p.m_cTrade.m_nIsTrading=0;CHECK(PhongThanUseConsumable(p,1)==1);
    clockMs+=500;PhongThanConsumableTick(p);CHECK(n.m_CurrentLife==800);
    Give(2,29,4,20);CHECK(PhongThanUseConsumable(p,2)==1);clockMs+=500;PhongThanConsumableTick(p);
    CHECK(n.m_CurrentMana==520 && !p.m_ItemList.owned[2]);
    Give(3,0,0,10);CHECK(PhongThanUseConsumable(p,3)==1);CHECK(Item[3].count==9 && n.applied[85]==400);
    p.UpdataCurData();CHECK(n.applied[85]==400);
    CHECK(PhongThanUseConsumable(p,3)==1);CHECK(Item[3].count==8 && n.applied[85]==400);
    clockMs+=3600000;utcSeconds+=3600;PhongThanConsumableTick(p);CHECK(n.applied[85]==400);
    PhongThanConsumableClear(p);p.UpdataCurData();CHECK(n.applied[85]==400);
    clockMs+=3600000;utcSeconds+=3600;PhongThanConsumableTick(p);CHECK(n.applied[85]==0);
    Give(4,15,0,1);CHECK(PhongThanUseConsumable(p,4)==1);CHECK(n.applied[89]==800 && !p.m_ItemList.owned[4]);
    Give(5,36,0,5);strcpy(Item[5].script,"pill1.lua");CHECK(PhongThanUseConsumable(p,5)==-1 && Item[5].count==5);
    CHECK(PhongThanUseConsumable(p,MAX_ITEM)==0);
    p.m_ItemList.m_Items[3].nPlace=pos_repositoryroom;CHECK(PhongThanUseConsumable(p,3)==0);
    p.m_ItemList.m_Items[3].nPlace=pos_equiproom;CHECK(PhongThanUseConsumable(p,3)==1);
    PhongThanConsumableClear(p);p.UpdataCurData();
    CHECK(n.applied[85]==400 && n.applied[89]==800);
    CHECK(Item[3].count==7); // Relog does not consume another charge.
    utcSeconds+=3600;PhongThanConsumableClear(p);p.UpdataCurData();CHECK(n.applied[85]==0 && n.applied[89]==0);
    Give(6,14,0,2);CHECK(PhongThanUseConsumable(p,6)==1);CHECK(n.applied[85]==800);
    PHONGTHAN_SELF_CONSUMABLES snapshot;ZeroMemory(&snapshot,sizeof(snapshot));
    snapshot.Vitals.MapId=1002;snapshot.Vitals.EntityId=7;
    PhongThanInitializeWireHeader(&snapshot.Vitals.Header,PHONGTHAN_MSG_WORLD_SELF_VITALS,sizeof(snapshot),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    PhongThanConsumableSnapshot(p,&snapshot);
    CHECK(PhongThanValidateSelfVitals(&snapshot,sizeof(snapshot)));
    CHECK(snapshot.RemainingSeconds[14]==3600 && snapshot.RemainingSeconds[0]==0);
    snapshot.RemainingSeconds[14]=PT_TIMED_ITEM_MAX_SECONDS+1;CHECK(PhongThanValidateSelfVitals(&snapshot,sizeof(snapshot)));
    snapshot.RemainingSeconds[14]=0x80000000UL;CHECK(!PhongThanValidateSelfVitals(&snapshot,sizeof(snapshot)));
    snapshot.RemainingSeconds[14]=3600;snapshot.ConsumableVersion=3;CHECK(!PhongThanValidateSelfVitals(&snapshot,sizeof(snapshot)));
    PTConsumableFile identity;char path[MAX_PATH];CHECK(PTConsumableFileIdentity(p,identity,path));
    CHECK(SetFileAttributesA(path,FILE_ATTRIBUTE_READONLY));
    int count=Item[3].count;CHECK(PhongThanUseConsumable(p,3)==0 && Item[3].count==count && n.applied[85]==800);
    CHECK(SetFileAttributesA(path,FILE_ATTRIBUTE_NORMAL));
    KPlayer& other=Player[2];other.m_dwID=8;other.m_nIndex=2;
    strcpy(other.AccountName,p.AccountName);sprintf(other.Name,"other_%lu",GetCurrentProcessId());
    other.UpdataCurData();CHECK(Npc[2].applied[85]==0);
    HANDLE corrupt=CreateFileA(path,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    CHECK(corrupt!=INVALID_HANDLE_VALUE);DWORD wrote=0;BYTE zero=0;
    CHECK(WriteFile(corrupt,&zero,1,&wrote,NULL));CloseHandle(corrupt);
    // Simulate restart: do not let a clean logout repair deliberate disk corruption.
    ZeroMemory(&ptConsumableStates[1],sizeof(PTConsumableState));
    PhongThanConsumableClear(p);p.UpdataCurData();CHECK(n.applied[85]==0);
    CHECK(PhongThanUseConsumable(p,3)==0 && Item[3].count==count);
    CHECK(DeleteFileA(path));
    // Upgrade an actual legacy 28-slot file without losing an active powder.
    PhongThanConsumableClear(p);DeleteFileA(path);
    struct LegacyFile {DWORD magic,version,saved;char account[32],role[32];DWORD end[28],checksum;} legacy;
    ZeroMemory(&legacy,sizeof(legacy));legacy.magic=0x42544350UL;legacy.version=1;legacy.saved=(DWORD)utcSeconds;
    strcpy(legacy.account,p.AccountName);strcpy(legacy.role,p.Name);legacy.end[14]=(DWORD)utcSeconds+3600;
    legacy.checksum=PTConsumableHash(&legacy,sizeof(legacy)-sizeof(DWORD));
    HANDLE oldFile=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    CHECK(oldFile!=INVALID_HANDLE_VALUE);CHECK(WriteFile(oldFile,&legacy,sizeof(legacy),&wrote,NULL));CloseHandle(oldFile);
    ZeroMemory(&ptConsumableStates[1],sizeof(PTConsumableState));p.UpdataCurData();
    CHECK(n.applied[85]==800);PhongThanConsumableClear(p);
    // Auto reservoirs dispense one charged dose; they never toggle continuous use.
    PhongThanConsumableClear(p);n.m_CurrentLife=500;n.m_CurrentMana=500;
    Give(7,28,3,200);clockMs+=1000;
    CHECK(PhongThanConsumableAutoDose(p,7));CHECK(n.m_CurrentLife==550);CHECK(Item[7].count==150);
    CHECK(!PhongThanConsumableAutoDose(p,7));CHECK(Item[7].count==150);
    clockMs+=500;CHECK(PhongThanConsumableAutoDose(p,7));CHECK(n.m_CurrentLife==600);
    n.m_Doing=do_death;clockMs+=500;CHECK(!PhongThanConsumableAutoDose(p,7));n.m_Doing=0;
    p.m_ItemList.owned[7]=0;CHECK(!PhongThanConsumableAutoDose(p,7));p.m_ItemList.owned[7]=1;
    Give(7,29,4,200);clockMs+=500;CHECK(PhongThanConsumableAutoDose(p,7));CHECK(n.m_CurrentMana==550);
    Give(7,14,0,2);clockMs+=500;CHECK(!PhongThanConsumableAutoDose(p,7));CHECK(Item[7].count==2);
    // Exercise every canonical descriptor with the actual adapter, not a model.
    PhongThanConsumableClear(p);
    for(int row=0;row<ptConsumableCount;++row) {
        PTConsumableDef& def=ptConsumableDefs[row];
        PhongThanConsumableClear(p);DeleteFileA(path);p.UpdataCurData();
        n.m_CurrentLife=500;n.m_CurrentMana=500;
        int capacity=def.kind>=3 ? 2 : def.value[0]+25;
        Give(7,def.detail,def.part,capacity);
        CHECK(PhongThanUseConsumable(p,7)==1);
        if(def.kind==3) {
            CHECK(Item[7].count==1);
            for(int a=0;a<2;++a) if(def.id[a]) CHECK(n.applied[def.id[a]]==def.value[a]);
        } else if(def.kind==5) {
            CHECK(Item[7].count==1);
            int slot=PhongThanTimedItemSlot(def.detail);CHECK(slot>=49);
            PhongThanConsumableSnapshot(p,&snapshot);CHECK(snapshot.RemainingSeconds[slot]==(DWORD)def.seconds);
            PhongThanConsumableClear(p);utcSeconds+=100;p.UpdataCurData();
            PhongThanConsumableSnapshot(p,&snapshot);CHECK(snapshot.RemainingSeconds[slot]==(DWORD)def.seconds-100);
            if(def.detail==133){
                CHECK(n.m_CurrentRunSpeed==100);n.m_bRideHorse=true;p.UpdataCurData();CHECK(n.m_CurrentRunSpeed>119.99f&&n.m_CurrentRunSpeed<120.01f);
                p.UpdataCurData();CHECK(n.m_CurrentRunSpeed>119.99f&&n.m_CurrentRunSpeed<120.01f);
                n.m_bRideHorse=false;p.UpdataCurData();CHECK(n.m_CurrentRunSpeed==100);
            }else for(int a=0;a<2;++a)if(def.id[a])CHECK(n.applied[def.id[a]]==def.value[a]);
            utcSeconds+=def.seconds;PhongThanConsumableTick(p);PhongThanConsumableSnapshot(p,&snapshot);CHECK(snapshot.RemainingSeconds[slot]==0);
        } else if(def.kind==4) {
            CHECK(Item[7].count==1);
            int slot=PhongThanTimedItemSlot(def.detail);CHECK(slot>=28);
            PhongThanConsumableSnapshot(p,&snapshot);
            CHECK(snapshot.RemainingSeconds[slot]==(DWORD)def.seconds);
            CHECK(n.applied[192]==0 && n.applied[196]==0);
            utcSeconds+=100;PhongThanConsumableClear(p);utcSeconds+=500;p.UpdataCurData();
            PhongThanConsumableSnapshot(p,&snapshot);
            if(snapshot.RemainingSeconds[slot]!=(DWORD)(def.seconds-(def.clock==0?100:600)))printf("DETAIL=%d CLOCK=%d EXPECT=%d ACTUAL=%lu LOADED=%d\n",def.detail,def.clock,def.seconds-(def.clock==0?100:600),snapshot.RemainingSeconds[slot],ptConsumableStates[1].loaded);CHECK(snapshot.RemainingSeconds[slot]==(DWORD)(def.seconds-(def.clock==0?100:600)));
            if(def.detail==233 || def.detail==390) {
                CHECK(PhongThanConsumableChatColor(p));
                DWORD previous=snapshot.RemainingSeconds[slot];CHECK(PhongThanUseConsumable(p,7)==1);
                PhongThanConsumableSnapshot(p,&snapshot);CHECK(snapshot.RemainingSeconds[slot]==previous+def.seconds);
                CHECK(PhongThanValidateSelfVitals(&snapshot,sizeof(snapshot)));
                PhongThanConsumableClear(p);p.UpdataCurData();CHECK(PhongThanConsumableChatColor(p));
                PhongThanConsumableSnapshot(p,&snapshot);CHECK(snapshot.RemainingSeconds[slot]==previous+def.seconds);
            }
        } else {
            clockMs+=500;PhongThanConsumableTick(p);
            int expected=min(def.value[0],500);
            CHECK((def.kind==1 ? n.m_CurrentLife : n.m_CurrentMana)==500+expected);
            CHECK(Item[7].count==capacity-expected);
        }
    }
    DeleteFileA(path);PhongThanConsumableClear(p);
    // Long stacked duration survives a fresh session, not merely a live timer.
    PTConsumableState& stacked=PTConsumableStateFor(p);int longSlot=PhongThanTimedItemSlot(233);
    stacked.buff[longSlot].end=(DWORD)utcSeconds+200UL*86400;
    CHECK(PTConsumableWrite(p,stacked));CHECK(strstr(path,"consumable_test")!=0);
    PhongThanConsumableClear(p);ZeroMemory(&ptConsumableStates[1],sizeof(PTConsumableState));
    utcSeconds+=1000;p.UpdataCurData();PhongThanConsumableSnapshot(p,&snapshot);
    CHECK(snapshot.RemainingSeconds[longSlot]==200UL*86400-1000);
    // Same role ID can outlive a socket/entity. Never reuse another identity's cache.
    char originalName[32];strcpy(originalName,p.Name);strcpy(p.Name,"same_id_other_role");
    p.UpdataCurData();PhongThanConsumableSnapshot(p,&snapshot);CHECK(snapshot.RemainingSeconds[longSlot]==0);
    strcpy(p.Name,originalName);p.UpdataCurData();PhongThanConsumableSnapshot(p,&snapshot);
    CHECK(snapshot.RemainingSeconds[longSlot]==200UL*86400-1000);
    // Failed read is retried and does not erase the last committed file.
    PhongThanConsumableClear(p);HANDLE locked=CreateFileA(path,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    CHECK(locked!=INVALID_HANDLE_VALUE);p.UpdataCurData();CHECK(ptConsumableStates[1].loaded==-1);
    CloseHandle(locked);clockMs+=5000;p.UpdataCurData();PhongThanConsumableSnapshot(p,&snapshot);
    CHECK(ptConsumableStates[1].loaded==1&&snapshot.RemainingSeconds[longSlot]==200UL*86400-1000);
    PhongThanConsumableClear(p);DeleteFileA(path);
    // Crafted elixir effect enhances recovery, not reservoir consumption.
    PhongThanConsumableClear(p);n.m_CurrentLife=500;n.m_CurrentLifeMax=1000;n.m_CurrentMana=500;n.m_CurrentManaMax=1000;n.m_Doing=0;
    n.m_PtCombat.LifePotionEffect=50;Give(7,28,3,200);clockMs+=500;CHECK(PhongThanConsumableAutoDose(p,7));CHECK(n.m_CurrentLife==575&&Item[7].count==150);
    n.m_PtCombat.ManaPotionEffect=20;Give(7,29,4,200);clockMs+=500;CHECK(PhongThanConsumableAutoDose(p,7));CHECK(n.m_CurrentMana==560&&Item[7].count==150);
    printf("PASS checks=%d; persistence/relog/expiry/corruption/storage-failure/isolation gates passed\n",checks);
    return 0;
}
#endif

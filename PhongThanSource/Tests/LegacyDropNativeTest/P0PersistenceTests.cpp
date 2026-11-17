#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "LegacyDropItemAdapter.h"
#include "PhongThanConsumableData.h"
#include "P0ItemMagicPersistence.h"
#include "../../Headers/PhongThanDiagnostics.h"
#include "PhongThanGemRules.h"
#include "PhongThanRpsSave.h"
#include "P1BlueMagicTrace.h" // Production save/load bodies now contain opt-in P1 trace calls.
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
#include <malloc.h>
#include <stdarg.h>
KPakList* g_pPakList=0;
static int checks=0,failures=0;
static void Check(bool ok,const char* name){++checks;if(!ok){++failures;printf("FAIL %s\n",name);}}
static void CoreLoginDiag(const char* format,...){va_list args;va_start(args,format);vprintf(format,args);puts("");va_end(args);}
#define DBLOADPERTIME_ITEM 10
typedef PlayerItem FixtureSlot;
class KItemList {public:
    bool RealmAllows(const KItem*) const{return true;}
    struct Chain {int size;Chain():size(0){}int GetNext(int n) const{return n<size?n+1:0;}} m_UseIdx;
    FixtureSlot m_Items[513];
    FixtureSlot m_sBackItems[513];
    int FindSame(int item){for(int i=1;i<=m_UseIdx.size;++i)if(m_Items[i].nIdx==item)return i;return 0;}
    int failAfter;
    KItemList():failAfter(-1){ZeroMemory(m_Items,sizeof(m_Items));}
    int Add(int item,int place,int x,int y,bool ignored=false){if(m_UseIdx.size>=512||(failAfter>=0&&m_UseIdx.size>=failAfter))return 0;int n=++m_UseIdx.size;m_Items[n].nIdx=item;m_Items[n].nPlace=place;m_Items[n].nX=x;m_Items[n].nY=y;return n;}
    int Remove(int item){for(int i=1;i<=m_UseIdx.size;++i)if(m_Items[i].nIdx==item){m_Items[i].nIdx=0;return 1;}return 0;}
    void Clear(){for(int i=1;i<=m_UseIdx.size;++i)if(m_Items[i].nIdx)ItemSet.Remove(m_Items[i].nIdx);m_UseIdx.size=0;ZeroMemory(m_Items,sizeof(m_Items));failAfter=-1;}
};
class P0PlayerHarness {public:KItemList m_ItemList;struct TradeState {int m_bTradeFolkGame;TradeState():m_bTradeFolkGame(0){}}m_cTrade;int CheckTrading(bool){return 0;}int LoadPlayerItemList(const PHONGTHAN_CHARACTER_STATE_HEADER*,unsigned int&);int SavePlayerItemList(PHONGTHAN_CHARACTER_STATE_HEADER*,unsigned int);};
#include "../../Output/LegacyDropNativeTest/P0PlayerBodies.inl"
static void InitState(std::vector<BYTE>& bytes,int role,bool legacy=false){bytes.resize(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);ZeroMemory(&bytes[0],bytes.size());PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];s->SchemaVersion=1;s->Revision=1;s->FightLevel=20;sprintf((char*)s->RoleName,legacy?"p0legacy":"p0role%02d",role);sprintf((char*)s->AccountName,"p0account%02d",role);}
static bool LoadItems(P0PlayerHarness& p,const PHONGTHAN_CHARACTER_STATE_HEADER* s){unsigned n=0;int result;do{result=p.LoadPlayerItemList(s,n);}while(result==0);return result==1;}
static std::string RolePath(const std::string& store,const char* role){std::string directory,name;PtAccountDirectory(store.c_str(),"p0account00",directory,false);PtRoleFileName(role,name);return directory+"\\"+name;}
static void Golden(const std::string& dir,int role,const std::vector<P0ItemMagic::Entry>& entries,bool write){char name[64];sprintf(name,"\\golden%02d.bin",role);std::string p=dir+name;if(write){FILE* f=fopen(p.c_str(),"wb");Check(f!=0,"golden capture");if(f){Check(fwrite(&entries[0],sizeof(entries[0]),entries.size(),f)==entries.size(),"golden entries write");fclose(f);}}else{std::string raw;Check(P0ItemMagic::ReadFile(p,raw),"golden reopen from previous process");Check(raw.size()==entries.size()*sizeof(entries[0])&&!memcmp(raw.c_str(),&entries[0],raw.size()),"exact magic/base/requirement/quality/serial across process restart");}}
int main(int argc,char** argv){
    if(argc!=5||!SetCurrentDirectoryA(argv[2]))return 2;const bool writer=!strcmp(argv[1],"write");std::string root=argv[3],store=root+"\\CharacterStore";CreateDirectoryA(store.c_str(),0);SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",root.c_str());
    g_SetRootPath(argv[2]);g_SetFilePath("\\");KPakList packs;Check(packs.Open("\\package.ini")!=0,"native Loose fixture");g_pPakList=&packs;g_SetPakFileMode(1);Check(ItemGen.Init()!=0,"native item generator");ItemSet.Init();LegacyDropItemAdapter adapter;Check(adapter.Init(ItemGen),"native adapter");CPhongThanCharacterStore chars;Check(chars.Open(store.c_str()),"isolated actual CharacterStore");
    int blueCount=0,whiteCount=0;std::vector<BYTE> bytes;P0PlayerHarness player;
    for(int role=0;role<6;++role){bool legacy=role==5;InitState(bytes,role,legacy);PHONGTHAN_CHARACTER_STATE_HEADER* state=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];
        if(writer){
            int count=legacy?20:104;
            for(int i=0;i<count;++i){PhongThanQuestItemTuple tuple;Check(adapter.Map(0,6,0,tuple),"fixture equipment identity");tuple.level=1+i%10;int index=0;std::string error;Check(adapter.GenerateMapped(tuple,index,error),"native fixture item create");if(!index)continue;
                if(!legacy&&i<100){KItemNormalAttrib attrs[MAX_ITEM_MAGICATTRIB];ZeroMemory(attrs,sizeof(attrs));int lines=1+i%6;for(int a=0;a<lines;++a){attrs[a].nAttribType=85+a;attrs[a].nValue[0]=10+i+a;attrs[a].nValue[1]=-1-a;attrs[a].nValue[2]=i*3+a;}if(i%25==0)attrs[0].nAttribType=magic_indestructible_b;Check(Item[index].SetMagicAttrib(attrs)!=0,"native TYPE+3 VALUE");Check(Item[index].GetQuality()==equip_magic,"native blue quality");++blueCount;}else ++whiteCount;
                Item[index].SetOwner(10000+role*1000+i);Item[index].SetTradePrice(200+i);Item[index].SetFortune(i%5);Item[index].SetParam(i+1);Item[index].SetFlash(i%4);Item[index].SetLockSell(i%2);Item[index].SetLockTrade(i%3==0);Item[index].SetLockDrop(i%4==0);if(i%25)Item[index].SetDurability(5+i%10);player.m_ItemList.Add(index,pos_equiproom,i%10,i/10);
            }
            if(legacy)SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",0);
            Check(player.SavePlayerItemList(state,bytes.size())==1,"actual production item save body");state->StateSize=PhongThanCharacterExpectedStateSize(state);Check(PhongThanValidateCharacterState(state,state->StateSize)!=0,"unchanged canonical state ABI");Check(chars.Create(state,state->StateSize),"actual atomic CharacterStore create");
            SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",root.c_str());
            std::vector<P0ItemMagic::Entry> expected;for(int k=1;k<=player.m_ItemList.m_UseIdx.size;++k){P0ItemMagic::Entry e;P0ItemMagic::Capture(Item[player.m_ItemList.m_Items[k].nIdx],k-1,e);expected.push_back(e);}if(legacy)for(size_t x=0;x<expected.size();++x)expected[x].serial=0;Golden(root,role,expected,true);
            FILE* canonical=fopen((root+"\\canonical"+(char)('0'+role)+".bin").c_str(),"wb");Check(canonical!=0,"canonical reference capture");if(canonical){fwrite(state,1,state->StateSize,canonical);fclose(canonical);}player.m_ItemList.Clear();
            Check(LoadItems(player,state),"in-process full production item load body");Check(player.m_ItemList.m_UseIdx.size==count,"in-process exact count");player.m_ItemList.Clear();
        }else{
            char roleName[32];sprintf(roleName,legacy?"p0legacy":"p0role%02d",role);PHONGTHAN_U32 size=0;Check(chars.Load(roleName,&bytes[0],bytes.size(),&size),"CharacterStore read after process restart");state=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];if(!size)continue;
            Check(LoadItems(player,state),"production item load in fresh process");Check(player.m_ItemList.m_UseIdx.size==(legacy?20:104),"no missing/duplicate restored item");
            std::vector<P0ItemMagic::Entry> actual;for(int k=1;k<=player.m_ItemList.m_UseIdx.size;++k){KItem& item=Item[player.m_ItemList.m_Items[k].nIdx];P0ItemMagic::Entry e;P0ItemMagic::Capture(item,k-1,e);actual.push_back(e);if(item.GetQuality()==equip_magic)++blueCount;else ++whiteCount;}
            if(legacy)for(size_t x=0;x<actual.size();++x){Check(actual[x].quality==equip_normal,"old white item remains white without companion");actual[x].serial=0;}Golden(root,role,actual,false);
            // Save again without changing any item metadata; canonical item bytes
            // (including GeneratorLevel) must remain identical.
            std::vector<BYTE> again=bytes;PHONGTHAN_CHARACTER_STATE_HEADER* saved=(PHONGTHAN_CHARACTER_STATE_HEADER*)&again[0];if(legacy)SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",0);
            Check(player.SavePlayerItemList(saved,again.size())==1,"second save metadata replay");Check(saved->ItemCount==state->ItemCount&&!memcmp(PhongThanCharacterItems(saved),PhongThanCharacterItems(state),state->ItemCount*sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD)),"all canonical metadata/GeneratorLevel unchanged");SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",root.c_str());player.m_ItemList.Clear();
        }
    }
    if(!writer){
        // Use committed role0 as the recovery baseline for all fault cases.
        std::vector<BYTE> baseline(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);PHONGTHAN_U32 n=0;Check(chars.Load("p0role00",&baseline[0],baseline.size(),&n),"failure baseline");PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&baseline[0];std::string key,error;Check(P0ItemMagic::StateHash(s,key,error),"snapshot binding");std::string dataPath=root+"\\"+key+".magic-v1",raw;Check(P0ItemMagic::ReadFile(dataPath,raw),"capture snapshot before corruption tests");
        HANDLE f=CreateFileA(dataPath.c_str(),GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);DWORD wrote=0;BYTE byte=0;Check(f!=INVALID_HANDLE_VALUE&&WriteFile(f,&byte,1,&wrote,0),"inject isolated corrupt version/header");if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);Check(!LoadItems(player,s)&&ItemSet.GetItemNumber()==0,"corrupt magic blocks before any item allocation");
        f=CreateFileA(dataPath.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);Check(f!=INVALID_HANDLE_VALUE&&WriteFile(f,raw.c_str(),raw.size(),&wrote,0),"restore isolated snapshot bytes");if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);
        std::string moved=dataPath+".held";Check(MoveFileA(dataPath.c_str(),moved.c_str())!=0,"hide required companion");Check(!LoadItems(player,s)&&ItemSet.GetItemNumber()==0,"missing required data rejects before allocations");Check(MoveFileA(moved.c_str(),dataPath.c_str())!=0,"restore companion");
        Check(LoadItems(player,s),"valid snapshot recovers after faults");int first=player.m_ItemList.m_Items[1].nIdx;Check(ItemSet.AddPersistent(&Item[first],Item[first].GetID())==0,"duplicate persistent serial rejected");player.m_ItemList.Clear();
        player.m_ItemList.failAfter=3;Check(!LoadItems(player,s)&&ItemSet.GetItemNumber()==0,"mid-load insertion failure rolls back every native allocation");player.m_ItemList.Clear();
        std::vector<BYTE> badSave=baseline;Check(player.SavePlayerItemList((PHONGTHAN_CHARACTER_STATE_HEADER*)&badSave[0],badSave.size())==-1,"failed load cannot save an empty inventory over committed role");
        Check(LoadItems(player,s),"full retry recovers after insertion rollback");player.m_ItemList.Clear();
        std::string markerPath=root+"\\"+key+".required";Check(MoveFileA(dataPath.c_str(),moved.c_str())&&MoveFileA(markerPath.c_str(),(markerPath+".held").c_str()),"hide both snapshot and marker");Check(!LoadItems(player,s)&&ItemSet.GetItemNumber()==0,"managed role never silently falls back to white when both files missing");Check(MoveFileA(moved.c_str(),dataPath.c_str())&&MoveFileA((markerPath+".held").c_str(),markerPath.c_str()),"restore complete required pair");Check(LoadItems(player,s),"complete pair recovery");player.m_ItemList.Clear();
        // A failed canonical save cannot switch the old committed role's magic.
        std::vector<BYTE> next=baseline;PHONGTHAN_CHARACTER_STATE_HEADER* candidate=(PHONGTHAN_CHARACTER_STATE_HEADER*)&next[0];candidate->Revision++;std::vector<P0ItemMagic::Entry> entries;bool present=false;Check(P0ItemMagic::Load(s,entries,present,error)&&present,"read committed companion");entries[0].magic[0].nValue[0]+=1000;Check(P0ItemMagic::Save(candidate,entries,error),"stage new snapshot before main save");
        std::string rolePath=RolePath(store,"p0role00");HANDLE lock=CreateFileA(rolePath.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);Check(lock!=INVALID_HANDLE_VALUE,"lock committed main file against atomic replace");Check(!chars.Save(candidate,candidate->StateSize),"actual CharacterStore save/rename failure");if(lock!=INVALID_HANDLE_VALUE)CloseHandle(lock);
        PHONGTHAN_U32 still=0;std::vector<BYTE> old(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);Check(chars.Load("p0role00",&old[0],old.size(),&still)&&still==n&&!memcmp(&old[0],&baseline[0],n),"old committed canonical state byte-exact after failed save");Check(LoadItems(player,(PHONGTHAN_CHARACTER_STATE_HEADER*)&old[0]),"old magic snapshot selected, orphan not applied");std::vector<P0ItemMagic::Entry> after;for(int k=1;k<=player.m_ItemList.m_UseIdx.size;++k){P0ItemMagic::Entry e;P0ItemMagic::Capture(Item[player.m_ItemList.m_Items[k].nIdx],k-1,e);after.push_back(e);}Golden(root,0,after,false);player.m_ItemList.Clear();
        entries[0].magic[0].nValue[0]++;Check(!P0ItemMagic::Save(candidate,entries,error)&&error=="P0_IMMUTABLE_SNAPSHOT_CONFLICT","same canonical state cannot overwrite a different magic snapshot");
        candidate->Revision++;std::string failedKey;Check(P0ItemMagic::StateHash(candidate,failedKey,error),"new failed-save binding");std::string blockedMarker=root+"\\"+failedKey+".required";Check(CreateDirectoryA(blockedMarker.c_str(),0)!=0,"inject marker publication failure");Check(!P0ItemMagic::Save(candidate,entries,error),"companion save publication failure returned");Check(RemoveDirectoryA(blockedMarker.c_str())!=0,"remove isolated fault directory");Check(chars.Load("p0role00",&old[0],old.size(),&still)&&still==n&&!memcmp(&old[0],&baseline[0],n),"companion failure leaves prior CharacterStore snapshot untouched");
    }
    Check(blueCount==500&&whiteCount==40,"500 blue plus40 white exact counts");Check(ItemSet.GetItemNumber()==0&&_heapchk()==_HEAPOK,"no allocation leak, native heap OK");
    printf("P0_RESULT phase=%s pid=%lu blue=%d white=%d checks=%d failures=%d item_record_size=%u\n",writer?"write":"restart-read",GetCurrentProcessId(),blueCount,whiteCount,checks,failures,sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD));g_pPakList=0;return failures?1:0;
}

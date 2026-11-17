#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "LegacyDropItemAdapter.h"
#include "PhongThanConsumableData.h"
#include "P0ItemMagicPersistence.h"
#include "P1BlueMagicTrace.h" // Production save/load bodies now contain opt-in P1 trace calls.
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
#include <malloc.h>
#include <stdarg.h>
KPakList* g_pPakList=0;
static int checks=0,failures=0;
static void Check(bool ok,const char* name){++checks;if(!ok){++failures;printf("FAIL %s\n",name);}}
static void CoreLoginDiag(const char* format,...){va_list args;va_start(args,format);vprintf(format,args);puts("");va_end(args);}
#define DBLOADPERTIME_ITEM 10
struct FixtureSlot {int nIdx,nPlace,nX,nY;};
class KItemList {public:
    struct Chain {int size;Chain():size(0){}int GetNext(int n) const{return n<size?n+1:0;}} m_UseIdx;
    FixtureSlot m_Items[513];
    int failAfter;
    KItemList():failAfter(-1){ZeroMemory(m_Items,sizeof(m_Items));}
    int Add(int item,int place,int x,int y,bool ignored=false){if(m_UseIdx.size>=512||(failAfter>=0&&m_UseIdx.size>=failAfter))return 0;int n=++m_UseIdx.size;m_Items[n].nIdx=item;m_Items[n].nPlace=place;m_Items[n].nX=x;m_Items[n].nY=y;return n;}
    int Remove(int item){for(int i=1;i<=m_UseIdx.size;++i)if(m_Items[i].nIdx==item){m_Items[i].nIdx=0;return 1;}return 0;}
    void Clear(){for(int i=1;i<=m_UseIdx.size;++i)if(m_Items[i].nIdx)ItemSet.Remove(m_Items[i].nIdx);m_UseIdx.size=0;ZeroMemory(m_Items,sizeof(m_Items));failAfter=-1;}
};
class P0PlayerHarness {public:KItemList m_ItemList;int LoadPlayerItemList(const PHONGTHAN_CHARACTER_STATE_HEADER*,unsigned int&);int SavePlayerItemList(PHONGTHAN_CHARACTER_STATE_HEADER*,unsigned int);};
#include "FinalCopyPlayerBodies.inl"

static bool LoadItems(P0PlayerHarness& p,const PHONGTHAN_CHARACTER_STATE_HEADER* s){unsigned n=0;int result;do{result=p.LoadPlayerItemList(s,n);}while(result==0);return result==1;}
int main(int argc,char**argv){
 if(argc!=4||!SetCurrentDirectoryA(argv[2]))return 2;bool writer=!strcmp(argv[1],"write");std::string root=argv[3],error;
 SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",root.c_str());
 Check(NativeDropRollout::MainContext("E:\\X\\PhongThanRuntime-Loose\\Server","E:\\X\\PhongThanRuntime-Loose\\Server","GameServer.exe"),"exact main admission");
 Check(!NativeDropRollout::MainContext("E:\\X\\Staging\\Server","E:\\X\\Staging\\Server","GameServer.exe"),"Staging refused");
 Check(!NativeDropRollout::MainContext("E:\\X\\PhongThanRuntime-Loose\\Server","E:\\X","GameServer.exe"),"cwd mismatch refused");
 Check(!NativeDropRollout::MainContext("E:\\X\\PhongThanRuntime-Loose\\Server","E:\\X\\PhongThanRuntime-Loose\\Server","Game.exe"),"client cannot enable persistence");
 g_SetRootPath(argv[2]);g_SetFilePath("\\");KPakList packs;Check(packs.Open("\\package.ini")!=0,"Main resource package read only");g_pPakList=&packs;g_SetPakFileMode(1);bool generatorReady=ItemGen.Init()!=0;Check(generatorReady,"native generator");if(!generatorReady)return 6;ItemSet.Init();
 CPhongThanCharacterStore store;Check(store.Open((root+"\\CharacterStore").c_str()),"copied store");
 WIN32_FIND_DATAA fd;HANDLE find=FindFirstFileA((root+"\\CharacterStore\\role_*.pthc").c_str(),&fd);int roles=0,items=0;
 if(find!=INVALID_HANDLE_VALUE){do{
  std::string filename=fd.cFileName,role;for(size_t k=5;k+1<filename.size()-5;k+=2){char hex[3]={filename[k],filename[k+1],0};role+=(char)strtoul(hex,0,16);}
  std::vector<BYTE> bytes(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);PHONGTHAN_U32 size=0;
  bool loaded=store.Load(role.c_str(),&bytes[0],bytes.size(),&size);Check(loaded,"copy canonical readable");if(!loaded)continue;
  PHONGTHAN_CHARACTER_STATE_HEADER*s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];P0PlayerHarness player;
  bool ok=LoadItems(player,s);Check(ok,"copy full production LoadPlayerItemList");if(!ok){printf("COPY_BLOCKED role=%s items=%u\n",role.c_str(),s->ItemCount);player.m_ItemList.Clear();continue;}
  Check(player.m_ItemList.m_UseIdx.size==(int)s->ItemCount,"copy inventory complete");std::vector<P0ItemMagic::Entry> entries;
  for(int j=1;j<=player.m_ItemList.m_UseIdx.size;++j){P0ItemMagic::Entry e;P0ItemMagic::Capture(Item[player.m_ItemList.m_Items[j].nIdx],j-1,e);entries.push_back(e);}
  std::string golden=root+"\\"+role+".golden";
  if(writer){
   FILE*f=fopen(golden.c_str(),"wb");Check(f!=0,"copy golden");if(f){if(!entries.empty())fwrite(&entries[0],sizeof(entries[0]),entries.size(),f);fclose(f);}
   std::string oldItems((const char*)PhongThanCharacterItems(s),s->ItemCount*sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD));
   ++s->Revision;bool saved=player.SavePlayerItemList(s,bytes.size())==1;Check(saved,"copy production save companion v1");
   Check(oldItems.size()==s->ItemCount*sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD)&&!memcmp(oldItems.data(),PhongThanCharacterItems(s),oldItems.size()),"legacy to companion migration preserves canonical metadata bytes");
   if(oldItems.size()==s->ItemCount*sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD))for(size_t off=0;off<oldItems.size();++off)if((unsigned char)oldItems[off]!=((const BYTE*)PhongThanCharacterItems(s))[off])printf("METADATA_DIFF role=%s item=%u offset=%u before=%u after=%u\n",role.c_str(),off/sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD),off%sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD),(unsigned char)oldItems[off],((const BYTE*)PhongThanCharacterItems(s))[off]);
   s->StateSize=PhongThanCharacterExpectedStateSize(s);if(saved)Check(store.Save(s,s->StateSize),"copy actual canonical commit");
  }else{
   std::string previous;Check(P0ItemMagic::ReadFile(golden,previous)&&previous.size()==entries.size()*sizeof(P0ItemMagic::Entry)&&(entries.empty()||!memcmp(previous.data(),&entries[0],previous.size())),"copy magic base requirements serial quality restart exact");
   std::vector<BYTE> again=bytes;PHONGTHAN_CHARACTER_STATE_HEADER*same=(PHONGTHAN_CHARACTER_STATE_HEADER*)&again[0];
   Check(player.SavePlayerItemList(same,again.size())==1,"copy idempotent resave");Check(same->ItemCount==s->ItemCount&&!memcmp(PhongThanCharacterItems(same),PhongThanCharacterItems(s),s->ItemCount*sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD)),"copy all canonical item metadata unchanged");
  }
  items+=s->ItemCount;++roles;player.m_ItemList.Clear();
  if(!writer){
   std::string key;Check(P0ItemMagic::StateHash(s,key,error),"copy snapshot key");std::string file=root+"\\"+key+".magic-v1",held=file+".held";
   Check(MoveFileA(file.c_str(),held.c_str())!=0,"copy hide companion");Check(!LoadItems(player,s)&&ItemSet.GetItemNumber()==0,"copy missing companion fails without allocations");
   std::vector<BYTE> rejected=bytes;Check(player.SavePlayerItemList((PHONGTHAN_CHARACTER_STATE_HEADER*)&rejected[0],rejected.size())==-1,"copy failed load cannot save empty inventory");
   Check(MoveFileA(held.c_str(),file.c_str())!=0,"copy restore companion");Check(LoadItems(player,s),"copy load recovers after rollback");player.m_ItemList.Clear();
   std::string canonical=root+"\\CharacterStore\\"+fd.cFileName,before,after;Check(P0ItemMagic::ReadFile(canonical,before),"copy capture committed bytes");
   std::vector<P0ItemMagic::Entry> persisted;bool present=false;Check(P0ItemMagic::Load(s,persisted,present,error)&&present,"copy committed snapshot");
   std::vector<BYTE> next=bytes;PHONGTHAN_CHARACTER_STATE_HEADER*candidate=(PHONGTHAN_CHARACTER_STATE_HEADER*)&next[0];++candidate->Revision;
   Check(P0ItemMagic::Save(candidate,persisted,error),"copy stage companion before canonical fault");
   HANDLE lock=CreateFileA(canonical.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);Check(lock!=INVALID_HANDLE_VALUE,"copy lock canonical replace");Check(!store.Save(candidate,candidate->StateSize),"copy actual atomic save failure");if(lock!=INVALID_HANDLE_VALUE)CloseHandle(lock);
   Check(P0ItemMagic::ReadFile(canonical,after)&&before==after,"copy canonical unchanged after failed save");Check(LoadItems(player,s),"copy rollback selects old snapshot not orphan");player.m_ItemList.Clear();
  }
 }while(FindNextFileA(find,&fd));FindClose(find);}
 Check(roles==2,"both main copied roles tested");Check(ItemSet.GetItemNumber()==0&&_heapchk()==_HEAPOK,"copy native heap and cleanup");
 printf("MAIN_COPY_RESULT mode=%s pid=%lu roles=%d items=%d checks=%d failures=%d companion_version=1 MAIN_DATA_WRITES=0\n",argv[1],GetCurrentProcessId(),roles,items,checks,failures);g_pPakList=0;return failures?1:0;
}

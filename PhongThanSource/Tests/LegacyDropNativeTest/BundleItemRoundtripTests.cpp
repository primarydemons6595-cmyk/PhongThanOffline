#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "LegacyDropItemAdapter.h"
#include "PhongThanConsumableData.h"
#define PT_NATIVE_DROP_ROLLOUT_SCOPE_H
namespace NativeDropRollout {inline bool Active(){return true;}inline int Root(std::string& root,std::string& error){root="NativeDropPersistenceV1";return 1;}}
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
int main(int argc,char**argv){
 if(argc!=4||!SetCurrentDirectoryA(argv[3]))return 2;bool writer=!strcmp(argv[1],"write");std::string root=argv[3];SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",0);
 CreateDirectoryA("NativeDropPersistenceV1",0);g_SetRootPath(argv[2]);g_SetFilePath("\\");g_SetPakFileMode(1);KPakList packs;Check(packs.Open("\\package.ini")!=0,"catalogue");g_pPakList=&packs;Check(ItemGen.Init()!=0,"generator");ItemSet.Init();LegacyDropItemAdapter adapter;Check(adapter.Init(ItemGen),"adapter");CPhongThanCharacterStore chars;Check(chars.Open("CharacterStore"),"store");

 if(writer){
  for(int row=0;row<160;++row)for(int requested=0;requested<4;++requested){
   const KBASICPROP_EQUIPMENT* table=ItemGen.CatalogEquipment(13,row);Check(table!=0,"seal catalogue row");if(!table)continue;
   int id=ItemSet.Add(0,requested,table->m_nLevel,0,13,table->m_nParticularType,0,0,12345,row);Check(id>0,"all seal rows generate");if(!id)continue;
   int expected=requested;for(int r=0;r<6;++r)if(table->m_aryPropReq[r].nType==magic_requireseries)expected=table->m_aryPropReq[r].nPara;
   Check(Item[id].GetSeries()==expected && Item[id].GetSeries()<=255,"seal fits real sender Series guard and required profession");Check(Item[id].GetDetailType()==13 && Item[id].GetRow()==row,"seal snapshot identity");ItemSet.Remove(id);
  }
  for(row=0;row<=99;row+=99){const KBASICPROP_EQUIPMENT* table=ItemGen.CatalogEquipment(13,row);KItem repaired;ZeroMemory(&repaired,sizeof(repaired));repaired.m_GeneratorParam.uRandomSeed=12345;
   Check(ItemGen.Gen_ExistEquipmentByTemplateRow(13,row,table->m_nSeries,0,0,1,&repaired)!=0,"legacy 3000/4000 seal regenerates");Check(repaired.GetSeries()==(row==99?3:series_nil),"existing invisible seals get valid wire series on relog");
  }
 }
 std::vector<BYTE> bytes;P0PlayerHarness player;
 if(!strcmp(argv[1],"repair")){
  std::string path="CharacterStore\\123456\\role_7175616e6768757933.pthc";PtCharacterBundle::Bundle b;Check(PtCharacterBundle::Read(path,b),"read two real legacy seals");
  bytes.assign((const BYTE*)b.state.data(),(const BYTE*)b.state.data()+b.state.size());PHONGTHAN_CHARACTER_STATE_HEADER* state=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];Check(LoadItems(player,state),"actual production loader repairs legacy seal series");Check(player.m_ItemList.m_UseIdx.size==2,"both legacy seals restored");
  for(int k=1;k<=player.m_ItemList.m_UseIdx.size;++k){KItem& item=Item[player.m_ItemList.m_Items[k].nIdx];P0ItemMagic::Entry actual;P0ItemMagic::Capture(item,k-1,actual);Check(!memcmp(&actual,b.magic.data()+144+(k-1)*sizeof(actual),sizeof(actual)),"legacy seal serial/quality/base/magic/requirements byte exact");Check(item.GetSeries()==(item.GetRow()==99?3:series_nil),"legacy seal now passes wire series guard");}
  ++state->Revision;Check(player.SavePlayerItemList(state,bytes.size())==1 && chars.Save(state,state->StateSize),"repaired seals save into same PTHC");PtCharacterBundle::Bundle saved;Check(PtCharacterBundle::Read(path,saved)&&saved.pending.empty(),"repair committed with no pending generation");Check(saved.magic.substr(144)==b.magic.substr(144),"item attributes remain identical after repaired save");player.m_ItemList.Clear();printf("LEGACY_SEAL_REPAIR checks=%d failures=%d exact_items=2\n",checks,failures);g_pPakList=0;return failures?1:0;
 }
 for(int role=0;role<5;++role){InitState(bytes,role);PHONGTHAN_CHARACTER_STATE_HEADER* state=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];
  if(writer){state->StateSize=sizeof(*state);Check(chars.Create(state,state->StateSize),"create before first capture");state->Revision=2;
   for(int i=0;i<104;++i){PhongThanQuestItemTuple tuple;Check(adapter.Map(0,6,0,tuple),"identity");tuple.level=1+i%10;int index=0;std::string error;Check(adapter.GenerateMapped(tuple,index,error),"native item");if(!index)continue;
    if(i<100){KItemNormalAttrib attrs[MAX_ITEM_MAGICATTRIB];ZeroMemory(attrs,sizeof(attrs));for(int a=0;a<1+i%6;++a){attrs[a].nAttribType=85+a;attrs[a].nValue[0]=10+i+a;attrs[a].nValue[1]=-1-a;attrs[a].nValue[2]=i*3+a;}Check(Item[index].SetMagicAttrib(attrs)!=0,"blue attrs");}
    Item[index].SetDurability(5+i%10);player.m_ItemList.Add(index,pos_equiproom,i%10,i/10);
   }
   int seal=ItemSet.Add(0,0,1,0,13,0,0,0,12345,0);Check(seal>0,"native seal generation");if(seal)player.m_ItemList.Add(seal,pos_equip,11,0);
   Check(player.SavePlayerItemList(state,bytes.size())==1,"production SavePlayerItemList including normalized StateSize");state->StateSize=PhongThanCharacterExpectedStateSize(state);Check(chars.Save(state,state->StateSize),"commit actual .pthc");
   std::vector<P0ItemMagic::Entry> expected;for(int k=1;k<=player.m_ItemList.m_UseIdx.size;++k){P0ItemMagic::Entry e;P0ItemMagic::Capture(Item[player.m_ItemList.m_Items[k].nIdx],k-1,e);expected.push_back(e);}Golden(root,role,expected,true);player.m_ItemList.Clear();
  }else{char name[32];sprintf(name,"p0role%02d",role);PHONGTHAN_U32 size=0;Check(chars.Load(name,&bytes[0],bytes.size(),&size),"fresh process canonical load");state=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];}
  Check(LoadItems(player,state),"production item load from embedded attributes");Check(player.m_ItemList.m_UseIdx.size==105,"exact item count");
  std::vector<P0ItemMagic::Entry> actual;for(int k=1;k<=player.m_ItemList.m_UseIdx.size;++k){P0ItemMagic::Entry e;P0ItemMagic::Capture(Item[player.m_ItemList.m_Items[k].nIdx],k-1,e);actual.push_back(e);}Golden(root,role,actual,false);
  if(player.m_ItemList.m_UseIdx.size==105){KItem& seal=Item[player.m_ItemList.m_Items[105].nIdx];Check(seal.GetDetailType()==13,"seal survives restart in F4 slot 11");}
  player.m_ItemList.Clear();
 }
 Check(ItemSet.GetItemNumber()==0&&_heapchk()==_HEAPOK,"native allocations/heap");printf("BUNDLE_ITEM_RESULT phase=%s checks=%d failures=%d blue=500 white=20 seals=5 actual_item_save_load=1\n",argv[1],checks,failures);g_pPakList=0;return failures?1:0;
}

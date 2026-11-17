#ifdef _SERVER
#include "../../../Headers/PhongThanRank.h"
#include "../../../Headers/PhongThanCharacter.h"
#include <stdlib.h>
#include <vector>
#include <string>
#ifndef PT_RANK_FILE_TEST
#include "KItemGenerator.h"
#include "P0ItemMagicPersistence.h"
struct PTRankRandomScope {UINT Seed;PTRankRandomScope():Seed(g_GetRandomSeed()){}~PTRankRandomScope(){g_RandomSeed(Seed);}};
static int PTRankEquipmentPower(const PHONGTHAN_CHARACTER_STATE_HEADER* state){
 PTRankRandomScope rng;std::vector<P0ItemMagic::Entry> magic;bool present=false;std::string error;
 if(!P0ItemMagic::Load(state,magic,present,error))return -1;
 const PHONGTHAN_CHARACTER_ITEM_RECORD* items=PhongThanCharacterItems(state);int total=0;
 for(unsigned int i=0;i<state->ItemCount;++i){const PHONGTHAN_CHARACTER_ITEM_RECORD& d=items[i];
  if(d.Genre!=item_equip||d.Container!=pos_equip||(d.ExpireTime&&d.ExpireTime<=(unsigned int)KSG_GetCurSec()))continue;
  KItem item;ZeroMemory(&item,sizeof(item));item.m_GeneratorParam.uRandomSeed=d.RandomSeed;
  memcpy(item.m_GeneratorParam.nGeneratorLevel,d.GeneratorLevel,sizeof(item.m_GeneratorParam.nGeneratorLevel));
  if(!ItemGen.Gen_ExistEquipmentByTemplateRow(d.DetailType,d.TemplateRow,d.Series,item.m_GeneratorParam.nGeneratorLevel,d.Luck,d.TableVersion,&item))return -1;
  item.SetParam(d.ScriptParam);item.SetFortune(d.Fortune);item.SetDurability(d.Durability);
  if(!item.ApplyUpgradeState(d.UpgradeLevel))return -1;
  item.m_CommonAttrib.nPhysicVal=d.PhysicalValue;item.m_CommonAttrib.nMagicVal=d.MagicValue;
  if(present&&!P0ItemMagic::Restore(item,magic[i],error))return -1;
  int power=0;if(!item.GetEquipmentPower(power)||power>MAX_INT-total)return -1;total+=power;
 }return total;
}
struct PTRankPowerCache {DWORD Hash,Checked;int Power;bool Initialized;PTRankPowerCache():Hash(0),Checked(0),Power(-1),Initialized(false){}};
static std::map<std::string,PTRankPowerCache> ptRankPowerCache;
#endif
#pragma pack(push,1)
struct PTRankFileHeader {DWORD Magic;WORD Version,HeaderSize;DWORD StateSize,Checksum;};
#pragma pack(pop)
static int PTRankTask(const PHONGTHAN_CHARACTER_STATE_HEADER* state,int id){
 const PHONGTHAN_CHARACTER_TASK_RECORD* tasks=PhongThanCharacterTasks(state);
 for(unsigned int i=0;i<state->TaskCount;++i)if(tasks[i].TaskId==id){char value[17];memcpy(value,tasks[i].Value,16);value[16]=0;return atoi(value);}return 0;
}
static bool PTReadRankRole(const char* path,PTRankEntry& role) {
    HANDLE file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
    if(file==INVALID_HANDLE_VALUE)return false;
    PTRankFileHeader header;DWORD got=0;bool ok=false;
    if(ReadFile(file,&header,sizeof(header),&got,0) && got==sizeof(header) && header.Magic==0x43544850 &&
       (header.Version==1||header.Version==2) && header.HeaderSize==sizeof(header) && header.StateSize>=sizeof(PHONGTHAN_CHARACTER_STATE_HEADER) &&
       header.StateSize<=PHONGTHAN_CHARACTER_MAX_STATE_SIZE) {
        BYTE bytes[PHONGTHAN_CHARACTER_MAX_STATE_SIZE];
        if(ReadFile(file,bytes,header.StateSize,&got,0) && got==header.StateSize) {
            DWORD hash=2166136261U;for(DWORD i=0;i<got;++i)hash=(hash^bytes[i])*16777619U;
            PHONGTHAN_CHARACTER_STATE_HEADER* state=(PHONGTHAN_CHARACTER_STATE_HEADER*)bytes;
            if(hash==header.Checksum && PhongThanValidateCharacterState(state,got)) {
                memset(&role,0,sizeof(role));memcpy(role.Name,state->RoleName,sizeof(role.Name));
                role.Level=state->FightLevel;role.Experience=state->FightExperience;role.Transcendence=state->Transcendence;
                role.Profession=state->Profession;role.Money=(__int64)state->Money+state->BankMoney;
                role.Repute=PTRankTask(state,TASKVALUE_PT_CREDIT);role.Justice=PTRankTask(state,TASKVALUE_PT_JUSTICE_EVIL);
                role.Fortune=state->StatTask[1];role.Honor=PTRankTask(state,TASKVALUE_PT_HELP_SCORE);role.Pk=state->PkValue;
                memcpy(role.Nation,state->ClanName,32);role.Nation[31]=0;role.Power=-1;
#ifndef PT_RANK_FILE_TEST
                PTRankPowerCache& cached=ptRankPowerCache[path];
                if(!cached.Initialized||cached.Hash!=hash||GetTickCount()-cached.Checked>300000){cached.Power=PTRankEquipmentPower(state);cached.Hash=hash;cached.Checked=GetTickCount();cached.Initialized=true;}role.Power=cached.Power;
#endif
                ok=true;
            }
        }
    }
    CloseHandle(file);return ok;
}
#ifndef PT_RANK_FILE_TEST
static std::vector<PTRankEntry> ptRankingSnapshot;
static void PhongThanRankTick() {
    static DWORD last=0;DWORD now=GetTickCount();if(last && now-last<30000)return;last=now;PTPerfScope rankPerformance(PTPERF_RANKING);
    enum {capacity=50000};static PTRankEntry roles[capacity];int count=0;bool complete=true;
    std::vector<std::string> roots;roots.push_back("CharacterStore");
    WIN32_FIND_DATAA directory;HANDLE dirs=FindFirstFileA("CharacterStore\\*",&directory);
    if(dirs!=INVALID_HANDLE_VALUE){do{if((directory.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)&&!(directory.dwFileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)&&directory.cFileName[0]!='.')roots.push_back(std::string("CharacterStore\\")+directory.cFileName);}while(FindNextFileA(dirs,&directory));FindClose(dirs);}
    for(size_t root=0;root<roots.size()&&complete;++root){WIN32_FIND_DATAA entry;HANDLE search=FindFirstFileA((roots[root]+"\\role_*.pthc").c_str(),&entry);if(search==INVALID_HANDLE_VALUE)continue;
    do{if(entry.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)continue;PTRankEntry role;
        if(!PTReadRankRole((roots[root]+"\\"+entry.cFileName).c_str(),role)){complete=false;break;}
        int index=-1;for(int i=0;i<count;++i)if(!_stricmp(roles[i].Name,role.Name)){index=i;break;}
        if(index>=0)roles[index]=role;else{if(count==capacity){complete=false;break;}roles[count++]=role;}
    }while(FindNextFileA(search,&entry));FindClose(search);}
    if(!complete)return;
    // The persisted snapshot can lag behind online experience. Overlay live roles.
    for(int player=1;player<MAX_PLAYER;++player) {
        int actor=Player[player].m_nIndex;
        if(actor<=0 || actor>=MAX_NPC || Player[player].m_nNetConnectIdx<0 || !Player[player].m_PlayerDBLoad)continue;
        int index=-1;for(int i=0;i<count;++i)if(!_stricmp(roles[i].Name,Npc[actor].Name)){index=i;break;}
        if(index<0){if(count==capacity)return;index=count++;}
        g_StrCpyLen(roles[index].Name,Npc[actor].Name,sizeof(roles[index].Name));roles[index].Level=Npc[actor].m_Level;
        roles[index].Transcendence=Npc[actor].m_byTranslife;roles[index].Experience=Player[player].m_nExp;
        roles[index].Profession=Player[player].m_cProfession.GetProfession();
        roles[index].Money=(__int64)Player[player].m_ItemList.GetEquipmentMoney()+Player[player].m_ItemList.GetRepositoryMoney();
        roles[index].Repute=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT);
        roles[index].Justice=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_JUSTICE_EVIL);
        roles[index].Fortune=Player[player].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN);
        roles[index].Honor=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_HELP_SCORE);
        roles[index].Pk=Player[player].m_cPK.GetPKValue();
        g_StrCpyLen(roles[index].Nation,Player[player].m_cTong.m_szName,32);
        roles[index].Power=0;
        for(int part=0;part<itempart_num;++part){int item=Player[player].m_ItemList.GetEquipment(part),power=0;
            if(item>0&&item<MAX_ITEM&&Item[item].GetEquipmentPower(power))roles[index].Power=min(MAX_INT-power,roles[index].Power)+power;}
    }
    qsort(roles,count,sizeof(roles[0]),PTRankCompare);
    ptRankingSnapshot.assign(roles,roles+count);
    for(player=1;player<MAX_PLAYER;++player) {
        int actor=Player[player].m_nIndex;
        if(actor<=0 || actor>=MAX_NPC || Player[player].m_nNetConnectIdx<0 || !Player[player].m_PlayerDBLoad)continue;
        for(int rank=0;rank<count;++rank)if(!_stricmp(roles[rank].Name,Npc[actor].Name)) {
            Player[player].m_nWorldStat=rank+1;
            if(Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_UI_WORLD_RANK)!=rank+1)
                Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_UI_WORLD_RANK,rank+1,TRUE);
            break;
        }
    }
}
// Each original category uses its own population and sort key. Unsupported
// cross-server statistics stay explicit instead of showing another ranking.
static int ptRankingCategory=0;
static int PTRankingMetric(int category){
 if(category==1||category==4||category==6||category==8||category==40)return 1;
 if(category==19)return 2;
 if(category==24||category==25)return 6;
 if(category==22)return 3;
 if(category==23)return 4;
 if(category>=35&&category<=38||category==43)return 5;
 return 0;
}
static int PTRankingSort(const void* left,const void* right){
 const PTRankEntry& a=*(const PTRankEntry*)left;const PTRankEntry& b=*(const PTRankEntry*)right;
 __int64 av=0,bv=0;
 switch(PTRankingMetric(ptRankingCategory)){
 case 1:av=a.Money;bv=b.Money;break;case 2:av=a.Repute;bv=b.Repute;break;
 case 3:av=a.Pk;bv=b.Pk;break;case 4:av=a.Honor;bv=b.Honor;break;
 case 5:av=a.Power;bv=b.Power;break;case 6:av=a.Justice;bv=b.Justice;if(av<0)av=-av;if(bv<0)bv=-bv;break;default:return PTRankCompare(left,right);}
 if(av!=bv)return av>bv?-1:1;return PTRankCompare(left,right);
}
bool PhongThanRankingPage(int category,int page,PTRankingPage& result){
 if(!PTRankingValidQuery(category,page))return false;
 PhongThanRankTick();result.Count=0;result.Total=0;result.Result=1;
 if(category==10){if(!PhongThanNationWealthRank(&result))result.Result=-1;return true;}
 int profession=-1;
 if(category==3||category==4||category==36)profession=0;
 if(category==5||category==6||category==37)profession=1;
 if(category==7||category==8||category==38)profession=2;
 if(category==39||category==40||category==43)profession=3;
 if(category==13||category==14)profession=0;if(category==15||category==16)profession=1;if(category==17||category==18)profession=2;
 if(category==41||category==42)profession=3;
 if(!(category<=8||category>=11&&category<=19||category>=22&&category<=25||category>=35&&category<=43)){result.Result=-2;return true;}
 std::vector<PTRankEntry> selected;
 for(size_t i=0;i<ptRankingSnapshot.size();++i){const PTRankEntry& r=ptRankingSnapshot[i];
  if(profession>=0&&r.Profession!=profession)continue;
  if(PTRankingMetric(category)==5&&r.Power<0)continue;
  if((category==11||category==13||category==15||category==17||category==24||category==41)&&r.Justice<=0)continue;
  if((category==12||category==14||category==16||category==18||category==25||category==42)&&r.Justice>=0)continue;
  // Human and celestial level rankings must not contain each other's roles.
  if(category==0&&r.Transcendence!=0||category==2&&r.Transcendence==0)continue;
  selected.push_back(r);}
 ptRankingCategory=category;if(!selected.empty())qsort(&selected[0],selected.size(),sizeof(PTRankEntry),PTRankingSort);
 result.Total=selected.size();
 if(category>=3&&category<=8||category>=11&&category<=18||category>=39&&category<=42)result.Total=min(10,result.Total);
 int start=page*PT_RANK_ROWS;
 for(int n=start;n<result.Total&&result.Count<PT_RANK_ROWS;++n)result.Rows[result.Count++]=selected[n];
 return true;
}
#endif
#endif

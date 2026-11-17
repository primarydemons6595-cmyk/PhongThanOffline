#include <windows.h>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <string.h>
enum {MAX_ITEM=20,item_equip=0,magic_requireseries=37,magic_requiremenpai=39,pos_equip=2,pos_hand=1};
struct KMagicAttrib{int nAttribType,nValue[3];};struct PlayerItem{int nIdx,nPlace;};
enum {Capacity=1};
struct KItem{int id,genre,profession,temporary,expiry;KMagicAttrib req;int GetGenre(){return genre;}DWORD GetID(){return id;}int IsTemp(){return temporary;}DWORD GetExpireTime(){return expiry;}int GetDetailType(){return id;}int GetParticular(){return 310;}int GetPhongThanQuality(){return 4;}int GetLevel(){return 2;}int GetUpgradeState(){return 0;}
 KMagicAttrib* GetRequirement(int at){if(at)return 0;req.nAttribType=id==2?magic_requiremenpai:magic_requireseries;req.nValue[0]=profession;return &req;}
}Item[MAX_ITEM];
struct Bag{PlayerItem entries[12];int count,at,hand;int Hand(){return hand;}PlayerItem* GetFirstItem(){at=0;return count?&entries[0]:0;}PlayerItem* GetNextItem(){return ++at<count?&entries[at]:0;}int Remove(int n){return Item[n].id!=0;}};
struct Profession{int value;int GetProfession(){return value;}};
struct KPlayer{bool m_bIsBot;Bag m_ItemList;Profession m_cProfession;int saves;int CheckTrading(){return 0;}int GetLockState(){return 0;}bool Save(){++saves;return true;}}Player[2];
struct ItemSetStub{void Remove(int n){Item[n].id=0;}}ItemSet;
struct Runtime{int Player;struct Config{char Id[33];}Config;char Error[128];};
static Runtime bots[Capacity];
static bool SocialAlive(int){return true;}static bool BotMarketWorthy(KItem&){return false;}static bool BotMarketReserved(int,int i){return i==4;}static int BotStoryRequiredCount(KPlayer&,KItem& item){return item.id==5;}
#include "../../Sources/Core/Src/PhongThanBotWarriorCleanup.inl"
int main(int argc,char**argv){if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;CreateDirectoryA("BotAIgent",0);DeleteFileA("BotAIgent\\maintenance.ini");DeleteFileA("BotAIgent\\warrior-cleanup.tsv");
 Runtime& r=bots[0];memset(&r,0,sizeof(r));r.Player=1;strcpy(r.Config.Id,"TESTBOT");KPlayer& p=Player[1];p.m_bIsBot=true;p.m_ItemList.count=10;
 for(int i=1;i<=10;++i){Item[i].id=i;Item[i].profession=3;p.m_ItemList.entries[i-1].nIdx=i;p.m_ItemList.entries[i-1].nPlace=3;}
 p.m_ItemList.entries[1].nPlace=5;p.m_ItemList.entries[2].nPlace=pos_equip;Item[3].profession=0;Item[9].profession=0;Item[6].profession=0;Item[7].temporary=1;Item[8].expiry=10;p.m_ItemList.entries[8].nPlace=pos_hand;Item[10].genre=8;
 BotCleanupWarriorLoot(r);if(!Item[1].id||p.saves)return 3;
 if(!WritePrivateProfileStringA("Cleanup","WarriorLoot","1","BotAIgent\\maintenance.ini"))return 4;
 BotCleanupWarriorLoot(r);if(!Item[1].id||p.saves)return 11;
 WritePrivateProfileStringA("Cleanup","ApplyWarriorLoot","1","BotAIgent\\maintenance.ini");
 p.m_bIsBot=false;BotCleanupWarriorLoot(r);if(!Item[1].id||p.saves)return 5;p.m_bIsBot=true;
 p.m_ItemList.hand=9;BotCleanupWarriorLoot(r);if(Item[1].id||Item[2].id||!Item[9].id||p.m_ItemList.hand!=9)return 6;p.m_ItemList.hand=0;
 BotCleanupWarriorLoot(r);if(Item[1].id||Item[2].id||p.saves!=2)return 7;
 for(int safe=3;safe<=10;++safe)if(!Item[safe].id)return 8;
 Item[1].id=11;BotCleanupWarriorLoot(r);if(!Item[1].id||p.saves!=2)return 9;
 if(GetPrivateProfileIntA("Completed","TESTBOT",0,"BotAIgent\\maintenance.ini")!=1)return 10;
 puts("PASS production bot cleanup: explicit request, live bot only, bag/storage targets; protects worn/hand/reserved/quest/usable/temporary/expired, receipt prevents replay");return 0;
}

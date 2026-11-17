#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define PT_TWO_FACTION_TEST
enum {MAX_PLAYER=3,MAX_NPC=8,MAX_ITEM=6,MAX_SUBWORLD=2,kind_normal=0,item_task=4,item_magicscript=6,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=18,pos_ptbag3=19};
struct Task {int v[5000];int GetSaveVal(int n){return v[n];}void SetSaveVal(int n,int val,int){v[n]=val;}};
struct Actor {DWORD m_dwID;int m_Kind,m_CurrentLife,m_NpcSettingIdx,m_SubWorldIndex;} Npc[MAX_NPC];
struct World {int m_SubWorldID;} SubWorld[MAX_SUBWORLD];
struct It {int genre,detail,count;int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetStackNum(){return count;}} Item[MAX_ITEM];
struct PlayerItem {int nIdx,nPlace;};struct Bag {PlayerItem items[MAX_ITEM];int cursor;PlayerItem* GetFirstItem(){cursor=0;return GetNextItem();}PlayerItem* GetNextItem(){while(++cursor<MAX_ITEM)if(items[cursor].nIdx)return items+cursor;return 0;}};
struct Profession {int type;int GetProfession(){return type;}};
struct Owner {DWORD m_dwID;int m_nIndex,m_PlayerDBLoad;Task m_cTask;Profession m_cProfession;Bag m_ItemList;bool CheckTrading(){return false;}int GetLockState(){return 0;}} Player[MAX_PLAYER];
struct PhongThanQuestItemTuple {int genre,detail;};struct PhongThanQuestExchangeRow {PhongThanQuestItemTuple item;int count;};
bool room=true;int awards=0;
bool PhongThanDecodeQuestItemTuple(int genre,int detail,int particular,int,int,int,PhongThanQuestItemTuple& t){t.genre=genre;t.detail=genre==6?particular:detail;return true;}
int PhongThanQuestExchange(int p,int t,int from,int to,void*,int,PhongThanQuestExchangeRow* gift,int gc,bool,int,bool){if(!room||Player[p].m_cTask.v[t]!=from)return 0;if(gc){Item[1].genre=gift[0].item.genre;Item[1].detail=gift[0].item.detail;Item[1].count+=gift[0].count;Player[p].m_ItemList.items[1].nIdx=1;Player[p].m_ItemList.items[1].nPlace=pos_equiproom;++awards;}Player[p].m_cTask.v[t]=to;return 1;}
#include "../../Sources/Core/Src/PhongThanTwoFaction.inl"
static DWORD serial=100;
static void Reset(int f){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));ZeroMemory(SubWorld,sizeof(SubWorld));PtTwoFaction::seen[1]=PtTwoFaction::Seen();PtTwoFaction::hunts[1]=PtTwoFaction::Hunt();room=true;awards=0;
 Player[1].m_dwID=77;Player[1].m_nIndex=1;Player[1].m_PlayerDBLoad=1;Player[1].m_cProfession.type=f;Npc[1].m_dwID=88;Npc[1].m_CurrentLife=100;Npc[2].m_dwID=++serial;}
static void Dead(int tid,int map){Npc[2].m_dwID=++serial;Npc[2].m_NpcSettingIdx=tid;Npc[2].m_CurrentLife=0;Npc[2].m_Kind=kind_normal;SubWorld[0].m_SubWorldID=map;}
int main(){using namespace PtTwoFaction;
 for(int prof=1;prof<=2;++prof){Reset(prof);Set(1,4550,5);Dead(18,1014);Kill(2,1);assert(awards==0);Dead(12,1014);room=false;Kill(2,1);assert(awards==0);room=true;Dead(12,1014);Kill(2,1);assert(CountGenre(1,277,6)==1&&Get(1,4550)==5);Dead(12,1014);Kill(2,1);assert(awards==1);}
 for(int i=0;i<PT_TWO_KILL_COUNT;++i)for(int f=1;f<=2;++f){const int* rule=PT_TWO_KILLS[i];if(rule[0]&&rule[0]!=f)continue;int task=rule[3]?rule[3]:(f==1?1:2);
  Reset(f);Player[1].m_cTask.v[task]=rule[4];Dead(rule[1],rule[2]+1);Kill(2,1);assert(Get(1,task)==rule[4]&&awards==0);
  Dead(rule[1],rule[2]);room=false;Kill(2,1);assert(Get(1,task)==rule[4]&&awards==0);room=true;Dead(rule[1],rule[2]);Kill(2,1);assert(Get(1,task)==rule[5]&&awards==(rule[6]>=0?1:0));
  int before=awards;Kill(2,1);assert(awards==before);
 }
 Reset(2);Set(1,35,3);for(int piece=0;piece<3;++piece){Dead(105,1012);Kill(2,1);assert(Get(1,35)==piece+4&&Count(1,28)==piece+1);Kill(2,1);assert(Count(1,28)==piece+1);}assert(Get(1,35)==6);
 for(int f=1;f<=2;++f){Reset(f);Set(1,4554,f==1?1:2);for(int n=0;n<45;++n){Dead(f==1?1:2,f==1?1008:1011);Kill(2,1);Kill(2,1);}assert(Get(1,4555)==40);}
 Reset(1);Set(1,WolfState,1);for(int king=0;king<3;++king){for(int wolf=0;wolf<22;++wolf){Dead(4,1009);Kill(2,1);}assert(Get(1,WolfCount)==20);
  Dead(606,1009);Kill(2,1);assert(Get(1,WolfKings)==king);Hunt& h=hunts[1];h.owner=77;h.actor=88;h.npc=2;h.entity=Npc[2].m_dwID;Dead(606,1009);h.entity=Npc[2].m_dwID;Kill(2,1);assert(Get(1,WolfKings)==king+1&&Get(1,WolfCount)==0);Kill(2,1);assert(Get(1,WolfKings)==king+1);
 }assert(Get(1,WolfState)==2);
 for(f=1;f<=2;++f){Reset(f);int task=f==1?1:2;Set(1,task,87);Dead(541,1070);Kill(2,1);Kill(2,1);assert(Get(1,4562)==1&&Get(1,task)==87);Dead(543,1070);Kill(2,1);assert(Get(1,4562)==5);Dead(542,1070);Kill(2,1);assert(Get(1,4562)==7&&Get(1,task)==88);
  Set(1,4563,(int)time(NULL)-1);Dead(2703,1070);Kill(2,1);assert(Get(1,task)==88);Set(1,4563,(int)time(NULL)+180);Dead(2703,1070);room=false;Kill(2,1);assert(Get(1,task)==88);room=true;Dead(2703,1070);Kill(2,1);assert(Get(1,task)==89&&Count(1,174)==1);Kill(2,1);assert(Count(1,174)==1);
 }
 puts("PASS production two-faction kill rules, class/map guards, bag failure, distinct fragments, local Giao Huan cap, 20 wolves per owned king, 3 kings, three story souls/timed boss and duplicate deaths");return 0;
}

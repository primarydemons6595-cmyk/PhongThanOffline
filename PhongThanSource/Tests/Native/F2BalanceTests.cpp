#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanF2Balance.h"
enum{CLIENT_PLAYER_INDEX=1,MAX_PLAYER=2,MAX_ITEM=12,item_task=4,room_equipment=0,pos_equiproom=1,pos_equiproomex=2,pos_ptbag2=3,pos_ptbag3=4,TASKVALUE_PT_BOUND_MONEY=4847};
struct PlayerItem{int nIdx,nPlace;};
struct Bag{PlayerItem slots[8];int at;PlayerItem* GetFirstItem(){at=0;return slots;}PlayerItem* GetNextItem(){return ++at<8?slots+at:0;}int GetMoney(int){return 19996450;}};
struct Task{int GetSaveVal(int task){assert(task==4847);return 55;}};
struct KPlayer{Bag m_ItemList;Task m_cTask;}Player[2];
struct Lock{int value;int IsLock(){return value;}};
struct ItemStub{int id,genre,detail,particular,level,series,count;Lock lock;int GetID(){return id;}int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetParticular(){return particular;}int GetLevel(){return level;}int GetSeries(){return series;}int GetStackNum(){return count;}Lock* GetLock(){return &lock;}}Item[MAX_ITEM];
#include "../../Sources/Core/Src/PhongThanF2Balance.inl"
int PhongThanF2BalanceValue(const PTF2BalanceQuery& q){return PhongThanF2BalanceValue(1,q);}
int main(){
 for(int i=1;i<=8;++i){Player[1].m_ItemList.slots[i-1].nIdx=i;Player[1].m_ItemList.slots[i-1].nPlace=i<=4?i:1;Item[i].id=i;Item[i].genre=4;Item[i].detail=47;Item[i].series=5;Item[i].count=i;}
 Item[5].lock.value=1;Item[6].detail=48;Player[1].m_ItemList.slots[6].nPlace=20;Item[8].id=0;
 PTF2BalanceQuery q={0,0,0,0,0,0};assert(PhongThanF2BalanceValue(q)==10);q.Type=1;assert(PhongThanF2BalanceValue(q)==19996450);q.Type=3;assert(PhongThanF2BalanceValue(q)==55);
 q.Type=2;q.Genre=4;q.Detail=47;assert(PhongThanF2BalanceValue(q)==10);q.Detail=48;assert(PhongThanF2BalanceValue(q)==6);
 q.Genre=0;q.Detail=1;q.Level=40;q.Series=2;Item[1].genre=0;Item[1].detail=1;Item[1].level=40;Item[1].series=2;assert(PhongThanF2BalanceValue(q)==1);q.Series=3;assert(PhongThanF2BalanceValue(q)==0);
 q.Type=0;Item[1].genre=4;Item[1].detail=47;Item[1].count=2147483647;assert(PhongThanF2BalanceValue(q)==2147483647);
 q.Type=99;assert(PhongThanF2BalanceValue(q)==0);puts("PASS actual F2 balance policy: all four bags, physical QuestKey47 Xu, silver, points, exact item identity, native fixed-series normalization, equipment series, lock/storage/invalid filters, overflow");return 0;
}

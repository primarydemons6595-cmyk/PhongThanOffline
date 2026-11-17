#include <stdio.h>
#include <string.h>
enum {MAX_PLAYER=3,TASKVALUE_PT_BAG_ROWS_1=20};
struct Tuple {int genre,detail,particular;};
struct PhongThanQuestExchangeRow {Tuple item;int count;};
struct Entry {Tuple item;int room,count;} entries[12];
static int rows[3][3],locked,trading,decode=1,calls;
static bool PhongThanDecodeQuestItemTuple(int g,int d,int p,int,int,int,Tuple& t){t.genre=g;t.detail=d;t.particular=p;return decode!=0;}
static int PhongThanQuestExchange(int player,int task,int expected,int next,PhongThanQuestExchangeRow* needs,int needCount,void*,int,bool=false,void* prototype=0,bool carried=false){
 ++calls;if(locked||trading||rows[player][task-20]!=expected||needCount!=1)return 0;
 for(int n=0;n<12;++n){Entry& e=entries[n];bool currency=needs[0].item.genre==4&&needs[0].item.detail==47;
  if(e.room!=0&&!(e.room>=1&&e.room<=3&&(carried||currency))&&!(e.room==4&&carried))continue;
  if(!memcmp(&e.item,&needs[0].item,sizeof(Tuple))&&e.count>=needs[0].count){e.count-=needs[0].count;rows[player][task-20]=next;return 1;}
 }return 0;
}
#include "bag-unlock-production.inl"
static int checks;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d\n",__LINE__);return 1;}}while(0)
static void Reset(int row=4){memset(entries,0,sizeof(entries));memset(rows,0,sizeof(rows));rows[1][0]=row;locked=trading=calls=0;decode=1;}
static void Add(int slot,int genre,int detail,int particular,int room,int count){entries[slot].item.genre=genre;entries[slot].item.detail=detail;entries[slot].item.particular=particular;entries[slot].room=room;entries[slot].count=count;}
int main(){
 for(int room=0;room<=4;++room){Reset();Add(0,8,270,2,room,1);Add(1,4,47,0,0,500);CHECK(PhongThanPayBagUnlock(1,1,4)==1);CHECK(entries[0].count==0&&entries[1].count==500&&rows[1][0]==5&&calls==1);}
 Reset();Add(0,8,270,2,10,1);CHECK(!PhongThanPayBagUnlock(1,1,4));CHECK(entries[0].count==1&&rows[1][0]==4);
 Reset();Add(0,4,47,0,3,128);CHECK(PhongThanPayBagUnlock(1,1,4)==1);CHECK(entries[0].count==0&&rows[1][0]==5);
 Reset();Add(0,4,47,0,0,127);CHECK(!PhongThanPayBagUnlock(1,1,4));CHECK(entries[0].count==127&&rows[1][0]==4);
 Reset(3);Add(0,8,269,2,2,1);CHECK(PhongThanPayBagUnlock(1,1,3)==1&&rows[1][0]==4);
 Reset();Add(0,8,270,2,0,1);locked=1;CHECK(!PhongThanPayBagUnlock(1,1,4)&&entries[0].count==1);
 locked=0;trading=1;CHECK(!PhongThanPayBagUnlock(1,1,4)&&entries[0].count==1);
 trading=0;CHECK(!PhongThanPayBagUnlock(1,1,3)&&entries[0].count==1); // stale row
 CHECK(!PhongThanPayBagUnlock(1,0,4));CHECK(!PhongThanPayBagUnlock(1,1,7));
 printf("PASS bag unlock payment: %d checks\n",checks);return 0;
}

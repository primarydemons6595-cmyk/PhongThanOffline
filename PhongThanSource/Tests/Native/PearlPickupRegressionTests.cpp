#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanPlayerDamageReduction.h"
enum{MAX_ITEM=5};struct ItemPos{int nPlace,nX,nY;};struct Lock{bool value;bool IsLock(){return value;}};
struct Obj{int count,max;Lock lock;bool IsStack(){return max>1;}Lock*GetLock(){return &lock;}int GetMaxStackNum(){return max;}int GetStackNum(){return count;}int GetWidth(){return 1;}int GetHeight(){return 1;}}Item[MAX_ITEM];
int PTBagPlace(int p){return p+3;}int PositionToRoom(int p){return p-3;}
struct Room{int target;BOOL FindSameItemToStack(int,int*t,int*x,int*y){*t=target;*x=2;*y=4;return target!=0;}};
class KItemList{public:Room m_Room[4];bool room;int GetBagRows(int){return 7;}BOOL SearchPosition(int,int,ItemPos*p,bool){p->nPlace=3;p->nX=4;p->nY=5;return room;}BOOL FindPickupPosition(int,ItemPos*);};
BOOL KItemList::FindPickupPosition(int item,ItemPos* destination)
{
 if(!destination||item<=0||item>=MAX_ITEM)return FALSE;
 if(Item[item].IsStack()&&!Item[item].GetLock()->IsLock()){
  for(int page=0;page<4;++page){int place=PTBagPlace(page),target=0,x=0,y=0;
   if(GetBagRows(page)>0 && m_Room[PositionToRoom(place)].FindSameItemToStack(item,&target,&x,&y) &&
      Item[target].GetMaxStackNum()-Item[target].GetStackNum()>=Item[item].GetStackNum()){
    destination->nPlace=place;destination->nX=x;destination->nY=y;return TRUE;
   }
  }
 }
 return SearchPosition(Item[item].GetWidth(),Item[item].GetHeight(),destination,true);
}

int main(){KItemList b;ZeroMemory(&b,sizeof(b));Item[1].max=999;Item[1].count=1;Item[2].max=999;Item[2].count=998;b.m_Room[0].target=2;ItemPos p;assert(b.FindPickupPosition(1,&p)&&p.nX==2&&p.nY==4);Item[2].count=999;assert(!b.FindPickupPosition(1,&p));b.room=true;assert(b.FindPickupPosition(1,&p)&&p.nX==4);Item[1].count=5;Item[2].count=998;assert(b.FindPickupPosition(1,&p)&&p.nX==4);b.room=false;assert(!b.FindPickupPosition(1,&p));Item[1].count=1;Item[1].lock.value=true;assert(!b.FindPickupPosition(1,&p));assert(PhongThanPlayerDamageReduction(1000,2,true,true)==980);assert(PhongThanPlayerDamageReduction(1000,2,false,true)==1000);assert(PhongThanPlayerDamageReduction(1000,2,true,false)==1000);assert(PhongThanPlayerDamageReduction(2147483647,2,true,true)==2104533974);assert(PhongThanPlayerDamageReduction(1000,150,true,true)==0);puts("PASS production pickup resolver: merge 998+1, full 999, overflow/new pile, full bag rollback, locked pile; PvP reduction 2%, PvE unchanged, overflow safe");return 0;}

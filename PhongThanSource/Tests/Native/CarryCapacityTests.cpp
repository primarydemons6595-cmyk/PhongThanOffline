#include <stdio.h>
#include <string.h>
enum {MAX_PLAYER=3,MAX_ITEM=8,MAX_ITEM_BASEATTRIB=2,MAX_ITEM_NORMAL_MAGICATTRIB=2,itempart_num=2,TASKVALUE_PT_CARRY_BONUS=20,magic_addtakeweight=99,pos_hand=1,pos_equiproom=2,pos_equiproomex=3,pos_ptbag2=4,pos_ptbag3=5,pos_immediacy=6};
struct KItemNormalAttrib {int nAttribType,nValue[3];};
struct KItem {int weight,count;KItemNormalAttrib base[2],m_aryMagicAttrib[2];int GetInventoryCount(){return count;}int GetWeight(){return weight;}const KItemNormalAttrib* P0BaseAttributes(){return base;}} Item[MAX_ITEM];
struct PlayerItem {int nPlace,nIdx;};
struct KItemList {PlayerItem rows[4];int size,at,equipment[2];PlayerItem* GetFirstItem(){at=0;return size?rows:0;}PlayerItem* GetNextItem(){return ++at<size?rows+at:0;}int GetEquipment(int slot){return equipment[slot];}};
struct Task {int bonus;int GetSaveVal(int){return bonus;}};
struct PlayerStub {KItemList m_ItemList;Task m_cTask;} Player[MAX_PLAYER];
#include "PhongThanCarry.inl"
static int checks;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d\n",__LINE__);return 1;}}while(0)
int main(){
 CHECK(PhongThanCarryCapacity(1,true)==200);
 Player[1].m_cTask.bonus=100;CHECK(PhongThanCarryCapacity(1,true)==300);
 Player[1].m_cTask.bonus+=1000;CHECK(PhongThanCarryCapacity(1,true)==1300);
 Player[1].m_ItemList.equipment[0]=1;Item[1].base[0].nAttribType=magic_addtakeweight;Item[1].base[0].nValue[0]=50;
 CHECK(PhongThanCarryCapacity(1,true)==1350);CHECK(PhongThanCarryCapacity(1,false)==1300);
 KItemList& bag=Player[1].m_ItemList;bag.size=2;bag.rows[0].nPlace=pos_ptbag3;bag.rows[0].nIdx=2;Item[2].weight=20;Item[2].count=3;
 bag.rows[1].nPlace=99;bag.rows[1].nIdx=3;Item[3].weight=10000;Item[3].count=20;
 CHECK(PhongThanCarryWeight(1)==60);CHECK(!PhongThanCarryBlocked(1));
 Item[2].count=100;CHECK(PhongThanCarryBlocked(1));CHECK(!PhongThanCarryCapacity(0,true));
 printf("PASS F4 carry capacity: %d checks\n",checks);return 0;
}

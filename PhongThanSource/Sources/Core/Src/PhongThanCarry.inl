#include "PhongThanCarry.h"
static int PtCarryClamp(__int64 value){return value<0?0:value>2147483647?2147483647:(int)value;}
int PhongThanCarryWeight(int player){
 if(player<=0||player>=MAX_PLAYER)return 0;__int64 total=0;
 KItemList& bag=Player[player].m_ItemList;
 for(PlayerItem* p=bag.GetFirstItem();p;p=bag.GetNextItem()){
  if(p->nPlace!=pos_hand&&p->nPlace!=pos_equiproom&&p->nPlace!=pos_equiproomex&&p->nPlace!=pos_ptbag2&&p->nPlace!=pos_ptbag3&&p->nPlace!=pos_immediacy)continue;
  if(p->nIdx<=0||p->nIdx>=MAX_ITEM)continue;KItem& item=Item[p->nIdx];
  int count=item.GetInventoryCount();if(count<1)count=1;
  if(item.GetWeight()>0)total+=(__int64)item.GetWeight()*count;
 }return PtCarryClamp(total);
}
int PhongThanCarryCapacity(int player,bool equipment){
 if(player<=0||player>=MAX_PLAYER)return 0;
 __int64 total=200+(__int64)Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CARRY_BONUS);
 if(equipment)for(int slot=0;slot<itempart_num;++slot){
  int index=Player[player].m_ItemList.GetEquipment(slot);if(index<=0||index>=MAX_ITEM)continue;
  KItem& item=Item[index];const KItemNormalAttrib* base=item.P0BaseAttributes();
  for(int n=0;n<MAX_ITEM_BASEATTRIB;++n)if(base[n].nAttribType==magic_addtakeweight)total+=base[n].nValue[0];
  for(n=0;n<MAX_ITEM_NORMAL_MAGICATTRIB;++n)if(item.m_aryMagicAttrib[n].nAttribType==magic_addtakeweight)total+=item.m_aryMagicAttrib[n].nValue[0];
 }return PtCarryClamp(total);
}
bool PhongThanCarryBlocked(int player){return PhongThanCarryWeight(player)>PhongThanCarryCapacity(player);}

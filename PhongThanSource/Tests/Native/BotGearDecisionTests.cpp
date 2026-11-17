#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanBotGearRating.h"
struct Inventory {
 int hand,equip,bag,stored,clicks;bool reject;
 Inventory():hand(0),equip(1),bag(2),stored(0),clicks(0),reject(false){}
 int Hand(){return hand;}int Equipped(int){return equip;}
 void Click(const PTBotInventoryCell& cell){++clicks;int old;
  if(cell.place==1){old=bag;bag=hand;hand=old;}
  else if(cell.place==2&&!reject){old=equip;equip=hand;hand=old;}
  else if(cell.place==3){old=stored;stored=hand;hand=old;}
 }
};
int main(){Inventory broken;broken.Click(PTBotInventoryCell(1));assert(broken.Hand()==2&&broken.Equipped(0)==1);
 Inventory normal;assert(PTBotEquipTransaction(normal,2,0,PTBotInventoryCell(1),PTBotInventoryCell(2),PTBotInventoryCell(1)));assert(normal.equip==2&&normal.bag==1&&normal.hand==0&&normal.clicks==3);
 Inventory denied;denied.reject=true;assert(!PTBotEquipTransaction(denied,2,0,PTBotInventoryCell(1),PTBotInventoryCell(2),PTBotInventoryCell(3)));assert(denied.equip==1&&denied.bag==2&&denied.hand==0);
 Inventory empty;empty.equip=0;assert(PTBotEquipTransaction(empty,2,0,PTBotInventoryCell(1),PTBotInventoryCell(2),PTBotInventoryCell(3)));assert(empty.equip==2&&empty.hand==0&&empty.clicks==2);
 Inventory held;held.hand=7;assert(!PTBotEquipTransaction(held,2,0,PTBotInventoryCell(1),PTBotInventoryCell(2),PTBotInventoryCell(3)));assert(held.hand==7&&held.clicks==0);
 int hp[3]={100,0,0},physical[3]={20,0,30},magic[3]={20,0,30},negative[3]={-10,0,0};
 assert(PTBotGearAttributeScore(magic_lifemax_v,hp,0)>0);
 assert(PTBotGearAttributeScore(magic_physicsdamage_v,physical,0)>PTBotGearAttributeScore(magic_physicsdamage_v,physical,1));
 assert(PTBotGearAttributeScore(magic_magicdamage_v,magic,1)>PTBotGearAttributeScore(magic_magicdamage_v,magic,0));
 assert(PTBotGearAttributeScore(magic_lifemax_v,negative,0)==0);
 assert(PTBotGearAttributeScore(magic_requirelevel,hp,0)==0);
 printf("PASS production equip transaction: pickup/equip/store, rejection rollback, empty slot, existing hand; actual attribute scoring\n");return 0;}

#include "../../Headers/PhongThanBank.h"
#include "../../Headers/PhongThanConsumableUi.h"
#include "../../Headers/PhongThanIBBuffUi.h"
#include "../../Headers/PhongThanF2Balance.h"
#include "../../Headers/PhongThanStatusUi.h"
#include "../../Headers/PhongThanStall.h"
#include <assert.h>
#include <stdio.h>
int main(){
 const unsigned ids[]={PT_GDI_STALL_STATUS,PT_BANK_LOCK_GET,PT_BANK_EXPIRY_GET,PT_GDI_CONSUMABLE_BUFFS,
   PT_GDI_IBBUFFS,PT_F2_BALANCE_GET,PT_F2_CURRENCY_GET,PT_GDI_STATUS_SKILLS,PT_GDI_STATUS_DETAIL,PT_GDI_STATUS_ORIGINAL};
 for(int i=0;i<sizeof(ids)/sizeof(ids[0]);++i)
  for(int j=i+1;j<sizeof(ids)/sizeof(ids[0]);++j)assert(ids[i]!=ids[j]);
 assert(PT_GDI_CONSUMABLE_BUFFS==0x50544246);
 assert(sizeof(PTConsumableUiState)==256);
 puts("PASS local Core/UI query identities: bank lock, timed buffs, IB buffs, F2 currencies and skill states are distinct");return 0;
}

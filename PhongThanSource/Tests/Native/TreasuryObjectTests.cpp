#include <assert.h>
#include <stdio.h>
#include "PhongThanTreasuryObjects.h"
int main(){
 assert(PTDuplicateTreasuryVariant(1,2,true,195));assert(PTDuplicateTreasuryVariant(2,1,true,384));
 assert(!PTDuplicateTreasuryVariant(1,2,true,385));assert(!PTDuplicateTreasuryVariant(1,2,false,0));
 assert(!PTDuplicateTreasuryVariant(1,1,true,0));assert(!PTDuplicateTreasuryVariant(2,2,true,0));
 assert(!PTDuplicateTreasuryVariant(3,2,true,0));assert(!PTDuplicateTreasuryVariant(1,2,true,-1));
 puts("PASS treasury variant scope: exact same service, only alternate 1/2, nearby; preserve unrelated chests");return 0;
}

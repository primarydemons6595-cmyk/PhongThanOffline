#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include "../../Headers/PhongThanEquipmentAppearance.h"
#include "../../Headers/PhongThanFactionBalance.h"
int main(){
    int checked=0;
    // Human Tinh Quan/ Pha Quan stays human even when its owner ascends.
    for(int side=0;side<=2;++side) for(int detail=2;detail<=9;++detail) {
        if(detail!=2&&detail!=7&&detail!=9)continue;
        assert(PhongThanResolveEquipmentPartResource(detail,51,10,0,side)==5);
    }
    for(int detail=2;detail<=9;++detail){if(detail!=2&&detail!=7&&detail!=9)continue;
        for(int p=0;p<=326;++p)for(int tier=1;tier<=10;++tier)for(int side=0;side<=2;++side){
            int r=PhongThanResolveEquipmentPartResource(detail,p,tier,0,side);
            assert(r>=1&&r<=8&&r!=6);if(detail==9)assert(r<=7);++checked;
        }
    }
    for(int profession=0;profession<3;++profession){
        assert(PhongThanResolveEquipmentPartResource(2,51+profession,10,0,0)==5);
        assert(PhongThanResolveEquipmentPartResource(7,54+profession,10,0,0)==5);
        assert(PhongThanResolveEquipmentPartResource(9,51+profession,10,0,0)==5);
        assert(PhongThanFactionDamage(1000,profession,(profession+1)%3)==2000);
        assert(PhongThanFactionDamage(1000,(profession+1)%3,profession)==500);
        assert(PhongThanFactionDamage(1000,profession,profession)==1000);
    }
    assert(PhongThanResolveEquipmentPartResource(2,13,10,1,1)==7);
    assert(PhongThanResolveEquipmentPartResource(2,13,10,1,2)==8);
    assert(PhongThanResolveEquipmentPartResource(2,-1,1,0,0)==-1);
    assert(PhongThanResolveEquipmentPartResource(2,1,11,0,0)==-1);
    assert(PhongThanFactionDamage(1000,-1,1)==1000);
    assert(PhongThanFactionDamage(1000,1,4)==1000);
    assert(PhongThanFactionDamage(1000,3,1)==2000);
    assert(PhongThanFactionDamage(1000,2,3)==2000);
    assert(PhongThanScaleCombatDamage(INT_MAX,115)==INT_MAX);
    assert(PhongThanFactionDamage(0,0,1)==0);
    assert(PhongThanScaleCombatDamage(1000,10)==100); // same PvP base for player and owner-credited pet
    printf("PASS %d clothing tuples; no equipped item selects unequipped body 0/6; all faction pairs, Vu Si, neutral and overflow.\n",checked);
    return 0;
}

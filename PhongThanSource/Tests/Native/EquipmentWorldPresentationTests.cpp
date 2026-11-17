#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanEquipmentWorldPresentation.h"

int main()
{
	assert(PhongThanResolveWeaponWorldEffect(0, 0) == 0);
	assert(PhongThanResolveWeaponWorldEffect(11, 0) == 0);
	assert(PhongThanResolveWeaponWorldEffect(12, 0) == 39);
	assert(PhongThanResolveWeaponWorldEffect(12, 1) == 39);
	assert(PhongThanResolveWeaponWorldEffect(12, 2) == 39);
	assert(PhongThanResolveWeaponWorldEffect(12, 3) == 39);
	assert(PhongThanResolveWeaponWorldEffect(12, 4) == 0);
	assert(PhongThanResolveSetWorldEffect(2, 1, 0) == 0);
	assert(PhongThanResolveSetWorldEffect(3, 0, 0) == 0);
	assert(PhongThanResolveSetWorldEffect(3, 1, 1) == 35);
	assert(PhongThanResolveSetWorldEffect(3, 1, 0) == 36);
	assert(PhongThanResolveSetWorldEffect(5, 1, 1) == 37);
	assert(PhongThanResolveSetWorldEffect(5, 1, 0) == 38);
	for (int profession = 0; profession < 3; ++profession)
	{
		assert(PhongThanResolveOrdinarySetWorldEffect(2, profession) == 0);
		unsigned char dim = PhongThanResolveOrdinarySetWorldEffect(3, profession);
		unsigned char full = PhongThanResolveOrdinarySetWorldEffect(5, profession);
		assert(dim == 154 + profession && full == 26 + profession);
		assert(PhongThanResolveOrdinarySetWorldEffect(4, profession) == dim);
		assert(PhongThanEquipmentGraphicState(dim) == full);
		assert(PhongThanEquipmentWorldEffectValid(dim));
		assert(PhongThanOrdinarySetAlpha(full, &dim, 1) == 128);
		assert(PhongThanOrdinarySetAlpha(full, &full, 1) == 255);
		unsigned char ordinary[2] = {0, 0};
		assert(PhongThanMergeEquipmentWorldEffect(ordinary, 2, dim));
		assert(ordinary[0] == full);
		assert(PhongThanMergeEquipmentWorldEffect(ordinary, 2, full));
		assert(ordinary[1] == 0);
	}
	assert(PhongThanResolveOrdinarySetWorldEffect(5, -1) == 0);
	assert(PhongThanResolveOrdinarySetWorldEffect(5, 3) == 0);
	assert(!PhongThanEquipmentWorldEffectValid(153));
	assert(!PhongThanEquipmentWorldEffectValid(157));

	unsigned char states[8] = {1, 2, 0, 0, 0, 0, 0, 0};
	assert(PhongThanMergeEquipmentWorldEffect(states, 8, 41) == 1);
	assert(PhongThanMergeEquipmentWorldEffect(states, 8, 38) == 1);
	assert(states[0] == 1 && states[1] == 2 && states[2] == 41 && states[3] == 38);
	assert(PhongThanMergeEquipmentWorldEffect(states, 8, 41) == 1);
	assert(states[4] == 0);

	unsigned char full[3] = {1, 2, 3};
	assert(PhongThanMergeEquipmentWorldEffect(full, 3, 39) == 0);
	assert(full[0] == 1 && full[1] == 2 && full[2] == 3);
	assert(PhongThanMergeEquipmentWorldEffect(states, 8, 43) == 0);
	assert(PhongThanEquipmentWorldEffectValid(0));
	assert(PhongThanEquipmentWorldEffectValid(39));
	assert(PhongThanEquipmentWorldEffectValid(42));
	assert(PhongThanEquipmentWorldEffectValid(35));
	assert(PhongThanEquipmentWorldEffectValid(38));
	assert(!PhongThanEquipmentWorldEffectValid(43));

    const int added[]={44,45,62,63,64,72,73};
    for(int s=0;s<7;++s) {
        assert(PhongThanEquipmentWorldEffectValid((unsigned char)added[s]));
        unsigned char merged[4]={0,0,0,0};
        assert(PhongThanMergeEquipmentWorldEffect(merged,4,(unsigned char)added[s]));
        assert(merged[0]==added[s]);
    }
    assert(!PhongThanEquipmentWorldEffectValid(61));
    assert(!PhongThanEquipmentWorldEffectValid(65));

    PT_EQUIPMENT_GLOW_PIECE gear[8];
    PT_EQUIPMENT_GLOW_RESULT glow;
    for(int family=1;family<=4;++family) {
        for(int i=0;i<5;++i) {
            gear[i].SetId=family==1?9:0;gear[i].Group=family==1?9:0;
            gear[i].Quality=family==1?4:(family==2?3:5);
            gear[i].Realm=family==4;gear[i].Tier=8;gear[i].Upgrade=0;
        }
        PhongThanSelectEquipmentGlow(gear,2,1,0,glow);
        assert(!glow.BaseEffect && !glow.UpgradeEffect);
        PhongThanSelectEquipmentGlow(gear,3,1,0,glow);
        assert(glow.Pieces==3 && glow.Family==family);
        assert(glow.BaseEffect==(family==1?155:family==2?44:family==3?45:63));
        PhongThanSelectEquipmentGlow(gear,5,1,0,glow);
        assert(glow.BaseEffect==(family==1?27:family==4?63:45));
        gear[4].Tier=7;
        PhongThanSelectEquipmentGlow(gear,5,1,0,glow);
        assert(glow.Pieces==(family==1?5:4));
        gear[4].Tier=8;
        for(i=0;i<5;++i)gear[i].Upgrade=12;
        PhongThanSelectEquipmentGlow(gear,5,1,1,glow);
        assert(glow.BaseEffect==0 && glow.UpgradeEffect==(family==4?72:37));
        PhongThanSelectEquipmentGlow(gear,3,1,0,glow);
        assert(glow.UpgradeEffect==(family==4?73:36));
        gear[1].Upgrade=11;
        PhongThanSelectEquipmentGlow(gear,3,1,0,glow);
        assert(!glow.UpgradeEffect && glow.BaseEffect);
    }
    // Complete set must not be hidden by an unrelated first equipped piece.
    for(int n=0;n<6;++n) {
        gear[n].SetId=n==0?1:20;gear[n].Group=gear[n].SetId;
        gear[n].Quality=4;gear[n].Realm=0;gear[n].Tier=8;gear[n].Upgrade=0;
    }
    PhongThanSelectEquipmentGlow(gear,6,2,0,glow);
    assert(glow.SetId==20 && glow.Pieces==5 && glow.BaseEffect==28);
    PT_EQUIPMENT_GLOW_PIECE first=gear[0];gear[0]=gear[5];gear[5]=first;
    PhongThanSelectEquipmentGlow(gear,6,2,0,glow);
    assert(glow.SetId==20 && glow.Pieces==5 && glow.BaseEffect==28);
    // Never add two ordinary, one orange and one purple item into one set.
    gear[2].Quality=3;gear[3].Quality=5;
    PhongThanSelectEquipmentGlow(gear,4,0,0,glow);
    assert(!glow.BaseEffect && !glow.UpgradeEffect);
    PhongThanSelectEquipmentGlow(0,0,0,0,glow);
    assert(!glow.BaseEffect && !glow.UpgradeEffect);

	puts("PASS: equipment world effect mapping and fail-closed state composition.");
	return 0;
}

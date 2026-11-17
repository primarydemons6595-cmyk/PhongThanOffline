#ifndef PHONGTHAN_ROLE_UI_H
#define PHONGTHAN_ROLE_UI_H
// Local, size-checked CoreClient/UI contract. This is not a wire packet.
enum { PT_GDI_ROLE_VALUES = 0x50545256, PT_GDI_BAG_FIRST = 0x50544230 };
struct PTRoleValues {
    int Credit, Exploit, WorldRank;
    int CarryWeight, CarryCapacity, BoundMoney;
    int BagRows[3];
    int PillCount,PillNew,PillBonus,PillAvailable;
    int UnlockCloth;
    int RealmSide,RealmLevel,EquipmentRealm,RealmStage;
    int RealmExperience,RealmExperienceNext,TienReputation,MaReputation;
    int RealmGrowth[4];
};
#endif

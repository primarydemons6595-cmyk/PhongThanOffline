#ifndef PHONGTHAN_STATUS_UI_H
#define PHONGTHAN_STATUS_UI_H
// Existing F3 ABI retained: verified against the deployed CoreClient (size 0xa8).
enum { PT_GDI_STATUS_DETAIL = 0x50545354 };
// Exact 53-value layout consumed by the original F3 property pages (GDI 0x124).
// A separate local query keeps the existing main F3 contract unchanged.
enum { PT_GDI_STATUS_ORIGINAL = 0x5054534f };
struct PTStatusOriginal { int Value[53]; };
typedef char PTStatusOriginalSize[(sizeof(PTStatusOriginal)==212)?1:-1];

inline bool PhongThanStatusOriginalAvailable(int page,int row)
{
    // Slots 16/52 expose ignore-evasion and monster-damage reduction.
    return page>=0 && page<3 && row>=0 && row<(page==2?6:14);
}

inline int PhongThanStatusOriginalRow(const PTStatusOriginal& state, int page, int row)
{
    static const int slots[3][14] = {
        {0,1,3,6,16,15,17,29,18,30,12,13,14,11},
        {33,2,34,35,39,44,23,31,24,32,41,42,43,40},
        {46,47,48,51,52,45,-1,-1,-1,-1,-1,-1,-1,-1}
    };
    if(page<0||page>2||row<0||row>13||(page==2&&row>5)) return 0;
    int value=state.Value[slots[page][row]];
    if(page==0&&row>=10) value+=state.Value[10];
    return value;
}

inline bool PhongThanStatusOriginalIsPercent(int page,int row)
{
    if(page==0) return row==0||row==4||row>=6;
    if(page==1) return row==0||row==3||row==5||(row>=6&&row<=9);
    return page==2&&(row==0||row>=3);
}
struct PTStatusDetail {
    char Profession[64];
    int ElementMin[4], ElementMax[4], CriticalPercent;
    int AttackRating, Evasion, Defence, HitRecover, FreezeTimeReduce;
    int IgnoreDefense, LifeReplenish, ManaReplenish;
    int Focus[5], IgnoreResist[4];
};
typedef char PTStatusDetailAbiSize[(sizeof(PTStatusDetail)==168)?1:-1];
// Local Core/UI query only. Skill IDs and item buffer IDs are separate namespaces.
enum { PT_GDI_STATUS_SKILLS = 0x5054534B, PT_STATUS_UI_MAX = 48 };
struct PTStatusSkillUi {
    int SkillId, LeftTicks;
    char Image[80], Name[80], Description[256];
};
struct PTStatusSkillsUi { int Count; PTStatusSkillUi Entries[PT_STATUS_UI_MAX]; };
#endif

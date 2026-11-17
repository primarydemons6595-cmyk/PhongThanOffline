#ifndef PHONGTHAN_LEGACY_FOCUSED_REDUCTION_H
#define PHONGTHAN_LEGACY_FOCUSED_REDUCTION_H
// Legacy item 306 values are retained on disk. Convert per item, including
// symmetric negative values used when removing an equipment contribution.
inline int PhongThanLegacyFocusedPercent(int value)
{
    const __int64 magnitude=value<0?-(__int64)value:(__int64)value;
    const int percent=(int)((magnitude+500)/1000);
    return value<0?-percent:percent;
}
inline int PhongThanFocusedDamageReduction(int damage,int percent,
    bool physical,bool focused)
{
    if(damage<=0 || percent<=0 || !physical || !focused)return damage;
    if(percent>=100)return 0;
    return (int)((__int64)damage*(100-percent)/100);
}
#endif

#ifndef PHONGTHAN_TIMED_ITEMS_H
#define PHONGTHAN_TIMED_ITEMS_H
enum { PT_CONSUMABLE_SLOTS = 64, PT_TIMED_ITEM_MAX_SECONDS = 2592000, PT_BUFF_REMAINING_MAX_SECONDS = 2147483647 };
inline int PhongThanTimedItemDetail(int slot)
{
    static const int details[] = {38,39,40,41,42,43,44,45,125,126,127,128,129,130,131,132,186,192,233,390,394,133,330,334,782};
    if(slot>=0 && slot<28) return slot;
    slot-=28;
    return slot>=0 && slot<sizeof(details)/sizeof(details[0]) ? details[slot] : -1;
}
inline int PhongThanTimedItemSlot(int detail)
{
    for(int slot=0;slot<PT_CONSUMABLE_SLOTS;++slot)
        if(PhongThanTimedItemDetail(slot)==detail) return slot;
    return -1;
}
#endif

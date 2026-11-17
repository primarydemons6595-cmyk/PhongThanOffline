#ifndef PHONGTHAN_BANK_H
#define PHONGTHAN_BANK_H
// Local Core/UI query. PTBF (0x50544246) already belongs to timed item buffs.
enum { PT_BANK_EXPIRY_GET=0x50544245, PT_BANK_LOCK_GET=0x5054424C, PT_BANK_FIRST_TASK=5340,
       PT_BANK_MIGRATED_TASK=5347, PT_BANK_PERMANENT=2147483647,
       PT_BANK_RENT_SECONDS=30*86400 };
inline int PTBankPrice(int page,int permanent) {
    const int buy[4]={45,60,99,149},rent[4]={9,20,30,45};
    return page>=2&&page<=5?(permanent?buy[page-2]:rent[page-2]):0;
}
inline int PTBankBasePrice(int page,int permanent) {
    const int buy[4]={63,95,180,270},rent[4]={21,34,45,68};
    return page>=2&&page<=5?(permanent?buy[page-2]:rent[page-2]):0;
}
inline bool PTBankActive(int expiry,int now) {
    return expiry==PT_BANK_PERMANENT || expiry>now;
}
inline int PTBankExtend(int expiry,int now,int permanent) {
    if(expiry==PT_BANK_PERMANENT)return 0;
    if(permanent)return PT_BANK_PERMANENT;
    int start=expiry>now?expiry:now;
    return start>PT_BANK_PERMANENT-PT_BANK_RENT_SECONDS?0:start+PT_BANK_RENT_SECONDS;
}
#ifdef _SERVER
bool PhongThanBankLocked(int player);
void PhongThanBankSetLocked(int player,bool locked);
void PhongThanBankOpen(int player,int page=-1);
bool PhongThanBankAccess(int player,int page);
bool PhongThanBankNearTreasury(int player);
void PhongThanBankSync(int player);
#endif
#endif

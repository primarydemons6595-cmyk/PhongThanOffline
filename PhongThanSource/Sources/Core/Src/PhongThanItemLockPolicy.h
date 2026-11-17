#ifndef PHONGTHAN_ITEM_LOCK_POLICY_H
#define PHONGTHAN_ITEM_LOCK_POLICY_H
// Same expiry rule as KPlayerDBFuns load: only the timed unlock state expires.
// Template sell/trade/drop restrictions and permanent/character locks are separate.
inline void PhongThanExpireItemUnlock(KLockItem& lock, DWORD now)
{
    if (lock.nState == LOCK_STATE_UNLOCK && lock.dwLockTime <= now)
        lock.Clear();
}
#endif

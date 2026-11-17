#include <stdio.h>
#include <stdlib.h>
typedef unsigned long DWORD;
enum { LOCK_STATE_CHARACTER=-3, LOCK_STATE_FOREVER=-2, LOCK_STATE_LOCK=-1,
       LOCK_STATE_NORMAL=0, LOCK_STATE_UNLOCK=1 };
struct KLockItem {
    short nState; DWORD dwLockTime;
    void Clear() { nState=LOCK_STATE_NORMAL; dwLockTime=0; }
};
#include "../../Sources/Core/Src/PhongThanItemLockPolicy.h"
int main() {
    int checks=0;
    for(int state=-3;state<=1;++state)
        for(int now=99;now<=101;++now) {
            KLockItem lock; lock.nState=state; lock.dwLockTime=100;
            PhongThanExpireItemUnlock(lock,now);
            bool expired=state==LOCK_STATE_UNLOCK && now>=100;
            if(lock.nState!=(expired?LOCK_STATE_NORMAL:state) ||
               lock.dwLockTime!=(expired?0:100)) exit(1);
            ++checks;
        }
    printf("PASS: %d lock policy checks\n",checks); return 0;
}

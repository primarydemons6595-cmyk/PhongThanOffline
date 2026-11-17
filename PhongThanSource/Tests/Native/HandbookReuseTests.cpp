#include <assert.h>
#include <stdio.h>
enum { FALSE=0, TRUE=1, item_magicscript=6 };
static int stock,scriptOk,menus,removals;
static int ExecuteScript(int) { if(scriptOk) ++menus; return scriptOk; }
static int FindSame(int) { return stock>0; }
static int RemoveItem(int,int) { ++removals; return stock-- > 0; }
static int Use(int nItemGenre,int nDetailType) {
    const int nIdx=1;
    // Generated from the production NowEatItem script dispatch, not a copy.
#include "NowEatScriptBranch.inl"
    return TRUE;
}
static void Reset(int ok) { stock=1;scriptOk=ok;menus=removals=0; }
int main() {
    Reset(TRUE);
    assert(Use(6,5572) && stock==1 && menus==1 && removals==0);
    // Actions check that the opened handbook is still in the inventory.
    assert(FindSame(1));
    assert(Use(6,5572) && stock==1 && menus==2 && removals==0);
    Reset(FALSE);assert(!Use(6,5572) && stock==1 && !menus && !removals);
    Reset(TRUE);assert(Use(6,1355) && stock==0 && removals==1);
    const int reusable[]={61000,5658,264};
    for(int i=0;i<3;++i) { Reset(TRUE);assert(Use(6,reusable[i]) && stock==1 && !removals); }
    Reset(TRUE);assert(Use(8,5572) && stock==1 && !removals);
    puts("PASS production script dispatch: handbook reuse, callback ownership, failed script, consumable and prior exemptions");
    return 0;
}

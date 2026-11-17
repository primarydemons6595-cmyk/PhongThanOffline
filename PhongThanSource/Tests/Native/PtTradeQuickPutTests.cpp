#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum { CGOG_NOTHING, CGOG_ITEM, UIS_S_TRADE_PLAYER=10, GDI_TRADE_OPER_DATA,
       UTOD_IS_LOCKED, UTOD_IS_TRADING, UTOD_IS_OTHER_TRADING,
       UOC_ITEM_TAKE_WITH, UOC_TO_BE_TRADE, GOI_SWITCH_OBJECT };
struct KUiDraggedObject { unsigned int uGenre,uId; int DataX,DataY,DataW,DataH; };
struct KUiObjAtContRegion {
    struct { unsigned int uGenre,uId; } Obj;
    struct { int h,v,Width,Height; } Region;
    int eContainer;
};
class KWndObjectMatrix {
public:
    int m_nNumUnitHori,m_nNUmUnitVert,m_nNumObjects;
    KUiDraggedObject* m_pObjects;
    bool FindFreePlace(int,int,int&,int&) const;
};
#include "../../Sources/GameClient/Ui/Elem/WndObjMatrixFreePlace.inl"
struct Core {
    int locked,committed,otherCommitted,sends;
    KUiObjAtContRegion from,to;
    int GetGameData(int,unsigned int query,int) {
        if(query==UTOD_IS_LOCKED) return locked;
        if(query==UTOD_IS_TRADING) return committed;
        if(query==UTOD_IS_OTHER_TRADING) return otherCommitted;
        abort(); return 0;
    }
    int OperationRequest(int op,unsigned int a,int b) {
        if(op!=GOI_SWITCH_OBJECT) abort();
        from=*(KUiObjAtContRegion*)a; to=*(KUiObjAtContRegion*)b; ++sends; return 0;
    }
} core,*g_pCoreShell=&core;
struct UiBase { int status; int GetStatus() { return status; } } g_UiBase;
KUiDraggedObject hand;
void Wnd_GetDragObj(KUiDraggedObject* out) { *out=hand; }
class KUiTrade {
public:
    KWndObjectMatrix m_SelfItemsBox;
    bool PutInventoryItem(const KUiDraggedObject*);
};
#include "../../Sources/GameClient/Ui/UiCase/UiTradeQuickPut.inl"
int checks=0;
void Check(bool ok,const char* label) { ++checks; if(!ok) { printf("FAIL %s\n",label); exit(1); } }
int main() {
    KUiTrade ui; memset(&ui,0,sizeof(ui)); memset(&core,0,sizeof(core)); memset(&hand,0,sizeof(hand));
    g_UiBase.status=UIS_S_TRADE_PLAYER;
    ui.m_SelfItemsBox.m_nNumUnitHori=5; ui.m_SelfItemsBox.m_nNUmUnitVert=3;
    KUiDraggedObject item={CGOG_ITEM,123,2,4,2,3};
    Check(ui.PutInventoryItem(&item) && core.sends==1,"one atomic request");
    Check(core.from.eContainer==UOC_ITEM_TAKE_WITH && core.from.Region.h==2 && core.from.Region.v==4,"source exact");
    Check(core.to.eContainer==UOC_TO_BE_TRADE && core.to.Region.h==0 && core.to.Region.v==0 && core.to.Obj.uId==123,"target exact");
    Check(ui.m_SelfItemsBox.m_nNumObjects==0,"no speculative UI mutation");
    KUiDraggedObject occupied={CGOG_ITEM,44,0,0,2,3};
    ui.m_SelfItemsBox.m_pObjects=&occupied; ui.m_SelfItemsBox.m_nNumObjects=1;
    Check(ui.PutInventoryItem(&item) && core.to.Region.h==2,"skip occupied cells");
    occupied.DataW=5; Check(!ui.PutInventoryItem(&item),"full refuses");
    occupied.DataW=2; occupied.DataX=-1; Check(!ui.PutInventoryItem(&item),"bad geometry refuses");
    ui.m_SelfItemsBox.m_nNumObjects=0;
    core.locked=1; Check(!ui.PutInventoryItem(&item),"locked refuses");
    core.locked=0; core.committed=1; Check(!ui.PutInventoryItem(&item),"committed refuses");
    core.committed=0; core.otherCommitted=1; Check(!ui.PutInventoryItem(&item),"peer committed refuses");
    core.otherCommitted=0; hand=item; Check(!ui.PutInventoryItem(&item),"holding item refuses");
    memset(&hand,0,sizeof(hand)); g_UiBase.status=0; Check(!ui.PutInventoryItem(&item),"idle refuses");
    g_UiBase.status=UIS_S_TRADE_PLAYER; Check(!ui.PutInventoryItem(0),"null refuses");
    item.DataW=0; Check(!ui.PutInventoryItem(&item),"zero dimensions refuse");
    item.DataW=0x7fffffff; Check(!ui.PutInventoryItem(&item),"huge dimensions refuse");
    item.DataW=2; ui.m_SelfItemsBox.m_nNUmUnitVert=1;
    Check(!ui.PutInventoryItem(&item),"oversized item never placed outside visible grid");
    item.DataW=item.DataH=1; Check(ui.PutInventoryItem(&item),"RPS one-row item fits");
    printf("PASS: %d trade quick-put checks\n",checks); return 0;
}

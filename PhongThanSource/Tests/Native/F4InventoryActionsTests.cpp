#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define FALSE 0
#define TRUE 1
#define EQUIPMENT_ROOM_WIDTH 5
#define EQUIPMENT_ROOM_HEIGHT 7
enum { CGOG_NOTHING, CGOG_ITEM, UIS_S_IDLE=0, UOC_EQUIPTMENT=2,
       UOC_ITEM_TAKE_WITH=3, UOC_ITEM_TAKE_WITH_EX=4,
       GDI_EQUIPEX_TIME=10, GDI_ITEM_TAKEN_WITH=11, GDI_ITEM_EXPANDITEM=12,
       GOI_SWITCH_OBJECT=20, UI_SI_PICKPUT_ITEM=30, UIS_S_TRADE_PLAYER=40,
       GDI_TRADE_OPER_DATA=41, UTOD_IS_LOCKED=42, UTOD_IS_TRADING=43,
       UTOD_IS_OTHER_TRADING=44 };
struct KUiRegion { int h,v,Width,Height; };
struct Object { unsigned int uGenre,uId; };
struct KUiObjAtRegion { Object Obj; KUiRegion Region; };
struct KUiObjAtContRegion { Object Obj; KUiRegion Region; int eContainer; };
struct KUiDraggedObject { unsigned int uGenre,uId; int DataX,DataY,DataW,DataH; };
struct KWndWindow {};
struct Button { int enabled; void Enable(int value) { enabled=value; } };
struct Matrix {
    int adds,removes,pickPut;
    void EnablePickPut(bool value) { pickPut=value; }
    void AddObject(KUiDraggedObject*,int) { ++adds; }
    void RemoveObject(KUiDraggedObject*) { ++removes; }
};
struct Box : KWndWindow {
    KUiDraggedObject item;
    bool GetObject(KUiDraggedObject& out) { out=item; return item.uId!=0; }
};
struct UiBase { int status; int GetStatus() { return status; } } g_UiBase;
struct Core {
    int expires,count,trade,sends,lastQuery,locked,committed,otherCommitted;
    KUiObjAtRegion items[35];
    KUiObjAtContRegion from,to;
    int GetTradeState() { return trade; }
    int GetGameData(int query,unsigned int out,int capacity) {
        lastQuery=query;
        if(query==GDI_EQUIPEX_TIME) return expires;
        if(query==GDI_TRADE_OPER_DATA) {
            if(out==UTOD_IS_LOCKED) return locked;
            if(out==UTOD_IS_TRADING) return committed;
            if(out==UTOD_IS_OTHER_TRADING) return otherCommitted;
            abort();
        }
        if(out && count>=0 && count<=capacity) memcpy((void*)out,items,count*sizeof(items[0]));
        return count;
    }
    int OperationRequest(int op,unsigned int a,int b) {
        if(op!=GOI_SWITCH_OBJECT) abort();
        from=*(KUiObjAtContRegion*)a; to=*(KUiObjAtContRegion*)b; ++sends; return 0;
    }
} core,*g_pCoreShell=&core;
static int dragging=0;
void Wnd_GetDragObj(KUiDraggedObject* item) {
    memset(item,0,sizeof(*item)); if(dragging) { item->uGenre=CGOG_ITEM; item->uId=99; }
}
void UiSoundPlay(int) {}
static struct { int nPosition; } EquipCtrlMap[1]={{7}};
class KUiItem {
public:
    int m_nInventoryPage,updates;
    Button m_PageBtn[4],m_JEvilEquipBtn;
    Matrix m_ItemBox;
    Box m_EquipBox[1];
    struct Controls {
        bool locked;
        void SetLocked(bool value) { locked=value; }
    } m_PtF4;
    int GetEquipBoxIndex(KWndWindow* source) { return source==&m_EquipBox[0]?0:-1; }
    int GetInventoryContainer() { return m_nInventoryPage?UOC_ITEM_TAKE_WITH_EX:UOC_ITEM_TAKE_WITH; }
    void SetInventoryPage(int page) { m_nInventoryPage=page; }
    void UpdateData() { ++updates; }
    void RefreshF4Availability();
    bool IsInventoryPageAccessible() const;
    void ApplyInventoryDelta(KUiObjAtRegion*,int);
    bool HandleEquipRightClick(KWndWindow*);
};
#include "../../Sources/GameClient/Ui/UiCase/UiItemF4Actions.inl"
static int checks=0;
static void Check(bool ok,const char* name) {
    ++checks; if(!ok) { printf("FAIL: %s\n",name); exit(1); }
}
static void Reset(KUiItem& ui) {
    memset(&ui,0,sizeof(ui)); memset(&core,0,sizeof(core));
    g_pCoreShell=&core; g_UiBase.status=0; dragging=0; core.expires=60;
    ui.m_EquipBox[0].item.uGenre=CGOG_ITEM; ui.m_EquipBox[0].item.uId=123;
    ui.m_EquipBox[0].item.DataW=2; ui.m_EquipBox[0].item.DataH=3;
}
int main() {
    KUiItem ui; Reset(ui);
    Check(ui.HandleEquipRightClick(&ui.m_EquipBox[0]) && core.sends==1,"empty inventory sends once");
    Check(core.from.eContainer==UOC_EQUIPTMENT && core.from.Region.v==7,"equipment source slot");
    Check(core.to.eContainer==UOC_ITEM_TAKE_WITH && core.to.Region.h==0 && core.to.Region.v==0,"main inventory target");
    Reset(ui); ui.m_nInventoryPage=1; ui.HandleEquipRightClick(&ui.m_EquipBox[0]);
    Check(core.to.eContainer==UOC_ITEM_TAKE_WITH_EX && core.lastQuery==GDI_ITEM_EXPANDITEM,"expansion query and destination");
    Reset(ui); core.count=1; core.items[0].Region.Width=5; core.items[0].Region.Height=7;
    ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"full inventory refuses");
    Reset(ui); core.count=1; core.items[0].Region.Width=1; core.items[0].Region.Height=7;
    ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==1 && core.to.Region.h==1,"occupied cells skipped");
    Reset(ui); core.count=1; core.items[0].Region.h=-1;
    ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"malformed inventory refuses");
    Reset(ui); core.count=36; ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"oversized inventory refuses");
    Reset(ui); core.count=1; core.items[0].Region.Width=0x7fffffff; core.items[0].Region.Height=1;
    ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"overflow dimensions refuse");
    Reset(ui); ui.m_nInventoryPage=1; core.expires=0; ui.HandleEquipRightClick(&ui.m_EquipBox[0]);
    Check(core.sends==0,"expired expansion refuses unequip");
    Reset(ui); dragging=1; ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"dragging refuses");
    Reset(ui); core.trade=1; ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"stall refuses");
    Reset(ui); g_UiBase.status=1; ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"nonidle refuses");
    Reset(ui); KWndWindow other; Check(!ui.HandleEquipRightClick(&other) && core.sends==0,"inventory click not intercepted");
    Reset(ui); ui.m_EquipBox[0].item.DataW=6; ui.HandleEquipRightClick(&ui.m_EquipBox[0]); Check(core.sends==0,"invalid equipment dimensions refuse");
    Reset(ui); g_pCoreShell=0; Check(ui.HandleEquipRightClick(&ui.m_EquipBox[0]),"null core safe");
    Reset(ui); ui.RefreshF4Availability(); Check(ui.m_PageBtn[1].enabled && ui.m_PageBtn[2].enabled && ui.m_PageBtn[3].enabled && !ui.m_JEvilEquipBtn.enabled,"locked tabs can be selected without enabling equipment set");
    ui.m_nInventoryPage=1; core.expires=0; ui.RefreshF4Availability(); Check(ui.m_nInventoryPage==1 && ui.m_PageBtn[1].enabled && ui.m_PtF4.locked && !ui.m_ItemBox.pickPut,"expired expansion stays selected and locked");
    const int refreshes=ui.updates; ui.RefreshF4Availability(); Check(ui.updates==refreshes,"unchanged lock state does not reload every frame");
    KUiObjAtRegion blockedDelta; memset(&blockedDelta,0,sizeof(blockedDelta));
    ui.ApplyInventoryDelta(&blockedDelta,1); Check(ui.m_ItemBox.adds==0,"locked inventory rejects network insertion into visible grid");
    for(int lockedPage=2;lockedPage<4;++lockedPage) {
        Reset(ui); ui.m_nInventoryPage=lockedPage; ui.RefreshF4Availability();
        ui.HandleEquipRightClick(&ui.m_EquipBox[0]);
        Check(ui.m_PtF4.locked && !ui.m_ItemBox.pickPut && core.sends==0,"unsupported bag never aliases expansion storage");
    }
    Reset(ui); ui.m_nInventoryPage=1; core.expires=0; ui.RefreshF4Availability();
    core.expires=60; ui.RefreshF4Availability();
    Check(!ui.m_PtF4.locked && ui.m_ItemBox.pickPut && ui.updates==2,"authoritative access transition unlocks existing expansion");
    Reset(ui); KUiObjAtRegion delta; memset(&delta,0,sizeof(delta));
    ui.ApplyInventoryDelta(&delta,1); ui.ApplyInventoryDelta(&delta,0);
    Check(ui.m_ItemBox.adds==1 && ui.m_ItemBox.removes==1,"delta add and remove");
    ui.ApplyInventoryDelta(0,1); Check(ui.updates==1,"null delta refreshes");
    Reset(ui); g_UiBase.status=UIS_S_TRADE_PLAYER; ui.RefreshF4Availability();
    Check(ui.m_ItemBox.pickPut,"trade and RPS main bag permits pick/put");
    core.locked=1; ui.RefreshF4Availability(); Check(!ui.m_ItemBox.pickPut,"locked offer blocks bag");
    core.locked=0; core.committed=1; ui.RefreshF4Availability(); Check(!ui.m_ItemBox.pickPut,"committed offer blocks bag");
    core.committed=0; core.otherCommitted=1; ui.RefreshF4Availability(); Check(!ui.m_ItemBox.pickPut,"peer committed blocks bag");
    core.otherCommitted=0; ui.RefreshF4Availability(); Check(ui.m_ItemBox.pickPut,"unlock restores pick/put every frame");
    ui.m_nInventoryPage=1; ui.RefreshF4Availability(); Check(!ui.m_ItemBox.pickPut,"trade cannot mutate unsnapshotted expansion");
    Reset(ui); g_UiBase.status=1; ui.RefreshF4Availability(); Check(!ui.m_ItemBox.pickPut,"NPC sale keeps click notification path");
    printf("PASS: %d F4 action checks\n",checks); return 0;
}

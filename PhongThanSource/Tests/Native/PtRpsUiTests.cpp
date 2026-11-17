#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum { GDI_TRADE_OPER_DATA,UTOD_IS_LOCKED,UTOD_IS_OTHER_LOCKED,UTOD_IS_TRADING,UTOD_IS_OTHER_TRADING };
struct KIniFile {
    int GetInteger(const char*,const char* key,int fallback,int* out) {
        *out=!strcmp(key,"SelfLeft")||!strcmp(key,"OtherLeft")?143:
            !strcmp(key,"SelfTop")?219:!strcmp(key,"OtherTop")?153:fallback;
        return 1;
    }
};
struct KWndWindow {
    bool hidden,enabled,checked; const char* section; int frame,left,top;
    KWndWindow():hidden(false),enabled(true),checked(false),section("") {}
    void Init(KIniFile*,const char* s) { section=s; }
    void Hide() { hidden=true; }
    void Show() { hidden=false; }
    void SetFrame(int value) { frame=value; }
    void SetPosition(int x,int y) { left=x;top=y; }
    void Enable(bool e) { enabled=e; }
    void CheckButton(bool c) { checked=c; }
    void SetText(const char*,int=-1) {}
    void EnableTracePutPos(bool) {}
    void EnablePickPut(bool e) { enabled=e; }
};
struct Core {
    int lock,otherLock,commit,otherCommit;
    int GetGameData(int,int query,int) {
        switch(query) {
        case UTOD_IS_LOCKED:return lock;
        case UTOD_IS_OTHER_LOCKED:return otherLock;
        case UTOD_IS_TRADING:return commit;
        default:return otherCommit;
        }
    }
} core,*g_pCoreShell=&core;
struct KUiTrade:KWndWindow {
    bool m_bTradeFolkGame,m_bShowRpsResult;int m_nRpsChoice;
    KWndWindow m_OtherName,m_Info,m_Ok,m_Trade,m_Cancel,m_RpsChoice[3],m_RpsImage[2],m_PtClose,
        m_TakewithMoney,m_TakewithItemsBox,m_SelfItemsBox,m_OtherItemsBox,
        m_SelfMoney,m_OtherMoney,m_AddMoney,m_ReduceMoney;
    struct {char Name[32];} m_OtherData;
    bool LoadRpsScheme(KIniFile*);
    bool UpdateRpsState();
    bool HandleRpsButton(unsigned int);
};
#include "../../Sources/GameClient/Ui/UiCase/UiRpsCompat.inl"
static int checks=0;
void Check(bool good,const char* label) {
    ++checks;if(!good) {printf("FAIL: %s\n",label);exit(1);}
}
int main() {
    KUiTrade ui;KIniFile ini;ui.m_bTradeFolkGame=true;ui.m_bShowRpsResult=false;ui.m_nRpsChoice=-1;
    strcpy(ui.m_OtherData.Name,"peer");
    Check(ui.LoadRpsScheme(&ini),"authentic RPS loader selected");
    Check(!strcmp(ui.section,"Main")&&!strcmp(ui.m_RpsChoice[0].section,"BtnForfex")&&
        !strcmp(ui.m_RpsChoice[1].section,"BtnStone")&&!strcmp(ui.m_RpsChoice[2].section,"BtnCloth"),"original three button bindings");
    Check(!ui.m_SelfMoney.hidden&&!ui.m_SelfItemsBox.hidden&&!ui.m_OtherItemsBox.hidden&&ui.m_TakewithItemsBox.hidden,"authentic stake boxes and separate F4");
    ui.UpdateRpsState();Check(!ui.m_Ok.enabled&&!ui.m_Trade.enabled,"selection required");
    ui.HandleRpsButton((unsigned int)(KWndWindow*)&ui.m_RpsChoice[1]);
    Check(ui.m_nRpsChoice==1&&ui.m_Ok.enabled&&ui.m_RpsChoice[1].checked,"mouse selects rock");
    core.lock=1;ui.UpdateRpsState();
    Check(!ui.m_RpsImage[0].hidden&&ui.m_RpsImage[0].frame==1&&ui.m_RpsImage[0].left==143&&ui.m_RpsImage[0].top==219,"locked rock displayed at authored self position");
    Check(ui.m_RpsImage[1].hidden,"unlocked peer hidden");
    Check(!ui.m_RpsChoice[0].enabled&&!ui.m_Trade.enabled,"locked hand cannot change, waits for peer");
    ui.HandleRpsButton((unsigned int)(KWndWindow*)&ui.m_RpsChoice[0]);
    Check(ui.m_nRpsChoice==1,"forged disabled click cannot change locked hand");
    core.otherLock=1;ui.UpdateRpsState();Check(ui.m_Trade.enabled,"both locks enable confirm");
    Check(!ui.m_RpsImage[1].hidden&&ui.m_RpsImage[1].frame==3&&ui.m_RpsImage[1].top==153,"locked peer displays unknown not leaked hand");
    core.commit=1;ui.UpdateRpsState();Check(!ui.m_Trade.enabled&&!ui.m_Ok.enabled,"no double submit");
    ui.m_bShowRpsResult=true;ui.m_nRpsChoice=-1;
    core.lock=core.otherLock=core.commit=core.otherCommit=0;
    ui.UpdateRpsState();
    Check(ui.m_SelfItemsBox.enabled && ui.m_SelfMoney.enabled,"tie result still permits withdrawing escrow");
    Check(ui.m_RpsChoice[0].enabled && ui.m_RpsChoice[1].enabled && ui.m_RpsChoice[2].enabled,"tie result re-enables next hand");
    Check(!ui.m_Ok.enabled && !ui.m_Trade.enabled,"new round requires selecting and locking again");
    ui.HandleRpsButton((unsigned int)(KWndWindow*)&ui.m_RpsChoice[2]);
    Check(!ui.m_bShowRpsResult && ui.m_nRpsChoice==2 && ui.m_Ok.enabled,"next hand works without closing session");
    ui.m_bTradeFolkGame=false;
    Check(!ui.LoadRpsScheme(&ini)&&!ui.UpdateRpsState()&&!ui.HandleRpsButton(0),"normal trade bypass unchanged");
    printf("PASS: %d RPS UI checks\n",checks);return 0;
}

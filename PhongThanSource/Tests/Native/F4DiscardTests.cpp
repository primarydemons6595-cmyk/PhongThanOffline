#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef unsigned long DWORD;
enum { CGOG_ITEM=1, WND_M_OTHER_WORK_RESULT=2 };
class KWndWindow {
public:
    virtual int WndProc(unsigned int,unsigned int,int) { return 0; }
};
struct KUiDraggedObject { unsigned int uGenre,uId; };
static DWORD ticks=100,player=55;
static unsigned int held=7,serial=1234;
static int drops=0,shows=0,saves=0;
static bool prepare=true,drag=true;
static KWndWindow* recipient=0;
DWORD GetTickCount() { return ticks; }
int Wnd_GetDragObj(KUiDraggedObject* item) {
    item->uGenre=CGOG_ITEM; item->uId=held; return drag?1:0;
}
void g_DebugLog(const char*) {}
struct KIniFile {
    int Load(const char*) { return 0; }
    void GetInteger(const char*,const char*,int value,int* out) { *out=value; }
    void WriteInteger(const char*,const char*,int) {}
    int Save(const char*) { ++saves; return 1; }
};
struct Core {
    int ThrowAwayItem() { ++drops; return 1; }
    int GetGameData(int,unsigned int,int) { return (int)serial; }
    DWORD GetdwIDPlayer(int) { return player; }
} core,*g_pCoreShell=&core;
struct Information {
    bool PreparePtPkConfirmation() { return prepare; }
} g_UiInformation;
void UIMessageBox(const char* text,KWndWindow* target,const char*,const char*,unsigned int,int) {
    if ((unsigned char)text[1]!=0xb9) abort(); // Explicit TCVN3, not UTF-8.
    ++shows; recipient=target;
}
#define PHONGTHAN_DISCARD_TEST
#include "../../Sources/GameClient/Ui/UiCase/UiItemDiscard.inl"
static int checks=0;
void Check(bool ok,const char* name) {
    ++checks; if(!ok) { printf("FAIL: %s\n",name); exit(1); }
}
void Reset() {
    s_PtInventoryDiscard=KPtInventoryDiscard(); g_pCoreShell=&core;
    ticks=100;player=55;held=7;serial=1234;drops=shows=saves=0;
    prepare=drag=true;recipient=0;
}
void Answer(int index) { if(recipient) recipient->WndProc(WND_M_OTHER_WORK_RESULT,1,index); }
int main() {
    Reset(); Check(s_PtInventoryDiscard.Enabled(),"confirmation defaults on");
    Check(PhongThanRequestDiscardHeldItem()==1 && shows==1 && drops==0,"request never drops before confirmation");
    Answer(0); Check(drops==1,"matching identity confirms once");
    Answer(0); Check(drops==1,"duplicate callback cannot drop twice");
    Reset(); PhongThanRequestDiscardHeldItem(); Answer(1); Check(drops==0,"cancel does not drop");
    Reset(); PhongThanRequestDiscardHeldItem(); ++serial; Answer(0); Check(drops==0,"reused index with different network ID refuses");
    Reset(); PhongThanRequestDiscardHeldItem(); ++held; Answer(0); Check(drops==0,"different held item refuses");
    Reset(); PhongThanRequestDiscardHeldItem(); ++player; Answer(0); Check(drops==0,"different player refuses");
    Reset(); PhongThanRequestDiscardHeldItem(); drag=false; Answer(0); Check(drops==0,"no held item refuses");
    Reset(); PhongThanRequestDiscardHeldItem(); ticks+=30000; Answer(0); Check(drops==0,"timeout refuses");
    Reset(); ticks=0xfffffff0; PhongThanRequestDiscardHeldItem(); ticks=20; Answer(0); Check(drops==1,"tick rollover within timeout accepted");
    Reset(); PhongThanRequestDiscardHeldItem(); PhongThanRequestDiscardHeldItem(); Check(shows==1 && drops==0,"pending request not duplicated");
    Reset(); prepare=false; Check(PhongThanRequestDiscardHeldItem()==1 && !shows && !drops,"resource failure fails closed");
    Reset(); serial=0; Check(PhongThanRequestDiscardHeldItem()==1 && !shows && !drops,"missing item identity fails closed");
    Reset(); player=0; Check(PhongThanRequestDiscardHeldItem()==1 && !shows && !drops,"missing player identity fails closed");
    Reset(); g_pCoreShell=0; Check(PhongThanRequestDiscardHeldItem()==0,"null core safe");
    Reset(); drag=false; Check(PhongThanRequestDiscardHeldItem()==0 && !drops,"normal world clicks not consumed");
    Reset(); s_PtInventoryDiscard.SetEnabled(false); Check(saves==1 && !s_PtInventoryDiscard.Enabled(),"preference saved and applied");
    Check(PhongThanRequestDiscardHeldItem()==1 && drops==1 && !shows,"disabled confirmation preserves direct discard");
    printf("PASS: %d F4 discard checks\n",checks);return 0;
}

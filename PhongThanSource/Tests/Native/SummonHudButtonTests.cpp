#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanCompanionUi.h"
#include "../../Headers/PhongThanStatusPages.h"
enum {WND_N_BUTTON_CLICK=100};
struct KWndWindow{};
struct KWndButton:KWndWindow{};
struct KWndImage:KWndWindow {int WndProc(unsigned int,unsigned int,int){return 7;}};
struct KPtCompanionHud:KWndImage {KWndButton m_AI;int WndProc(unsigned int,unsigned int,int);};
struct Core {
    PT_COMPANION_HUD hud;int sent;PT_STATUS_PAGES_REQUEST request;
    int GetGameData(unsigned int kind,unsigned int result,int size) {
        assert(kind==PT_COMPANION_HUD_GET&&size==sizeof(hud));memcpy((void*)result,&hud,size);return 1;
    }
    int TeamOperation(unsigned int kind,unsigned int source,int size) {
        assert(kind==PT_STATUS_PAGES_ACTION&&size==sizeof(request));request=*(PT_STATUS_PAGES_REQUEST*)source;++sent;return 1;
    }
} core;
Core* g_pCoreShell=&core;
#include "SummonHudButton.inl"
int main() {
    memset(&core,0,sizeof(core));KPtCompanionHud window;
    core.hud.Id=44;core.hud.Locked=1;
    assert(window.WndProc(WND_N_BUTTON_CLICK,(unsigned int)(KWndWindow*)&window.m_AI,0)==1);
    assert(core.sent==1&&core.request.Operation==PT_STATUS_SUMMON_LOCK&&core.request.Value==0);
    core.hud.Locked=0;window.WndProc(WND_N_BUTTON_CLICK,(unsigned int)(KWndWindow*)&window.m_AI,0);
    assert(core.sent==2&&core.request.Value==1);
    core.hud.Id=0;window.WndProc(WND_N_BUTTON_CLICK,(unsigned int)(KWndWindow*)&window.m_AI,0);assert(core.sent==2);
    assert(window.WndProc(0,0,0)==7);
    puts("PASS actual HUD handler: sword toggles original normal/target-lock modes; correct on/off requests; no pet sends nothing");
    return 0;
}

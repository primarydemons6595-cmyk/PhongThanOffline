#include "../../../../Headers/PhongThanF4Protocol.h"
class KPtF4Unlock : public KWndWindow {
    DWORD owner,started;int page,rows;bool pending;
public:
    KPtF4Unlock():owner(0),started(0),page(0),rows(0),pending(false){}
    void Request(int selected)
    {
        if(!g_pCoreShell || selected<1 || selected>3 || g_UiBase.GetStatus()!=UIS_S_IDLE)return;
        PTRoleValues state;memset(&state,0,sizeof(state));
        if(!g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&state,sizeof(state)))return;
        if(pending && GetTickCount()-started<30000)return;
        for(int prior=1;prior<selected;++prior)if(state.BagRows[prior-1]<7) {
            KPtBagConfirmation::Open(PT_F4_PREVIOUS_PAGE);return;
        }
        if(state.BagRows[selected-1]>=7)return;
        bool thick=state.BagRows[selected-1]>=4;
        owner=g_pCoreShell->GetdwIDPlayer(0);started=GetTickCount();page=selected;rows=state.BagRows[selected-1];pending=true;
        if(!KPtBagConfirmation::Open(thick?PT_F4_UNLOCK_THICK_PROMPT:PT_F4_UNLOCK_PROMPT,this))pending=false;
    }
    virtual int WndProc(unsigned int message,unsigned int param,int result)
    {
        if(message!=WND_M_OTHER_WORK_RESULT || param!=0x4634)return 0;
        bool wasPending=pending;pending=false;
        if(!wasPending || result!=0 || !g_pCoreShell || GetTickCount()-started>=30000 ||
           owner!=g_pCoreShell->GetdwIDPlayer(0) || g_UiBase.GetStatus()!=UIS_S_IDLE)return 0;
        g_pCoreShell->OperationRequest(PT_GOI_F4,PT_F4_UNLOCK,page|(rows<<8));return 0;
    }
};
static KPtF4Unlock s_PtF4Unlock;

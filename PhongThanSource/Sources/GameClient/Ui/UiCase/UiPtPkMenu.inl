#include "../../../../Headers/PhongThanPkContract.h"
#include "../../../Engine/Src/Text.h"
#include "UiInformation.h"
#include "../../../../Headers/PhongThanPkText.h"

void KUiToolsControlBar::SwitchPK()
{
    s_PtPkConfirmPending = false;
    if (!g_pCoreShell || !m_pSelf) return;
    KIniFile ini;
    int count = 0;
    if (!ini.Load(PHONGTHAN_VNG_TOOLBAR_INI)) return;
    ini.GetInteger("PK","ItemCount",0,&count);
    if (count != 6) return;
    KPopupMenuData* menu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(count));
    if (!menu) return;
    KPopupMenu::InitMenuData(menu,count);
    menu->nNumItem = count;
    menu->usMenuFlag |= PM_F_COLOR_SWATCH;
    menu->nItemTitleIndent = 26;
    menu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
    // Original row colors already encode text color; no heavy black outline.
    menu->uTextBorderColor=0; menu->uSelTextBorderColor=0;
    for (int i=0; i<count; ++i) {
        char key[32], label[64]; int r,g,b;
        sprintf(key,"MenuText_%d",i);
        if (!ini.GetString("PK",key,"",label,sizeof(label)) || !label[0]) { free(menu); return; }
        sprintf(key,"Color_%d",i);
        ini.GetInteger3("PK",key,&r,&g,&b);
        const char* text = label[0]=='$' ? label+1 : label;
        int len = strlen(text);
        if (len > (int)sizeof(menu->Items[i].szData)-6) { free(menu); return; }
        menu->Items[i].szData[0] = KTC_COLOR;
        menu->Items[i].szData[1] = (char)r;
        menu->Items[i].szData[2] = (char)g;
        menu->Items[i].szData[3] = (char)b;
        memcpy(menu->Items[i].szData+4,text,len);
        menu->Items[i].szData[4+len] = KTC_COLOR_RESTORE;
        menu->Items[i].szData[5+len] = 0;
        menu->Items[i].uDataLen = len+5;
    }
    int x,y,left,top;
    m_pSelf->GetAbsolutePos(&x,&y);
    ini.GetInteger("PK","Left",0,&left); ini.GetInteger("PK","Top",0,&top);
    x+=left; y+=top;
    menu->nX=(short)x; menu->nY=(short)y;
    KPopupMenu::Popup(menu,m_pSelf,0);
}

void KUiToolsControlBar::SwitchFastPK() { SwitchPK(); }

void KUiToolsControlBar::SwitchPK(int row)
{
    s_PtPkConfirmPending = false;
    int camp = PtPkMenuCamp(row);
    if (!g_pCoreShell || camp < 0) return;
    if (camp == 8) {
        KIniFile ini; char prompt[64];
        if (!ini.Load(PHONGTHAN_VNG_TOOLBAR_INI) ||
            !ini.GetString("PK","MenuText_0","",prompt,sizeof(prompt)) || !prompt[0]) return;
        if (!g_UiInformation.PreparePtPkConfirmation()) return;
        s_PtPkConfirmPending = true;
        // Keep explicit slaughter confirmation; reuse authored PT label/default OK.
        UIMessageBox(prompt[0]=='$' ? prompt+1 : prompt,m_pSelf,PT_PK_OK,PT_PK_CANCEL,0x7D1);
        return;
    }
    g_pCoreShell->OperationRequest(GOI_PK_SETTING,0,camp);
}

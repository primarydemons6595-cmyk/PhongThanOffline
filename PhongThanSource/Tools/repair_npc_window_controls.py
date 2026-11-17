"""Ensure service windows can always release their input capture."""
from repair_npc_dialog_exit import change
change('Sources/GameClient/Ui/UiCase/UiMsgSel.h','#include "../Elem/WndText.h"','#include "../Elem/WndText.h"\n#include "../Elem/WndButton.h"')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.h','    KWndImage m_Image;','    KWndImage m_Image;\n    KWndButton m_CloseBtn;')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp','    AddChild(&m_MsgScrollList);','    AddChild(&m_CloseBtn);\n    AddChild(&m_MsgScrollList);')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp','        m_pSelf->Init(&Ini, "Main");','        m_pSelf->Init(&Ini, "Main");\n        m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp','        case WM_MOUSEMOVE: {',
       '        case WND_N_BUTTON_CLICK:\n            if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn) { OnClickMsg(-1); return 1; }\n            break;\n        case WM_MOUSEMOVE: {')
change('Sources/GameClient/Ui/UiCase/UiStoreBox.cpp','    switch (uMsg) {\n        case WND_N_ITEM_PICKDROP:',
       '    switch (uMsg) {\n        case WM_KEYDOWN:\n            if(uParam==VK_ESCAPE) { CloseWindow(); return 1; }\n            break;\n        case WND_N_ITEM_PICKDROP:')
change('Sources/GameClient/Ui/UiCase/UiStoreBox.cpp','        m_pSelf->Show();\n        Wnd_GameSpaceHandleInput(false);',
       '        if(m_pSelf->m_Width<=0 || m_pSelf->m_Height<=0) { CloseWindow(); return NULL; }\n        m_pSelf->Show();\n        Wnd_GameSpaceHandleInput(false);')
print('NPC close button and warehouse escape/uninitialized-layout guard updated.')

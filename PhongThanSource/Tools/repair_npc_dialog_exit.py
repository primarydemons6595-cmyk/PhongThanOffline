"""Preserve legacy source encodings while adding a common NPC cancel action."""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
def change(relative, old, new):
    path=ROOT/relative
    raw=path.read_bytes()
    crlf=raw.count(b'\r\n') > raw.count(b'\n')//2
    data=raw.replace(b'\r\n',b'\n')
    old=old.encode('ascii')
    new=new.encode('ascii')
    if new in data:return
    if data.count(old)!=1:raise RuntimeError(f'Expected one match: {relative}: {old!r}')
    data=data.replace(old,new)
    path.write_bytes(data.replace(b'\n',b'\r\n') if crlf else data)

change('Sources/GameClient/Ui/UiCase/UiMsgSel.h',
       '    unsigned int m_uLeftTime;', '    int m_AnswerCount; // Local close row is never a script callback.\n    unsigned int m_uLeftTime;')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp',
       '    m_uLeftTime = 0;', '    m_AnswerCount = 0;\n    m_uLeftTime = 0;')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp',
       '            if (uParam == VK_RETURN) {',
       '            if (uParam == VK_ESCAPE) {\n                OnClickMsg(-1);\n                return 1;\n            } else if (uParam == VK_RETURN) {')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp',
       '        m_pSelf->CloseWindow(true);\n        g_pCoreShell->OperationRequest(GOI_QUESTION_CHOOSE, 0, nMsg);',
       '        // Translate the appended local exit row to the native cancel value.\n        const int selection = nMsg >= 0 && nMsg < m_pSelf->m_AnswerCount ? nMsg : -1;\n        m_pSelf->CloseWindow(true);\n        if (g_pCoreShell) g_pCoreShell->OperationRequest(GOI_QUESTION_CHOOSE, 0, selection);')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp',
       '    KWndShowAnimate::Show();\n    Wnd_SetExclusive((KWndWindow *) this);',
       '    m_AnswerCount = pQAA->AnswerCount;\n    if (m_AnswerCount > 0)\n        m_MsgScrollList.GetMessageListBox()->AddOneMessage("Thoat (Esc)", -1);\n    KWndShowAnimate::Show();\n    Wnd_SetExclusive((KWndWindow *) this);')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp',
       '        if (m_uLeftTime == 0)\n            CloseWindow(true);',
       '        if (m_uLeftTime == 0) {\n            OnClickMsg(-1);\n            return; // CloseWindow destroys this instance.\n        }')
change('Sources/GameClient/Ui/UiCase/UiMsgSel.cpp',
       '    return 0;\n}\n\n//--------------------------------------------------------------------------\n//',
       '    return nRet;\n}\n\n//--------------------------------------------------------------------------\n//')
change('Sources/Core/Src/PhongThanPlayerDialog.inl',
       '\t\tm_UiDialogToken = 0;\n\t\treturn;',
       '\t\tm_UiDialogToken = 0;\n\t\tm_UiDialogView = 0;\n\t\treturn;')
print('NPC dialogue: local exit row, Esc, expiry cancellation and handled key propagation updated.')

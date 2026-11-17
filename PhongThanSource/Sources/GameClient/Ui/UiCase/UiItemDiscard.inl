#ifndef PHONGTHAN_DISCARD_TEST
#include "UiInformation.h"
#endif
#include "../../../Core/Src/PhongThanInventoryUi.h"

// This callback object outlives the F4 window. Never keep a pointer to a
// possibly destroyed inventory window as the confirmation recipient.
class KPtInventoryDiscard : public KWndWindow
{
    bool m_Loaded, m_Enabled, m_Pending;
    unsigned int m_Index, m_NetworkId;
    DWORD m_PlayerId, m_Started;
public:
    KPtInventoryDiscard() : m_Loaded(false), m_Enabled(true), m_Pending(false),
        m_Index(0), m_NetworkId(0), m_PlayerId(0), m_Started(0) {}
    bool Enabled()
    {
        if (!m_Loaded)
        {
            KIniFile ini;
            int enabled = 1;
            if (ini.Load("\\Settings\\PhongThanInventoryUi.ini"))
                ini.GetInteger("Inventory", "BlockDiscard", 1, &enabled);
            m_Enabled = enabled != 0;
            m_Loaded = true;
        }
        return m_Enabled;
    }
    void SetEnabled(bool enabled)
    {
        m_Loaded = true;
        m_Enabled = enabled;
        KIniFile ini;
        ini.Load("\\Settings\\PhongThanInventoryUi.ini");
        ini.WriteInteger("Inventory", "BlockDiscard", enabled ? 1 : 0);
        if (!ini.Save("\\Settings\\PhongThanInventoryUi.ini"))
            g_DebugLog("F4_DISCARD_PREFERENCE_SAVE_FAILED");
    }
    int Request()
    {
        if (!g_pCoreShell)
            return 0;
        KUiDraggedObject item;
        memset(&item, 0, sizeof(item));
        if (!Wnd_GetDragObj(&item) || item.uGenre != CGOG_ITEM || !item.uId)
            return 0;
        if (Enabled())
            return 1;
        return g_pCoreShell->ThrowAwayItem();
        const unsigned int serial = (unsigned int)g_pCoreShell->GetGameData(
            PHONGTHAN_GDI_ITEM_NETWORK_ID, 0, item.uId);
        const DWORD player = g_pCoreShell->GetdwIDPlayer(0);
        // Consume the click fail-closed if identity or dialog resources fail.
        if (!serial || !player)
            return 1;
        if (m_Pending && GetTickCount() - m_Started < 30000)
            return 1;
        if (!g_UiInformation.PreparePtPkConfirmation())
            return 1;
        m_Index = item.uId;
        m_NetworkId = serial;
        m_PlayerId = player;
        m_Started = GetTickCount();
        m_Pending = true;
        // Explicit TCVN3 bytes. The new source remains ASCII-only.
        UIMessageBox(
            "\x42\xB9\x6E\x20\x63\xE3\x20\x6D\x75\xE8\x6E\x20\x76\xF8\x74\x20\x62\xE1\x20\x76\xCB\x74\x20\x70\x68\xC8\x6D\x20\xAE\x61\x6E\x67\x20\x63\xC7\x6D\x20\x6B\x68\xAB\x6E\x67\x3F",
            this, "\x58\xB8\x63\x20\x6E\x68\xCB\x6E", "\x48\xF1\x79\x20\x62\xE1", 1, 30);
        return 1;
    }
    int WndProc(unsigned int message, unsigned int param, int result)
    {
        if (message != WND_M_OTHER_WORK_RESULT || param != 1)
            return 0;
        const bool pending = m_Pending;
        m_Pending = false;
        if (!pending || result != 0 || !g_pCoreShell ||
            GetTickCount() - m_Started >= 30000 ||
            g_pCoreShell->GetdwIDPlayer(0) != m_PlayerId)
            return 0;
        KUiDraggedObject item;
        memset(&item, 0, sizeof(item));
        if (!Wnd_GetDragObj(&item) || item.uGenre != CGOG_ITEM || item.uId != m_Index)
            return 0;
        const unsigned int serial = (unsigned int)g_pCoreShell->GetGameData(
            PHONGTHAN_GDI_ITEM_NETWORK_ID, 0, item.uId);
        if (serial == m_NetworkId)
            g_pCoreShell->ThrowAwayItem();
        return 0;
    }
};
static KPtInventoryDiscard s_PtInventoryDiscard;
int PhongThanRequestDiscardHeldItem() { return s_PtInventoryDiscard.Request(); }

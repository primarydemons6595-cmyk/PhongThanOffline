// ASCII source; menu text is explicitly encoded in the game's TCVN3 bytes.
static const char* PhongThanPeopleActionName(int action)
{
    static const char* names[] = {
        "\x4D\xCB\x74",
        "\x54\x68\xAA\x6D\x20\x68\xB6\x6F\x20\x68\xF7\x75",
        "\x47\x69\x61\x6F\x20\x64\xDE\x63\x68",
        "\x58\x69\x6E\x20\x76\xB5\x6F\x20\x6E\x68\xE3\x6D",
        "\x54\xE6\x20\xAE\xE9\x69",
        "\x54\x68\x65\x6F\x20\x73\x61\x75",
        "\x58\x65\x6D\x20\x74\x72\x61\x6E\x67\x20\x62\xDE",
        "\x54\x68\xAA\x6D\x20\x76\xB5\x6F\x20\x73\xE6\x20\x62\xD7\x61\x20\xAE\x65\x6E",
        "\x42\x61\x6E\x67\x20\x68\xE9\x69",
        "\x58\x69\x6E\x20\x76\xB5\x6F\x20\x62\x61\x6E\x67",
        "\x4D\xEA\x69\x20\x76\xB5\x6F\x20\x62\x61\x6E\x67",
        "\x43\xF5\x75\x20\x73\xB8\x74",
        "\x4F\xBC\x6E\x20\x74\xEF\x20\x74\xD7",
        "\x47\xF6\x69\x20\x62\x69\xD3\x75\x20\x63\xB6\x6D"
    };
    static const char* extra[]={"Göi b­u kiÖn","Tra xem täa ®é","NhËn lµm ®Ö tö","H¶o h÷u t¹m thêi","B¸o c¸o"};
    if(action>=15&&action<=19)return extra[action-15];
    return action == 14 ? "\x48\xF1\x79\x20\x63\x68\xE4\x6E" : (action >= 0 && action <= ACTION_CONTEXT_FACE ? names[action] : "");
}

bool PhongThanOpenPlayerContextMenu(int x, int y)
{
    if (!g_pCoreShell || g_UiBase.GetStatus() != UIS_S_IDLE ||
        g_pCoreShell->CheckMouse())
        return false;
    KUiPlayerItem player;
    memset(&player, 0, sizeof(player));
    player.nIndex = -1;
    int kind = -1;
    if (!g_pCoreShell->FindSelectNPC(x, y,
            relation_none | relation_ally | relation_enemy, false,
            &player, kind, true))
        return false;
    player.Name[sizeof(player.Name) - 1] = 0;
    if (kind != kind_player || player.nIndex <= 0 || !player.uId ||
        !player.Name[0] || KUiPlayerBar::IsSelfName(player.Name))
        return false;
    g_DebugLog("PLAYER_CONTEXT_OPEN id=%u index=%d x=%d y=%d",
        player.uId, player.nIndex, x, y);
    g_pCoreShell->OperationRequest(PT_PLAYER_TARGET_SELECT,player.nIndex,(int)player.uId);
    PopUpContextPeopleMenu(player, x, y);
    return true;
}

static bool PhongThanHandlePlayerContextChord(unsigned int message,
    unsigned int keys, int coordinates)
{
    if ((message != WM_RBUTTONDOWN && message != WM_RBUTTONDBLCLK) ||
        !(keys & MK_CONTROL) || (keys & MK_SHIFT) ||
        (GetKeyState(VK_MENU) & 0x8000))
        return false;
    // Consume this UI chord even with no target or a held inventory object.
    // It must never fall through to right-skill, movement or item discard.
    PhongThanOpenPlayerContextMenu((short)LOWORD(coordinates),
        (short)HIWORD(coordinates));
    return true;
}

static void PhongThanProcessPlayerMenuAction(KUiPlayerItem* selected, int action)
{
    if (!selected || action < ACTION_CHAT || action > 19 ||
        !g_pCoreShell || g_UiBase.GetStatus() != UIS_S_IDLE ||
        g_pCoreShell->CheckMouse())
        return;
    if(selected->nIndex==-2){PhongThanRemotePeopleBegin(*selected,action);return;}
    KUiPlayerItem player = *selected;
    player.Name[sizeof(player.Name) - 1] = 0;
    if (!player.Name[0] || KUiPlayerBar::IsSelfName(player.Name)) return;
    if (action != ACTION_CHAT && action != ACTION_MAKEFRIEND &&
        action != ACTION_BLACKLIST && action != 18)
    {
        // A menu can remain open while an NPC slot is reused. Resolve the
        // same network identity again, refreshing menu/trade state as well.
        KUiPlayerItem current;
        memset(&current, 0, sizeof(current));
        current.nIndex = -1;
        int kind = -1;
        if (!player.uId ||
            !g_pCoreShell->FindSpecialNPC(player.Name, &current, kind) ||
            kind != kind_player || current.uId != player.uId || current.nIndex <= 0)
        {
            g_DebugLog("PLAYER_CONTEXT_STALE action=%d id=%u", action, player.uId);
            return;
        }
        current.Name[sizeof(current.Name) - 1] = 0;
        player = current;
    }
    g_DebugLog("PLAYER_CONTEXT_ACTION action=%d id=%u index=%d",
        action, player.uId, player.nIndex);
    ProcessPeople(&player, action);
}

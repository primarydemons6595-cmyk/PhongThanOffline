#include "../../../../Headers/PhongThanRealmUi.h"
// F4 actions use the existing item movement protocol. No local item mutation.
bool KUiItem::IsInventoryPageAccessible() const
{
    PTRoleValues role;memset(&role,0,sizeof(role));
    return g_pCoreShell && (m_nInventoryPage==0 ||
        (g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role)) && role.BagRows[m_nInventoryPage-1]>0));
}

void KUiItem::RefreshF4Availability()
{
    if (!g_pCoreShell)
        return;
    m_PtF4.UpdateValues(g_pCoreShell);
    // A locked tab must still be selectable, as in the original PT UI.
    // Selecting a tab does not authorize access to its storage.
    for (int page = 0; page < 4; ++page)
        m_PageBtn[page].Enable(TRUE);
    PTRoleValues realm;memset(&realm,0,sizeof(realm));
    if(g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&realm,sizeof(realm))){
        const bool unlocked=PhongThanRealmUnlocked(realm.RealmStage,realm.RealmSide,realm.RealmLevel);
        m_JEvilEquipBtn.Enable(unlocked);
        if(unlocked)m_JEvilEquipBtn.Show();else m_JEvilEquipBtn.Hide();
        m_NormalEquipBtn.CheckButton(realm.EquipmentRealm==0);
        m_JEvilEquipBtn.CheckButton(realm.EquipmentRealm==1);
        UpdateAllEquips();
    }else{m_JEvilEquipBtn.Hide();m_JEvilEquipBtn.Enable(false);}
    const bool locked = !IsInventoryPageAccessible();
    if (locked != m_PtF4.locked)
    {
        m_PtF4.SetLocked(locked);
        UpdateData();
    }
    PTRoleValues role;memset(&role,0,sizeof(role));
    if(g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role)))
        m_PtF4.SetRows(m_nInventoryPage?role.BagRows[m_nInventoryPage-1]:7);
    bool canPick = g_UiBase.GetStatus() == UIS_S_IDLE;
    if (g_UiBase.GetStatus() == UIS_S_TRADE_PLAYER)
    {
        // Every carried page uses the same server-validated trade state.
        canPick =
            !g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_LOCKED, 0) &&
            !g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_TRADING, 0) &&
            !g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_OTHER_TRADING, 0);
    }
    // NPC sale/repair modes deliberately use click notifications, not pick/put.
    m_ItemBox.EnablePickPut(!locked && canPick && !g_pCoreShell->GetTradeState());
}

void KUiItem::ApplyInventoryDelta(KUiObjAtRegion* item, int add)
{
    m_InventoryDirty=true;
    if (!g_pCoreShell)
        return;
    if (!IsInventoryPageAccessible())
        return;
    if (!item)
    {
        UpdateData();
        return;
    }
    KUiDraggedObject obj;
    memset(&obj, 0, sizeof(obj));
    obj.uGenre = item->Obj.uGenre;
    obj.uId = item->Obj.uId;
    obj.DataX = item->Region.h;
    obj.DataY = item->Region.v;
    obj.DataW = item->Region.Width;
    obj.DataH = item->Region.Height;
    if (add)
        m_ItemBox.AddObject(&obj, 1);
    else
        m_ItemBox.RemoveObject(&obj);
    UiSoundPlay(UI_SI_PICKPUT_ITEM);
}

bool KUiItem::HandleEquipRightClick(KWndWindow* source)
{
    const int box = GetEquipBoxIndex(source);
    if (box < 0)
        return false;
    if(g_pCoreShell && g_UiBase.GetStatus()==UIS_S_TRADE_REPAIR){
        KUiDraggedObject repair;memset(&repair,0,sizeof(repair));
        if(m_EquipBox[box].GetObject(repair))OnRepairItem(&repair,true);
        return true;
    }
    // Never sell, split, use, or compose an equipped object as an inventory item.
    if (!g_pCoreShell || g_UiBase.GetStatus() != UIS_S_IDLE ||
        g_pCoreShell->GetTradeState())
        return true;
    if (!IsInventoryPageAccessible())
        return true;
    KUiDraggedObject hand;
    memset(&hand, 0, sizeof(hand));
    Wnd_GetDragObj(&hand);
    if (hand.uGenre != CGOG_NOTHING && hand.uId)
        return true;
    KUiDraggedObject equip;
    memset(&equip, 0, sizeof(equip));
    if (!m_EquipBox[box].GetObject(equip) || equip.uGenre != CGOG_ITEM ||
        equip.DataW <= 0 || equip.DataH <= 0 ||
        equip.DataW > EQUIPMENT_ROOM_WIDTH || equip.DataH > EQUIPMENT_ROOM_HEIGHT)
        return true;

    const int query = PT_GDI_BAG_FIRST+m_nInventoryPage;
    const int capacity = EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT;
    KUiObjAtRegion items[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];
    memset(items, 0, sizeof(items));
    const int expected = g_pCoreShell->GetGameData(query, 0, 0);
    if (expected < 0 || expected > capacity)
        return true;
    const int count = expected ? g_pCoreShell->GetGameData(query, (unsigned int)items, capacity) : 0;
    if (count != expected)
        return true;
    bool occupied[EQUIPMENT_ROOM_HEIGHT][EQUIPMENT_ROOM_WIDTH];
    memset(occupied, 0, sizeof(occupied));
    for (int i = 0; i < count; ++i)
    {
        const KUiRegion& region = items[i].Region;
        if (region.h < 0 || region.v < 0 || region.Width <= 0 || region.Height <= 0 ||
            region.h > EQUIPMENT_ROOM_WIDTH || region.v > EQUIPMENT_ROOM_HEIGHT ||
            region.Width > EQUIPMENT_ROOM_WIDTH || region.Height > EQUIPMENT_ROOM_HEIGHT ||
            region.h + region.Width > EQUIPMENT_ROOM_WIDTH ||
            region.v + region.Height > EQUIPMENT_ROOM_HEIGHT)
            return true;
        for (int y = region.v; y < region.v + region.Height; ++y)
            for (int x = region.h; x < region.h + region.Width; ++x)
                occupied[y][x] = true;
    }
    for (int top = 0; top <= EQUIPMENT_ROOM_HEIGHT - equip.DataH; ++top)
        for (int left = 0; left <= EQUIPMENT_ROOM_WIDTH - equip.DataW; ++left)
        {
            bool free = true;
            for (int y = top; y < top + equip.DataH && free; ++y)
                for (int x = left; x < left + equip.DataW; ++x)
                    if (occupied[y][x]) { free = false; break; }
            if (!free)
                continue;
            KUiObjAtContRegion from, to;
            memset(&from, 0, sizeof(from));
            memset(&to, 0, sizeof(to));
            from.Obj.uGenre = to.Obj.uGenre = equip.uGenre;
            from.Obj.uId = to.Obj.uId = equip.uId;
            from.Region.v = EquipCtrlMap[box].nPosition;
            from.Region.Width = to.Region.Width = equip.DataW;
            from.Region.Height = to.Region.Height = equip.DataH;
            from.eContainer = UOC_EQUIPTMENT;
            to.eContainer = GetInventoryContainer();
            to.Region.h = left;
            to.Region.v = top;
            g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)&from, (int)&to);
            return true;
        }
    return true;
}

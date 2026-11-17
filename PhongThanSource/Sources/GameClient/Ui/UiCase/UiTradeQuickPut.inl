bool KUiTrade::PutInventoryItem(const KUiDraggedObject* item,int container)
{
    if (!g_pCoreShell || !item || item->uGenre != CGOG_ITEM || !item->uId ||
        g_UiBase.GetStatus() != UIS_S_TRADE_PLAYER ||
        g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_LOCKED, 0) ||
        g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_TRADING, 0) ||
        g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_OTHER_TRADING, 0))
        return false;
    KUiDraggedObject hand;
    memset(&hand, 0, sizeof(hand));
    Wnd_GetDragObj(&hand);
    if (hand.uGenre != CGOG_NOTHING && hand.uId) return false;
    KUiObjAtContRegion from, to;
    memset(&from, 0, sizeof(from)); memset(&to, 0, sizeof(to));
    if (!m_SelfItemsBox.FindFreePlace(item->DataW, item->DataH, to.Region.h, to.Region.v))
        return false;
    from.Obj.uGenre = to.Obj.uGenre = item->uGenre;
    from.Obj.uId = to.Obj.uId = item->uId;
    from.Region.h = item->DataX; from.Region.v = item->DataY;
    from.Region.Width = to.Region.Width = item->DataW;
    from.Region.Height = to.Region.Height = item->DataH;
    if(container!=UOC_ITEM_TAKE_WITH&&container!=UOC_ITEM_TAKE_WITH_EX&&container!=UOC_PT_BAG2&&container!=UOC_PT_BAG3)return false;
    from.eContainer = (UIOBJECT_CONTAINER)container;
    to.eContainer = UOC_TO_BE_TRADE;
    // The existing atomic cross-container move resolves through the server hand.
    // Never remove/add UI objects speculatively; authoritative deltas update F4.
    g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT, (unsigned int)&from, (int)&to);
    return true;
}

bool KUiTrade::ReturnInventoryItem(const KUiDraggedObject* item)
{
    if(!g_pCoreShell||!item||item->uGenre!=CGOG_ITEM||!item->uId||
       g_UiBase.GetStatus()!=UIS_S_TRADE_PLAYER||
       g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA,UTOD_IS_LOCKED,0)||
       g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA,UTOD_IS_TRADING,0)||
       g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA,UTOD_IS_OTHER_TRADING,0))return false;
    KUiDraggedObject hand;memset(&hand,0,sizeof(hand));Wnd_GetDragObj(&hand);
    if(hand.uGenre!=CGOG_NOTHING&&hand.uId)return false;
    KUiItem* inventory=KUiItem::GetIfVisible();if(!inventory)return false;
    const int containers[]={UOC_ITEM_TAKE_WITH,UOC_ITEM_TAKE_WITH_EX,UOC_PT_BAG2,UOC_PT_BAG3};
    int selected=0;for(int i=0;i<4;++i)if(containers[i]==inventory->CurrentInventoryContainer())selected=i;
    PTRoleValues role;memset(&role,0,sizeof(role));
    if(!g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role)))return false;
    // Prefer the currently displayed bag, then another unlocked carried bag.
    for(int step=0;step<4;++step){int page=(selected+step)%4,rows=page?role.BagRows[page-1]:7;
        if(rows<1||rows>EQUIPMENT_ROOM_HEIGHT)continue;
        KUiObjAtRegion objects[EQUIPMENT_ROOM_WIDTH*EQUIPMENT_ROOM_HEIGHT];memset(objects,0,sizeof(objects));
        int query=PT_GDI_BAG_FIRST+page,capacity=sizeof(objects)/sizeof(objects[0]);
        int expected=g_pCoreShell->GetGameData(query,0,0);if(expected<0||expected>capacity)continue;
        int count=expected?g_pCoreShell->GetGameData(query,(unsigned int)objects,capacity):0;if(count!=expected)continue;
        bool occupied[EQUIPMENT_ROOM_HEIGHT][EQUIPMENT_ROOM_WIDTH];memset(occupied,0,sizeof(occupied));bool valid=true;
        for(int n=0;n<count;++n){KUiRegion& z=objects[n].Region;
            if(z.h<0||z.v<0||z.Width<=0||z.Height<=0||z.h+z.Width>EQUIPMENT_ROOM_WIDTH||z.v+z.Height>rows){valid=false;break;}
            for(int y=z.v;y<z.v+z.Height;++y)for(int x=z.h;x<z.h+z.Width;++x)occupied[y][x]=true;
        }
        if(!valid)continue;
        for(int top=0;top<=rows-item->DataH;++top)for(int left=0;left<=EQUIPMENT_ROOM_WIDTH-item->DataW;++left){
            bool free=true;for(int y=top;y<top+item->DataH&&free;++y)for(int x=left;x<left+item->DataW;++x)if(occupied[y][x]){free=false;break;}
            if(!free)continue;
            KUiObjAtContRegion from,to;memset(&from,0,sizeof(from));memset(&to,0,sizeof(to));
            from.Obj.uGenre=to.Obj.uGenre=item->uGenre;from.Obj.uId=to.Obj.uId=item->uId;
            from.Region.h=item->DataX;from.Region.v=item->DataY;
            from.Region.Width=to.Region.Width=item->DataW;from.Region.Height=to.Region.Height=item->DataH;
            from.eContainer=UOC_TO_BE_TRADE;to.eContainer=(UIOBJECT_CONTAINER)containers[page];to.Region.h=left;to.Region.v=top;
            g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,(unsigned int)&from,(int)&to);return true;
        }
    }
    return false;
}

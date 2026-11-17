// Existing item movement request remains authoritative on the server.
bool KUiStoreBox::PutInventoryItem(KUiDraggedObject* item, UIOBJECT_CONTAINER container)
{
    if(!item || !g_pCoreShell || g_UiBase.GetStatus()!=UIS_S_IDLE ||
        g_pCoreShell->GetTradeState() || item->uGenre!=CGOG_ITEM ||
        !PageAccessible())return false;
    KUiDraggedObject hand;memset(&hand,0,sizeof(hand));Wnd_GetDragObj(&hand);
    if(hand.uGenre!=CGOG_NOTHING && hand.uId)return false;
    int x=0,y=0;
    if(!m_ItemBox.FindFreePlace(item->DataW,item->DataH,x,y))return false;
    KUiObjAtContRegion from,to;memset(&from,0,sizeof(from));memset(&to,0,sizeof(to));
    from.Obj.uGenre=to.Obj.uGenre=item->uGenre;from.Obj.uId=to.Obj.uId=item->uId;
    from.Region.h=item->DataX;from.Region.v=item->DataY;
    from.Region.Width=to.Region.Width=item->DataW;from.Region.Height=to.Region.Height=item->DataH;
    to.Region.h=x;to.Region.v=y;from.eContainer=container;to.eContainer=GetPageContainer();
    return g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,(unsigned int)&from,(int)&to)!=0;
}

UIOBJECT_CONTAINER KUiStoreBox::GetPageContainer() const
{
    switch(m_nCurPage){
    case REPOSITORY_ONE:return UOC_STORE_BOX1;
    case REPOSITORY_TWO:return UOC_STORE_BOX2;
    case REPOSITORY_THREE:return UOC_STORE_BOX3;
    case REPOSITORY_FOUR:return UOC_STORE_BOX4;
    case REPOSITORY_FIVE:return UOC_STORE_BOX5;
    default:return UOC_STORE_BOX;
    }
}

bool KUiItem::PutExternalItem(KUiDraggedObject* item, UIOBJECT_CONTAINER container)
{
    if(!item || !g_pCoreShell || !IsInventoryPageAccessible() ||
        g_UiBase.GetStatus()!=UIS_S_IDLE || g_pCoreShell->GetTradeState() || item->uGenre!=CGOG_ITEM)return false;
    KUiDraggedObject hand;memset(&hand,0,sizeof(hand));Wnd_GetDragObj(&hand);
    if(hand.uGenre!=CGOG_NOTHING && hand.uId)return false;
    int x=0,y=0;
    if(!m_ItemBox.FindFreePlace(item->DataW,item->DataH,x,y))return false;
    KUiObjAtContRegion from,to;memset(&from,0,sizeof(from));memset(&to,0,sizeof(to));
    from.Obj.uGenre=to.Obj.uGenre=item->uGenre;from.Obj.uId=to.Obj.uId=item->uId;
    from.Region.h=item->DataX;from.Region.v=item->DataY;
    from.Region.Width=to.Region.Width=item->DataW;from.Region.Height=to.Region.Height=item->DataH;
    to.Region.h=x;to.Region.v=y;from.eContainer=container;to.eContainer=GetInventoryContainer();
    return g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,(unsigned int)&from,(int)&to)!=0;
}

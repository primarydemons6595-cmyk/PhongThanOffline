// UI-only commerce links resolve the endpoint from synchronized task/cargo state.
static int PTCommerceQuery(PTCommerceRouteInfo& info,int origin){return g_pCoreShell&&g_pCoreShell->OperationRequest(PT_GOI_COMMERCE_ROUTE_INFO,(unsigned int)&info,origin);}
static void PTCommerceRouteLabel(const PTCommerceRouteInfo& info,char* text,int capacity){
 const char* prefix=info.Shop==info.Origin?"\126\322\040\147\151\141\157\072\040":"\247\325\156\040\155\165\141\072\040";
 _snprintf(text,capacity-1,"%s%s",prefix,PT_COMMERCE_DESTINATIONS[info.Shop-1].Name);text[capacity-1]=0;
}
static void PTCommerceNavigate(int origin){PTCommerceRouteInfo info;if(PTCommerceQuery(info,origin))KUiMiniMap::NavigateToNpc(info.Map,info.X,info.Y);}
void KUiTaskNote::RefreshTradeLinks(){
 PTCommerceRouteInfo info;bool show=m_Selected==3090&&PTCommerceQuery(info,0);int w,h;m_TaskInfo.GetSize(&w,&h);m_TaskInfo.SetSize(w,show?max(20,m_TradeInfoHeight-32):m_TradeInfoHeight);
 if(show){m_TradeBuy.SetText("\247\325\156\040\124\150\255\254\156\147\040\247\151\325\155");m_TradeReturn.SetText("\126\322\040\147\151\141\157\040\150\265\156\147");m_TradeBuy.Show();m_TradeReturn.Show();}else{m_TradeBuy.Hide();m_TradeReturn.Hide();}
}

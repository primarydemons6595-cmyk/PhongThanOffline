// Quest exchanges are rendered by the original UiShop and original purchase dialog.
#include "../Elem/MouseHover.h"
#include "../../../Core/Src/PhongThanStudioQuestServiceText.h"
#include "../../../Engine/Src/Text.h"
void KUiShop::QuestIcon::SetOffer(const PTEventShopRow& row){goods=row.Id;int w,h;GetSize(&w,&h);SetImage(ISI_T_SPR,row.Image,true);SetSize(w,h);SetFrame(0);}
int KUiShop::QuestIcon::WndProc(unsigned int msg,unsigned int param,int value){
 int result=KWndButton::WndProc(msg,param,value);
 if((msg==WM_MOUSEMOVE||msg==WM_MOUSEHOVER)&&!g_MouseOver.IsMoseHoverWndObj(this,0)){
  PTEventShopTooltip tip;if(!goods||!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_TOOLTIP,(unsigned int)&tip,goods))return result;
  g_MouseOver.CancelMouseHoverInfo();g_MouseOver.SetMouseHoverInfo(this,0,LOWORD(value),HIWORD(value),false,false);IR_InitUiImageRef(g_MouseOver.m_HoverImage);lstrcpynA(g_MouseOver.m_HoverImage.szImage,tip.Image,sizeof(g_MouseOver.m_HoverImage.szImage));memset(&g_MouseOver.m_SocketVisuals,0,sizeof(g_MouseOver.m_SocketVisuals));g_MouseOver.SetMouseHoverImage(tip.ImageFlags!=0,(tip.ImageFlags&2)!=0,(tip.ImageFlags&4)!=0);
  int length=TEncodeText(tip.Title,strlen(tip.Title));g_MouseOver.SetMouseHoverTitle(tip.Title,length,0xffffffff);length=TEncodeText(tip.Properties,strlen(tip.Properties));g_MouseOver.SetMouseHoverProp(tip.Properties,length,0xffffffff);length=TEncodeText(tip.Description,strlen(tip.Description));g_MouseOver.SetMouseHoverDesc(tip.Description,length,0xffffffff);
 }return result;
}
void KUiShop::InitQuestLayout(KIniFile* ini){
 for(int i=0;i<14;++i){char section[32];sprintf(section,"ItemBox_%d",i);int x,y,ix,iy,w,h;ini->GetInteger(section,"Left",0,&x);ini->GetInteger(section,"Top",0,&y);ini->GetInteger(section,"ImageLeft",0,&ix);ini->GetInteger(section,"ImageTop",0,&iy);ini->GetInteger(section,"ImageWidth",32,&w);ini->GetInteger(section,"ImageHeight",32,&h);m_QuestIcons[i].goods=0;m_QuestIcons[i].Init(ini,section);m_QuestIcons[i].SetPosition(x+ix,y+iy);m_QuestIcons[i].SetSize(w,h);m_QuestIcons[i].Hide();}
}
void KUiShop::OpenQuestShop(){
 PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data))||!(data.EventId&0x40000000UL))return;
 if(m_pSelf&&m_pSelf->m_bQuestShop){m_pSelf->UpdateQuestShop(true);return;}
 KUiShop* shop=OpenWindow();if(!shop)return;shop->m_bQuestShop=true;shop->m_nCurrentPage=0;shop->UpdateQuestShop(true);
}
void KUiShop::UpdateQuestShop(bool response){
 PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data))||!(data.EventId&0x40000000UL)||!data.Count){CloseWindow();return;}
 if(response)KUiTradeConfirm::CloseWindow(false);m_QuestData=data;m_nPageCount=(data.Count+13)/14;if(m_nCurrentPage>=m_nPageCount)m_nCurrentPage=0;m_ItemsBox.Hide();m_BuyBtn.Hide();m_SellBtn.Hide();m_RepairBtn.Hide();m_RepairAllBtn.Hide();
 for(int i=0;i<14;++i){m_PtSlots[i].Hide();m_QuestIcons[i].Hide();m_PtNames[i].Clear();m_PtPrices[i].Clear();int at=m_nCurrentPage*14+i;if(at>=(int)data.Count)continue;const PTEventShopRow& row=data.Rows[at];m_QuestIcons[i].SetOffer(row);m_QuestIcons[i].Show();char name[110];sprintf(name,"%s x%u",row.Name,row.Count);m_PtNames[i].SetText(name);m_PtNames[i].Show();m_PtPrices[i].SetText(row.Price);m_PtPrices[i].Show();}
 m_PreBtn.Enable(m_nCurrentPage>0);m_NextBtn.Enable(m_nCurrentPage+1<m_nPageCount);
 if(response&&data.Result<0)UIMessageBox(PTSQ_TRADE_FAILED);else if(response&&data.Result==2)UIMessageBox(PTSQ_TRADE_OK);
}
void KUiShop::QuestBuy(unsigned id){for(unsigned i=0;i<m_QuestData.Count;++i)if(m_QuestData.Rows[i].Id==id){KUiTradeConfirm::OpenQuestShop(m_QuestData.Rows[i],m_QuestData.Token);return;}}

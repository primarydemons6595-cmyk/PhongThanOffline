// Event shop only: quantity confirmation uses the selected event's native SPRs.
void KUiEventShop::InitConfirmation(KIniFile& ini){
 EventShopIni(ini,"Confirm",170,210,300,140);ini.WriteString("Confirm","Image","\\spr\\phongthan\\event_shop_tet\\confirm-tight.spr");m_Confirm.Init(&ini,"Confirm");m_Confirm.owner=this;AddChild(&m_Confirm);
 EventShopIni(ini,"ConfirmTitle",54,1,192,18,14);ini.WriteInteger("ConfirmTitle","HAlign",1);m_ConfirmTitle.Init(&ini,"ConfirmTitle");m_Confirm.AddChild(&m_ConfirmTitle);m_ConfirmTitle.SetText("\115\165\141\040\166\313\164\040\160\150\310\155");
 EventShopIni(ini,"ConfirmName",14,22,272,30);ini.WriteInteger("ConfirmName","HAlign",1);ini.WriteInteger("ConfirmName","MultiLine",1);m_ConfirmName.Init(&ini,"ConfirmName");m_Confirm.AddChild(&m_ConfirmName);
 EventShopIni(ini,"QuantityLabel",14,55,76,20);m_QuantityLabel.Init(&ini,"QuantityLabel");m_QuantityLabel.SetText("\123\350\040\154\255\356\156\147");m_Confirm.AddChild(&m_QuantityLabel);
 EventShopIni(ini,"Quantity",96,54,70,20);ini.WriteInteger("Quantity","Type",1);ini.WriteInteger("Quantity","MaxLen",5);ini.WriteString("Quantity","FocusBKColor","0,0,0");ini.WriteInteger("Quantity","FocusBKColorAlpha",180);m_Quantity.Init(&ini,"Quantity");m_Confirm.AddChild(&m_Quantity);
 EventShopButton(ini,"Minus","\\spr\\phongthan\\event_shop_tet\\confirm-step.spr",190,51,32,24);m_Minus.Init(&ini,"Minus");m_Minus.SetLabel("-");m_Confirm.AddChild(&m_Minus);
 EventShopButton(ini,"Plus","\\spr\\phongthan\\event_shop_tet\\confirm-step.spr",230,51,32,24);m_Plus.Init(&ini,"Plus");m_Plus.SetLabel("+");m_Confirm.AddChild(&m_Plus);
 EventShopIni(ini,"ConfirmPrice",14,79,272,26,12);ini.WriteInteger("ConfirmPrice","HAlign",1);ini.WriteInteger("ConfirmPrice","MultiLine",1);m_ConfirmPrice.Init(&ini,"ConfirmPrice");m_Confirm.AddChild(&m_ConfirmPrice);
 EventShopIni(ini,"ConfirmHint",14,79,272,26,12);ini.WriteInteger("ConfirmHint","HAlign",1);ini.WriteInteger("ConfirmHint","MultiLine",1);m_ConfirmHint.Init(&ini,"ConfirmHint");m_Confirm.AddChild(&m_ConfirmHint);
 EventShopButton(ini,"ConfirmBuy","\\spr\\phongthan\\event_shop_tet\\confirm-button.spr",28,110,110,26);m_ConfirmBuy.Init(&ini,"ConfirmBuy");m_ConfirmBuy.SetLabel("\130\270\143\040\156\150\313\156");m_Confirm.AddChild(&m_ConfirmBuy);
 EventShopButton(ini,"Cancel","\\spr\\phongthan\\event_shop_tet\\confirm-button.spr",162,110,110,26);m_Cancel.Init(&ini,"Cancel");m_Cancel.SetLabel("\110\361\171\040\142\341");m_Confirm.AddChild(&m_Cancel);
}
void KUiEventShop::ConfirmPane::PaintWindow(){int x,y;GetAbsolutePos(&x,&y);if(g_pRepresentShell){KRUShadow bg;memset(&bg,0,sizeof(bg));bg.Color.Color_b.a=16;bg.oPosition.nX=x+6;bg.oPosition.nY=y+20;bg.oEndPos.nX=x+294;bg.oEndPos.nY=y+136;g_pRepresentShell->DrawPrimitives(1,&bg,RU_T_SHADOW,true);}KWndImage::PaintWindow();}
int KUiEventShop::ConfirmPane::WndProc(unsigned int msg,unsigned int param,int value){if(msg==WND_N_BUTTON_CLICK||msg==WND_N_EDIT_CHANGE||msg==WM_KEYDOWN)return owner->WndProc(msg,param,value);return KWndImage::WndProc(msg,param,value);}
void KUiEventShop::CancelConfirmation(){m_Selected=0;m_Confirm.Hide();Update(false);}
void KUiEventShop::SelectGoods(unsigned int index){
 if(index>=m_Data.Count||m_Pending)return;m_Selected=index+1;const PTEventShopRow& row=m_Data.Rows[index];g_MouseOver.CancelMouseHoverInfo();m_ConfirmName.SetText(row.Name);
 const char* themes[]={"classic","tet","christmas","midautumn","halloween"};char image[128];const char* theme=themes[m_Data.Theme<=4?m_Data.Theme:1];sprintf(image,"\\spr\\phongthan\\event_shop_%s\\confirm-tight.spr",theme);m_Confirm.SetImage(ISI_T_SPR,image,false);
 sprintf(image,"\\spr\\phongthan\\event_shop_%s\\confirm-button.spr",theme);m_ConfirmBuy.SetImage(ISI_T_SPR,image,false);m_Cancel.SetImage(ISI_T_SPR,image,false);
 sprintf(image,"\\spr\\phongthan\\event_shop_%s\\confirm-step.spr",theme);m_Plus.SetImage(ISI_T_SPR,image,false);m_Minus.SetImage(ISI_T_SPR,image,false);
 m_Quantity.SetIntText(row.Count);for(int i=0;i<6;++i){m_Buy[i].Enable(false);m_Icons[i].Enable(false);}m_Previous.Enable(false);m_Next.Enable(false);UpdateConfirmation();m_Confirm.BringToTop();m_Confirm.Show();Wnd_SetFocusWnd(&m_Quantity);
}
void KUiEventShop::UpdateConfirmation(){
 if(m_Initializing||!m_Selected||m_Selected>m_Data.Count)return;const PTEventShopRow& row=m_Data.Rows[m_Selected-1];unsigned int quantity=m_Quantity.GetIntNumber(),max=PTEventShopMaxLots(row.Count,row.UnitPrice),lots=quantity/row.Count;bool valid=quantity&&quantity%row.Count==0&&lots<=max;
 m_ConfirmBuy.Enable(valid&&!m_Pending);char text[160];if(!valid){sprintf(text,"\116\150\313\160\040\142\351\151\040\163\350\040\045\165\054\040\164\350\151\040\256\141\040\045\165\056",row.Count,max*row.Count);m_ConfirmHint.SetText(text);m_ConfirmPrice.SetText("");return;}
 m_ConfirmHint.SetText("");unsigned int total=row.UnitPrice*lots;char cost[128];if(row.PaymentKind==0)PTEventShopSilver(total,cost);else if(row.PaymentKind==1)sprintf(cost,"%u Xu",total);else _snprintf(cost,sizeof(cost)-1,"%s x%u",row.PaymentName,total);cost[127]=0;sprintf(text,"\124\346\156\147\072\040\045\163",cost);m_ConfirmPrice.SetText(text);m_Plus.Enable(lots<max);m_Minus.Enable(lots>1);
}
bool KUiEventShop::ConfirmClick(unsigned int param){
 if(param==(unsigned int)(KWndWindow*)&m_Cancel){CancelConfirmation();return true;}
 if(!m_Selected||m_Selected>m_Data.Count)return false;const PTEventShopRow& row=m_Data.Rows[m_Selected-1];unsigned int quantity=m_Quantity.GetIntNumber(),max=PTEventShopMaxLots(row.Count,row.UnitPrice),lots=quantity/row.Count;
 if(param==(unsigned int)(KWndWindow*)&m_Plus||param==(unsigned int)(KWndWindow*)&m_Minus){if(param==(unsigned int)(KWndWindow*)&m_Plus){if(lots<max)++lots;}else if(lots>1)--lots;if(!lots)lots=1;m_Quantity.SetIntText(lots*row.Count);UpdateConfirmation();return true;}
 if(param==(unsigned int)(KWndWindow*)&m_ConfirmBuy){if(quantity&&quantity%row.Count==0&&lots&&lots<=max&&g_pCoreShell->OperationRequest(PT_EVENT_SHOP_PURCHASE,row.Id,lots)){m_Selected=0;m_Confirm.Hide();m_Pending=true;for(int i=0;i<6;++i)m_Buy[i].Enable(false);m_Info.SetText("\247\141\156\147\040\170\366\040\154\375\056\056\056");}return true;}return false;
}

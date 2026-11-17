#include "../../../../Headers/PhongThanAuction.h"
#include "../../../Core/Src/PhongThanInventoryUi.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndLabeledButton.h"
#include "UiItem.h"
class PtAuctionWindow:public KWndImage {
 KIniFile ini;KWndImage panel;
 KWndButton close,tabs[5],rows[7],previous,next,bid,buy,cancelLot,newLot,searchButton;
 KPtRankButton categories[11];KWndLabeledButton inbox,claim,confirm,back;
 KWndImage rowFrames[7],icons[7],fieldFrames[4],chosenImage;
 KWndText80 names[7],levels[7],owners[7],prices[7],heads[4],pageLabel,chosenName,labels[5];KWndText256 status;
 KWndEdit80 search,offer,fields[4];
 PTAuctionRow records[7];int previews[7],view,category,page,total,selected;unsigned int serial,lastSequence,lastPoll,sentAt,actor;
 bool composing,busy,selectingSupply,waitingReceipt;
 void Text(KWndText& control,const char* key){char text[256];ini.GetString("PT_Text",key,"",text,sizeof(text));control.SetText(text);}
 void Release(){for(int i=0;i<7;++i){if(previews[i])g_pCoreShell->TeamOperation(PT_PS_RELEASE,previews[i],0);previews[i]=0;icons[i].RemoveImage();}}
 void Message(int result){char key[32];sprintf(key,"Result%d",result);Text(status,key);}
 void Read(){PTAuctionCommand q;memset(&q,0,sizeof(q));q.View=view;q.Category=category;q.Page=page;search.GetText(q.Search,80,false);Send(q);}
 void Send(PTAuctionCommand& q){
  q.View=view;q.Category=category;q.Page=page;search.GetText(q.Search,80,false);
  if(g_pCoreShell->TeamOperation(PT_AUCTION_ACTION,(unsigned int)&q,sizeof(q))){busy=true;sentAt=GetTickCount();lastPoll=sentAt;}
 }
 void Controls(){
  // Purchase requests sell to the buyer; retain the original "Ban" artwork.
  buy.Init(&ini,view==3?"PBW_BtnSell":"BW_BtnBuy");int bx,by;buy.GetPosition(&bx,&by);buy.SetPosition(bx+25,by+47);
  for(int i=0;i<7;++i){if(composing){rows[i].Hide();rowFrames[i].Hide();icons[i].Hide();names[i].Hide();levels[i].Hide();owners[i].Hide();prices[i].Hide();}
   else{rows[i].Show();rowFrames[i].Show();icons[i].Show();names[i].Show();levels[i].Show();owners[i].Show();prices[i].Show();}}
  for(i=0;i<11;++i){if(!composing&&view!=5)categories[i].Show();else categories[i].Hide();categories[i].CheckButton(i==category);}
  KWndWindow* normal[]={&previous,&next,&search,&searchButton,&pageLabel};for(i=0;i<5;++i){if(composing)normal[i]->Hide();else normal[i]->Show();}
  for(i=0;i<4;++i){if(composing)heads[i].Hide();else heads[i].Show();if(composing){fields[i].Show();fieldFrames[i].Show();labels[i].Show();}else{fields[i].Hide();fieldFrames[i].Hide();labels[i].Hide();}}
  KWndWindow* compose[]={&confirm,&back,&chosenImage,&chosenName,&labels[4]};for(i=0;i<5;++i){if(composing)compose[i]->Show();else compose[i]->Hide();}
  if(composing){bid.Hide();buy.Hide();cancelLot.Hide();newLot.Hide();claim.Hide();offer.Hide();}
  else{if(view<=1){bid.Show();buy.Show();offer.Show();}else{bid.Hide();buy.Hide();offer.Hide();}
   if(view==2||view==4){cancelLot.Show();newLot.Show();}else{cancelLot.Hide();newLot.Hide();}
   if(view==3)buy.Show();if(view==5)claim.Show();else claim.Hide();}
  for(i=0;i<5;++i)tabs[i].CheckButton(view==i);
 }
 void SetView(int value){view=value;category=0;page=0;selected=-1;serial=0;selectingSupply=false;composing=false;search.SetText("");Release();Controls();Read();}
 void Compose(){composing=true;serial=0;chosenImage.RemoveImage();Text(chosenName,"ChooseItem");
  fields[0].SetIntText(1000);fields[1].SetIntText(0);fields[2].SetIntText(24);fields[3].SetIntText(1);Controls();
  fields[1].Enable(view!=4);fields[3].Enable(view==4);Text(status,"ComposeHint");KUiItem::OpenWindow();}
 void Populate(const PTAuctionPage& s){
  unsigned int previousId=selected>=0&&selected<7?records[selected].Id:0;
  Release();memcpy(records,s.Rows,sizeof(records));total=s.Total;selected=-1;char text[256];
  for(int i=0;i<7;++i){names[i].SetText("");levels[i].SetText("");owners[i].SetText("");prices[i].SetText("");rows[i].CheckButton(false);rows[i].Enable(i<s.Count);
   if(i>=s.Count)continue;const PTAuctionRow& r=s.Rows[i];
   rowFrames[i].SetFrame(r.Id==previousId?1:0);if(r.Id==previousId){selected=i;rows[i].CheckButton(true);}
   if(r.Item.Stack>1){sprintf(text,"%.65s x%d",r.Item.Name,r.Item.Stack);names[i].SetText(text);}else names[i].SetText(r.Item.Name);
   levels[i].SetIntText(r.Item.Level);owners[i].SetText(view==2?r.Bidder:r.Owner);
   if(view==5)sprintf(text,"%d",r.Money);else sprintf(text,"%d",r.Price);prices[i].SetText(text);
   previews[i]=r.Item.Stack?g_pCoreShell->TeamOperation(PT_PS_PREVIEW,(unsigned int)&r.Item,sizeof(PT_PS_ITEM)):0;
   char image[256];image[0]=0;if(previews[i]&&g_pCoreShell->GetGameData(GDI_CHAT_ITEM_IMAGE,(unsigned int)image,previews[i]))icons[i].SetImage(ISI_T_SPR,image,false);
   sprintf(text,"%.79s\nID %u | x%d\n%d / %d",r.Item.Name,r.Id,r.Item.Stack,r.Price,r.Buyout);rows[i].SetToolTipInfo(text,strlen(text));
  }
  sprintf(text,"%d / %d",page+1,max(1,(total+6)/7));pageLabel.SetText(text);previous.Enable(page>0);next.Enable((page+1)*7<total);
  ini.GetString("PT_Text","Inbox","",text,sizeof(text));char label[32];_snprintf(label,31,"%.19s (%d)",text,s.Deliveries);label[31]=0;inbox.SetLabel(text);inbox.SetToolTipInfo(label,strlen(label));
 }
public:
 PtAuctionWindow():view(0),category(0),page(0),total(0),selected(-1),serial(0),lastSequence(0),lastPoll(0),sentAt(0),actor(0),composing(false),busy(false),selectingSupply(false),waitingReceipt(false){memset(previews,0,sizeof(previews));memset(records,0,sizeof(records));}
 bool Initialize(){
  char scheme[256],path[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\PtAuction.ini",scheme);if(!ini.Load(path))return false;
  Init(&ini,"Main");AddChild(&close);close.Init(&ini,"Auction_BtnClose");
  const char* keys[]={"Auction_BtnBrowse","Auction_BtnBid","Auction_BtnOwner","Auction_BtnPurchaseBrowse","Auction_BtnPurchase"};
  for(int i=0;i<5;++i){AddChild(&tabs[i]);tabs[i].Init(&ini,keys[i]);}
  AddChild(&panel);panel.Init(&ini,"Ation_Browse");panel.SetSize(480,371);panel.Enable(false);
  const char* buttons[]={"BW_BtnPrePage","BW_BtnNextPage","BW_BtnBid","BW_BtnBuy","OW_BtnCelAuction","OW_BtnGoOnAuction","BW_BtnSearch"};
  KWndButton* controls[]={&previous,&next,&bid,&buy,&cancelLot,&newLot,&searchButton};
  for(i=0;i<7;++i){AddChild(controls[i]);controls[i]->Init(&ini,buttons[i]);int x,y;controls[i]->GetPosition(&x,&y);controls[i]->SetPosition(x+25,y+47);}
  AddChild(&pageLabel);pageLabel.Init(&ini,"BW_PageNum");pageLabel.SetPosition(31,363);
  AddChild(&search);search.Init(&ini,"PT_Search");AddChild(&offer);offer.Init(&ini,"PT_Offer");
  for(i=0;i<11;++i){AddChild(&categories[i]);categories[i].Init(&ini,"PT_Category");categories[i].SetPosition(37,61+i*26);char section[32],text[80];sprintf(section,"BW_SelDefList_Btn%d",i);ini.GetString(section,"SelDefName","",text,sizeof(text));categories[i].SetText(text);categories[i].SetToolTipInfo(text,strlen(text));}
  for(i=0;i<7;++i){AddChild(&rowFrames[i]);rowFrames[i].Init(&ini,"PT_RowFrame");rowFrames[i].SetPosition(117,80+i*39);rowFrames[i].Enable(false);
   AddChild(&rows[i]);rows[i].Init(&ini,"PT_Row");rows[i].SetPosition(117,80+i*39);
   AddChild(&icons[i]);icons[i].Init(&ini,"PT_Icon");icons[i].SetPosition(117,80+i*39);icons[i].Enable(false);
   KWndText80* cols[]={&names[i],&levels[i],&owners[i],&prices[i]};const int xs[]={153,274,306,413},ws[]={120,30,105,86};
   for(int c=0;c<4;++c){AddChild(cols[c]);cols[c]->Init(&ini,"PT_Cell");cols[c]->SetPosition(xs[c],91+i*39);cols[c]->SetSize(ws[c],26);cols[c]->Enable(false);}
  }
  const char* headKeys[]={"BW_TxtItemNameTitle","BW_TxtItemLevelTitle","BW_TxtBestPriceName","BW_TxtCurPriceTitle"};const int hx[]={153,274,306,413};
  for(i=0;i<4;++i){AddChild(&heads[i]);heads[i].Init(&ini,headKeys[i]);heads[i].SetPosition(hx[i],55);heads[i].SetSize(i==0?120:i==1?30:i==2?105:86,18);}
  AddChild(&status);status.Init(&ini,"PT_Status");AddChild(&inbox);inbox.Init(&ini,"PT_Inbox");AddChild(&claim);claim.Init(&ini,"PT_Claim");
  AddChild(&confirm);confirm.Init(&ini,"PT_Confirm");AddChild(&back);back.Init(&ini,"PT_Back");
  AddChild(&chosenImage);chosenImage.Init(&ini,"PT_ChosenImage");AddChild(&chosenName);chosenName.Init(&ini,"PT_ChosenName");
  for(i=0;i<4;++i){char section[24];sprintf(section,"PT_Field%d",i);AddChild(&fieldFrames[i]);fieldFrames[i].Init(&ini,"PT_FieldFrame");fieldFrames[i].SetPosition(240,160+i*36);
   AddChild(&fields[i]);fields[i].Init(&ini,section);AddChild(&labels[i]);labels[i].Init(&ini,"PT_Cell");labels[i].SetPosition(70,160+i*36);labels[i].SetSize(165,26);char key[24];sprintf(key,"Field%d",i);Text(labels[i],key);}
  AddChild(&labels[4]);labels[4].Init(&ini,"PT_ComposeHint");
  Wnd_AddWindow(this,WL_TOPMOST);int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-531)/2),max(0,(sh-434)/2));Controls();return true;
 }
 void Open(){actor=g_pCoreShell?g_pCoreShell->GetdwIDPlayer(0):0;Show();BringToTop();busy=false;SetView(0);}
 void Shutdown(){Release();Hide();busy=false;serial=0;composing=false;}
 bool Put(const KUiDraggedObject* item,int container){
  if(!IsVisible()||(!composing&&!selectingSupply))return false;if(!item||!item->uId)return true;
  if(container!=UOC_ITEM_TAKE_WITH&&container!=UOC_ITEM_TAKE_WITH_EX&&container!=UOC_PT_BAG2&&container!=UOC_PT_BAG3)return true;
  serial=g_pCoreShell->GetGameData(PHONGTHAN_GDI_ITEM_NETWORK_ID,0,item->uId);if(!serial)return true;
  char image[256];image[0]=0;if(g_pCoreShell->GetGameData(GDI_CHAT_ITEM_IMAGE,(unsigned int)image,item->uId))chosenImage.SetImage(ISI_T_SPR,image,false);
  Text(chosenName,"ItemSelected");Text(status,"ItemSelected");selectingSupply=false;return true;
 }
 virtual void Breathe(){
  if(!IsVisible()||!g_pCoreShell)return;if(actor!=g_pCoreShell->GetdwIDPlayer(0)){Shutdown();return;}
  PTAuctionPage* state=new PTAuctionPage;bool got=g_pCoreShell->TeamOperation(PT_AUCTION_GET,(unsigned int)state,sizeof(*state))!=0;
  if(got&&lastSequence!=state->Header.Sequence){lastSequence=state->Header.Sequence;busy=false;
   if(state->View==view&&state->Category==category&&state->Page==page){
    if(state->Operation&&state->Result>0){composing=false;serial=0;Controls();}if(!composing)Populate(*state);
    if(state->Operation||state->Result<0){Message(state->Result);waitingReceipt=state->Result==2;}
    else if(waitingReceipt&&!state->Pending){Message(1);waitingReceipt=false;}
   }}delete state;
  if(busy&&GetTickCount()-sentAt>8000){busy=false;Read();return;}
  if(!composing&&!busy&&GetTickCount()-lastPoll>2000)Read();
 }
 virtual int WndProc(unsigned int msg,unsigned int param,int data){
  if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Shutdown();return 1;}
  if(msg==WND_N_BUTTON_CLICK){
   if(param==(unsigned int)&close){Shutdown();return 1;}
   if(busy)return 1;
   for(int i=0;i<5;++i)if(param==(unsigned int)&tabs[i]){SetView(i);return 1;}
   if(param==(unsigned int)&inbox){SetView(5);return 1;}
   if(param==(unsigned int)&newLot){Compose();return 1;}
   if(param==(unsigned int)&back){composing=false;Controls();Read();return 1;}
   if(param==(unsigned int)&confirm){PTAuctionCommand q;memset(&q,0,sizeof(q));q.Operation=view==4?PT_AUCTION_ORDER:PT_AUCTION_POST;q.Serial=serial;q.Price=fields[0].GetIntNumber();q.Buyout=fields[1].GetIntNumber();q.Hours=fields[2].GetIntNumber();q.Count=fields[3].GetIntNumber();Send(q);return 1;}
   for(i=0;i<11;++i)if(param==(unsigned int)&categories[i]){category=i;page=0;Controls();Read();return 1;}
   for(i=0;i<7;++i)if(param==(unsigned int)&rows[i]&&records[i].Id){selected=i;for(int j=0;j<7;++j){rows[j].CheckButton(i==j);rowFrames[j].SetFrame(i==j?1:0);}return 1;}
   if(param==(unsigned int)&previous||param==(unsigned int)&next){page=max(0,min(max(0,(total-1)/7),page+(param==(unsigned int)&next?1:-1)));Read();return 1;}
   if(param==(unsigned int)&searchButton){page=0;Read();return 1;}
   if(selected>=0&&selected<7){PTAuctionCommand q;memset(&q,0,sizeof(q));q.Id=records[selected].Id;q.Revision=records[selected].Revision;q.Price=offer.GetIntNumber();
    if(param==(unsigned int)&bid)q.Operation=PT_AUCTION_BID;
    if(param==(unsigned int)&buy){if(view==3&&!serial){selectingSupply=true;Text(status,"SupplyHint");KUiItem::OpenWindow();return 1;}q.Operation=view==3?PT_AUCTION_SUPPLY:PT_AUCTION_BUY;q.Serial=serial;}
    if(param==(unsigned int)&cancelLot)q.Operation=PT_AUCTION_CANCEL;
    if(param==(unsigned int)&claim)q.Operation=PT_AUCTION_CLAIM;
    if(q.Operation){Send(q);return 1;}
   }
  }return KWndImage::WndProc(msg,param,data);
 }
};
static PtAuctionWindow* ptAuctionWindow=0;
static void PtAuctionOpen(){if(!ptAuctionWindow){ptAuctionWindow=new PtAuctionWindow;if(!ptAuctionWindow->Initialize()){delete ptAuctionWindow;ptAuctionWindow=0;return;}}ptAuctionWindow->Open();}
static void PtAuctionClose(){if(ptAuctionWindow){ptAuctionWindow->Shutdown();ptAuctionWindow->Destroy();ptAuctionWindow=0;}}
bool PhongThanAuctionPutInventoryItem(const KUiDraggedObject* item,int container){return ptAuctionWindow&&ptAuctionWindow->Put(item,container);}

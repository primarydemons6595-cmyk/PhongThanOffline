#include "UiEventShop.h"
#include "../Elem/MouseHover.h"
#include "../../../Core/Src/CoreShell.h"
#include "../UiBase.h"
#include "../../../Engine/Src/Text.h"
#include "../../../../Headers/PhongThanEventShopPresentation.h"
extern iCoreShell* g_pCoreShell;
extern iRepresentShell* g_pRepresentShell;
KUiEventShop* KUiEventShop::s_Window=0;
static void EventShopIni(KIniFile& ini,const char* section,int x,int y,int w,int h,int font=14){
 ini.WriteInteger(section,"Left",x);ini.WriteInteger(section,"Top",y);ini.WriteInteger(section,"Width",w);ini.WriteInteger(section,"Height",h);ini.WriteInteger(section,"Font",font);
 ini.WriteString(section,"Color","255,232,176");ini.WriteString(section,"BorderColor","0,0,0");ini.WriteInteger(section,"HAlign",0);
}
static void EventShopButton(KIniFile& ini,const char* section,const char* image,int x,int y,int w,int h){
 EventShopIni(ini,section,x,y,w,h);ini.WriteString(section,"Image",image);ini.WriteInteger(section,"Up",0);ini.WriteInteger(section,"Over",0);ini.WriteInteger(section,"Down",2);ini.WriteInteger(section,"LabelYOffset",PTEventShopLabelY(h,14));
 ini.WriteString(section,"OverColor","255,255,220");ini.WriteString(section,"DisableColor","130,120,100");
}
static void EventShopCard(int left,int top,int right,int bottom){
 if(!g_pRepresentShell)return;KRUShadow background;memset(&background,0,sizeof(background));background.Color.Color_b.a=24;background.oPosition.nX=left;background.oPosition.nY=top;background.oEndPos.nX=right;background.oEndPos.nY=bottom;g_pRepresentShell->DrawPrimitives(1,&background,RU_T_SHADOW,true);
 KRURect outline;memset(&outline,0,sizeof(outline));outline.Color.Color_dw=0xffe7bd63;outline.oPosition=background.oPosition;outline.oEndPos=background.oEndPos;g_pRepresentShell->DrawPrimitives(1,&outline,RU_T_RECT,true);
}
void KUiEventShop::ShopButton::PaintWindow(){
 SetFrame(PTEventShopButtonFrame((m_Style&WND_S_DISABLE)==0,(m_Flag&WNDBTN_F_DOWN)!=0,(m_Flag&WNDBTN_F_OVER)!=0));KWndLabeledButton::PaintWindow();
}
void KUiEventShop::ItemIcon::SetItem(const PTEventShopRow& row){
 goodsId=row.Id;
 SetImage(ISI_T_SPR,row.Image,true);
}
int KUiEventShop::ItemIcon::WndProc(unsigned int msg,unsigned int param,int value){
 int result=KWndButton::WndProc(msg,param,value);
 if((msg==WM_MOUSEMOVE||msg==WM_MOUSEHOVER)&&!g_MouseOver.IsMoseHoverWndObj(this,0)){
  PTEventShopTooltip tip;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_TOOLTIP,(unsigned int)&tip,goodsId))return result;
  g_MouseOver.CancelMouseHoverInfo();g_MouseOver.SetMouseHoverInfo(this,0,LOWORD(value),HIWORD(value),false,false);
  IR_InitUiImageRef(g_MouseOver.m_HoverImage);lstrcpynA(g_MouseOver.m_HoverImage.szImage,tip.Image,sizeof(g_MouseOver.m_HoverImage.szImage));memset(&g_MouseOver.m_SocketVisuals,0,sizeof(g_MouseOver.m_SocketVisuals));
  g_MouseOver.SetMouseHoverImage(tip.ImageFlags!=0,(tip.ImageFlags&2)!=0,(tip.ImageFlags&4)!=0);
  int length=TEncodeText(tip.Title,strlen(tip.Title));g_MouseOver.SetMouseHoverTitle(tip.Title,length,0xffffffff);
  length=TEncodeText(tip.Properties,strlen(tip.Properties));g_MouseOver.SetMouseHoverProp(tip.Properties,length,0xffffffff);
  length=TEncodeText(tip.Description,strlen(tip.Description));g_MouseOver.SetMouseHoverDesc(tip.Description,length,0xffffffff);
 }
 return result;
}
bool KUiEventShop::Initialize(){
 m_Selected=0;m_Initializing=true;
 KIniFile ini;int sw=800,sh=600;Wnd_GetScreenSize(sw,sh);EventShopIni(ini,"Main",(sw-640)/2,(sh-560)/2,640,560);ini.WriteInteger("Main","Moveable",1);ini.WriteInteger("Main","Trans",0);ini.WriteString("Main","Image","\\spr\\phongthan\\event_shop_tet\\frame.spr");
 if(!Init(&ini,"Main"))return false;
 EventShopIni(ini,"Title",145,49,350,34,PT_EVENT_SHOP_TITLE_FONT);ini.WriteInteger("Title","HAlign",1);ini.WriteInteger("Title","VAlign",1);m_Title.Init(&ini,"Title");AddChild(&m_Title);
 for(int i=0;i<6;++i){int x=i%2?332:64,y=130+i/2*110;char section[32];
  sprintf(section,"Icon%d",i);EventShopIni(ini,section,x+8,y+4,64,72);m_Icons[i].Init(&ini,section);AddChild(&m_Icons[i]);
  sprintf(section,"Name%d",i);EventShopIni(ini,section,x+86,y+3,154,34);ini.WriteInteger(section,"MultiLine",1);m_Names[i].Init(&ini,section);AddChild(&m_Names[i]);
  sprintf(section,"Price%d",i);EventShopIni(ini,section,x+86,y+43,154,18);ini.WriteString(section,"Color","255,222,100");m_Prices[i].Init(&ini,section);AddChild(&m_Prices[i]);
  sprintf(section,"Buy%d",i);EventShopButton(ini,section,"\\spr\\phongthan\\event_shop_tet\\buy.spr",x+96,y+65,128,36);m_Buy[i].Init(&ini,section);m_Buy[i].SetLabel("Mua");AddChild(&m_Buy[i]);
 }
 EventShopIni(ini,"Info",65,477,510,22);ini.WriteInteger("Info","HAlign",1);m_Info.Init(&ini,"Info");AddChild(&m_Info);
 EventShopIni(ini,"Page",286,516,100,22);ini.WriteInteger("Page","HAlign",1);m_PageText.Init(&ini,"Page");AddChild(&m_PageText);
 EventShopButton(ini,"Previous","\\spr\\phongthan\\event_shop_tet\\nav.spr",65,510,88,30);m_Previous.Init(&ini,"Previous");m_Previous.SetLabel("<");AddChild(&m_Previous);
 EventShopButton(ini,"Next","\\spr\\phongthan\\event_shop_tet\\nav.spr",169,510,88,30);m_Next.Init(&ini,"Next");m_Next.SetLabel(">");AddChild(&m_Next);
 EventShopButton(ini,"Close","\\spr\\phongthan\\event_shop_tet\\close.spr",502,510,72,30);m_Close.Init(&ini,"Close");m_Close.SetLabel("\247\343\156\147");AddChild(&m_Close);
 InitConfirmation(ini);m_Confirm.Hide();memset(&m_Data,0,sizeof(m_Data));m_Page=0;m_Pending=false;m_Initializing=false;Wnd_AddWindow(this);Hide();return true;
}
void KUiEventShop::Open(){
 if(!s_Window){s_Window=new KUiEventShop;if(!s_Window->Initialize()){delete s_Window;s_Window=0;return;}}
 s_Window->Update(true);if(!s_Window->m_Data.Count)return;s_Window->BringToTop();s_Window->Show();
}
void KUiEventShop::Update(bool response){
 PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data))){m_Data.Count=0;Hide();return;}
 if(data.Token!=m_Data.Token){m_Page=0;m_Selected=0;m_Confirm.Hide();}if(response){m_Pending=false;m_Selected=0;m_Confirm.Hide();}m_Data=data;const char* themes[]={"classic","tet","christmas","midautumn","halloween"};char frame[160];sprintf(frame,"\\spr\\phongthan\\event_shop_%s\\frame.spr",themes[data.Theme<=4?data.Theme:1]);SetImage(ISI_T_SPR,frame,false);
 sprintf(frame,"\\spr\\phongthan\\event_shop_%s\\buy.spr",themes[data.Theme<=4?data.Theme:1]);for(int b=0;b<6;++b)m_Buy[b].SetImage(ISI_T_SPR,frame,false);
 sprintf(frame,"\\spr\\phongthan\\event_shop_%s\\nav.spr",themes[data.Theme<=4?data.Theme:1]);m_Previous.SetImage(ISI_T_SPR,frame,false);m_Next.SetImage(ISI_T_SPR,frame,false);
 sprintf(frame,"\\spr\\phongthan\\event_shop_%s\\close.spr",themes[data.Theme<=4?data.Theme:1]);m_Close.SetImage(ISI_T_SPR,frame,false);
 int pages=(data.Count+5)/6;if(m_Page>=pages)m_Page=pages-1;if(m_Page<0)m_Page=0;
 m_Title.SetText(data.Title);char text[160];sprintf(text,"%d / %d",m_Page+1,pages);m_PageText.SetText(text);
 for(int i=0;i<6;++i){int at=m_Page*6+i;if(at>=(int)data.Count){m_Icons[i].Hide();m_Names[i].Hide();m_Prices[i].Hide();m_Buy[i].Hide();continue;}
  const PTEventShopRow& row=data.Rows[at];m_Icons[i].SetItem(row);int w,h;m_Icons[i].GetSize(&w,&h);m_Icons[i].SetPosition((i%2?332:64)+(72-w)/2,130+i/2*110+(100-h)/2);
  m_Icons[i].Show();m_Icons[i].Enable(!m_Selected);sprintf(text,"%s x%u",row.Name,row.Count);m_Names[i].SetText(text);m_Names[i].Show();m_Prices[i].SetText(row.Price);m_Prices[i].Show();m_Buy[i].Show();m_Buy[i].Enable(!m_Pending&&!m_Selected);
 }
 m_Previous.Enable(m_Page>0&&!m_Selected);m_Next.Enable(m_Page+1<pages&&!m_Selected);
 m_Info.SetText(data.Result==2?"\115\165\141\040\164\150\265\156\150\040\143\253\156\147\056":data.Result<0?"\103\150\255\141\040\155\165\141\040\256\255\356\143\056\040\113\151\323\155\040\164\162\141\040\164\151\322\156\054\040\166\313\164\040\160\150\310\155\040\256\346\151\040\166\265\040\143\150\347\040\164\162\350\156\147\056":"\103\150\344\156\040\166\313\164\040\160\150\310\155\040\256\323\040\155\165\141\056");
}
void KUiEventShop::Dismiss(){m_Selected=0;m_Confirm.Hide();Hide();g_MouseOver.CancelMouseHoverInfo();if(g_pCoreShell)g_pCoreShell->OperationRequest(PT_EVENT_SHOP_DISMISS,0,0);}
void KUiEventShop::Breathe(){if(!IsVisible())return;PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data)))Dismiss();}
void KUiEventShop::PaintWindow(){
 if(g_pRepresentShell){int x,y;GetAbsolutePos(&x,&y);KRUShadow glass;PTEventShopGlass(glass,x,y);g_pRepresentShell->DrawPrimitives(1,&glass,RU_T_SHADOW,true);}
 KWndShowAnimate::PaintWindow();
 if(g_pRepresentShell){int left,top;GetAbsolutePos(&left,&top);
  for(int i=0;i<6&&m_Page*6+i<(int)m_Data.Count;++i){int x=left+(i%2?332:64),y=top+130+i/2*110;
   EventShopCard(x-8,y-5,x+252,y+104);
  }
 }
}
int KUiEventShop::WndProc(unsigned int msg,unsigned int param,int value){
 if(msg==WM_KEYDOWN&&param==VK_ESCAPE){if(m_Selected)CancelConfirmation();else Dismiss();return 1;}
 if(msg==WND_N_EDIT_CHANGE&&param==(unsigned int)(KWndWindow*)&m_Quantity){UpdateConfirmation();return 1;}
 if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)(KWndWindow*)&m_Close){Dismiss();return 1;}
  if(param==(unsigned int)(KWndWindow*)&m_Previous){if(m_Page>0)--m_Page;Update(false);return 1;}
  if(param==(unsigned int)(KWndWindow*)&m_Next){if((m_Page+1)*6<(int)m_Data.Count)++m_Page;Update(false);return 1;}
  if(ConfirmClick(param))return 1;
  for(int i=0;i<6;++i)if(param==(unsigned int)(KWndWindow*)&m_Buy[i]){SelectGoods(m_Page*6+i);return 1;}
 }
 return KWndShowAnimate::WndProc(msg,param,value);
}

#include "UiEventShopConfirm.inl"

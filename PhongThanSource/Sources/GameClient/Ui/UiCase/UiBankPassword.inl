#include "../Elem/WndEdit.h"
#include "../../../../Headers/PhongThanBankSecurity.h"
// Original treasury password sprites and the existing game keyboard assets.
class PTBankPasswordWindow : public KWndImage {
 KWndEdit32 fields[3];KWndButton yes,no,keys[51];KWndText32 labels[48],notice;
 char chars[48][2];bool shift;int mode,active;static PTBankPasswordWindow* self;
 void Letters(){for(int i=0;i<48;++i){char text[2]={chars[i][shift?1:0],0};labels[i].SetText(text);}}
 void Send(){PTBankSecurityRequest q;memset(&q,0,sizeof(q));q.Action=mode;
  if(mode==1){fields[0].GetText(q.Password,16,true);fields[1].GetText(q.Confirm,16,true);}
  else if(mode==2)fields[0].GetText(q.Old,16,true);
  else {fields[0].GetText(q.Old,16,true);fields[1].GetText(q.Password,16,true);fields[2].GetText(q.Confirm,16,true);}
  if(g_pCoreShell)g_pCoreShell->OperationRequest(PT_GOI_BANK_SECURITY,(unsigned int)&q,sizeof(q));ZeroMemory(&q,sizeof(q));
 }
 bool Initialize(int action){
  mode=action;active=0;shift=false;KIniFile ini;char path[320],scheme[256];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
  sprintf(path,"%s\\UiBankPassword%d.ini",scheme,mode);if(!ini.Load(path))return false;
  // Coordinates match the recovered 182x113 artwork, not an older form.
  const char* buttonSections[]={"BtnConfirm","BtnCancel"};
  for(int btn=0;btn<2;++btn){ini.WriteInteger(buttonSections[btn],"Left",btn?108:25);ini.WriteInteger(buttonSections[btn],"Top",88);ini.WriteInteger(buttonSections[btn],"Width",47);ini.WriteInteger(buttonSections[btn],"Height",19);ini.WriteInteger(buttonSections[btn],"Over",0);ini.WriteInteger(buttonSections[btn],"Trans",0);}
  KWndImage::Init(&ini,"Main");yes.Init(&ini,"BtnConfirm");no.Init(&ini,"BtnCancel");AddChild(&yes);AddChild(&no);
  const char* create[]={"Password1","Password2","Password2"};const char* change[]={"OldPassword","NewPassword1","NewPassword2"};int count=mode==1?2:mode==2?1:3;
  for(int i=0;i<count;++i){const char* section=mode==4?change[i]:mode==2?"Password":create[i];
   ini.WriteInteger(section,"Left",mode==2?44:76);ini.WriteInteger(section,"Top",mode==2?44:mode==4?20+i*19:29+i*19);ini.WriteInteger(section,"Width",mode==2?99:89);ini.WriteInteger(section,"Height",15);ini.WriteInteger(section,"HAlign",0);ini.WriteString(section,"Color","255,255,255");ini.WriteString(section,"BorderColor","0,0,0");
   fields[i].Init(&ini,section);AddChild(&fields[i]);fields[i].Clear(true);}
  ini.WriteInteger("Notice","Left",4);ini.WriteInteger("Notice","Top",114);ini.WriteInteger("Notice","Width",300);ini.WriteInteger("Notice","Height",32);ini.WriteInteger("Notice","Font",14);ini.WriteString("Notice","Color","255,220,100");notice.Init(&ini,"Notice");AddChild(&notice);
  sprintf(path,"%s\\UiLoginKeyboard.ini",scheme);KIniFile keyboard;if(!keyboard.Load(path))return false;
  int w,h;GetSize(&w,&h);int sw,sh;Wnd_GetScreenSize(sw,sh);int left=20,top=sh-270;
  for(int k=0;k<51;++k){char section[32];if(k<48)sprintf(section,"CharKey_%d",k);else if(k<50)sprintf(section,"FuncKey_%d",k-48);else strcpy(section,"ShiftKey");
   keyboard.WriteInteger(section,"Over",0);keyboard.WriteInteger(section,"Trans",0);keys[k].Init(&keyboard,section);AddChild(&keys[k]);int x,y;keys[k].GetPosition(&x,&y);keys[k].SetPosition(x,152+y);
   if(k<48){char value[8];keyboard.GetString(section,"Char"," ",value,sizeof(value));chars[k][0]=value[0];keyboard.GetString(section,"ShiftChar"," ",value,sizeof(value));chars[k][1]=value[0];
    keyboard.WriteInteger("KeyLabel","Left",7);keyboard.WriteInteger("KeyLabel","Top",4);keyboard.WriteInteger("KeyLabel","Width",18);keyboard.WriteInteger("KeyLabel","Height",17);keyboard.WriteInteger("KeyLabel","Font",14);keyboard.WriteInteger("KeyLabel","DummyWnd",1);keyboard.WriteString("KeyLabel","Color","255,255,255");keyboard.WriteString("KeyLabel","BorderColor","0,0,0");labels[k].Init(&keyboard,"KeyLabel");keys[k].AddChild(&labels[k]);}
  }
  m_Width=350;m_Height=270;SetPosition(left,max(0,top));Letters();Wnd_AddWindow(this,WL_TOPMOST);Show();Wnd_SetFocusWnd(&fields[0]);return true;
 }
 int WndProc(unsigned int msg,unsigned int param,int data){
  if(msg==WND_N_EDIT_LCLICK||msg==WND_N_EDIT_KILL_FOCUS)for(int i=0;i<3;++i)if(param==(unsigned int)(KWndWindow*)&fields[i])active=i;
  if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}
  if(msg==WND_N_EDIT_SPECIAL_KEY_DOWN&&data==VK_RETURN){Send();return 1;}
  if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)(KWndWindow*)&yes){Send();return 1;}if(param==(unsigned int)(KWndWindow*)&no){Hide();return 1;}
   for(int k=0;k<51;++k)if(param==(unsigned int)(KWndWindow*)&keys[k]){Wnd_SetFocusWnd(&fields[active]);if(k==50){shift=keys[k].IsButtonChecked()!=0;Letters();}else if(k<48)fields[active].WndProc(WM_CHAR,(unsigned char)chars[k][shift?1:0],0);else if(k==48)fields[active].WndProc(WM_KEYDOWN,VK_BACK,0);else Send();return 1;}
  }return KWndImage::WndProc(msg,param,data);
 }
public:
 static void Receive(const PTBankSecurityState& state){
  if(state.Result>0){if(self){for(int i=0;i<3;++i)self->fields[i].Clear(true);self->Hide();}return;}
  if(state.Result<0){if(self&&self->IsVisible())self->notice.SetText("MËt khÈu kh«ng ®óng hoÆc hai « kh«ng trïng nhau.");return;}
  if(self){self->Destroy();self=0;}self=new PTBankPasswordWindow;if(!self->Initialize(!state.Configured?1:state.Locked?2:4)){delete self;self=0;}
 }
};
PTBankPasswordWindow* PTBankPasswordWindow::self=0;
void KUiStoreBox::BankPasswordResponse(const PTBankSecurityState& state){
 if(m_pSelf){char scheme[256],path[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\UiStoreBox.ini",scheme);KIniFile ini;if(ini.Load(path))m_pSelf->m_LockBtn.Init(&ini,state.Configured?"BtnChangePassword":"BtnCreatePassword");}
 if(state.Result>0&&state.Locked&&m_pSelf)CloseWindow();
 PTBankPasswordWindow::Receive(state);
}

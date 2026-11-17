#include <time.h>
void KUiStatus::InitPetLog(){
 m_PetLogVisible=false;m_PetLogStamp=0;
 char scheme[256],path[300];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\UiPetLog.ini",scheme);KIniFile ini;
 AddChild(&m_PetLogFrame);if(!ini.Load(path))return;
 m_PetLogFrame.Init(&ini,"Main");m_PetLogFrame.SetPosition(238,56);m_PetLogFrame.SetSize(255,180);
 m_PetLogFrame.AddChild(&m_PetLogText);m_PetLogText.Init(&ini,"Information");m_PetLogText.SetPosition(10,48);m_PetLogText.SetSize(220,85);
 m_PetLogFrame.AddChild(&m_PetLogClose);m_PetLogClose.Init(&ini,"BtnClose");
 KIniFile title;PtPageControl(title,"Title",15,28,215,16);title.WriteInteger("Title","HAlign",1);
 m_PetLogFrame.AddChild(&m_PetLogTitle);m_PetLogTitle.Init(&title,"Title");m_PetLogTitle.SetText("NhËt ký Linh thó");
 KIniFile status;status.Load("\\Ui\\ui3\\UiStatus.ini");
 m_PetLogFrame.AddChild(&m_PetLogScroll);m_PetLogScroll.Init(&status,"CompoundPage_ScrDesc_Bar");m_PetLogScroll.SetPosition(234,62);m_PetLogScroll.SetSize(16,56);
 m_PetLogFrame.AddChild(&m_PetLogUp);m_PetLogUp.Init(&status,"CompoundPage_ScrDesc_BtnUp");m_PetLogUp.SetPosition(234,48);
 m_PetLogFrame.AddChild(&m_PetLogDown);m_PetLogDown.Init(&status,"CompoundPage_ScrDesc_BtnDown");m_PetLogDown.SetPosition(234,119);
 m_PetLogFrame.Hide();
}
void KUiStatus::OpenPetLog(){m_PetLogVisible=true;m_PetLogStamp=0;m_PetLogScroll.SetScrollPos(0);UpdatePetLog();m_PetLogFrame.Show();m_PetLogFrame.BringToTop();}
void KUiStatus::UpdatePetLog(){
 PT_STATUS_PAGES_STATE state;if(!g_pCoreShell->GetGameData(PT_STATUS_PAGES_GET,(unsigned int)&state,sizeof(state)))return;
 unsigned int stamp=2166136261U;const unsigned char* bytes=(const unsigned char*)state.Pet.Log;
 for(int b=0;b<sizeof(state.Pet.Log);++b)stamp=(stamp^bytes[b])*16777619U;
 if(stamp!=m_PetLogStamp){m_PetLogStamp=stamp;char text[4096];text[0]=0;
  for(int i=0;i<8;++i){const PT_PET_LOG& event=state.Pet.Log[i];if(!event.Time)continue;
   time_t when=event.Time;struct tm* tm=localtime(&when);char date[64],line[256];
   if(tm)strftime(date,sizeof(date),"%d/%m %H:%M",tm);else strcpy(date,"--/-- --:--");
   const char* action="Ho¹t ®éng Linh thó";
   switch(event.Event){case 1:action="NhËn nu«i Linh thó";break;case 10:action="§æi tªn Linh thó";break;case 11:action="Thay ®æi thiÕt lËp";break;case 20:action="BiÕn ®æi h×nh d¹ng";break;case 30:action="Hé chñ";break;case 40:action="NhÆt vËt phÈm";break;case 41:action="B¸n vËt phÈm";break;case 42:action="Söa trang bÞ";break;case 414:action="Cho ¨n Phôc Ma Linh";break;case 415:action="Cho ¨n Hãa Linh T¸n";break;case 416:action="Cho ¨n Ph¸c Ngäc";break;case 417:action="Cho ¨n Tô Linh Ch©u";break;}
   sprintf(line,"<color=yellow>%s<color>\n%s\n",date,action);strcat(text,line);
  }
  m_PetLogText.SetText(text[0]?text:"Ch­a cã nhËt ký.");int last=m_PetLogText.GetLineCount()-6;if(last<0)last=0;m_PetLogScroll.SetValueRange(0,last);
 }m_PetLogText.SetTopLine(m_PetLogScroll.GetScrollPos());
}
bool KUiStatus::PetLogClick(unsigned int param){
 if(!m_PetLogVisible)return false;
 if(param==(unsigned int)(KWndWindow*)&m_PetLogClose){m_PetLogVisible=false;m_PetLogFrame.Hide();return true;}
 if(param==(unsigned int)(KWndWindow*)&m_PetLogUp||param==(unsigned int)(KWndWindow*)&m_PetLogDown){m_PetLogScroll.ScrollLine(param==(unsigned int)(KWndWindow*)&m_PetLogUp);UpdatePetLog();return true;}
 return false;
}

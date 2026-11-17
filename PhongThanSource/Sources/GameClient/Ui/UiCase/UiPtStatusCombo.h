#pragma once
#include "KIniFile.h"
#include "../Elem/PopupMenu.h"
#include <stdlib.h>
// Original combobox artwork plus the engine's existing popup-menu controller.
class KPtStatusCombo : public KWndWindow {
 KWndButton m_Button;KWndText80 m_Value;int m_Count,m_Selected;
 char m_Items[512][80];int m_Ids[512];
public:
 KPtStatusCombo():m_Count(0),m_Selected(-1){}
 int Init(KIniFile* ini,const char* section){
  KWndWindow::Init(ini,section);char name[128];sprintf(name,"%s_Btn",section);AddChild(&m_Button);m_Button.Init(ini,name);
  sprintf(name,"%s_Edit",section);AddChild(&m_Value);m_Value.Init(ini,name);
  m_Count=0;m_Selected=-1;int count=0;ini->GetInteger(section,"SelCount",0,&count);
  for(int i=0;i<count&&i<512;++i){char key[32],text[80];sprintf(key,"SelText_%d",i);ini->GetString(section,key,"",text,sizeof(text));AddItem(i,text);}
  SelectIndex(m_Count?0:-1);return 1;
 }
 void ClearItems(){m_Count=0;m_Selected=-1;m_Value.SetText("");}
 void AddItem(int id,const char* text){if(m_Count>=512)return;m_Ids[m_Count]=id;lstrcpyn(m_Items[m_Count++],text[0]=='#'?text+1:text,80);}
 void SelectIndex(int index){m_Selected=index>=0&&index<m_Count?index:-1;m_Value.SetText(m_Selected>=0?m_Items[m_Selected]:"");}
 void SelectId(int id){for(int i=0;i<m_Count;++i)if(m_Ids[i]==id){SelectIndex(i);return;}SelectIndex(-1);}
 int SelectedIndex()const{return m_Selected;}
 int SelectedId()const{return m_Selected>=0?m_Ids[m_Selected]:0;}
 int Count()const{return m_Count;}
 const char* SelectedText()const{return m_Selected>=0?m_Items[m_Selected]:"";}
 int WndProc(unsigned int msg,unsigned int param,int value){
  if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_Button){
   if(!m_Count)return 1;KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(m_Count));if(!menu)return 1;
   KPopupMenu::InitMenuData(menu,m_Count);menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;menu->byFontSize=14;menu->nSelectedItem=m_Selected;
   int x,y;GetAbsolutePos(&x,&y);menu->nX=x;menu->nY=y+15;menu->nItemWidth=m_Width;
   for(int i=0;i<m_Count;++i){lstrcpyn(menu->Items[i].szData,m_Items[i],64);menu->Items[i].uDataLen=strlen(menu->Items[i].szData);menu->Items[i].uID=i;}
   KPopupMenu::Popup(menu,this,0);return 1;
  }
  if(msg==WND_M_MENUITEM_SELECTED){SelectIndex((short)LOWORD(value));return 1;}
  return KWndWindow::WndProc(msg,param,value);
 }
};

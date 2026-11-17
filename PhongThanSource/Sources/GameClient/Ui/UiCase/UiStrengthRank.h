#ifndef PHONGTHAN_UI_STRENGTH_RANK_H
#define PHONGTHAN_UI_STRENGTH_RANK_H
#include "KIniFile.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndScrollBar.h"
#include "../../../Core/Src/GameDataDef.h"
#include "../../../../Headers/PhongThanRanking.h"
class KPtRankButton:public KWndButton {
 KWndText80 label;
public:
 int Init(KIniFile* ini,const char* section){
  int result=KWndButton::Init(ini,section),w,h;GetSize(&w,&h);
  KIniFile text;text.WriteInteger("Label","Width",w);text.WriteInteger("Label","Height",h);
  int font=12;ini->GetInteger(section,"Font",12,&font);
  text.WriteInteger("Label","Font",font);text.WriteString("Label","Color","218,255,165");text.WriteInteger("Label","MultiLine",1);
  AddChild(&label);label.Init(&text,"Label");label.Enable(false);
  char value[80];ini->GetString(section,"Text","",value,sizeof(value));SetText(value);return result;
 }
 void SetText(const char* s){label.SetText(s);}
};
class KPtRankCell:public KWndText80 {
public:int WndProc(unsigned int m,unsigned int p,int v){if((m==WM_RBUTTONDOWN||m==WM_LBUTTONDOWN)&&GetParent())return GetParent()->WndProc(m==WM_RBUTTONDOWN?WND_M_POPUPMENU:WND_N_BUTTON_LCLICK,(unsigned int)(KWndWindow*)this,v);return KWndText80::WndProc(m,p,v);}
};
class KUiStrengthRank:public KWndImage {
 static KUiStrengthRank* m_pSelf;
 KWndButton m_Close,m_Tabs[6];KPtRankButton m_Categories[10],m_Previous,m_Next;
 KWndText80 m_Head[7],m_Page;KPtRankCell m_Cells[PT_RANK_ROWS][7];KWndText256 m_Notice;
 KWndScrollBar m_Scroll;KIniFile m_Scheme;int m_Tab,m_Category,m_PageIndex,m_PageCount,m_CategoryIds[10];
 DWORD m_LastSequence;int m_Fields[7];
 int m_SelectedRow,m_RowCount;char m_PlayerNames[PT_RANK_ROWS][32];
 void SelectPlayerRow(int row);
 void Initialize();void SelectTab(int tab);void Query(int category,int page);void ClearRows();void Columns();
 virtual int WndProc(unsigned int msg,unsigned int param,int data);virtual void Breathe();
public:
 static KUiStrengthRank* OpenWindow();static KUiStrengthRank* OpenDefaultWindow();
 static KUiStrengthRank* GetIfVisible();static void CloseWindow(bool destroy=TRUE);
 static void LoadScheme(const char*){}
 static void NewIndexArrive(unsigned int,KRankIndex*,bool=false){}
 static void NewRankArrive(unsigned int,KRankMessage*){}
 static int AttachTempFile(){return 1;}static void RemoveTempFile(){}
};
#endif

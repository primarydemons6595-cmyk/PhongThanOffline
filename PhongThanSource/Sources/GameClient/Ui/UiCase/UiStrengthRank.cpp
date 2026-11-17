#include "KWin32.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../../../Core/Src/CoreShell.h"
#include "UiStrengthRank.h"
#include "UiGame.h"
extern iCoreShell* g_pCoreShell;
KUiStrengthRank* KUiStrengthRank::m_pSelf=0;
static const int ptRankCategories[6][10]={
 {0,2,1,-1,-1,-1,-1,-1,-1,-1},{3,4,5,6,7,8,39,40,-1,-1},
 {11,12,13,14,15,16,17,18,41,42},{9,10,26,44,-1,-1,-1,-1,-1,-1},
 {19,20,21,23,24,25,27,28,34,-1},{35,36,37,38,43,-1,-1,-1,-1,-1}};
KUiStrengthRank* KUiStrengthRank::GetIfVisible(){return m_pSelf&&m_pSelf->IsVisible()?m_pSelf:0;}
KUiStrengthRank* KUiStrengthRank::OpenWindow(){return OpenDefaultWindow();}
KUiStrengthRank* KUiStrengthRank::OpenDefaultWindow(){
 if(!m_pSelf){m_pSelf=new KUiStrengthRank;m_pSelf->Initialize();}
 m_pSelf->Show();m_pSelf->BringToTop();m_pSelf->SelectTab(0);return m_pSelf;
}
void KUiStrengthRank::CloseWindow(bool destroy){if(m_pSelf){m_pSelf->Hide();if(destroy){m_pSelf->Destroy();m_pSelf=0;}}}
void KUiStrengthRank::Initialize(){
 char scheme[256],path[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\PtRanking.ini",scheme);
 m_Scheme.Load(path);Init(&m_Scheme,"Main");m_Tab=0;m_Category=0;m_PageIndex=0;m_PageCount=1;m_LastSequence=0;
 AddChild(&m_Close);m_Close.Init(&m_Scheme,"BtnClose");
 for(int i=0;i<6;++i){char section[32];sprintf(section,"Tab%d",i);AddChild(&m_Tabs[i]);m_Tabs[i].Init(&m_Scheme,section);}
 for(i=0;i<10;++i){AddChild(&m_Categories[i]);m_Categories[i].Init(&m_Scheme,"Category");m_Categories[i].SetPosition(28,89+i*28);}
 for(int col=0;col<7;++col){char section[32];sprintf(section,"Column%d",col);AddChild(&m_Head[col]);m_Head[col].Init(&m_Scheme,section);
  for(int row=0;row<PT_RANK_ROWS;++row){AddChild(&m_Cells[row][col]);m_Cells[row][col].Init(&m_Scheme,section);
   int x,y;m_Head[col].GetPosition(&x,&y);m_Cells[row][col].SetPosition(x,y+26+row*14);m_Cells[row][col].SetText("");}}
 AddChild(&m_Notice);m_Notice.Init(&m_Scheme,"Notice");AddChild(&m_Page);m_Page.Init(&m_Scheme,"Page");
 AddChild(&m_Previous);m_Previous.Init(&m_Scheme,"Previous");AddChild(&m_Next);m_Next.Init(&m_Scheme,"Next");
 AddChild(&m_Scroll);m_Scroll.Init(&m_Scheme,"Scroll");m_Scroll.SetValueRange(0,0);
 Wnd_AddWindow(this,WL_TOPMOST);int sw,sh,w,h;Wnd_GetScreenSize(sw,sh);GetSize(&w,&h);SetPosition(max(0,(sw-w)/2),max(0,(sh-h)/2));
}
void KUiStrengthRank::SelectPlayerRow(int row){
 m_SelectedRow=row;for(int r=0;r<PT_RANK_ROWS;++r)for(int c=0;c<7;++c)m_Cells[r][c].SetTextColor(r==row?0xffffd75c:0xffdaffa5);
}
void KUiStrengthRank::ClearRows(){m_RowCount=0;ZeroMemory(m_PlayerNames,sizeof(m_PlayerNames));SelectPlayerRow(-1);for(int r=0;r<PT_RANK_ROWS;++r)for(int c=0;c<7;++c)m_Cells[r][c].SetText("");}
void KUiStrengthRank::SelectTab(int tab){
 m_Tab=tab;for(int i=0;i<6;++i)m_Tabs[i].CheckButton(i==tab);
 for(i=0;i<10;++i){m_CategoryIds[i]=ptRankCategories[tab][i];if(m_CategoryIds[i]<0){m_Categories[i].Hide();continue;}
  char section[24],text[256];sprintf(section,"Btn_%d",m_CategoryIds[i]);m_Scheme.GetString(section,"Text","",text,sizeof(text));
  m_Categories[i].SetText(text[0]=='$'?text+1:text);m_Categories[i].Show();}
 Query(m_CategoryIds[0],0);
}
void KUiStrengthRank::Query(int category,int page){
 m_Category=category;m_PageIndex=page;m_LastSequence=0;ClearRows();Columns();
 for(int i=0;i<10;++i)m_Categories[i].CheckButton(m_CategoryIds[i]==category);
 char text[256];m_Scheme.GetString("Text","Loading","...",text,sizeof(text));m_Notice.SetText(text);m_Notice.Show();
 PTRankingQuery q;memset(&q,0,sizeof(q));q.Category=category;q.Page=page;
 if(g_pCoreShell)g_pCoreShell->TeamOperation(PT_RANK_ACTION,(unsigned int)&q,sizeof(q));
}
void KUiStrengthRank::Columns(){
 static const int ordinary[]={0,1,2,3,4,5,6},celestial[]={0,1,7,3,9,5,6},help[]={0,1,8,2,3,6,-1},justice[]={0,1,9,7,3,6,-1},power[]={0,1,10,2,3,6,-1},nation[]={0,1,2,-1,5,-1,6};
 const int* layout=ordinary;
 if(m_Category==2||m_Category>=11&&m_Category<=18||m_Category==41||m_Category==42)layout=celestial;
 if(m_Category==23)layout=help;if(m_Category==24||m_Category==25)layout=justice;
 if(m_Category>=35&&m_Category<=38||m_Category==43)layout=power;
 if(m_Category==10)layout=nation;
 char section[32];sprintf(section,"List_%d",m_Category);int columns=7;m_Scheme.GetInteger(section,"ColumnCount",7,&columns);columns=min(7,max(1,columns));
 static const int widths7[]={53,110,56,46,57,74,121},widths6[]={53,130,85,56,60,133,0};int x=140;
 for(int c=0;c<7;++c){m_Fields[c]=layout[c];if(c>=columns){m_Head[c].Hide();for(int r=0;r<PT_RANK_ROWS;++r)m_Cells[r][c].Hide();continue;}
  char key[32],text[256];sprintf(key,"ColumnText_%d",c);m_Scheme.GetString(section,key,"",text,sizeof(text));
  m_Head[c].SetText(text[0]=='$'?text+1:text);m_Head[c].SetPosition(x,74);m_Head[c].SetSize(columns==6?widths6[c]:widths7[c],26);m_Head[c].Show();
  for(int r=0;r<PT_RANK_ROWS;++r){m_Cells[r][c].SetPosition(x,106+r*14);m_Cells[r][c].SetSize(columns==6?widths6[c]:widths7[c],14);m_Cells[r][c].Show();}
  x+=columns==6?widths6[c]:widths7[c];
 }
}
void KUiStrengthRank::Breathe(){
 if(!g_pCoreShell)return;PTRankingPage page;
 if(!g_pCoreShell->GetGameData(PT_RANK_GET,(unsigned int)&page,sizeof(page))||page.Category!=m_Category||page.Page!=m_PageIndex)return;
 if(page.Header.Sequence==m_LastSequence)return;m_LastSequence=page.Header.Sequence;
 ClearRows();m_PageCount=max(1,(page.Total+PT_RANK_ROWS-1)/PT_RANK_ROWS);char text[256];
 m_Scroll.SetValueRange(0,m_PageCount-1);m_Scroll.SetScrollPos(m_PageIndex);m_Previous.Enable(m_PageIndex>0);m_Next.Enable(m_PageIndex+1<m_PageCount);
 sprintf(text,"%d / %d",m_PageIndex+1,m_PageCount);m_Page.SetText(text);
 if(page.Result<0||!page.Count){m_Scheme.GetString("Text",page.Result==-2?"Unavailable":"Empty","...",text,sizeof(text));m_Notice.SetText(text);m_Notice.Show();return;}
 m_Notice.Hide();
 m_RowCount=page.Count;
 for(int r=0;r<page.Count;++r){const PTRankingRow& row=page.Rows[r];lstrcpyn(m_PlayerNames[r],row.Name,32);
  for(int c=0;c<7;++c){text[0]=0;switch(m_Fields[c]){
   case 0:sprintf(text,"%d",m_PageIndex*PT_RANK_ROWS+r+1);break;case 1:lstrcpyn(text,row.Name,sizeof(text));break;
   case 2:sprintf(text,"%d",row.Level);break;case 3:{char key[24];sprintf(key,"Profession%d",row.Profession);m_Scheme.GetString("Text",key,"?",text,sizeof(text));break;}
   case 4:sprintf(text,"%d",row.Repute);break;
   case 5:if(row.Money>=1000000000)sprintf(text,"%.1f B",row.Money/1000000000.0);else if(row.Money>=1000000)sprintf(text,"%.1f M",row.Money/1000000.0);else sprintf(text,"%I64d",row.Money);break;
   case 6:lstrcpyn(text,row.Nation,sizeof(text));break;case 7:sprintf(text,"%d",row.Transcendence);break;
   case 8:sprintf(text,"%d",row.Honor);break;case 9:{__int64 value=row.Justice;if(value<0)value=-value;sprintf(text,"%I64d",value);break;}
   case 10:sprintf(text,"%d",row.Power);break;default:strcpy(text,"-");break;
  }m_Cells[r][c].SetText(text);}
 }
}
int KUiStrengthRank::WndProc(unsigned int msg,unsigned int param,int data){
 if((msg==WM_LBUTTONDOWN||msg==WND_N_BUTTON_LCLICK||msg==WM_RBUTTONDOWN||msg==WND_M_POPUPMENU)&&m_Category!=10){
  int x=(short)LOWORD(data),y=(short)HIWORD(data);
  for(int r=0;r<m_RowCount;++r)for(int c=0;c<7;++c)if(m_Cells[r][c].IsVisible()&&m_Cells[r][c].PtInWindow(x,y)){
   SelectPlayerRow(r);if(msg==WM_RBUTTONDOWN||msg==WND_M_POPUPMENU)PhongThanOpenRemotePeopleMenu(m_PlayerNames[r],x,y);return 1;
  }
 }
 if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)&m_Close){CloseWindow(false);return 1;}
  for(int i=0;i<6;++i)if(param==(unsigned int)&m_Tabs[i]){SelectTab(i);return 1;}
  for(i=0;i<10;++i)if(param==(unsigned int)&m_Categories[i]&&m_CategoryIds[i]>=0){Query(m_CategoryIds[i],0);return 1;}
  if(param==(unsigned int)&m_Previous&&m_PageIndex>0){Query(m_Category,m_PageIndex-1);return 1;}
  if(param==(unsigned int)&m_Next&&m_PageIndex+1<m_PageCount){Query(m_Category,m_PageIndex+1);return 1;}}
 if(msg==WND_N_SCORLLBAR_POS_CHANGED&&param==(unsigned int)&m_Scroll&&data!=m_PageIndex){Query(m_Category,data);return 1;}
 if(msg==WM_KEYDOWN&&param==VK_ESCAPE){CloseWindow(false);return 1;}
 return KWndImage::WndProc(msg,param,data);
}

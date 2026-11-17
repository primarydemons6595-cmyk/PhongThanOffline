#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "../../Sources/GameClient/Ui/Elem/WndWindow.h"
#define WNDIMG_ES_EXCLUDE_TRANS 0x0001
// Minimal storage around the unmodified production hit-test methods.
KWndWindow::KWndWindow(){m_Style=WND_S_VISIBLE;m_nAbsoluteLeft=m_nAbsoluteTop=0;m_Width=m_Height=0;m_pPreviousWnd=m_pNextWnd=m_pFirstChild=m_pParentWnd=0;}
KWndWindow::~KWndWindow(){}
int KWndWindow::Init(KIniFile*,const char*){return 1;}
int KWndWindow::WndProc(unsigned int,unsigned int,int){return 0;}
void KWndWindow::PaintWindow(){}
void KWndWindow::SetSize(int,int){}
void KWndWindow::Show(){m_Style|=WND_S_VISIBLE;}
void KWndWindow::Hide(){m_Style&=~WND_S_VISIBLE;}
void KWndWindow::Enable(int){}
struct Render {int calls;int GetImagePixelAlpha(const char*,int,int,int,int){++calls;return 0;}} render;
Render* g_pRepresentShell=&render;
class KWndImage:public KWndWindow {
public:
 struct Image {char szImage[1];int nFrame,nType;}m_Image;
 KWndImage(){m_Image.szImage[0]=0;m_Image.nFrame=m_Image.nType=0;}
 int PtInWindow(int,int);
 void Setup(int x,int y,int w,int h,unsigned int style=0){m_nAbsoluteLeft=x;m_nAbsoluteTop=y;m_Width=w;m_Height=h;m_Style=WND_S_VISIBLE|style;}
 void Child(KWndImage& child){child.m_pParentWnd=this;if(!m_pFirstChild)m_pFirstChild=&child;else {KWndWindow* last=m_pFirstChild;while(last->GetNextWnd())last=last->GetNextWnd();((KWndImage*)last)->m_pNextWnd=&child;child.m_pPreviousWnd=last;}}
 void Hide(){m_Style&=~WND_S_VISIBLE;}
};
#include "SummonHudHitTesting.inl"
#include "SummonHudRootConfig.inl"
int main(){
 assert(HUD_ROOT_Trans==0&&HUD_ROOT_DummyWnd==1);
 for(int pet=0;pet<2;++pet){
  KWndImage root,petPanel,summonPanel,button;
  root.Setup(6,130,220,130,WND_S_SIZE_WITH_ALL_CHILD);
  petPanel.Setup(6,130,220,49);if(!pet)petPanel.Hide();
  int y=130+(pet?72:0);summonPanel.Setup(6,y,220,49);button.Setup(198,y+10,28,32);
  root.Child(petPanel);root.Child(summonPanel);summonPanel.Child(button);
  assert(root.TopChildFromPoint(210,y+20)==&button);
  assert(root.TopChildFromPoint(210,125)==0);
  assert(root.TopChildFromPoint(210,190)==0); // empty space between panels/below single panel
  summonPanel.Hide();assert(root.TopChildFromPoint(210,y+20)==0);
 }
 // Reproduce the previous empty-image alpha gate: children become unreachable.
 KWndImage oldRoot,panel,button;oldRoot.Setup(6,130,220,130,WNDIMG_ES_EXCLUDE_TRANS);
 panel.Setup(6,130,220,49);button.Setup(198,140,28,32);oldRoot.Child(panel);panel.Child(button);
 assert(oldRoot.TopChildFromPoint(210,150)==0&&render.calls>0);
 puts("PASS production hit routing: sword reachable with/without pet; hidden panels and empty space pass through; previous bug reproduced");
 return 0;
}

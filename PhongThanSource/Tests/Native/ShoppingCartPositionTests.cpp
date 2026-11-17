#include <assert.h>
#include <stdio.h>
#define CURSOR_HEIGHT 32
int cursorX,cursorY,screenW,screenH;
void Wnd_GetCursorPos(int* x,int* y){*x=cursorX;*y=cursorY;}
void Wnd_GetScreenSize(int& w,int& h){w=screenW;h=screenH;}
void Wnd_SetFocusWnd(void*){}
void Wnd_SetExclusive(void*){}
void ALW_GetWndPosition(int&,int&,int,int,bool=false,bool=false);
struct KWndWindow{};
struct KWndShowAnimate:KWndWindow {
 struct Point{int x,y;}m_oFixPos;
 int m_Width,m_Height,x,y;
 KWndShowAnimate():m_Width(160),m_Height(144),x(10),y(100){m_oFixPos.x=10;m_oFixPos.y=100;}
 void SetPosition(int a,int b){x=a;y=b;}
 void Show(){SetPosition(m_oFixPos.x,m_oFixPos.y);}
};
struct KUiShoppingCart:KWndShowAnimate {
 static KUiShoppingCart* m_pSelf;int m_GoodsInfo_CountEdit;void Show();
};
KUiShoppingCart* KUiShoppingCart::m_pSelf=0;
#include "ShoppingCartPosition.inl"
int main(){
 KUiShoppingCart cart;KUiShoppingCart::m_pSelf=&cart;
 int sizes[][2]={{800,600},{1024,768},{1280,720},{1920,1080}};
 for(int r=0;r<4;++r){
  screenW=sizes[r][0];screenH=sizes[r][1];
  int points[][2]={{0,0},{screenW-1,0},{0,screenH-1},{screenW-1,screenH-1},{screenW/2,screenH/2},{420,185}};
  for(int i=0;i<6;++i){
   cursorX=points[i][0];cursorY=points[i][1];cart.Show();
   assert(cart.x>=0&&cart.y>=0&&cart.x+cart.m_Width<=screenW&&cart.y+cart.m_Height<=screenH);
   assert(cursorX>=cart.x&&cursorX<=cart.x+cart.m_Width);
   assert((cart.y==cursorY+32)||(cart.y+cart.m_Height==cursorY));
   assert(cart.m_oFixPos.x==cart.x&&cart.m_oFixPos.y==cart.y);
  }
 }
 puts("PASS actual F2 cart Show and popup placement: follows each click, survives ShowAnimate position reset, stays inside four screen sizes");
 return 0;
}

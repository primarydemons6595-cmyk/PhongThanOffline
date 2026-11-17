#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <string>
#include "../../Headers/PhongThanUiLayoutRules.h"
#define WHEEL_DELTA 120
struct KWndWindow {
 int x,y,w,h,visible;
 KWndWindow():x(0),y(240),w(280),h(360),visible(1){}
 int IsVisible(){return visible;}
 void GetAbsolutePos(int* l,int* t){*l=x;*t=y;}
 void GetSize(int* a,int* b){*a=w;*b=h;}
 virtual int PtInWindow(int px,int py){return visible&&px>=x&&px<x+w&&py>=y&&py<y+h;}
};
struct KWndScrollBar:KWndWindow {
 int pos,maximum;KWndScrollBar():pos(0),maximum(80){w=14;}
 int GetScrollPos(){return pos;}int GetMaxValue(){return maximum;}
 void SetScrollPos(int n){pos=n<0?0:n>maximum?maximum:n;}
};
struct KWndQuestTracker:KWndWindow {
 KWndScrollBar* scroll;KWndWindow tradeLink;bool tradeVisible;int wheelRemainder,layouts;
 KWndQuestTracker():scroll(0),tradeVisible(false),wheelRemainder(0),layouts(0){}
 int PtInWindow(int,int);bool ScrollAt(int,int,int);void Layout(){++layouts;}
};
struct KUiTaskNote {
 KWndQuestTracker m_Tracker;static KUiTaskNote* m_pSelf;
 static bool ScrollTrackerAt(int,int,int,KWndWindow*);
};
KUiTaskNote* KUiTaskNote::m_pSelf=0;
#include "QuestUiInput.inl"
int screenW=1024,screenH=768;
void Wnd_GetScreenSize(int& w,int& h){w=screenW;h=screenH;}
struct KIniFile {
 std::map<std::string,int> values;
 void GetInteger(const char*,const char* k,int d,int* n){*n=values.count(k)?values[k]:d;}
 void WriteInteger(const char*,const char* k,int n){values[k]=n;}
 void GetString(const char*,const char* k,const char*,char* out,int){strcpy(out,values.count(std::string(k)+"X")?"pair":"");}
 void GetInteger2(const char*,const char* k,int* x,int* y){*x=values[std::string(k)+"X"];*y=values[std::string(k)+"Y"];}
 void WriteInteger2(const char*,const char* k,int x,int y){values[std::string(k)+"X"]=x;values[std::string(k)+"Y"]=y;}
};
#include "../../Sources/GameClient/Ui/UiCase/PhongThanNativeUiLayout.h"
int main(){
 KUiTaskNote note;KUiTaskNote::m_pSelf=&note;KWndScrollBar bar;note.m_Tracker.scroll=&bar;
 KWndQuestTracker& t=note.m_Tracker;
 assert(!t.PtInWindow(90,260)); // text and its background pass map clicks through
 assert(t.PtInWindow(5,260)); // left scrollbar remains interactive
 bar.visible=0;assert(!t.PtInWindow(5,260));bar.visible=1;
 t.tradeLink.x=18;t.tradeLink.y=560;t.tradeLink.w=258;t.tradeLink.h=16;
 t.tradeVisible=true;assert(t.PtInWindow(40,565));t.tradeVisible=false;
 assert(KUiTaskNote::ScrollTrackerAt(90,260,-120,0)&&bar.pos==3);
 assert(KUiTaskNote::ScrollTrackerAt(90,260,120,&t)&&bar.pos==0);
 assert(t.ScrollAt(90,260,-60)&&bar.pos==0);assert(t.ScrollAt(90,260,-60)&&bar.pos==3);
 KWndWindow panel;assert(!KUiTaskNote::ScrollTrackerAt(90,260,-120,&panel)&&bar.pos==3);
 assert(!t.ScrollAt(300,260,-120));t.visible=0;assert(!t.ScrollAt(90,260,-120));t.visible=1;
 bar.pos=79;t.ScrollAt(90,260,-240);assert(bar.pos==80);t.ScrollAt(90,260,100000);assert(bar.pos==0);
 bar.maximum=0;assert(!t.ScrollAt(90,260,-120));
 int sizes[][2]={{800,600},{1024,768},{1280,720},{1920,1080}};
 for(int i=0;i<4;++i){
  screenW=sizes[i][0];screenH=sizes[i][1];KIniFile task,item;
  task.values["Top"]=item.values["Top"]=116;
  task.values["StartPosX"]=-215;task.values["StartPosY"]=116;
  item.values["StartPosX"]=800;item.values["StartPosY"]=116;
  PhongThanAnchorNativePanel(task,false);PhongThanAnchorNativePanel(item,true);
  int top=task.values["Top"];assert(top==item.values["Top"]);
  assert(task.values["StartPosY"]==top);assert(item.values["StartPosY"]==top);
  assert(PhongThanShopAlignedTop(top,435,400,screenH)==top);
 }
 assert(PhongThanShopAlignedTop(-20,435,400,600)==0);
 assert(PhongThanShopAlignedTop(500,435,400,600)==200);
 puts("PASS quest map click-through, left scrollbar/link clicks, wheel/clamps/partial deltas/occlusion, F11/F4/shop top alignment at four resolutions");
 return 0;
}

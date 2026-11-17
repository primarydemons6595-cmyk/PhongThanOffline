#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum {SMT_WAR=8};
struct Button {
 int x,y,w,h;bool visible,enabled;
 void GetSize(int* a,int* b) {*a=w;*b=h;}
 void SetPosition(int a,int b) {x=a;y=b;}
 void Enable(bool v) {enabled=v;}
 void Show() {visible=true;}
 void Hide() {visible=false;}
};
struct KUiSysMsgCentre {
 struct {int nNumValid;}m_MsgHeap[10];
 Button m_MsgIconBtn[SMT_WAR];
 int m_nIconAnchorX,m_nIconAnchorY;
 void SyncMessageButtons();
};
#include "../../Sources/GameClient/Ui/UiCase/UiSysMsgInteraction.inl"
static int checks=0;
void Check(bool good,const char* label) {
 ++checks;if(!good){printf("FAIL: %s\n",label);exit(1);}
}
int main() {
 KUiSysMsgCentre centre;memset(&centre,0,sizeof(centre));
 centre.m_nIconAnchorX=172;centre.m_nIconAnchorY=0;
 for(int i=0;i<SMT_WAR;++i){centre.m_MsgIconBtn[i].w=50;centre.m_MsgIconBtn[i].h=19;}
 centre.SyncMessageButtons();
 for(i=0;i<SMT_WAR;++i)Check(!centre.m_MsgIconBtn[i].visible&&!centre.m_MsgIconBtn[i].enabled,"empty icon cannot intercept");
 centre.m_MsgHeap[0].nNumValid=1;centre.SyncMessageButtons();
 Check(centre.m_MsgIconBtn[0].visible&&centre.m_MsgIconBtn[0].enabled&&centre.m_MsgIconBtn[0].x==172,"live invitation matches drawn target");
 for(i=1;i<SMT_WAR;++i)Check(!centre.m_MsgIconBtn[i].visible,"overlapping empty siblings hidden");
 for(i=0;i<SMT_WAR;++i)centre.m_MsgHeap[i].nNumValid=1;
 centre.SyncMessageButtons();
 for(i=0;i<SMT_WAR;++i) {
  Check(centre.m_MsgIconBtn[i].visible&&centre.m_MsgIconBtn[i].x==172-i*50,"multiple queues packed using actual SPR width");
  Check(!i || centre.m_MsgIconBtn[i].x+50<=centre.m_MsgIconBtn[i-1].x,"visible icon hitboxes never overlap");
 }
 centre.m_MsgHeap[1].nNumValid=0;centre.SyncMessageButtons();
 Check(!centre.m_MsgIconBtn[1].visible&&centre.m_MsgIconBtn[2].x==122,"removed queue releases hitbox, following icon compacts");
 printf("PASS: %d invitation hitbox checks\n",checks);return 0;
}

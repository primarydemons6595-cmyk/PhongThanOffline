#include "../../Headers/PhongThanSystemChatHistory.h"
#include "../../Headers/PhongThanUiPopupAnchor.h"
#include <string.h>
#include <stdio.h>
struct Item {int serial;};
struct History {
 int count,length;char bytes[128];Item item;
 int AddOneMessage(const char* text,int len,int,int,Item* source){++count;length=len;memcpy(bytes,text,len);item=*source;return 1;}
};
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
 History system;memset(&system,0,sizeof(system));Item item;item.serial=1234;
 // Rich text includes a zero byte: the encoded length must survive intact.
 const char encoded[]={1,0,2,'S','e','r','v','e','r'};
 CHECK(PtKeepSystemChat(system,false,true,3,3,9,4,encoded,sizeof(encoded),&item));
 CHECK(system.count==1&&system.length==sizeof(encoded)&&!memcmp(system.bytes,encoded,sizeof(encoded))&&system.item.serial==1234);
 CHECK(PtKeepSystemChat(system,true,false,5,3,4,4,encoded,sizeof(encoded),&item));CHECK(system.count==2);
 CHECK(PtKeepSystemChat(system,false,true,7,3,4,4,encoded,sizeof(encoded),&item));CHECK(system.count==3);
 // The normal CH_SYSTEM receiver must not copy a second time to itself.
 CHECK(!PtKeepSystemChat(system,false,false,7,3,4,4,encoded,sizeof(encoded),&item));CHECK(system.count==3);
 // Nearby/world/team/faction/private messages stay out, even if they say GM.
 for(int resource=0;resource<9;++resource)if(resource!=4){CHECK(!PtKeepSystemChat(system,false,true,8,3,resource,4,"GM: system message",18,&item));CHECK(system.count==3);}
 CHECK(!PtKeepSystemChat(system,false,true,8,-1,-1,-1,encoded,sizeof(encoded),&item));
 // Real display sizes: lower-half click with enough space must not flip up.
 int x=300,y=400;PtPopupAtClick(x,y,235,360,1024,768);CHECK(x==300&&y==400);
 x=300;y=500;PtPopupAtClick(x,y,235,360,1920,1080);CHECK(x==300&&y==500);
 x=1000;y=750;PtPopupAtClick(x,y,235,360,1024,768);CHECK(x==789&&y==408);
 x=-10;y=-20;PtPopupAtClick(x,y,235,360,1024,768);CHECK(x==0&&y==0);
 const int screens[][2]={{800,600},{1024,768},{1280,720},{1366,768},{1920,1080}};
 for(int s=0;s<5;++s)for(int cy=0;cy<=screens[s][1];cy+=50)for(int cx=0;cx<=screens[s][0];cx+=100){
  x=cx;y=cy;PtPopupAtClick(x,y,235,360,screens[s][0],screens[s][1]);
  CHECK(x>=0&&y>=0&&x+235<=screens[s][0]&&y+360<=screens[s][1]);
  if(cx+235<=screens[s][0]&&cy+360<=screens[s][1])CHECK(x==cx&&y==cy);
 }
 printf("PASS SYSTEM_CHAT_MENU %d checks: server/GM/local-system, rich text+item preserved, no player-chat leakage or double insertion, click anchor and screen bounds\n",checks);return 0;
}

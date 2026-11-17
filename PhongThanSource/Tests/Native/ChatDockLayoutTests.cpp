#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanChatLayout.h"
int main(){int checks=0;for(int width=800;width<=1920;width+=160)for(int height=600;height<=1080;height+=120){
 PTChatLayout normal=PTMakeChatLayout(width,height,156),dock=PTMakeChatDockLayout(width,height,156,260),restored=PTMakeChatDockLayout(width,height,156,0);
 if(dock.RootX!=width||dock.RootY||dock.Width!=260||dock.BodyHeight+45!=height||dock.SystemX||dock.SystemY+dock.SystemBodyHeight+25!=height||dock.FilterY!=normal.FilterY||dock.InputY!=normal.InputY||memcmp(&normal,&restored,sizeof(normal)))return 1;
 ++checks;
 }printf("PASS native chat dock geometry: %d resolutions, unchanged world width, separate system panel, reversible normal layout\n",checks);return 0;}

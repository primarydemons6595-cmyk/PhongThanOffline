#include "KCore.h"
#include "KPakList.h"
#include <stdio.h>
int main(int argc,char**argv){
 if(argc!=3)return 2;FILE*f=fopen(argv[2],"rb");if(!f)return 3;
 g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList pak;if(!pak.Open("\\package.ini"))return 4;g_SetPakFileMode(1);
 int count=0,failed=0;unsigned short n;
 while(fread(&n,2,1,f)==1){char path[1024];if(n>=sizeof(path)||fread(path,1,n,f)!=n)return 5;path[n]=0;
  SPROFFS*offsets=0;SPRHEAD*h=SprGetHeader(path,offsets);++count;
  if(!h||!h->Frames||!h->Directions){++failed;printf("FAIL index=%d path=%s\n",count,path);}if(h)SprReleaseHeader(h);
 }
 fclose(f);printf("NATIVE_VISUAL_ALIAS_RESULT checked=%d failed=%d NOT_WORLD_VISUAL_PROOF\n",count,failed);return failed?1:0;
}

#include "KWin32.h"
#include "KFilePath.h"
#include "KTabFile.h"
#include <stdio.h>
enum {item_ibitem=8};
#include "../../Sources/Core/Src/PhongThanConsumableData.h"
int main(int argc,char**argv){
 if(argc!=2)return 1;g_SetRootPath(argv[1]);g_SetFilePath("\\");
 if(!PTConsumableLoad()){puts("FAIL actual native consumable table loader");return 1;}
 int ids[]={14,15,16,17,18,19,20,21,22,23,24,25,26,27,330,334,782};
 for(int i=0;i<sizeof(ids)/sizeof(ids[0]);++i){bool found=false;for(int j=0;j<ptConsumableCount;++j)if(ptConsumableDefs[j].detail==ids[i]&&ptConsumableDefs[j].part==0&&ptConsumableDefs[j].seconds>0)found=true;
  if(!found){printf("FAIL detail=%d\n",ids[i]);return 1;}}
 printf("PASS real KTabFile loader: %d definitions, all Ngoc powders and Lam Tien/Thien Huong durations mapped\n",ptConsumableCount);return 0;
}

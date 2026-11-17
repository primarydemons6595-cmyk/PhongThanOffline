#ifndef PHONGTHAN_QUICK_USE_H
#define PHONGTHAN_QUICK_USE_H
#include <string.h>
inline bool PhongThanHasUseScript(const char* script){return script&&script[0]&&strcmp(script,"NONE")&&strcmp(script,"none")&&strcmp(script,"0");}
inline bool PhongThanQuickUse(int genre,const char* script,bool effect,bool authored){
 if(genre==0)return false; // All worn equipment, including mounts/fabao/jade/seals.
 if(genre==1||genre==5||genre==7)return true;
 if(genre==8)return PhongThanHasUseScript(script)||effect||authored;
 if(genre==2||genre==3||genre==4||genre==6)return PhongThanHasUseScript(script);
 return false;
}
#endif

#include <windows.h>
#include <stdio.h>
#include <string.h>
static void g_StrCpyLen(char* to,const char* from,int count){if(count>0){strncpy(to,from,count-1);to[count-1]=0;}}
struct KTabFile {void Load(const char*){}void GetString(char* key,const char*,const char*,char* result,int count){if(!strcmp(key,"Monster"))g_StrCpyLen(result,"Hoan Cau Tinh",count);else result[0]=0;}};
#include "../../Sources/Core/Src/PhongThanNpcDisplayNames.inl"
int main(){
 const char* name=PhongThanRestoredNpcDisplayName("<color=green>Monster<color>");char plain[128];unsigned int color=0;
 if(!name||!PhongThanParseNpcName(name,plain,sizeof(plain),color)||strcmp(plain,"Hoan Cau Tinh")||color!=0xff00ff00)return 1;
 name=PhongThanRestoredNpcDisplayName("<c=#ff8040>Monster<c>");color=0;
 if(!name||!PhongThanParseNpcName(name,plain,sizeof(plain),color)||strcmp(plain,"Hoan Cau Tinh")||color!=0xffff8040)return 2;
 name=PhongThanRestoredNpcDisplayName("Monster");if(!name||strcmp(name,"Hoan Cau Tinh"))return 3;
 if(PhongThanRestoredNpcDisplayName("unknown"))return 4;
 puts("PASS native NPC translation preserves authored named/RGB colours and plain identity");return 0;
}

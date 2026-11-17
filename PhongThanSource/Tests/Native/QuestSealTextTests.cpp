#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include <map>
#include <set>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include "../../Sources/Core/Src/PhongThanQuestSealText.inl"
#define CHECK(c) do{if(!(c)){printf("FAIL seal metadata line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){
 CHECK(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);
 int one=PhongThanQuestSealParam(1),two=PhongThanQuestSealParam(2);
 const char* name=PhongThanQuestSealName(one);CHECK(name&&name[0]);const char* other=PhongThanQuestSealName(two);CHECK(other&&strcmp(name,other));
 CHECK(PhongThanQuestSealImage(one)&&!strcmp(PhongThanQuestSealImage(one),"spr\\phongthan\\dialog\\answer.spr"));
 CHECK(PhongThanQuestSealTooltip(two)&&PhongThanQuestSealTooltip(two)[0]);
 CHECK(!PhongThanQuestSealName(0)&&!PhongThanQuestSealName(0x2e000001)&&!PhongThanQuestSealName(0x2c000065));
 CHECK(PhongThanQuestSealFromParam(one)==1&&PhongThanQuestSealFromParam(two)==2&&PhongThanQuestSealFromParam(0)==0);
 puts("PASS native seal metadata: separate names, exact client SPR path, tooltip, private param namespace and unchanged event/base item names");return 0;
}

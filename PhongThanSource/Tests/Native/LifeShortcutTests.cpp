#include <string.h>
#include <assert.h>
#include <stdio.h>
#define strcmpi _stricmp
#include "LifeShortcutLookup.inl"
int main(){
 assert(FindWindow("liveSkill")==22&&FindWindow("LIVESKILL")==22);
 assert(FindWindow("skills")==4&&FindWindow("items")==3&&FindWindow("tasknote")==8);
 assert(FindWindow("ibshop")==20&&FindWindow("tongmanager")==18&&FindWindow("unknown")==-1);
 puts("PASS actual shortcut window lookup: L liveSkill route, case insensitive, combat/F4/F11/F2 slots unchanged");return 0;
}

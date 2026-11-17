#include "../../Headers/PhongThanQuickUse.h"
#include <assert.h>
#include <stdio.h>
int main(){
 for(int detail=0;detail<14;++detail)assert(!PhongThanQuickUse(0,"\\script\\equip.lua",true,true));
 assert(PhongThanQuickUse(1,"NONE",true,false));assert(PhongThanQuickUse(5,"NONE",false,false));assert(PhongThanQuickUse(7,"NONE",false,false));
 assert(PhongThanQuickUse(6,"\\script\\phongthan\\items\\compass.lua",false,false));
 assert(PhongThanQuickUse(6,"\\script\\phongthan\\items\\treasure_map.lua",false,false));
 assert(PhongThanQuickUse(8,"\\script\\phongthan\\items\\f2_gifts.lua",false,false));assert(PhongThanQuickUse(8,"NONE",true,false));
 assert(!PhongThanQuickUse(6,"NONE",false,false));assert(!PhongThanQuickUse(3,"NONE",false,false));assert(!PhongThanQuickUse(4,"NONE",false,false));
 assert(PhongThanQuickUse(4,"\\script\\quest.lua",false,false));assert(!PhongThanQuickUse(8,"NONE",false,false));
 puts("PASS quick-use policy: medicine/portal/books/scripted gifts/maps/buffs; all equipment families and inert materials rejected");return 0;
}

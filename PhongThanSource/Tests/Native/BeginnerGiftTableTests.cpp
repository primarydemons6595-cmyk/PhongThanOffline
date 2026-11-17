#include "KCore.h"
#include "KPakList.h"
#include "KTabFile.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do {if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char**argv) {
    if(argc!=2)return 2;
    SetCurrentDirectoryA(argv[1]);g_SetRootPath(NULL);g_SetFilePath("\\");
    KPakList packs;CHECK(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(0);
    KTabFile potions;CHECK(potions.Load("\\settings\\item\\001\\potion.txt"));
    int found[2]={0,0};
    for(int row=2;row<=potions.GetHeight();++row){
        int genre=-1,detail=-1,part=-1,level=-1,stack=0;
        potions.GetInteger(row,2,-1,&genre);potions.GetInteger(row,3,-1,&detail);
        potions.GetInteger(row,4,-1,&part);potions.GetInteger(row,12,-1,&level);
        potions.GetInteger(row,13,0,&stack);
        if(genre==1 && (detail==0 || detail==3) && part==0 && level==1){
            CHECK(stack==100);found[detail==0?0:1]++;
        }
    }
    CHECK(found[0]==1 && found[1]==1);
    KTabFile items;CHECK(items.Load("\\settings\\item\\001\\magicscript.txt"));
    int bags=0;
    for(int r=2;r<=items.GetHeight();++r){int id=0;items.GetInteger(r,4,0,&id);
        if(id==9173){char script[128];items.GetString(r,15,"",script,sizeof(script));
            CHECK(!strcmp(script,"\\script\\item\\camnangtanthu.lua"));bags++;}}
    CHECK(bags==1);g_pPakList=NULL;
    printf("PASS BEGINNER_GIFT_ENGINE_TABLES %s potion_stacks=100 bag=9173 disk_first_PAK_fallback\n",argv[1]);
    return 0;
}

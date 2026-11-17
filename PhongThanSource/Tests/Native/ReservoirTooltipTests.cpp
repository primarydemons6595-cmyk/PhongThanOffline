#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KLuaScript.h"
#include "KSG_StringProcess.h"
#include "../../Headers/PhongThanReservoirs.h"
#include "../../Sources/Core/Src/PhongThanReservoirTooltip.inl"
#include "ReservoirItemMethods.inc"
#include <limits.h>
#define MAX_INT INT_MAX
static TestItem Item[1];
static int CarriedWeight(TestItem& input) {
    Item[0]=input;
    struct Entry {int nIdx;} entry={0};Entry* item=&entry;
    struct Values {int CarryWeight;} result={0};Values* values=&result;
#include "ReservoirCarryWeight.inc"
    return result.CarryWeight;
}
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv)
{
    if(argc!=3)return 2;
    CHECK(SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetPakFileMode(0);
    FILE* f=fopen(argv[2],"r");CHECK(f!=0);
    int detail,part,capacity,effect,rate,rows=0;
    while(fscanf(f,"%d%d%d%d%d",&detail,&part,&capacity,&effect,&rate)==5) {
        const char* path=PhongThanReservoirTooltip(8,detail,part,capacity,"NONE");CHECK(path!=0);
        CHECK(!PhongThanReservoirTooltip(8,detail,part,capacity,"custom.lua"));
        CHECK(!PhongThanReservoirTooltip(1,detail,part,capacity,"NONE"));
        CHECK(!PhongThanReservoirTooltip(8,detail,part+1,capacity,"NONE"));
        TestItem item;item.m_CommonAttrib.nItemGenre=8;item.m_CommonAttrib.nDetailType=detail;
        item.m_CommonAttrib.nParticularType=part;item.m_CommonAttrib.nMaxStack=capacity;
        CHECK(item.IsReservoir() && !item.IsStack());
        item.SetStackNum(capacity-17);CHECK(item.GetStackNum()==capacity-17);
        CHECK(item.GetInventoryCount()==1);
        item.weight=7;CHECK(CarriedWeight(item)==7);
        item.AddStackNum(20);CHECK(item.GetStackNum()==capacity-17);
        item.SetStackNum(capacity+10);CHECK(item.GetStackNum()==capacity);
        CHECK(item.GetInventoryCount()==1);
        CHECK(CarriedWeight(item)==7);
        int values[]={capacity,capacity-17,1,0,-1,capacity+1};
        for(int v=0;v<6;++v) {
            char out[1024],expected[100],speed[100];
            CHECK(PTReservoirDescribe(path,values[v],out,sizeof(out)));
            int clamped=values[v]<0?0:(values[v]>capacity?capacity:values[v]);
            sprintf(expected,">%d / %d<",clamped,capacity);CHECK(strstr(out,expected)!=0);
            sprintf(speed,"%d ",rate);CHECK(strstr(out,speed)!=0);
            CHECK(strstr(out,effect==153?"Sinh l\xf9\x63":"N\xe9i l\xf9\x63")!=0);
        }
        ++rows;
    }
    fclose(f);CHECK(rows==25);
    CHECK(!PhongThanReservoirTooltip(8,1054,3,1,"NONE"));
    TestItem pile;pile.m_CommonAttrib.nItemGenre=1;pile.m_CommonAttrib.nMaxStack=100;
    CHECK(!pile.IsReservoir() && pile.IsStack());pile.SetStackNum(30);pile.AddStackNum(5);CHECK(pile.GetStackNum()==35);
    CHECK(pile.GetInventoryCount()==35);
    pile.weight=7;CHECK(CarriedWeight(pile)==245);
    CHECK(!PTReservoirDescribe("\\missing_reservoir_test.lua",1,0,0));
    printf("PASS reservoirs=%d checks=%d; real Lua4 formatter, exact item methods, partial/full/empty/clamp/cache-switch\n",rows,checks);
    return 0;
}

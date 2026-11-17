#include <stdio.h>
#include <string.h>
#include "PhongThanDropRules.h"
static int checks = 0;
struct TextIni
{
    const char* raw;
    bool GetString(const char*,const char*,const char*,char* out,int size)
    { if(!raw)return false;strncpy(out,raw,size-1);out[size-1]=0;return true; }
};
#define CHECK(x) do { ++checks; if (!(x)) { printf("FAIL line=%d\n",__LINE__); return 1; } } while(0)
int main()
{
    PtDropTable table; memset(&table,0,sizeof(table));
    table.draws=2; table.range=1000000; table.count=3;
    for (int i=0;i<3;++i) { table.entries[i].genre=1; table.entries[i].level=1; table.entries[i].quantity=1; }
    table.entries[0].weight=120000; table.entries[1].weight=80000; table.entries[2].weight=50000;
    CHECK(PtDropValidate(table));
    CHECK(PtDropPick(table,0)==0); CHECK(PtDropPick(table,119999)==0);
    CHECK(PtDropPick(table,120000)==1); CHECK(PtDropPick(table,199999)==1);
    CHECK(PtDropPick(table,200000)==2); CHECK(PtDropPick(table,249999)==2);
    CHECK(PtDropPick(table,250000)==-1); CHECK(PtDropPick(table,999999)==-1);
    CHECK(PtDropPick(table,-1)==-1); CHECK(PtDropPick(table,1000000)==-1);
    int counts[4]={0,0,0,0};
    for(i=0;i<table.range;++i) { int pick=PtDropPick(table,i); ++counts[pick+1]; }
    CHECK(counts[0]==750000 && counts[1]==120000 && counts[2]==80000 && counts[3]==50000);
    table.entries[0].weight=0; CHECK(PtDropValidate(table)); CHECK(PtDropPick(table,0)==1);
    table.entries[0].weight=-1; CHECK(!PtDropValidate(table)); table.entries[0].weight=1000000; CHECK(!PtDropValidate(table));
    table.entries[0].weight=120000; table.draws=11; CHECK(!PtDropValidate(table)); table.draws=0; CHECK(!PtDropValidate(table)); table.draws=2;
    table.entries[0].quantity=21; CHECK(!PtDropValidate(table)); table.entries[0].quantity=1;
    table.entries[0].genre=2; CHECK(!PtDropValidate(table)); table.entries[0].genre=1;
    table.entries[0].luck=1; CHECK(!PtDropValidate(table)); table.entries[0].luck=0;
    table.count=129; CHECK(!PtDropValidate(table)); table.count=3;
    CHECK(PtDropNameValid("basic_1")); CHECK(!PtDropNameValid("../x")); CHECK(!PtDropNameValid(""));
    CHECK(!PtDropNameValid("abcdefghijklmnopqrstuvwxyz123456"));
    TextIni ini; int number;
    ini.raw="120000";CHECK(PtDropInt(ini,"s","k",-1,&number) && number==120000);
    ini.raw="12junk";CHECK(!PtDropInt(ini,"s","k",-1,&number));
    ini.raw="9999999999999999999999";CHECK(!PtDropInt(ini,"s","k",-1,&number));
    ini.raw="";CHECK(!PtDropInt(ini,"s","k",-1,&number));
    ini.raw=NULL;CHECK(!PtDropInt(ini,"s","k",-1,&number) && number==-1);
    printf("PASS NPC_DROP_RULES checks=%d; exhaustive_rolls=1000000\n",checks); return 0;
}

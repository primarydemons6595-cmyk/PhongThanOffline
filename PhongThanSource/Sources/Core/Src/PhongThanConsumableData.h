#include "../../../Headers/PhongThanTimedItems.h"
#pragma once
#include "KTabFile.h"
#include "KMagicAttrib.h"
struct PTConsumableDef {int detail,part,kind,seconds,id[2],value[2],clock;};
static PTConsumableDef ptConsumableDefs[128];
static int ptConsumableCount=0;
static BOOL PTConsumableLoad()
{
    if(ptConsumableCount) return TRUE;
    KTabFile tab;
    if(!tab.Load("\\settings\\phongthan\\ConsumableEffects.txt")) return FALSE;
    int count=tab.GetHeight()-1;
    if(count<=0 || count>128) return FALSE;
    for(int i=0;i<count;++i) {
        PTConsumableDef& d=ptConsumableDefs[i];ZeroMemory(&d,sizeof(d));
        int r=i+2;
        tab.GetInteger(r,1,-1,&d.detail);tab.GetInteger(r,2,-1,&d.part);
        tab.GetInteger(r,3,0,&d.kind);tab.GetInteger(r,4,0,&d.seconds);
        tab.GetInteger(r,5,0,&d.id[0]);tab.GetInteger(r,6,0,&d.value[0]);
        tab.GetInteger(r,7,0,&d.id[1]);tab.GetInteger(r,8,0,&d.value[1]);tab.GetInteger(r,9,1,&d.clock);
        if(d.detail<0 || d.part<0 || d.kind<1 || d.kind>5 ||
           (d.kind==3 && (d.detail>=28 || d.seconds<=0 || d.seconds>86400)) ||
           (d.kind<=2 && d.value[0]<=0)) return FALSE;
        for(int a=0;a<2;++a) if(d.id[a]<0 || d.id[a]>=magic_normal_end ||
            d.value[a]<0 || d.value[a]>1000000 || (!d.id[a] && d.value[a])) return FALSE;
        if(d.kind==3 && (!d.id[0] && !d.id[1])) return FALSE;
        if(d.kind==4 && (PhongThanTimedItemSlot(d.detail)<28 || d.part!=0 || d.clock<0 || d.clock>1 || d.seconds<=0 || d.seconds>PT_TIMED_ITEM_MAX_SECONDS || (d.id[0]!=192 && d.id[0]!=196) || d.value[0]<=0 || d.id[1])) return FALSE;
        if(d.kind==5 && (d.part!=0 || d.clock!=1 ||
            !((d.detail==133 && d.seconds==3600 && d.id[0]==111 && d.value[0]==20 && !d.id[1]) ||
              ((d.detail==330 || d.detail==782) && d.seconds==3600 && d.id[0]==187 && d.value[0]==100 && !d.id[1]) ||
              (d.detail==334 && d.seconds==86400 && d.id[0]==181 && d.value[0]==50 && d.id[1]==187 && d.value[1]==100))))return FALSE;
        if(d.kind==1 && (d.id[0]!=153 || d.id[1])) return FALSE;
        if(d.kind==2 && (d.id[0]!=154 || d.id[1])) return FALSE;
        for(int j=0;j<i;++j) if(d.detail==ptConsumableDefs[j].detail && d.part==ptConsumableDefs[j].part) return FALSE;
    }
    ptConsumableCount=count;return TRUE;
}
int PhongThanConsumableInitialCharges(int genre,int detail,int part,int capacity,const char* script)
{
    if(genre!=item_ibitem || !script || strcmp(script,"NONE") || capacity<=0 || !PTConsumableLoad()) return 0;
    for(int i=0;i<ptConsumableCount;++i)
        if(ptConsumableDefs[i].detail==detail && ptConsumableDefs[i].part==part) return capacity;
    return 0;
}

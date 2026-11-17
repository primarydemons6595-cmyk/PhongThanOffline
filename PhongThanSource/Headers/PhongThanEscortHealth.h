#ifndef PHONGTHAN_ESCORT_HEALTH_H
#define PHONGTHAN_ESCORT_HEALTH_H
#include <stdio.h>
inline void PhongThanEscortHealthText(int life,int maximum,char* text){
    int value=maximum>0?(int)(10000.0*(life>0?life:0)/maximum):0;
    if(value>10000)value=10000;
    if(value==10000)sprintf(text,"100%%");
    else sprintf(text,"%d.%02d%%",value/100,value%100);
}
#endif

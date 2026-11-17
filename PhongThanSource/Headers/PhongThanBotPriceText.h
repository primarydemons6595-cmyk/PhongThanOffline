#ifndef PT_BOT_PRICE_TEXT_H
#define PT_BOT_PRICE_TEXT_H
#include <stdio.h>
#include <string.h>
// Whole-van quotations. Settlement uses the same rounded quote.
inline void PTBotPriceText(int silver,char* out,int capacity){
 if(!out||capacity<1)return;if(silver<0)silver=0;
 int whole=(int)(((__int64)silver+5000)/10000);if(silver>0&&whole==0)whole=1;
 _snprintf(out,capacity-1,"%d v\271n",whole);
 out[capacity-1]=0;
}
#endif

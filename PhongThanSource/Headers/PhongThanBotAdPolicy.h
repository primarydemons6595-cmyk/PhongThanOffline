#ifndef PHONGTHAN_BOT_AD_POLICY_H
#define PHONGTHAN_BOT_AD_POLICY_H
#include <string.h>
struct PTBotAdLedger {
 struct Entry {unsigned long at,hash;int variant;};Entry recent[64];int next,total;
 PTBotAdLedger():next(0),total(0){memset(recent,0,sizeof(recent));}
 bool Available(int variant,unsigned long hash,unsigned long now)const{for(int i=0;i<total;++i){unsigned long age=now-recent[i].at;if((recent[i].variant==variant&&age<600000)||(hash&&recent[i].hash==hash&&age<3600000))return false;}return true;}
 void Sent(int variant,unsigned long hash,unsigned long now){recent[next].variant=variant;recent[next].hash=hash;recent[next].at=now;next=(next+1)%64;if(total<64)++total;}
};
// Length-delimited native rich text: icon IDs and RGB can contain zero bytes.
inline int PTBotStyleAd(char* out,int capacity,const char* body,int size,unsigned seed){
 if(!out||!body||size<1||size+12>capacity||size+12>255)return 0;
 static const unsigned char colors[][3]={{255,215,60},{90,240,130},{250,140,220},{90,210,255},{255,175,65},{210,155,255}};
 int color=seed%6,used=0;out[used++]=2;for(int i=0;i<3;++i)out[used++]=colors[color][i];
 out[used++]=4;out[used++]=1;out[used++]=1;out[used++]=1;
 if(seed%3!=0){unsigned short icon=(unsigned short)(1+(seed/7)%8);out[used++]=6;memcpy(out+used,&icon,2);used+=2;out[used++]=' ';}
 memcpy(out+used,body,size);return used+size;
}
#endif

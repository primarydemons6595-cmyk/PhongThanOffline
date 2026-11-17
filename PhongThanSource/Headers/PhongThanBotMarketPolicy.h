#ifndef PHONGTHAN_BOT_MARKET_POLICY_H
#define PHONGTHAN_BOT_MARKET_POLICY_H
#include <stdlib.h>
#include <string.h>
// A local asking-price heuristic, not a claim about the player market price.
static int PTBotAskingPrice(int npcPrice,int power,int quality,int units){
 __int64 value=(__int64)(npcPrice>0?npcPrice:1)*2+(power>0?(__int64)power*25:0)+(quality>0?(__int64)quality*500:0);
 if(value>500000000)return 500000000;if(units>1){if(value>500000000/units)return 500000000;value*=units;}if(value<100)value=100;return (int)value;
}
static bool PTBotFairWager(__int64 human,__int64 bot){return human>=0&&bot>=human&&bot<=human+human/2;}
static bool PTBotSalePayment(int asking,int money,__int64 neededBarter){return asking>0&&money>=0&&(__int64)money+neededBarter>=asking;}
// Chat coordinates use the same scale as UiMiniMap::UpdatePosition.
static bool PTBotMeetingCoordinates(const char* text,int& x,int& y){
 for(const char* c=text;*c;++c)if(*c>='0'&&*c<='9'&&(c==text||c[-1]<'0'||c[-1]>'9')){
  char* end=0;long a=strtol(c,&end,10);while(*end==' ')++end;if(*end!=':'&&*end!=','&&*end!='/')continue;++end;while(*end==' ')++end;if(*end<'0'||*end>'9')continue;
  char* tail=0;long b=strtol(end,&tail,10);if(a<1||a>4095||b<1||b>4095)return false;x=(int)a*256;y=(int)b*512;return true;
 }return false;
}
static bool PTBotMapOnlyRequest(const char* query,const char* map){
 if(!query||!map||!map[0])return false;const char* at=strstr(query,map);if(!at)return false;
 char rest[256];int before=at-query,after=strlen(at+strlen(map));if(before+after+2>256)return false;memcpy(rest,query,before);rest[before]=' ';memcpy(rest+before+1,at+strlen(map),after+1);
 const char* allowed[]={"di","den","toi","phu","len","ve","cho","doi","tui","ong","ban","o","tai","day","nhe","nha","voi","truoc","giup","gap"};
 char* token=strtok(rest," .!?");while(token){bool known=false;for(int i=0;i<sizeof(allowed)/sizeof(allowed[0]);++i)if(!strcmp(token,allowed[i]))known=true;if(!known)return false;token=strtok(0," .!?");}return true;
}
#endif

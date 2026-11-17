#ifndef PHONGTHAN_BOT_AUTONOMY_POLICY_H
#define PHONGTHAN_BOT_AUTONOMY_POLICY_H
// Integer-only policies shared with native integration checks (VC6 compatible).
// Autonomous novices stay within their own starter towns/fields. Authored
// objectives and explicit player commands choose their own destinations.
inline int PTBotStarterAreaOwner(int map){
 if(map>=1002&&map<=1004)return map-1002;
 if(map>=1005&&map<=1013)return (map-1005)/3;
 return -1;
}
inline bool PTBotAutomaticAreaAllowed(int profession,int level,int map){
 int owner=PTBotStarterAreaOwner(map);
 return profession<0||profession>2||level>=20||owner<0||owner==profession;
}
inline int PTBotTrainingFloor(int level,int fallback){
 int distance=fallback>=2?255:fallback==1?20:5;
 return level>distance?level-distance:1;
}
inline unsigned long PTBotRetryDelay(int failures){
 unsigned long delay=10000;while(failures>1&&delay<120000){delay*=2;--failures;}
 return delay>120000?120000:delay;
}
struct PTBotQuote {int minimum,reference,maximum;PTBotQuote():minimum(0),reference(0),maximum(0){}};
inline PTBotQuote PTBotReferenceQuote(int npc,int power,int quality,int units,int powerValue,
 int minimumVan=0,int referenceVan=0,int maximumVan=0){
 PTBotQuote q;if(units<1||units>100000||npc<0||power<0||quality<0||powerValue<1)return q;
 __int64 reference=referenceVan>0?(__int64)referenceVan*10000:(__int64)npc*3+(__int64)power*powerValue+(__int64)quality*powerValue*10;
 if(reference<100)reference=100;
 __int64 low=minimumVan>0?(__int64)minimumVan*10000:reference*80/100;
 __int64 high=maximumVan>0?(__int64)maximumVan*10000:reference*120/100;
 low*=units;reference*=units;high*=units;
 if(low<1||low>reference||reference>high||high>500000000)return q;
 q.minimum=(int)low;q.reference=(int)reference;q.maximum=(int)high;return q;
}
inline int PTBotWholeVanMoney(int value){if(value<=0)return 0;__int64 rounded=((__int64)value+5000)/10000*10000;return (int)(rounded?rounded:10000);}
inline PTBotQuote PTBotWholeVanQuote(PTBotQuote q){if(q.reference>0){q.minimum=PTBotWholeVanMoney(q.minimum);q.reference=PTBotWholeVanMoney(q.reference);q.maximum=PTBotWholeVanMoney(q.maximum);}return q;}
inline bool PTBotQuoteAccepts(const PTBotQuote& q,__int64 price){return q.reference>0&&price>=q.minimum&&price<=q.maximum;}
inline int PTBotMarketBudget(__int64 owned,int percent,int maxVan){
 if(owned<=0||percent<1||percent>50||maxVan<1||maxVan>50000)return 0;
 __int64 value=owned*percent/100,limit=(__int64)maxVan*10000;
 return (int)(value<limit?value:limit);
}
inline __int64 PTBotXuValue(int detail,int units,int xuVan){
 if((detail!=46&&detail!=47)||units<1||units>100000||xuVan<1||xuVan>50000)return 0;
 return (__int64)units*(detail==46?20:1)*xuVan*10000;
}
#endif

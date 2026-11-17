#ifndef PHONGTHAN_BAKED_EQUIPMENT_VISUAL_H
#define PHONGTHAN_BAKED_EQUIPMENT_VISUAL_H
#include <string.h>
// Visual metadata only. It must never apply ordinary upgrade deltas to the
// baked Tinh Quan templates. Both tokens are exact TCVN3 catalog bytes.
inline int PhongThanBakedEquipmentVisualLevel(const char* name,int nativeLevel){
 if(!name||nativeLevel<1||nativeLevel>3||!strstr(name,"Tinh Qu\xA9n"))return 0;
 const char* rank=strstr(name,"C\xCAp ");if(!rank||rank[4]<'1'||rank[4]>'3'||rank[4]-'0'!=nativeLevel||rank[5]>='0'&&rank[5]<='9')return 0;
 const char* star=strstr(rank,"(+");if(!star)return 0;star+=2;int n=0;
 while(*star>='0'&&*star<='9'){n=n*10+(*star++-'0');if(n>12)return 0;}
 return *star==')'&&n>=1?n:0;
}
#endif

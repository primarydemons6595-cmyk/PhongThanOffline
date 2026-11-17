#ifndef PHONGTHAN_AVATAR_INFO_H
#define PHONGTHAN_AVATAR_INFO_H
#include <string.h>
// Local HUD text only. A status caption is operator-authored, not a health probe.
inline int PhongThanAvatarInfoY(int skin){return skin==2?71:skin==1?61:54;}
inline int PhongThanAvatarInfoSelect(int mode,unsigned int tick,int seconds,const char texts[4][80]){
 if(mode<1||mode>5)return -1;if(mode<=4)return texts[mode-1][0]?mode-1:-1;
 int slots[4],count=0;for(int i=0;i<4;++i)if(texts[i][0])slots[count++]=i;
 if(!count)return -1;if(seconds<2)seconds=2;if(seconds>120)seconds=120;return slots[(tick/(seconds*1000))%count];
}
inline void PhongThanAvatarInfoClip(const char* source,char* out,int capacity,int font){
 if(capacity<1)return;int n=(int)strlen(source),limit=140*2/(font<10?10:font);if(limit>capacity-1)limit=capacity-1;
 if(n<=limit){memcpy(out,source,n);out[n]=0;}else{if(limit<3){out[0]=0;return;}memcpy(out,source,limit-3);memcpy(out+limit-3,"...",3);out[limit]=0;}
}
#endif

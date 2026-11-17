#ifndef PHONGTHAN_RECONNECT_ROLE_H
#define PHONGTHAN_RECONNECT_ROLE_H
#include <string.h>
// A list position is not character identity. Never fall back to another role
// when a remembered role was requested, including reordered or missing lists.
template<class Role> int PhongThanReconnectRole(const Role* roles,int count,int capacity,const char* remembered,int recommended){
 if(!roles||count<=0||count>capacity||!remembered||!memchr(remembered,0,32))return -1;
 if(remembered[0]){int found=-1;for(int i=0;i<count;++i){if(!memchr(roles[i].Name,0,sizeof(roles[i].Name)))return -1;if(!strcmp(remembered,roles[i].Name)){if(found!=-1)return -1;found=i;}}return found;}
 // First automatic login without a previous character retains the recommendation.
 int index=(recommended>=0&&recommended<count)?recommended:0;
 return roles[index].Name[0]&&memchr(roles[index].Name,0,sizeof(roles[index].Name))?index:-1;
}
#endif

#ifndef PHONGTHAN_SET_IDENTITY_H
#define PHONGTHAN_SET_IDENTITY_H
#include <string.h>
// Component SetID is a tier shared by the three original professions.
// The original set-effects table has a separate row for each profession.
inline int PhongThanSetEffectId(int tier,int profession,const char* name)
{
 if(profession<0||profession>2)return tier;
 if(tier>=1&&tier<=5)return tier+profession*5;
 if(tier==7&&name&&(strstr(name,"\x54\xF6\x20\x4C\xAB\x69")||strstr(name,"\x50\x68\xEF\x20\x4C\xAA")||strstr(name,"\x48\x6F\xB5\x6E\x67\x20\x4D\x69\x6E\x68")))return 52+profession;
 if(tier>=6&&tier<=8)return 16+(tier-6)*3+profession;
 return tier;
}
#endif

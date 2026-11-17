#ifndef PHONGTHAN_ESCORT_RULES_H
#define PHONGTHAN_ESCORT_RULES_H
// 4700..4703 belong to starter gifts. Keep escorts in their own audited range.
inline int PtEscortBase(int kind){return kind==1?4501:kind==2?4525:kind==3?4460:-1;}
inline int PtEscortDay(long utc){return (int)((utc+25200)/86400);}
inline bool PtEscortCanAccept(int phase,int other,int runs,int limit,int level,int minimum,
    bool requireTong,bool hasTong,int credit,int minimumCredit)
{return phase==0&&other==0&&runs>=0&&runs<limit&&level>=minimum&&(!requireTong||hasTong)&&credit>=minimumCredit;}
inline bool PtEscortNear(int x,int y,int tx,int ty,int radius){double dx=(double)x-tx,dy=(double)y-ty;return dx*dx+dy*dy<=(double)radius*radius;}
#endif

#ifndef PHONGTHAN_WEEKLY_DRAGON_H
#define PHONGTHAN_WEEKLY_DRAGON_H
inline bool PtWeeklyDragonItem(int genre,int detail,int part){return genre==8&&detail==7445&&part==2;}
inline bool PtWeeklyDragonState(int value,int& uses,int& day)
{
    uses=0;day=-1;
    if(value==0||value==-1)return true;
    // Older generic IB creation stored Unix creation time in ScriptParam.
    if(value>=1451581200)return true;
    if(((unsigned int)value&0xff000000U)!=0x54000000U)return false;
    uses=(value>>16)&255;day=value&65535;
    return uses>=1&&uses<=7;
}
inline unsigned int PtWeeklyDragonExpiry(int state,unsigned int stored,unsigned int now)
{
 if(stored)return stored;
 if(state>=1451581200)return (unsigned int)(state-1451581200)+604800U;
 return now+604800U;
}
// KSG time starts at 2016-01-01 00:00 UTC+7; its day boundary is local midnight.
inline int PtWeeklyDragonDay(unsigned int now){return now/86400U;}
inline int PtWeeklyDragonPack(int uses,int day){return 0x54000000|(uses<<16)|day;}
inline int PtWeeklyDragonCost(int uses){return uses?1000000:0;}
#endif

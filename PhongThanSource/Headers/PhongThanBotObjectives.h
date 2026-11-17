#ifndef PHONGTHAN_BOT_OBJECTIVES_H
#define PHONGTHAN_BOT_OBJECTIVES_H
// Priorities are preferences, not synchronized timers. A player must satisfy
// the activity's real requirements before it may enter this selection.
enum PTBotWork {PTBOT_STORY=1,PTBOT_DAILY,PTBOT_EVENT,PTBOT_BOSS,PTBOT_TRAIN,PTBOT_VTT};
inline int PTBotVttBand(int level){return level>=30&&level<=50?1:level>=51&&level<=70?2:level>=71&&level<=90?3:level>=91&&level<=120?4:0;}
inline int PTBotObjectiveScore(int work,int hops,int danger,int congestion,unsigned long variation,bool urgent=false){
 if(work<PTBOT_STORY||work>PTBOT_VTT||hops<0||danger>100)return -1;
 int priority=work==PTBOT_VTT?-2000:work==PTBOT_STORY?0:work==PTBOT_DAILY?600:work==PTBOT_EVENT?1100:work==PTBOT_BOSS?1600:2300;
 if(urgent&&(work==PTBOT_EVENT||work==PTBOT_BOSS))priority-=1500;
 return priority+hops*50+danger*12+congestion*180+(variation%360);
}
#endif

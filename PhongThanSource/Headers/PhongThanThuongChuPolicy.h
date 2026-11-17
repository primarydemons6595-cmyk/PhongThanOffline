#ifndef PT_THUONG_CHU_POLICY_H
#define PT_THUONG_CHU_POLICY_H
// TGPT article rules. One immutable mode is selected for the entire session.
struct PTThuongChuMatch {
 int phase,winner,firstMarshal,score[3],generals[3],marshal[3],mode,flags[3];
 unsigned long opened,fightAt,endAt,exitAt;int registration,duration;
 PTThuongChuMatch(){Reset();}
 void Reset(){phase=winner=firstMarshal=0;mode=1;opened=fightAt=endAt=exitAt=0;registration=600;duration=1800;for(int i=0;i<3;++i)score[i]=generals[i]=marshal[i]=flags[i]=0;}
 void Open(unsigned long now,int reg,int fight,int selected=1){Reset();mode=selected>=1&&selected<=3?selected:1;phase=1;opened=now;registration=reg;duration=fight;fightAt=now+reg;endAt=fightAt+fight;}
 bool Admit(int side,int a,int b)const{return phase==1&&(side==1||side==2)&&(side==1?a:b)<100&&(side==1?a-b:b-a)<10;}
 int Begin(unsigned long now,int a,int b,int minimum=5){if(phase!=1||now<fightAt)return 0;if(a<minimum||b<minimum){Finish(now,0);return -1;}phase=2;return 1;}
 int World()const{return mode==1?1071:1064;}
 bool DeliverFlag(int side,bool commander,unsigned long now){if(phase!=2||side<1||side>2)return false;if(commander){++flags[side];if(flags[side]>=3)Finish(now,side);}return true;}
 int MarshalDue(unsigned long now)const{if(phase!=2)return -1;for(int i=0;i<3;++i)if(!marshal[i]&&now>=fightAt+(unsigned long)(600*(i+1)))return i;return -1;}
 void Finish(unsigned long now,int side){phase=3;winner=side;exitAt=now+30;}
 bool Capture(int slot,int side,bool neutral,unsigned long now){if(phase!=2||slot<0||slot>2||side<1||side>2||generals[slot]==side)return false;if(neutral&&!firstMarshal)firstMarshal=side;generals[slot]=side;score[side]+=neutral?100:30;int n=0;for(int i=0;i<3;++i)if(generals[i]==side)++n;if(n==3)Finish(now,side);if(neutral&&slot<2&&!marshal[slot+1])marshal[slot+1]=-1;return true;}
 int Decide(int randomSide)const{int a=0,b=0;for(int i=0;i<3;++i){if(generals[i]==1)++a;if(generals[i]==2)++b;}if(a!=b)return a>b?1:2;if(score[1]!=score[2])return score[1]>score[2]?1:2;if(firstMarshal)return firstMarshal;return randomSide==1?1:2;}
 static int Points(int current,int delta){if(delta>0&&current>2000000000-delta)return 2000000000;int n=current+delta;return n<0?0:n;}
 static int QuestGoal(int kind){return kind==4?3:kind>=1&&kind<=3?10:0;}
 static int QuestCopper(int previous){return previous>=3?3:0;}
 static bool CanTakeQuest(int previous){return previous>=0&&previous<10;}
 static int Reward(int side,int winning,bool quest){if(!winning)return 0;return side==winning?(quest?2000000:1000000):(quest?1500000:700000);}
};
#endif

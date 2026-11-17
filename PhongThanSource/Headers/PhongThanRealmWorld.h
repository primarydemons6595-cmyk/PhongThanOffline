#ifndef PHONGTHAN_REALM_WORLD_H
#define PHONGTHAN_REALM_WORLD_H
// Calendar days are supplied as UTC+7 day numbers, never process uptime.
namespace PtRealmWorld {
struct State {int Phase,Day,Mask,LastFullDay,Streak,Reputation,FinalDay,FinalMask;};
inline bool Valid(const State& s){return s.Phase>=0&&s.Phase<=3&&s.Day>=0&&s.Mask>=0&&s.Mask<=7&&
 s.LastFullDay>=0&&s.Streak>=0&&s.Streak<=3&&s.Reputation>=0&&s.Reputation<=1500&&
 s.FinalDay>=0&&s.FinalMask>=0&&s.FinalMask<=7&&
 (s.Phase<1||s.Streak==3)&&(s.Phase<2||s.Reputation==1500)&&(s.Phase<3||s.FinalMask==7);}
inline bool AdvanceDay(State& s,int day){
 if(!Valid(s)||day<=0)return false;
 if(s.Phase==0&&day>s.Day){s.Day=day;s.Mask=0;if(s.LastFullDay>0&&day>s.LastFullDay+1)s.Streak=0;return true;}
 if(s.Phase==2&&day>s.FinalDay){s.FinalDay=day;s.FinalMask=0;return true;}return false;
}
inline bool Dragon(State& s,int day,int dragon,bool registered){
 if(!Valid(s)||day<=0||dragon<0||dragon>2||s.Phase==1||s.Phase==3)return false;
 int bit=1<<dragon;
 if(s.Phase==0){
  if(day<s.Day||day<=s.LastFullDay)return false;
  if(day!=s.Day){s.Day=day;s.Mask=0;}
  if(s.Mask&bit)return false;s.Mask|=bit;
  if(s.Mask==7){s.Streak=s.LastFullDay==day-1?s.Streak+1:1;s.LastFullDay=day;if(s.Streak==3)s.Phase=1;}
  return true;
 }
 if(!registered||day<s.FinalDay)return false;
 if(day!=s.FinalDay){s.FinalDay=day;s.FinalMask=0;}
 if(s.FinalMask&bit)return false;s.FinalMask|=bit;if(s.FinalMask==7)s.Phase=3;return true;
}
inline void Reputation(State& s){if(s.Phase==1&&s.Reputation<1500){++s.Reputation;if(s.Reputation==1500){s.Phase=2;s.FinalDay=s.FinalMask=0;}}}
inline bool IsRealmMap(int map){return (map>=1073&&map<=1077)||map==60015;}
inline bool CanEnter(const State& s,int map,int stage,int side,int level){
 if(!IsRealmMap(map))return true;
 if(!Valid(s)||s.Phase!=3)return false;
 if(map==1073&&(stage==11||stage==12))return true;
 return stage==13&&(side==1||side==2)&&level>=1;
}
}
#endif

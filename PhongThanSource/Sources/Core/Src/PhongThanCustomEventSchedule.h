#ifndef PT_CUSTOM_EVENT_SCHEDULE_H
#define PT_CUSTOM_EVENT_SCHEDULE_H
#include <time.h>
// Dates and minutes are wall-clock Vietnam time, never host-local time.
struct PTCustomEventSchedule {
 int startDay,startMinute,endDay,endMinute;bool yearly;
 PTCustomEventSchedule():startDay(0),startMinute(0),endDay(0),endMinute(0),yearly(false){}
 bool Present()const{return startDay!=0;}
 static int Ordinal(int day){int y=day/10000,m=day/100%100,d=day%100;static int length[]={31,28,31,30,31,30,31,31,30,31,30,31};
  if(y<1||y>9999||m<1||m>12||d<1)return -1;bool leap=y%4==0&&(y%100!=0||y%400==0);if(d>length[m-1]+(m==2&&leap?1:0))return -1;
  int n=(y-1)*365+(y-1)/4-(y-1)/100+(y-1)/400+d;for(int i=1;i<m;++i)n+=length[i-1]+(i==2&&leap?1:0);return n;
 }
 bool Valid()const{int a=Ordinal(startDay),b=Ordinal(endDay);if(a<0||b<0||startMinute<0||startMinute>1439||endMinute<0||endMinute>1439||b<a||(a==b&&endMinute<=startMinute))return false;
  if(yearly&&(b-a>366||(b-a==366&&endMinute>=startMinute)||(startDay%10000==endDay%10000&&startMinute==endMinute)))return false;return true;
 }
 bool Active(int day,int minute)const{if(!Present())return true;if(day<startDay||(day==startDay&&minute<startMinute))return false;
  if(!yearly)return day<endDay||(day==endDay&&minute<endMinute);
  int a=(startDay%10000)*1440+startMinute,b=(endDay%10000)*1440+endMinute,n=(day%10000)*1440+minute;
  return a<b?(n>=a&&n<b):(n>=a||n<b);
 }
 bool ActiveNow(time_t now)const{if(!Present())return true;now+=7*60*60;struct tm* t=gmtime(&now);return t&&Active((t->tm_year+1900)*10000+(t->tm_mon+1)*100+t->tm_mday,t->tm_hour*60+t->tm_min);}
};
#endif

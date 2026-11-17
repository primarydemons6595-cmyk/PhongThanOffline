#ifndef PHONGTHAN_LIVE_CAMPAIGN_POLICY_H
#define PHONGTHAN_LIVE_CAMPAIGN_POLICY_H
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
namespace PtCampaignPolicy {
struct Scope {bool all;std::vector<int> maps;Scope():all(false){}bool Matches(int id)const{if(all)return true;for(unsigned i=0;i<maps.size();++i)if(maps[i]==id)return true;return false;}};
struct Drop {int enabled,genre,detail,part,level,series,probability,quantity;Scope scope;};
struct Campaign {std::string id;int enabled,start,end,days,from,until,xp,drop,extras;Scope scope;std::vector<Drop> items;
 bool Active(long utc)const {if(!enabled||(start&&utc<start)||(end&&utc>=end))return false;time_t local=utc+25200;struct tm* t=gmtime(&local);if(!t)return false;int minute=t->tm_hour*60+t->tm_min;return (days&(1<<t->tm_wday))&&minute>=from&&minute<until;}
};
struct Snapshot {long revision;std::vector<Campaign> campaigns;Snapshot():revision(0){}
 int Rate(int map,long utc,bool xp)const {int rate=1000;for(unsigned i=0;i<campaigns.size();++i){const Campaign& c=campaigns[i];if(c.Active(utc)&&c.scope.Matches(map)){int n=xp?c.xp:c.drop;if(n>rate)rate=n;}}return rate;}
};
inline std::vector<std::string> Split(const std::string& s,char delimiter){std::vector<std::string> r;unsigned begin=0;for(unsigned i=0;i<=s.size();++i)if(i==s.size()||s[i]==delimiter){r.push_back(s.substr(begin,i-begin));begin=i+1;}return r;}
inline bool Number(const std::string& s,int& n,int low,int high){if(s.empty()||s.size()>10)return false;errno=0;char* end=0;long v=strtol(s.c_str(),&end,10);if(errno||!end||*end||v<low||v>high)return false;n=(int)v;return true;}
inline bool Id(const std::string& s){if(s.size()!=32)return false;for(unsigned i=0;i<s.size();++i)if(!((s[i]>='0'&&s[i]<='9')||(s[i]>='a'&&s[i]<='f')))return false;return true;}
inline bool Maps(const std::string& s,Scope& out){if(s=="*"){out.all=true;return true;}std::vector<std::string> rows=Split(s,',');if(rows.empty()||rows.size()>128)return false;std::map<int,int> seen;for(unsigned i=0;i<rows.size();++i){int n;if(!Number(rows[i],n,1,65535)||seen[n])return false;seen[n]=1;out.maps.push_back(n);}return true;}
inline bool Parse(const std::string& bytes,Snapshot& output,std::string& error){
 if(bytes.find('\0')!=std::string::npos){error="embedded_nul";return false;}
 error="";Snapshot next;std::vector<std::string> lines=Split(bytes,'\n');std::map<std::string,int> ids;bool header=false,ended=false;int itemCount=0,budget=0;
 for(unsigned at=0;at<lines.size();++at){std::string line=lines[at];if(!line.empty()&&line[line.size()-1]=='\r')line.erase(line.size()-1);if(line.empty())continue;std::vector<std::string> f=Split(line,'\t');
  if(!header){int revision;if(f.size()!=3||f[0]!="PT_LIVE_CAMPAIGNS"||f[1]!="1"||!Number(f[2],revision,1,2147483647)){error="invalid_header";return false;}next.revision=revision;header=true;continue;}
  if(ended){error="trailing_data";return false;}
  if(f[0]=="C"){
   if(f.size()!=12||!Id(f[1])||ids.find(f[1])!=ids.end()||next.campaigns.size()>=32){error="invalid_campaign";return false;}
   Campaign c;c.id=f[1];int* values[9]={&c.enabled,&c.start,&c.end,&c.days,&c.from,&c.until,&c.xp,&c.drop,&c.extras};
   int lows[9]={0,0,0,1,0,1,1000,1000,0},highs[9]={1,2147483647,2147483647,127,1439,1440,100000,10000,1};
   for(int k=0;k<9;++k)if(!Number(f[2+k],*values[k],lows[k],highs[k])){error="invalid_campaign_value";return false;}
   if(c.from>=c.until||(c.start&&c.end&&c.start>=c.end)||!Maps(f[11],c.scope)){error="invalid_schedule_or_maps";return false;}
   ids[c.id]=(int)next.campaigns.size();next.campaigns.push_back(c);
  }else if(f[0]=="I"){
   if(f.size()!=11||ids.find(f[1])==ids.end()||++itemCount>128){error="invalid_item";return false;}
   Drop d;if(!Number(f[2],d.enabled,0,1)||!Maps(f[3],d.scope)){error="invalid_item_scope";return false;}
   int* values[7]={&d.genre,&d.detail,&d.part,&d.level,&d.series,&d.probability,&d.quantity};int highs[7]={8,1000000,1000000,255,5,1000000,20};
   for(int k=0;k<7;++k)if(!Number(f[4+k],*values[k],k==6?1:0,highs[k])){error="invalid_item_value";return false;}
   if(d.genre==2||d.genre==5){error="unsupported_item_genre";return false;}
   if(d.enabled)budget+=d.quantity;if(budget>64){error="item_budget";return false;}
   next.campaigns[ids[f[1]]].items.push_back(d);
  }else if(f[0]=="END"){
   int nc,ni;if(f.size()!=3||!Number(f[1],nc,0,32)||!Number(f[2],ni,0,128)||nc!=(int)next.campaigns.size()||ni!=itemCount){error="invalid_footer";return false;}ended=true;
  }else{error="unknown_record";return false;}
 }
 if(!header||!ended){error="incomplete_snapshot";return false;}output=next;return true;
}
inline int Experience(int damage,int xp,int life,int rate){if(damage<=0||xp<=0||life<=0||rate<=0)return 0;__int64 base=(__int64)damage*xp/life;if(base>((__int64)2147483647*1000)/rate)return 2147483647;__int64 result=base*rate/1000;return result>2147483647?2147483647:(int)result;}
}
#endif

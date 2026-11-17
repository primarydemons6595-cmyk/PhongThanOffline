#include "../../../Headers/PhongThanQuestSeals.h"
#include "KIniFile.h"
namespace PtQuestSealText {
struct Row {const char* name;const char* image;const char* tooltip;Row():name(0),image(0),tooltip(0){}};
struct Cache {std::map<int,Row> rows;std::set<std::string> strings;DWORD next;CRITICAL_SECTION lock;Cache():next(0){InitializeCriticalSection(&lock);}~Cache(){DeleteCriticalSection(&lock);}};
static Cache cache;
static const char* Intern(const std::string& s){return s.empty()?0:cache.strings.insert(s).first->c_str();}
static bool Hex(KIniFile& ini,const char* section,const char* key,std::string& out,int limit){
 char raw[4096];ini.GetString((char*)section,(char*)key,"",raw,sizeof(raw));int n=strlen(raw);if(n%2||n>limit*2)return false;out="";
 for(int i=0;i<n;i+=2){if(!isxdigit((unsigned char)raw[i])||!isxdigit((unsigned char)raw[i+1]))return false;char pair[3]={raw[i],raw[i+1],0};int c=strtol(pair,0,16);if(c<=0||c>255)return false;out+=(char)c;}return true;
}
static Row Get(int param){Row empty;int id=PhongThanQuestSealFromParam(param);if(!id)return empty;
 EnterCriticalSection(&cache.lock);DWORD now=GetTickCount();
 if((int)(now-cache.next)>=0){cache.next=now+1000;KIniFile ini;std::map<int,Row> next;
  if(ini.Load("\\settings\\phongthan\\QuestSeals.ini")){int version=0,count=0;ini.GetInteger("Main","Version",0,&version);ini.GetInteger("Main","Count",0,&count);
   if(version==1&&count>=0&&count<=PT_QUEST_SEAL_MAX)for(int i=1;i<=count;++i){char section[32];sprintf(section,"Seal%d",i);int key=0;ini.GetInteger(section,"Id",0,&key);std::string name,image,tooltip;
    if(key>0&&key<=PT_QUEST_SEAL_MAX&&Hex(ini,section,"NameHex",name,60)&&Hex(ini,section,"ImageHex",image,255)&&Hex(ini,section,"TooltipHex",tooltip,1200)){Row r;r.name=Intern(name);r.image=Intern(image);r.tooltip=Intern(tooltip);next[PhongThanQuestSealParam(key)]=r;}}
  }cache.rows.swap(next);
 }
 std::map<int,Row>::const_iterator found=cache.rows.find(param);Row result=found==cache.rows.end()?empty:found->second;LeaveCriticalSection(&cache.lock);return result;
}
}
const char* PhongThanQuestSealName(int param){return PtQuestSealText::Get(param).name;}
const char* PhongThanQuestSealImage(int param){return PtQuestSealText::Get(param).image;}
const char* PhongThanQuestSealTooltip(int param){PtQuestSealText::Row row=PtQuestSealText::Get(param);return row.name?(row.tooltip?row.tooltip:""):0;}

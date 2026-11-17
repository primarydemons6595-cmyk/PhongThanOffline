// Per-instance event text. Original item templates retain their names.
#include <map>
#include <set>
#include <string>
#include <ctype.h>
namespace PtCustomItemText {
struct Record {const char* tooltip;const char* plain;const char* rich;Record():tooltip(0),plain(0),rich(0){}};
struct Cache {std::map<int,Record> rows;std::set<std::string> strings;DWORD next;CRITICAL_SECTION lock;Cache():next(0){InitializeCriticalSection(&lock);}~Cache(){DeleteCriticalSection(&lock);}};
static Cache cache;
static const char* Intern(const std::string& value){return value.empty()?0:cache.strings.insert(value).first->c_str();}
static bool Hex(const std::string& raw,std::string& out,unsigned int maximum){if(raw.size()%2||raw.size()>maximum*2)return false;out="";for(unsigned int i=0;i<raw.size();i+=2){if(!isxdigit((unsigned char)raw[i])||!isxdigit((unsigned char)raw[i+1]))return false;char pair[3]={raw[i],raw[i+1],0};char* end=0;long n=strtol(pair,&end,16);if(*end||n<=0||n>255)return false;out+=(char)n;}return true;}
static std::string Plain(const std::string& rich){std::string out;bool tag=false;for(unsigned int i=0;i<rich.size();++i){char c=rich[i];if(c=='<'){tag=true;continue;}if(c=='>'){tag=false;continue;}if(!tag&&c!='\r'&&c!='\n'&&!(out.empty()&&c=='#'))out+=c;}return out;}
static Record Get(int param){Record none;int tag=param&0xff000000;if(tag!=0x2e000000&&tag!=0x2d000000)return none;
 EnterCriticalSection(&cache.lock);DWORD now=GetTickCount();
 if((int)(now-cache.next)>=0){cache.next=now+1000;FILE* file=fopen("settings\\phongthan\\custom_event_items.tsv","rb");
  if(file){std::map<int,Record> candidate;char line[4096];bool valid=true;
   while(fgets(line,sizeof(line),file)){unsigned int len=strcspn(line,"\r\n");if(len==sizeof(line)-1){valid=false;break;}line[len]=0;if(!len)continue;char* end=0;long key=strtol(line,&end,10);int prefix=key&0xff000000;if(end==line||*end!='\t'||(prefix!=0x2e000000&&prefix!=0x2d000000)||candidate.find((int)key)!=candidate.end()){valid=false;break;}std::string tooltip,name;char* raw=end+1;char* extra=strchr(raw,'\t');if(extra)*extra++=0;
    if(!Hex(raw,tooltip,1200)||(extra&&!Hex(extra,name,160))){valid=false;break;}std::string plain=Plain(name);if(plain.size()>60){valid=false;break;}
    Record record;record.tooltip=Intern(tooltip);record.plain=Intern(plain);record.rich=record.plain?Intern(name):0;candidate[(int)key]=record;
   }if(ferror(file))valid=false;fclose(file);if(valid)cache.rows.swap(candidate);
  }else cache.rows.clear();
 }
 std::map<int,Record>::const_iterator found=cache.rows.find(param);Record result=found==cache.rows.end()?none:found->second;LeaveCriticalSection(&cache.lock);return result;
}
}
const char* PhongThanCustomItemName(int param){const char* seal=PhongThanQuestSealName(param);return seal?seal:PtCustomItemText::Get(param).plain;}
static const char* PhongThanCustomItemRichName(int param){return PtCustomItemText::Get(param).rich;}
static const char* PhongThanCustomItemTooltip(int param){return PtCustomItemText::Get(param).tooltip;}

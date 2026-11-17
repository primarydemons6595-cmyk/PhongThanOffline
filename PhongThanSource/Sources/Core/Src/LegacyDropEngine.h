// Reusable, offline-tested legacy data reader. No runtime hook is enabled here.
// CP936 bytes are kept verbatim. Compatibility selection is NOT Kingsoft parity.
#ifndef PT_LEGACY_DROP_ENGINE_H
#define PT_LEGACY_DROP_ENGINE_H
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
namespace LegacyDrop {
typedef std::map<std::string,std::string> Fields;
struct Section { std::string name; Fields fields; };
struct Table {
    std::vector<Section> sections;
    std::vector<std::string> errors;
    int count, range;
    Table():count(0),range(0){}
};
inline std::string Trim(const std::string& s) {
    size_t a=s.find_first_not_of(" \t\r\n"),b=s.find_last_not_of(" \t\r\n");
    return a==std::string::npos?std::string():s.substr(a,b-a+1);
}
inline bool Integer(const std::string& s,int& n) {
    if(s.empty())return false;
    for(size_t i=(s[0]=='-'||s[0]=='+')?1:0;i<s.size();++i)
        if(s[i]<'0'||s[i]>'9')return false;
    char* end=0; double v=strtod(s.c_str(),&end);
    if(!end||*end||v<INT_MIN||v>INT_MAX||end==s.c_str())return false;
    n=(int)v;return true;
}
inline const Section* Find(const Table& t,const std::string& name) {
    for(size_t i=0;i<t.sections.size();++i)if(t.sections[i].name==name)return &t.sections[i];
    return 0;
}
inline bool Get(const Section* s,const char* key,int& n) {
    if(!s)return false;
    Fields::const_iterator i=s->fields.find(key);
    return i!=s->fields.end()&&Integer(i->second,n);
}
inline Table Parse(const std::string& bytes) {
    Table t; int current=-1;size_t start=0;
    if(bytes.size()>2*1024*1024){t.errors.push_back("INPUT_TOO_LARGE");return t;}
    if(bytes.find('\0')!=std::string::npos){t.errors.push_back("EMBEDDED_NUL");return t;}
    if(bytes.size()>=2&&((unsigned char)bytes[0]==255||(unsigned char)bytes[0]==254)){
        t.errors.push_back("UNSUPPORTED_ENCODING_UTF16");return t;
    }
    while(start<bytes.size()) {
        size_t end=bytes.find('\n',start);if(end==std::string::npos)end=bytes.size();
        std::string line=Trim(bytes.substr(start,end-start));start=end+1;
        if(line.empty()||line[0]==';'||line[0]=='#')continue;
        if(line[0]=='['&&line[line.size()-1]==']') {
            Section s;s.name=Trim(line.substr(1,line.size()-2));
            if(Find(t,s.name))t.errors.push_back("DUPLICATE_SECTION:"+s.name);
            t.sections.push_back(s);current=(int)t.sections.size()-1;continue;
        }
        size_t eq=line.find('=');
        if(current<0||eq==std::string::npos){t.errors.push_back("MALFORMED_LINE");continue;}
        std::string key=Trim(line.substr(0,eq)),value=Trim(line.substr(eq+1));
        if(t.sections[current].fields.find(key)!=t.sections[current].fields.end())t.errors.push_back("DUPLICATE_KEY:"+key);
        t.sections[current].fields[key]=value;
    }
    const Section* main=Find(t,"Main");
    bool countOK=Get(main,"Count",t.count)&&t.count>=0&&t.count<=4096;
    if(!countOK)t.errors.push_back("INVALID_COUNT");
    if(!Get(main,"RandRange",t.range)||t.range<=0)t.errors.push_back("INVALID_RANGE");
    int rows=0;
    for(size_t i=0;i<t.sections.size();++i) {
        if(t.sections[i].name=="Main")continue;
        int number=0;
        if(!Integer(t.sections[i].name,number)||number<1||number>t.count){t.errors.push_back("UNEXPECTED_SECTION:"+t.sections[i].name);continue;}
        ++rows;int n;
        const char* keys[]={"Genre","Detail","Particular","RandRate"};
        for(int k=0;k<4;++k)if(!Get(&t.sections[i],keys[k],n)||n<0)t.errors.push_back(std::string("INVALID_ROW_FIELD:")+keys[k]);
        if(Get(&t.sections[i],"RandRate",n)&&n>t.range)t.errors.push_back("RATE_EXCEEDS_RANGE");
    }
    if(rows!=t.count)t.errors.push_back("COUNT_MISMATCH");
    if(countOK)for(int j=1;j<=t.count;++j){char name[32];sprintf(name,"%d",j);if(!Find(t,name))t.errors.push_back(std::string("MISSING_SECTION:")+name);}
    return t;
}
// Explicit laboratory policy: independent Bernoulli trials per row. Sum may
// exceed range; no row is clipped or rescaled. Disabled for production until
// level, magic, money, slot and Treasure contracts have passed acceptance.
class Random {
    unsigned long state;
public:
    Random(unsigned long seed):state(seed?seed:1){}
    unsigned long Next(){state^=state<<13;state^=state>>17;state^=state<<5;return state;}
    unsigned long Below(unsigned long range) {
        if(!range)return 0; // Legacy wrapper; callers requiring errors use TryBelow.
        unsigned long threshold=(0UL-range)%range,x;
        do{x=Next();}while(x<threshold);return x%range;
    }
    bool TryBelow(unsigned long range,unsigned long& value){if(!range)return false;value=Below(range);return true;}
};
inline bool Selected(int rate,int range,int roll) {return range>0&&rate>=0&&rate<=range&&roll>=0&&roll<range&&roll<rate;}
class PreparedTable {
    int range;
    std::vector<int> rates;
    bool valid;
public:
    PreparedTable():range(0),valid(false){}
    bool Prepare(const Table& table,std::string& error) {
        valid=false;range=0;rates.clear();error="";
        if(!table.errors.empty()){error="PARSE_REJECT";return false;}
        if(table.range<=0||table.count<0||table.count>4096){error="INVALID_TABLE";return false;}
        for(size_t s=0;s<table.sections.size();++s){
            if(table.sections[s].name=="Main")continue;int rate;
            if(!Get(&table.sections[s],"RandRate",rate)||rate<0||rate>table.range){error="INVALID_RATE";rates.clear();return false;}
            rates.push_back(rate);
        }
        if(rates.size()!=(size_t)table.count){error="COUNT_MISMATCH";rates.clear();return false;}
        range=table.range;valid=true;return true;
    }
    bool Draw(Random& rng,std::vector<int>& selected,std::string& error) const {
        selected.clear();error="";if(!valid){error="UNPREPARED_TABLE";return false;}
        for(size_t i=0;i<rates.size();++i){unsigned long roll;
            if(!rng.TryBelow(range,roll)){selected.clear();error="INVALID_RNG_RANGE";return false;}
            if(Selected(rates[i],range,roll))selected.push_back((int)i);
        }
        return true;
    }
};
// Slot is supplied explicitly; Treasure fields are never interpreted as draws.
struct NpcSnapshot {int templateId;unsigned long entity;std::string slots[4];int treasure,treasure1;};
inline bool Resolve(const NpcSnapshot& npc,int slot,std::string& path) {
    if(slot<0||slot>3)return false;path=npc.slots[slot];
    if(path.empty()||path.size()>=240||path=="0"||path.find("..")!=std::string::npos||path.find(':')!=std::string::npos)return false;
    for(size_t k=0;k<path.size();++k)if((unsigned char)path[k]<32||path[k]=='"'||path[k]=='*'||path[k]=='?'||path[k]=='|')return false;
    for(size_t i=0;i<path.size();++i)if(path[i]=='/')path[i]='\\';
    const std::string prefix="\\settings\\item\\droprate\\";
    if(path.find('\\')==std::string::npos)path=prefix+path;
    return path.substr(0,prefix.size())==prefix;
}
// Immutable preflight lists; a blocked table disables the entire NPC plan.
// This is intentionally stronger than skipping one slot/row after RNG.
struct Quarantine {
    std::map<int,std::string> npcs;
    std::map<std::string,std::string> tables;
private:
    bool ready;
public:
    Quarantine():ready(false){}
    static int ReadToken(FILE* file,char* text,unsigned limit){
        int c;do{c=fgetc(file);}while(c!=EOF&&isspace((unsigned char)c));
        if(c==EOF)return EOF;unsigned n=0;
        do{if(n+1>=limit)return 0;text[n++]=(char)c;c=fgetc(file);}while(c!=EOF&&!isspace((unsigned char)c));
        text[n]=0;return 1;
    }
    bool Load(FILE* file,std::string& error) {
        ready=false;npcs.clear();tables.clear();error="";
        if(!file){error="QUARANTINE_MISSING";return false;}
        char version[16];if(ReadToken(file,version,sizeof(version))!=1||strcmp(version,"V1")){error="QUARANTINE_VERSION";return false;}
        char kind,key[2048],kindText[2];int rows=0,status;
        while((status=ReadToken(file,kindText,sizeof(kindText)))==1){
            kind=kindText[0];if(ReadToken(file,key,sizeof(key))!=1){error="QUARANTINE_TRUNCATED";return false;}
            if(++rows>8192){error="QUARANTINE_TOO_LARGE";return false;}
            if(kind=='N'){int id;if(!Integer(key,id)||id<0||npcs.find(id)!=npcs.end()){error="INVALID_QUARANTINE_NPC";return false;}npcs[id]="DATA_BLOCKED";}
            else if(kind=='T'){NpcSnapshot npc;npc.slots[0]=key;std::string resolved;if(!Resolve(npc,0,resolved)||resolved!=key||tables.find(resolved)!=tables.end()){error="INVALID_QUARANTINE_TABLE";return false;}tables[resolved]="DATA_BLOCKED";}
            else {error="INVALID_QUARANTINE_RECORD";return false;}
        }
        if(status!=EOF||ferror(file)){error="QUARANTINE_TRUNCATED";return false;}
        ready=true;return true;
    }
    bool Allows(const NpcSnapshot& npc,std::string& error) const {
        if(!ready){error="QUARANTINE_UNINITIALIZED";return false;}
        if(npc.templateId<0){error="INVALID_NPC_ID";return false;}
        if(npcs.find(npc.templateId)!=npcs.end()){error="NPC_QUARANTINED";return false;}
        for(int slot=0;slot<4;++slot){
            const std::string& raw=npc.slots[slot];if(raw.empty()||raw=="0")continue;
            std::string resolved;if(!Resolve(npc,slot,resolved)){error="INVALID_NPC_PATH";return false;}
            if(tables.find(resolved)!=tables.end()){error="TABLE_QUARANTINED";return false;}
        }
        error="";return true;
    }
};
template<class Loader> bool BuildLabPlan(const NpcSnapshot& npc,int slot,
    const Quarantine& quarantine,Loader& loader,PreparedTable& plan,std::string& error) {
    // No RNG or item effects occur before all these checks succeed.
    plan=PreparedTable(); // Never let a failed replan reuse an earlier valid plan.
    if(!quarantine.Allows(npc,error))return false;
    std::string resolved;if(!Resolve(npc,slot,resolved)){error="NO_DROP_SLOT";return false;}
    std::string bytes;if(!loader.Load(resolved,bytes,error))return false;
    return plan.Prepare(Parse(bytes),error);
}
inline bool BuildProductionPlan(PreparedTable& plan,std::string& error) {
    plan=PreparedTable();error="PRODUCTION_POLICY_NOT_APPROVED";return false;
}
// Lifetime belongs to one spawned entity, not a process-wide ID cache.
struct DeathToken {bool consumed;DeathToken():consumed(false){}bool Consume(){if(consumed)return false;consumed=true;return true;}};
}
#endif

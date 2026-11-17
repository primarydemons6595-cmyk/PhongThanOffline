#ifndef PT_BLUE_MAGIC_TEST_POLICY_H
#define PT_BLUE_MAGIC_TEST_POLICY_H
#include "BlueMagicTableLoader.h"
namespace BlueMagic {
struct Config {
    enum Mode { OFF,SHADOW,LIVE_TEST } mode;
    int npc,version,group,chance,lines;
    bool uniqueType,blankElementUnrestricted,attributeLevelIsTier,allowBoss86,allowAllWorldNpcs;
    Config():mode(OFF),npc(18),version(-1),group(-1),chance(-1),lines(-1),uniqueType(false),blankElementUnrestricted(false),attributeLevelIsTier(false),allowBoss86(false),allowAllWorldNpcs(false){}
};
struct Attribute {int row,type,level,value[3],roll,totalWeight;};
struct Result {bool blue;int chanceDraw;std::vector<Attribute> attributes;Result():blue(false),chanceDraw(-1){}};
class BlueMagicTestPolicy {
    Config config;std::vector<const Row*> eligible;int limit;bool ready;
public:
    BlueMagicTestPolicy():limit(0),ready(false){}
    int CandidateCount() const{return eligible.size();}
    int LineLimit() const{return limit;}
    const Row* Candidate(int index) const{return index>=0&&index<(int)eligible.size()?eligible[index]:0;}
    bool Prepare(const BlueMagicTableLoader& table,const BlueLimits& limits,const Config& c,int npc,int detail,int tier,int element,std::string& error){return PrepareBound(table,limits,c,npc,detail,tier,element,false,error);}
    // World caller must establish isolated companion-v1/P1 deployment readiness.
    bool PrepareWorld(const BlueMagicTableLoader& table,const BlueLimits& limits,const Config& c,int npc,int detail,int tier,int element,std::string& error){return PrepareBound(table,limits,c,npc,detail,tier,element,true,error);}
private:
    bool PrepareBound(const BlueMagicTableLoader& table,const BlueLimits& limits,const Config& c,int npc,int detail,int tier,int element,bool world,std::string& error){
        ready=false;eligible.clear();config=c;currentDetail=detail;
        if(c.mode==Config::OFF){ready=true;error="";return true;}
        if(c.mode==Config::LIVE_TEST&&!world){error="BLUE_PERSISTENCE_ABI_UNSUPPORTED";return false;}
        if(!table.valid||!limits.valid||limits.version!=c.version){error="BLUE_SOURCE_OR_VERSION_NOT_VALIDATED";return false;}
        if(c.npc!=npc||(!world&&npc!=18)||(world&&!c.allowAllWorldNpcs&&npc!=0&&npc!=7&&npc!=11&&npc!=15&&npc!=18&&!(npc==86&&c.allowBoss86))){error="BLUE_NPC_NOT_APPROVED";return false;}
        if((c.version!=0&&c.version!=1)||c.group<0||c.group>2||c.chance<0||c.chance>1000000||c.lines<0||!c.uniqueType||!c.blankElementUnrestricted||!c.attributeLevelIsTier){error="BLUE_EXPERIMENT_POLICY_UNBOUND";return false;}
        if(detail<0||detail>=11||element<0||element>4){error="BLUE_ITEM_TYPE_OR_ELEMENT_UNRESOLVED";return false;}
        if(!limits.Limit(detail,tier,limit,error))return false;
        if(c.lines>limit){error="BLUE_LINE_COUNT_EXCEEDS_LIMIT";return false;}
        std::map<int,bool> unique;
        for(size_t i=0;i<table.rows.size();++i){const Row& r=table.rows[i];int required;
            if(!r.reason.empty()||r.level>tier||r.weights[c.group][detail]<=0)continue;
            if(!r.element.empty()&&(!LegacyDrop::Integer(r.element,required)||required!=element))continue;
            eligible.push_back(&r);unique[r.type]=true;
        }
        double total=0;for(size_t w=0;w<eligible.size();++w)total+=eligible[w]->weights[c.group][detail];
        if(total>INT_MAX){error="BLUE_WEIGHT_SUM_INVALID";return false;}
        if(c.lines>0&&(eligible.empty()||(int)unique.size()<c.lines)){error="BLUE_INSUFFICIENT_DISTINCT_CANDIDATES";return false;}
        ready=true;error="";return true;
    }
public:
    bool Draw(LegacyDrop::Random& rng,Result& out,std::string& error) const {
        out=Result();if(!ready){error="BLUE_POLICY_UNPREPARED";return false;}
        if(config.mode==Config::OFF){error="";return true;}
        unsigned long chance;rng.TryBelow(1000000,chance);out.chanceDraw=chance;
        if((int)chance>=config.chance||config.lines==0){error="";return true;}
        std::map<int,bool> used;
        for(int line=0;line<config.lines;++line){double sum=0;for(size_t i=0;i<eligible.size();++i)if(!used[eligible[i]->type])sum+=eligible[i]->weights[config.group][currentDetail];
            if(sum<=0||sum>INT_MAX){error="BLUE_WEIGHT_SUM_INVALID";return false;}
            unsigned long roll;rng.TryBelow((unsigned long)sum,roll);const Row* chosen=0;unsigned long cursor=0;
            for(i=0;i<eligible.size();++i){const Row& r=*eligible[i];if(used[r.type])continue;cursor+=r.weights[config.group][currentDetail];if(roll<cursor){chosen=&r;break;}}
            if(!chosen){error="BLUE_WEIGHT_SELECTION_FAILED";return false;}Attribute a;a.row=chosen->sourceRow;a.type=chosen->type;a.level=chosen->level;a.roll=roll;a.totalWeight=(int)sum;
            for(int p=0;p<3;++p){unsigned long value;rng.TryBelow((unsigned long)((double)chosen->maximum[p]-chosen->minimum[p]+1),value);a.value[p]=chosen->minimum[p]+(int)value;}
            out.attributes.push_back(a);used[a.type]=true;
        }
        out.blue=!out.attributes.empty();error="";return true;
    }
private:int currentDetail;
};
}
#endif

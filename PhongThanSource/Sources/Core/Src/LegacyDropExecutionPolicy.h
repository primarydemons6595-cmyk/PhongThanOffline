// C++98 shared policy laboratory. No runtime registration or death hook.
// Experimental contracts are opt-in; NONE is a deliberate fail-closed default.
#ifndef PT_LEGACY_DROP_EXECUTION_POLICY_H
#define PT_LEGACY_DROP_EXECUTION_POLICY_H
#include "LegacyDropEngine.h"
#include "LegacyDropMonsterEquipmentResolver.h"
namespace LegacyDrop {
namespace Execution {
enum Profile { PROFILE_UNBOUND=-1, PROFILE_A=0, PROFILE_B=1 };
enum TreasureRule { TREASURE_UNBOUND, REPEATED_ROW_TRIALS, SUCCESS_CAP };
enum LevelRule { LEVEL_UNBOUND, EXPLICIT_OR_UNIFORM_ZERO_SCALE,
    EXPERIMENTAL_MAIN_CATALOG, EXPERIMENTAL_NPC_DIVISOR_CATALOG,
    EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG };
enum MoneyRule { MONEY_UNBOUND, SYNTHETIC_FLAT_BERNOULLI };
enum MagicRule { MAGIC_UNBOUND, EXPERIMENTAL_NATIVE_CATALOG_ONLY };
inline bool LevelGateAllows(bool enabled,int maxOverLevel,int playerLevel,int npcLevel){
    return npcLevel>0&&playerLevel>=0&&maxOverLevel>=0&&(!enabled||playerLevel-npcLevel<=maxOverLevel);
}
inline bool LiveNormalEngineEligible(unsigned long customDropScript){return customDropScript==0;}
class Observer {
public:
    virtual ~Observer(){}
    virtual void Roll(int slot,int row,int trial,int range,int threshold,unsigned long value,bool hit){}
    virtual void Money(int slot,int range,int rate,int scale,unsigned long draw,int amount){}
    virtual void Level(int slot,int row,int low,int high,int chosen,unsigned long draw,int candidates){}
};
struct Config {
    Profile profile;
    TreasureRule treasure;
    LevelRule level;
    MoneyRule money;
    MagicRule magic;
    bool allowInactiveMissingSlots,selectedProfileQuarantine;
    int maxTrials,maxItems,npcLevel,maxMoney,equipmentRequirementAhead;
    Config():allowInactiveMissingSlots(false),selectedProfileQuarantine(false),profile(PROFILE_UNBOUND),treasure(TREASURE_UNBOUND),
        level(LEVEL_UNBOUND),money(MONEY_UNBOUND),magic(MAGIC_UNBOUND),maxTrials(256),maxItems(4096),npcLevel(0),maxMoney(INT_MAX),equipmentRequirementAhead(0){}
};
inline bool PlanQuarantineAllows(const NpcSnapshot& npc,const Quarantine& q,const Config& c,std::string& error){
    NpcSnapshot selected=npc;
    if(c.selectedProfileQuarantine&&(c.profile==PROFILE_A||c.profile==PROFILE_B))for(int k=0;k<4;++k)if(k/2!=(int)c.profile)selected.slots[k]="";
    return q.Allows(selected,error);
}
struct Intent {int slot,row,level;Intent(int s,int r,int l):slot(s),row(r),level(l){}};
struct Result {
    std::vector<Intent> items;
    int money;
    Result():money(0){}
};
// Separate arithmetic probes, NOT a selected interpretation of original scales.
enum ScaleHypothesis { ADD_LEVEL, MULTIPLY_LEVEL, PERCENT_NPC_LEVEL };
inline bool ProbeScale(ScaleHypothesis h,int bound,int scale,int npcLevel,int& out) {
    out=0;if(bound<0||scale<0||npcLevel<0)return false;
    double x;
    if(h==ADD_LEVEL)x=(double)bound+scale;
    else if(h==MULTIPLY_LEVEL)x=(double)bound*scale;
    else if(h==PERCENT_NPC_LEVEL)x=(double)bound+(double)npcLevel*scale/100;
    else return false;
    if(x>INT_MAX)return false;out=(int)x;return true;
}
inline bool MoneyAtRoll(int rate,int range,int scale,int roll,int& amount) {
    amount=0;if(range<=0||rate<0||rate>range||scale<0||roll<0||roll>=range)return false;
    if(Selected(rate,range,roll))amount=scale;return true;
}
// Catalog must validate EVERY candidate level before execution. This interface
// accepts no fabricated tuple or nearest-level fallback.
class Catalog {
public:
    virtual ~Catalog(){}
    virtual bool Has(int genre,int detail,int particular,int level) const=0;
    virtual bool EquipmentRequirement(int detail,int particular,int level,int& required) const{return false;}
};
struct Row {int genre,detail,particular,rate,low,high,number;std::vector<int> levels;};
struct Part {int slot,treasure,range,moneyRate,moneyScale;std::vector<Row> rows;};
class Plan {
    bool ready;
    int failedSlot,failedRow;
    Config config;
    std::vector<Part> parts;
public:
    Plan():ready(false),failedSlot(-1),failedRow(-1){}
    int RejectedSlot() const{return failedSlot;}
    int RejectedRow() const{return failedRow;}
    // Tables must be the two resolved slots of the EXPLICIT profile, in order.
    // Both are validated even when Treasure=0; no malformed data is ignored.
    bool Prepare(const NpcSnapshot& npc,const Quarantine& q,const Config& c,
                 const Table& first,const Table& second,const Catalog& catalog,std::string& error) {
        ready=false;parts.clear();error="";failedSlot=failedRow=-1;
        if(!PlanQuarantineAllows(npc,q,c,error))return false;
        if(c.profile!=PROFILE_A&&c.profile!=PROFILE_B){error="PROFILE_UNBOUND";return false;}
        if(c.treasure!=REPEATED_ROW_TRIALS&&c.treasure!=SUCCESS_CAP){error="TREASURE_UNBOUND";return false;}
        if(c.level!=EXPLICIT_OR_UNIFORM_ZERO_SCALE&&c.level!=EXPERIMENTAL_MAIN_CATALOG&&c.level!=EXPERIMENTAL_NPC_DIVISOR_CATALOG&&c.level!=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG){error="LEVEL_UNBOUND";return false;}
        if(c.money!=SYNTHETIC_FLAT_BERNOULLI){error="MONEY_UNBOUND";return false;}
        if(c.equipmentRequirementAhead<0||c.equipmentRequirementAhead>100){error="INVALID_EQUIPMENT_REQUIREMENT_AHEAD";return false;}
        if(c.maxTrials<1||c.maxTrials>256||c.maxItems<1||c.maxItems>4096){error="INVALID_BUDGET";return false;}
        const Table* tables[2]={&first,&second};
        for(int k=0;k<2;++k) {
            Part p;p.slot=(c.profile==PROFILE_A?0:2)+k;
            failedSlot=p.slot;failedRow=-1;
            p.treasure=k?npc.treasure1:npc.treasure;
            std::string resolved;if(!Resolve(npc,p.slot,resolved)){
                if(c.allowInactiveMissingSlots&&p.treasure==0&&(npc.slots[p.slot].empty()||npc.slots[p.slot]=="0"))continue;
                error="REQUIRED_PROFILE_SLOT_MISSING";return false;}
            if(p.treasure<0||p.treasure>c.maxTrials){error="TREASURE_OUT_OF_BUDGET";return false;}
            const Table& t=*tables[k];PreparedTable parserGuard;
            if(!parserGuard.Prepare(t,error))return false;
            p.range=t.range;const Section* main=Find(t,"Main");int magic,lo,hi,ls,hs;
            if(!Get(main,"MagicRate",magic)||magic<0){error="INVALID_MAGIC_RATE";return false;}
            // No adapter for the original positive MagicRate has been proved.
            if(magic!=0&&c.magic!=EXPERIMENTAL_NATIVE_CATALOG_ONLY){error="MAGIC_POLICY_UNVERIFIED";return false;}
            if(!Get(main,"MoneyRate",p.moneyRate)||!Get(main,"MoneyScale",p.moneyScale)||
               p.moneyRate<0||p.moneyRate>p.range||p.moneyScale<0){error="INVALID_MONEY";return false;}
            if(!Get(main,"MinItemLevel",lo)||!Get(main,"MaxItemLevel",hi)||
               !Get(main,"MinItemLevelScale",ls)||!Get(main,"MaxItemLevelScale",hs)||lo<0||hi<lo||hi-lo>256){error="INVALID_LEVEL_BOUNDS";return false;}
            if(c.level==EXPLICIT_OR_UNIFORM_ZERO_SCALE&&(ls!=0||hs!=0)){error="SCALE_POLICY_UNVERIFIED";return false;}
            if(ls<0||hs<0){error="INVALID_SCALE";return false;}
            if(c.level==EXPERIMENTAL_NPC_DIVISOR_CATALOG){
                if(ls<=0||hs<=0||c.npcLevel<=0){error="INVALID_DIVISOR_SCALE";return false;}
                int a=c.npcLevel/ls,b=c.npcLevel/hs;
                if(a<lo)a=lo;if(a>hi)a=hi;if(b<lo)b=lo;if(b>hi)b=hi;
                if(a>b){error="INVERTED_SCALED_BOUNDS";return false;}lo=a;hi=b;
            }
            for(size_t j=0;j<t.sections.size();++j) {
                const Section& s=t.sections[j];if(s.name=="Main")continue;Row r;
                Integer(s.name,failedRow);
                if(!Integer(s.name,r.number)||!Get(&s,"Genre",r.genre)||!Get(&s,"Detail",r.detail)||
                   !Get(&s,"Particular",r.particular)||!Get(&s,"RandRate",r.rate)){error="INVALID_ROW";return false;}
                r.low=lo;r.high=hi;
                if(s.fields.find("Level")!=s.fields.end()) {
                    if(!Get(&s,"Level",r.low)||r.low<0){error="INVALID_EXPLICIT_LEVEL";return false;}r.high=r.low;
                }
                else if(c.level!=EXPLICIT_OR_UNIFORM_ZERO_SCALE){
                    // Explicit experimental category policy, matching the native
                    // adapter's existing fixed-ID probe convention. Never a new ID.
                    if(r.genre==2||r.genre==3||r.genre==4||r.genre==5||r.genre==8)r.low=r.high=0;
                    else if(r.genre==6||r.genre==7)r.low=r.high=1;
                }
                if(c.level==EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG&&r.genre==0){
                    const bool explicitLevel=s.fields.find("Level")!=s.fields.end();
                    if(!MonsterEquipmentResolver::Bounds(c.npcLevel,lo,hi,ls,hs,explicitLevel,r.low,r.low,r.high,error))return false;
                }
                for(int lv=r.low;;++lv){bool has=catalog.Has(r.genre,r.detail,r.particular,lv);
                    if(has&&c.level==EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG&&r.genre==0){
                        int required=-1;
                        has=catalog.EquipmentRequirement(r.detail,r.particular,lv,required)&&required>=0&&(double)required<=(double)c.npcLevel+c.equipmentRequirementAhead;
                    }
                    if(!has&&c.level==EXPLICIT_OR_UNIFORM_ZERO_SCALE){error="CATALOG_LEVEL_MISSING";return false;}
                    if(has)r.levels.push_back(lv);if(lv==r.high)break;}
                if(r.levels.empty()){error=c.level==EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG&&r.genre==0?"EQUIPMENT_NO_MONSTER_VALID_CANDIDATE":"CATALOG_LEVEL_MISSING";return false;}
                p.rows.push_back(r);
            }
            parts.push_back(p);
        }
        // Preflight worst-case item and currency budgets, before any RNG.
        double items=0,money=0;
        for(size_t i=0;i<parts.size();++i){
            const Part& p=parts[i];
            items+=c.treasure==REPEATED_ROW_TRIALS?(double)p.treasure*p.rows.size():
                (p.treasure<(int)p.rows.size()?p.treasure:(int)p.rows.size());
            if(p.moneyRate)money+=p.moneyScale; // One synthetic money trial per part.
        }
        if(items>c.maxItems){error="ITEM_BUDGET_EXCEEDED";return false;}
        if(money>INT_MAX){error="MONEY_OVERFLOW";return false;}
        if(c.maxMoney<0||money>c.maxMoney){error="MONEY_NATIVE_BUDGET_EXCEEDED";return false;}
        config=c;ready=true;return true;
    }
    bool Draw(Random& rng,Result& out,std::string& error,Observer* observer=0) const {
        out=Result();error="";if(!ready){error="EXECUTION_PLAN_UNPREPARED";return false;}
        for(size_t k=0;k<parts.size();++k){const Part& p=parts[k];int successes=0;
            int attempts=config.treasure==REPEATED_ROW_TRIALS?p.treasure:(p.treasure?1:0);
            for(int a=0;a<attempts;++a)for(size_t j=0;j<p.rows.size();++j){
                if(config.treasure==SUCCESS_CAP&&successes>=p.treasure)break;
                const Row& r=p.rows[j];unsigned long roll;rng.TryBelow(p.range,roll);
                if(observer)observer->Roll(p.slot,r.number,a,p.range,r.rate,roll,Selected(r.rate,p.range,roll));
                if(!Selected(r.rate,p.range,roll))continue;
                int level=r.levels[0];unsigned long levelDraw=0;if(r.levels.size()>1){rng.TryBelow((unsigned long)r.levels.size(),levelDraw);level=r.levels[levelDraw];}
                if(observer)observer->Level(p.slot,r.number,r.low,r.high,level,levelDraw,(int)r.levels.size());
                out.items.push_back(Intent(p.slot,r.number,level));++successes;
            }
            if(p.moneyRate){unsigned long roll;rng.TryBelow(p.range,roll);int amount;
                MoneyAtRoll(p.moneyRate,p.range,p.moneyScale,roll,amount);out.money+=amount;if(observer)observer->Money(p.slot,p.range,p.moneyRate,p.moneyScale,roll,amount);}
            else if(observer)observer->Money(p.slot,p.range,0,p.moneyScale,0,0);
        }
        return true;
    }
};
// Shared resolver/loader path. Direct Plan::Prepare is a test seam; production
// consumers must bind actual slot bytes through this method, never arbitrary tables.
template<class Loader> bool Build(const NpcSnapshot& npc,const Quarantine& q,const Config& c,
    Loader& loader,const Catalog& catalog,Plan& plan,std::string& error) {
    plan=Plan();if(!PlanQuarantineAllows(npc,q,c,error))return false;
    if(c.profile!=PROFILE_A&&c.profile!=PROFILE_B){error="PROFILE_UNBOUND";return false;}
    Table tables[2];
    for(int k=0;k<2;++k){std::string path,bytes;int slot=(c.profile==PROFILE_A?0:2)+k;
        if(!Resolve(npc,slot,path)){
            if(c.allowInactiveMissingSlots&&(k?npc.treasure1:npc.treasure)==0&&(npc.slots[slot].empty()||npc.slots[slot]=="0"))continue;
            error="REQUIRED_PROFILE_SLOT_MISSING";return false;}
        if(!loader.Load(path,bytes,error))return false;tables[k]=Parse(bytes);}
    return plan.Prepare(npc,q,c,tables[0],tables[1],catalog,error);
}
}
}
#endif

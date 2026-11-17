#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanHandbookSkills.h"
#include "../../Sources/Core/Src/PhongThanEventHandbookSkills.inl"
struct Actor {
    int profession,levels[2048],bad,free,changed,synced;
    Actor(int p):profession(p),bad(0),free(79),changed(0),synced(0){memset(levels,0,sizeof(levels));}
    int Profession(){return profession;}
    int Learned(int id){return levels[id];}
    int FreeSlots(){return free;}
    bool Ready(int id,int cap){return id!=bad && id>0 && id<2048 && cap>0 && cap<100;}
    bool Learn(int id,int cap){levels[id]=cap;++changed;return true;}
    void Sync(){++synced;}
};
static int Run(Actor& a){return PhongThanHandbookMaxSkills(kPtHandbookSkills,sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]),a);}
int main(){
    int total=0;
    for(int profession=0;profession<4;++profession){
        Actor a(profession);int count=Run(a);assert(count>0 && a.changed==count && a.synced==1);total+=count;
        for(int i=0;i<sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]);++i){const PtHandbookSkill& s=kPtHandbookSkills[i];assert(a.levels[s.id]==(s.profession==profession?s.maximum:0));}
        for(int summon=450;summon<=458;++summon)assert(a.levels[summon]==(profession==2?10:0));
        assert(a.levels[459]==0 && a.levels[460]==0 && a.levels[461]==0);
        assert(Run(a)==count && a.changed==count && a.synced==2); // repeat never duplicates/recasts
        Actor full(profession);full.free=count-1;assert(Run(full)==0 && full.changed==0 && full.synced==0);
        Actor missing(profession);for(int k=0;k<sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]);++k)if(kPtHandbookSkills[k].profession==profession){missing.bad=kPtHandbookSkills[k].id;break;}
        assert(Run(missing)==0 && missing.changed==0 && missing.synced==0);
    }
    Actor invalid(9);assert(Run(invalid)==0 && !invalid.changed && !invalid.synced);
    Actor trained(2);for(int id=450;id<=458;++id)trained.levels[id]=id-449;
    assert(Run(trained)>0);for(int check=450;check<=458;++check)assert(trained.levels[check]==10);
    for(int bad=450;bad<=458;++bad){Actor missingSummon(2);missingSummon.bad=bad;
        assert(Run(missingSummon)==0 && !missingSummon.changed && !missingSummon.synced);}
    printf("PASS handbook max_skills %d player_skills four_class_isolation nine_summons caps idempotence full_list missing_script invalid_class\n",total);
    return 0;
}

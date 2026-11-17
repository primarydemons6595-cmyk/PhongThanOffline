#ifndef PT_HANDBOOK_SKILLS_H
#define PT_HANDBOOK_SKILLS_H
// All data/script and capacity checks finish before the first skill changes.
// Adapter owns class identity and applies the real server skill list changes.
template<class SkillRow,class Adapter>
int PhongThanHandbookMaxSkills(const SkillRow* rows,int size,Adapter& actor) {
    int count=0,missing=0;
    for(int i=0;i<size;++i){
        const SkillRow& s=rows[i];if(s.profession!=actor.Profession())continue;
        if(!actor.Ready(s.id,s.maximum))return 0;
        if(!actor.Learned(s.id))++missing;++count;
    }
    if(!count || missing>actor.FreeSlots())return 0;
    for(int n=0;n<size;++n){
        const SkillRow& s=rows[n];if(s.profession!=actor.Profession())continue;
        if(actor.Learned(s.id)<s.maximum && !actor.Learn(s.id,s.maximum))return 0;
    }
    actor.Sync();return count;
}
#endif

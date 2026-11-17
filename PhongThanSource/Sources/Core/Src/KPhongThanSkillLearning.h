#ifndef PT_NATIVE_SKILL_LEARNING_H
#define PT_NATIVE_SKILL_LEARNING_H
#include "PhongThanSkillLearning.h"
#include "KIniFile.h"
inline bool PhongThanNativeCanLearn(int id,KSkillList& skills,int playerLevel){
    if(!PhongThanIsProfessionSkill(id))return true;
    static KIniFile original;static bool attempted=false,loaded=false;
    if(!attempted){attempted=true;loaded=original.Load("\\settings\\skills.ini")!=0;}
    char section[20],rule[512];sprintf(section,"%d",id);
    int minimum=0,use=0,missing=0,required=0;
    if(!loaded||!original.GetInteger(section,"PlayerLevel",-1,&minimum)||minimum<0||playerLevel<minimum)return false;
    original.GetInteger("Option","UseSkillParent",-1,&use);
    if(use==0)return true;if(use!=1)return false;
    original.GetString(section,"SkillParent","",rule,sizeof(rule));
    bool ok=PhongThanSkillParentsAllow(rule,skills,&missing,&required);
    if(!ok)g_DebugLog("[PTSkillLearning] REJECT skill=%d parent=%d required=%d actual=%d source=settings/skills.ini",id,missing,required,missing?skills.GetLevel(missing):0);
    return ok;
}
#endif

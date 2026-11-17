#ifndef PHONGTHAN_THAN_KY_EXPERIMENT_H
#define PHONGTHAN_THAN_KY_EXPERIMENT_H
// USER-APPROVED LOOSE TRIAL. These numbers are administrator configuration,
// never recovered VNG formulas. Only the six missing-formula families enter.
#include "KIniFile.h"
#include "../../../Headers/PhongThanThanKy.h"
#include <stdio.h>
#include <string.h>
namespace PtTkExperiment {
struct Property {const char* name;int base[3],step[3];};
struct Skill {int root,count;Property attrs[4];};
struct Group {bool enabled;int manaBase,manaStep;};
struct Config {bool valid;Skill skills[15];Group groups[6];int cooldown[15];};
inline int NewGroup(int id){
 if(id>=1986&&id<=1989)return 0;
 if(id>=1996&&id<=1999)return 1;
 if(id==1990||id==2000)return 2;
 if(id==1991||id==1992)return 3;
 if(id==1993)return 4;
 if(id==1994||id==1995)return 5;return -1;
}
inline int Root(int group){static const int ids[]={1986,1996,1990,1991,1993,1994};return group>=0&&group<6?ids[group]:0;}
inline bool Triple(const char* raw,int* base,int* step){char extra;
 return raw&&sscanf(raw,"%d,%d,%d,%d,%d,%d%c",base,base+1,base+2,step,step+1,step+2,&extra)==6;
}
inline bool PropertyValid(const Property& p){
 bool damage=strstr(p.name,"damage_v")!=0;
 for(int level=0;level<10;++level){__int64 v[3];for(int k=0;k<3;++k){v[k]=(__int64)p.base[k]+(__int64)level*p.step[k];if(v[k]<-1000000||v[k]>1000000)return false;}
  if(damage){if(v[0]<0||v[2]<v[0]||v[1]<0||v[1]>1800)return false;}
  else{
   if(v[0]<-1000||v[0]>10000||v[1]<0||v[1]>1800||v[2]!=0)return false;
   if((!strcmp(p.name,"reducemagicdeadlystirke")||!strcmp(p.name,"fireeffectenhance_p"))&&v[0]&&v[1]<=0)return false;
   if(!strcmp(p.name,"ignoredefense_p")&&(v[0]<0||v[0]>100))return false;
  }
 }
 return true;
}
inline Config& Settings(){static Config result;static bool loaded=false;if(loaded)return result;loaded=true;memset(&result,0,sizeof(result));
 for(int g=0;g<15;++g)result.cooldown[g]=-1;
 KIniFile ini;int version=0;if(!ini.Load("\\settings\\phongthan\\ThanKyExperimental.ini")||!ini.GetInteger("Main","Version",0,&version)||version!=1)return result;
 static const char* names[15][4]={
  {"physicsenhance_p","skill_eventskilllevel",0,0},{"skill_eventskilllevel",0,0,0},{"physicsenhance_p","skill_eventskilllevel",0,0},{"physicsenhance_p",0,0,0},
  {"firedamage_v","skill_eventskilllevel",0,0},{"lightingdamage_v","fireeffectenhance_p",0,0},{"lightingdamage_v","skill_eventskilllevel",0,0},{"colddamage_v",0,0,0},
  {"earthdamage_v","ignoredefense_p",0,0},{"earthdamage_v","skill_eventskilllevel",0,0},
  {"physicsenhance_p","reducemagicdeadlystirke","skill_eventskilllevel",0},{"physicsenhance_p","skill_eventskilllevel",0,0},{"physicsenhance_p","skill_eventskilllevel",0,0},{"physicsenhance_p",0,0,0},
  {"firedamage_v","skill_eventskilllevel",0,0}};
 for(g=0;g<6;++g){char section[32];sprintf(section,"Group_%d",Root(g));int enabled=0;
  if(!ini.GetInteger(section,"Enabled",0,&enabled)||enabled<0||enabled>1||
     !ini.GetInteger(section,"ManaBase",0,&result.groups[g].manaBase)||!ini.GetInteger(section,"ManaStep",0,&result.groups[g].manaStep))return result;
  for(int lv=0;lv<10;++lv){__int64 mana=(__int64)result.groups[g].manaBase+(__int64)lv*result.groups[g].manaStep;if(mana<0||mana>10000)return result;}
  result.groups[g].enabled=enabled!=0;
 }
 for(int id=1986;id<=2000;++id){Skill& s=result.skills[id-1986];s.root=Root(NewGroup(id));char section[32];sprintf(section,"Skill_%d",id);
  for(int a=0;a<4&&names[id-1986][a];++a){Property& p=s.attrs[s.count++];p.name=names[id-1986][a];
   if(!strcmp(p.name,"skill_eventskilllevel"))continue;
   char value[160];ini.GetString(section,p.name,"",value,sizeof(value));if(!Triple(value,p.base,p.step)||!PropertyValid(p))return result;
  }
 }
 for(g=0;g<15;++g){char key[24];sprintf(key,"%d",PtThanKySkill(g));int value=-1;
  ini.GetInteger("CooldownTicks",key,-1,&value);if(value<-1||value>10800)return result;result.cooldown[g]=value;
 }
 result.valid=true;return result;
}
inline bool Enabled(int skill){int group=NewGroup(skill);if(group<0)return true;Config& c=Settings();return c.valid&&c.groups[group].enabled;}
inline const Skill* Find(int id){Config& c=Settings();return NewGroup(id)>=0&&c.valid&&Enabled(id)?&c.skills[id-1986]:0;}
inline void Values(const Property& p,int level,int out[3]){
 if(!strcmp(p.name,"skill_eventskilllevel")){out[0]=level;out[1]=out[2]=0;return;}
 for(int k=0;k<3;++k)out[k]=(int)((__int64)p.base[k]+(__int64)(level-1)*p.step[k]);
}
inline int Mana(int id,int level){int group=NewGroup(id);if(group<0||id!=Root(group))return 0;Config& c=Settings();return c.valid?c.groups[group].manaBase+(level-1)*c.groups[group].manaStep:0;}
inline int Cooldown(int id,int nativeTicks){int group=PtThanKyGroup(id);if(group<0)return nativeTicks;Config& c=Settings();return c.valid&&c.cooldown[group]>=0?c.cooldown[group]:nativeTicks;}
}
#endif

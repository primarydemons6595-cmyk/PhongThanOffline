#ifndef PHONGTHAN_LIFE_SKILLS_H
#define PHONGTHAN_LIFE_SKILLS_H
enum {PT_LIFE_SKILLS_GET=0x50544C53,PT_LIFE_LEVEL_BEGIN=5100,PT_LIFE_EXP_BEGIN=5120,PT_LIFE_FORMULA_BEGIN=5200,
 PT_LIFE_RECEIPT=5140,PT_LIFE_ENERGY=5141,PT_LIFE_STRENGTH=5142,PT_LIFE_REFRESH=5143,
 PT_LIFE_SELECTED_RECIPE=5144,PT_LIFE_SELECTED_SKILL=5145,PT_LIFE_EXPECTED_RANK=5146};
struct PT_LIFE_SKILLS_VIEW {int Levels[11];int Experience[11];int FormulaUses[129];int MiningBook,BuildingBook;int Energy,Strength,MaxEnergy,MaxStrength;};
inline bool PhongThanIsLifeProfession(int id){return id==1||(id>=3&&id<=10);}
inline bool PhongThanLifeTrainerAllows(int map,int tid,int skill){
 if((map==1002&&tid==1359)||(map==1003&&tid==1361)||(map==1004&&tid==1360))return skill==0||skill==1||skill==3;
 if(map==1021&&tid==1363)return skill==0||skill==4||skill==8||skill==6;
 if(map==1020&&tid==1362)return skill==0||skill==10||skill==9||skill==7||skill==5;
 return false;
}
inline int PhongThanLifeLearnCost(int rank){return rank<0||rank>=10?0:rank==0?1000:10000*(1<<rank);}
inline int PhongThanLifeRequiredLevel(int rank){return rank==10?120:(rank+1)*10;}
inline bool PhongThanIsLivingBook(int detail,int skill){return (detail==58&&skill==62)||(detail==59&&skill==128);}
#endif

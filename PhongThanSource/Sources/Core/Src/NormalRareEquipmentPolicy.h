#ifndef PT_NORMAL_RARE_EQUIPMENT_POLICY_H
#define PT_NORMAL_RARE_EQUIPMENT_POLICY_H
namespace NormalRareEquipment {
// This is an additional keep probability, not the base table drop chance.
inline bool Eligible(bool registryReady,bool boss,bool owned,bool scripted,
                     int special,int treasure,int treasure1,bool eventProfile){
 return registryReady&&!boss&&!owned&&!scripted&&special==0&&
        treasure>=0&&treasure<=1&&treasure1>=0&&treasure1<=1&&!eventProfile;
}
inline int KeepRate(int quality){
 switch(quality){case 1:return 5000;case 2:return 1000;case 3:return 100;
 case 4:return 250;case 5:return 100;default:return 10000;}
}
inline bool Keep(bool eligible,bool equipment,int quality,unsigned long draw){
 return !eligible||!equipment||draw<(unsigned long)KeepRate(quality);
}
}
#endif

#ifndef PHONGTHAN_PET_SALE_POLICY_H
#define PHONGTHAN_PET_SALE_POLICY_H
// Only ordinary body gear and weapons are junk-sale candidates. Fabao uses
// the legacy boots family, wings use pendant, and seals/jade have own families.
inline bool PhongThanPetJunkFamily(int genre,int detail,int setId,int forge,int upgrade){
 if(genre!=0||setId>0||forge||upgrade)return false;
 return detail==0||detail==1||detail==2||detail==3||detail==5||detail==6||detail==7||detail==8;
}
#endif

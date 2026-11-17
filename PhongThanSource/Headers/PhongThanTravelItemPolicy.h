#ifndef PHONGTHAN_TRAVEL_ITEM_POLICY_H
#define PHONGTHAN_TRAVEL_ITEM_POLICY_H
// External PT item identities; use-count is charges in one object, not a pile.
inline int PhongThanTravelProfile(int genre,int detail,int part){
 if(genre==5)return 8;
 if(genre==6&&(detail==264||detail==5658))return 1;
 if(genre!=8||part!=2)return 0;
 switch(detail){
 case 30:case 391:return 12;
 case 35:case 159:case 567:case 9248:return 1;
 case 119:return 2;case 120:return 3;case 121:return 4;case 122:return 5;case 123:return 6;case 124:return 7;
 case 291:case 733:return 8;
 case 9249:case 9580:case 9582:return 9;
 case 9250:case 9581:case 9583:return 10;
 case 258:case 737:case 9252:return 11;
 }return 0;
}
inline bool PhongThanTravelHasCharges(int genre,int detail,int part){return genre==8&&PhongThanTravelProfile(genre,detail,part)>0;}
#endif

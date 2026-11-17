#ifndef PT_TREASURY_OBJECTS_H
#define PT_TREASURY_OBJECTS_H
inline bool PTDuplicateTreasuryVariant(int oldType,int newType,bool sameService,int distance){
    return sameService&&distance>=0&&distance<=384&&oldType!=newType&&
        (oldType==1||oldType==2)&&(newType==1||newType==2);
}
#endif

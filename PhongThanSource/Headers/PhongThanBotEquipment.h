#ifndef PT_BOT_EQUIPMENT_H
#define PT_BOT_EQUIPMENT_H
inline bool PTBotShouldEquip(bool usable,bool candidateKnown,int candidatePower,bool empty,bool currentKnown,int currentPower){return usable&&candidateKnown&&candidatePower>=0&&(empty||(currentKnown&&candidatePower>currentPower));}
inline bool PTBotCarryNeedsShop(int weight,int capacity){return capacity>0&&(__int64)weight*100>=(__int64)capacity*80;}
inline bool PTBotCarryCanPick(int weight,int capacity,int itemWeight,int count,bool important){
 if(capacity<=0||weight<0||itemWeight<0||count<1)return false;
 __int64 after=(__int64)weight+(__int64)itemWeight*count;
 return important?after<=capacity:after*100<=(__int64)capacity*80;
}
#endif

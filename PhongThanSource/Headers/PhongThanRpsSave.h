#ifndef PHONGTHAN_RPS_SAVE_H
#define PHONGTHAN_RPS_SAVE_H
inline bool PhongThanKeepRpsOnSave(bool liveTrade,bool rps){return liveTrade && rps;}
template<class Slot>
inline int PhongThanNextRpsSaveSlot(const Slot* slots,int previous,int count) {
    if(!slots || previous<0 || previous>=count)return 0;
    for(int i=previous+1;i<count;++i)if(slots[i].nIdx)return i;
    return 0;
}
#endif

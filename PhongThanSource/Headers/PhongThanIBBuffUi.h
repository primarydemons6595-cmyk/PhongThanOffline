#ifndef PHONGTHAN_IBBUFF_UI_H
#define PHONGTHAN_IBBUFF_UI_H
enum {PT_GDI_IBBUFFS=0x50544942,PT_IBBUFF_UI_MAX=48};
struct PTIBBuffUiEntry {int Id,Level,Stacks;unsigned long Seconds;};
struct PTIBBuffUiState {int Count;PTIBBuffUiEntry Entries[PT_IBBUFF_UI_MAX];};
inline bool PTHasIBBuff(const PTIBBuffUiState& state,int id) {
    for(int i=0;i<state.Count && i<PT_IBBUFF_UI_MAX;++i)if(state.Entries[i].Id==id)return true;
    return false;
}
#endif

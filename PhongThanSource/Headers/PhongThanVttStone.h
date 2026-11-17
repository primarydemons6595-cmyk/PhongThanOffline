#ifndef PHONGTHAN_VTT_STONE_H
#define PHONGTHAN_VTT_STONE_H
enum {PT_VTT_STONE_SECONDS=30*24*60*60};
inline bool PhongThanIsVttStone(int genre,int detail,int particular){return genre==8&&detail>=144&&detail<=147&&particular==2;}
inline bool PhongThanVttStoneValid(int born,int now){return born>0&&born<=now&&now-born<PT_VTT_STONE_SECONDS;}
inline int PhongThanVttStoneDeadline(int born){return born-1451581200+PT_VTT_STONE_SECONDS;}
#endif

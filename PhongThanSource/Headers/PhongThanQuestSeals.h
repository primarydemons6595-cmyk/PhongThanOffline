#ifndef PHONGTHAN_QUEST_SEALS_H
#define PHONGTHAN_QUEST_SEALS_H
enum {PT_QUEST_SEAL_MAX=64,PT_QUEST_SEAL_FIRST_BUFF=9700,PT_QUEST_SEAL_ITEM_TAG=0x2c000000};
inline int PhongThanQuestSealParam(int id){return id>0&&id<=PT_QUEST_SEAL_MAX?PT_QUEST_SEAL_ITEM_TAG|id:0;}
inline int PhongThanQuestSealFromParam(int param){int id=param&0x00ffffff;return (param&0xff000000)==PT_QUEST_SEAL_ITEM_TAG&&id>0&&id<=PT_QUEST_SEAL_MAX?id:0;}
const char* PhongThanQuestSealName(int param);
const char* PhongThanQuestSealImage(int param);
const char* PhongThanQuestSealTooltip(int param);
#endif

#ifndef PHONGTHAN_NPC_QUEST_MARK_H
#define PHONGTHAN_NPC_QUEST_MARK_H

// A per-player projection, never part of the shared NPC template or spawn.
#define PT_NPC_QUEST_MARK_NOTE 0x50544d
#define PT_NPC_QUEST_MARK_KEY "pt_npc_quest_marks_v2"
#define PT_NPC_QUEST_MARK_MAX 128
inline int PhongThanQuestMarkVariant(int state, int requiredLevel, int playerLevel)
{
    if(state<1 || state>3)return -1;
    if(state==2)return 2;
    const bool lowerQuest=requiredLevel>0 && playerLevel-requiredLevel>5;
    return state==1 ? (lowerQuest?1:0) : (lowerQuest?4:3);
}
inline int PhongThanQuestMarkPriority(int state)
{
    return state==3?3:(state==2?2:(state==1?1:0));
}
#endif

#ifndef PHONGTHAN_STUDIO_QUESTS_H
#define PHONGTHAN_STUDIO_QUESTS_H
#ifdef _SERVER
BOOL PhongThanStudioQuestTryRoute(int,DWORD,const char*,int,BOOL*);
void PhongThanStudioQuestKilled(int,int);
void PhongThanStudioQuestRevived(int);
void PhongThanStudioQuestLogin(int);
void PhongThanStudioQuestTick();
void PhongThanStudioQuestGatherDamaged(int);
void PhongThanStudioQuestNodesChanged();
#endif
#endif

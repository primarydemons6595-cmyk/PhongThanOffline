#ifndef PHONGTHAN_LIVE_CAMPAIGNS_H
#define PHONGTHAN_LIVE_CAMPAIGNS_H
#ifdef _SERVER
int PhongThanLiveExperience(int map,int damage,int xp,int life);
int PhongThanLiveDropRate(int map);
void PhongThanLiveCampaignTick();
void PhongThanLiveCampaignDrop(int npc,int owner);
void PhongThanLiveCampaignReset(int npc);
#endif
#endif

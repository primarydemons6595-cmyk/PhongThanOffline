#include "../../Headers/PhongThanEventShop.h"
static PTEventShopData questShopPacket;static int questShopSends=0;
static void QuestShopMockSend(int,const PTEventShopData& d,unsigned size){assert(PTValidateEventShop(&d,size));questShopPacket=d;++questShopSends;}
#define PTSQ_SHOP_SEND(p,data,size) QuestShopMockSend(p,data,size)
static int questBossSpawnCount=0,questBossRemoveCount=0;
static int PhongThanSpawnNpc(int type,int level,int world,int x,int y,BOOL){for(int n=5;n<MAX_NPC;++n)if(!Npc[n].m_dwID){Npc[n].m_NpcSettingIdx=type;Npc[n].m_Level=level;Npc[n].m_SubWorldIndex=world;Npc[n].m_CurrentLife=100;Npc[n].m_dwID=10000+(++questBossSpawnCount);Npc[n].x=Npc[n].m_OriginX=x;Npc[n].y=Npc[n].m_OriginY=y;return n;}return 0;}
static void PhongThanRemoveNpc(int n){++questBossRemoveCount;ZeroMemory(&Npc[n],sizeof(Npc[n]));}

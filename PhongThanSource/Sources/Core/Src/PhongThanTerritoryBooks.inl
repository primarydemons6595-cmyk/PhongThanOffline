#include "KGameData.h"
namespace PTTerritory {
static PTNationBook& Books(int nation){PTNationBook& b=s_PtNation.State.Books[nation];unsigned int owner=s_PtNation.State.Nations[nation].Id;if(b.NationId!=owner){memset(&b,0,sizeof(b));b.NationId=owner;}
 if(!b.Migrated){for(int i=0;i<MAX_PLAYER;++i){KDataGroup& g=GameData.m_sDataGroup[i];if(g.nValue[0]==7102&&(unsigned int)g.nValue[1]==owner&&g.nValue[2]==1&&g.nValue[3]>0)b.Fragments[0]=g.nValue[3];}b.Migrated=1;}return b;
}
static bool BookNear(int p){int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex,last=Player[p].m_nLastNpcIndex;if(world<0||world>=MAX_SUBWORLD||last<1||last>=MAX_NPC||!Npc[last].m_dwID||Npc[last].m_SubWorldIndex!=world)return false;
 bool found=false;for(int i=0;i<PT_BOOK_NPC_COUNT;++i)if(Npc[last].m_ActionScriptID==g_FileName2Id((char*)PT_BOOK_NPC_PATHS[i]))found=true;if(!found)return false;int x,y,nx,ny;Npc[actor].GetMpsPos(&x,&y);Npc[last].GetMpsPos(&nx,&ny);return abs(x-nx)<=512&&abs(y-ny)<=512;
}
}
int PhongThanTerritoryBookCount(int p,int pool){if(!PTNationLive(p)||!PTNationReady()||pool<0||pool>1)return 0;int n=PTTerritory::NationForPlayer(p);return n>=0?PTTerritory::Books(n).Fragments[pool]:0;}
int PhongThanTerritoryBeginBook(int p,int pool,int book,int before,int& after){if(!PTNationLive(p)||!PTNationReady()||pool<0||pool>1||Player[p].CheckTrading()||Player[p].GetLockState()||!PTTerritory::BookNear(p)||before<0||before>=1000000000)return 0;
 int m=PTNationSelf(p),n=PTTerritory::NationForPlayer(p);if(m<0||n<0||s_PtNation.State.Members[m].Duty<1||s_PtNation.State.Members[m].Duty>4||Npc[Player[p].m_nIndex].m_Level<15)return 0;
 if(pool==1&&Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_LEVEL)<14)return 0;
 PTNationBook& b=PTTerritory::Books(n);PTTerritoryContribution& c=PTTerritory::Contribution(m);
 for(int i=0;i<PT_NATION_MEMBER_CAPACITY;++i)if(s_PtNation.State.Contributions[i].NationId==b.NationId&&s_PtNation.State.Contributions[i].PendingAfter)return 0;
 int cost=GetPrivateProfileIntA("Main","BookFragmentsPerVolume",250,".\\settings\\phongthan\\TerritoryPolicy.ini");if(cost<1||cost>1000000)cost=250;
 if(book&&((book<58||book>61)||s_PtNation.State.Members[m].Duty!=1||b.Fragments[pool]<cost))return 0;
 if(!book&&(b.Fragments[pool]==INT_MAX||c.Total==INT_MAX||c.Usable==INT_MAX||c.ThisWeek==INT_MAX))return 0;
 PTNationState* old=new PTNationState;memcpy(old,&s_PtNation.State,sizeof(*old));c.PendingBefore=before;c.PendingAfter=before+1;c.PendingResource=book?5+pool:3+pool;c.PendingAmount=book?cost:1;c.PendingPoints=book?0:1;c.PendingDay=(int)(time(0)/86400);if(book)b.Fragments[pool]-=cost;
 bool ok=PTTerritory::Save();if(!ok)memcpy(&s_PtNation.State,old,sizeof(*old));delete old;if(!ok)return 0;after=c.PendingAfter;return 1;
}

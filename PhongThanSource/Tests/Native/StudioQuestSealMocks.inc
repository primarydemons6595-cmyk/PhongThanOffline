#include "../../Headers/PhongThanQuestSeals.h"
#define questSeconds questTestNow
static unsigned long QuestSealSeconds(){return questSeconds;}
#define KSG_GetCurSec QuestSealSeconds
struct PHONGTHAN_IBBUFF_ENTRY {int nId,nTimes,nLevel;DWORD dwExpireTime;};
struct PHONGTHAN_IBBUFF_STORE {DWORD owner;PHONGTHAN_IBBUFF_ENTRY Entry[48];};
static PHONGTHAN_IBBUFF_STORE questBuffs[MAX_PLAYER];
static int FindIBBuffSlot(PHONGTHAN_IBBUFF_STORE* store,int id){if(!store)return -1;for(int i=0;i<48;++i)if(store->Entry[i].nId==id&&store->Entry[i].nTimes>0)return i;return -1;}
static PHONGTHAN_IBBUFF_STORE* GetPlayerIBBuffStore(int p){if(p<=0||p>=MAX_PLAYER)return 0;PHONGTHAN_IBBUFF_STORE& s=questBuffs[p];if(s.owner!=Player[p].m_dwID){ZeroMemory(&s,sizeof(s));s.owner=Player[p].m_dwID;}for(int i=0;i<48;++i)if(s.Entry[i].dwExpireTime&&s.Entry[i].dwExpireTime<=questSeconds)ZeroMemory(&s.Entry[i],sizeof(s.Entry[i]));return &s;}
static int AddIBBuffToStore(PHONGTHAN_IBBUFF_STORE* store,int,int id,int seconds,int level){if(!store)return 0;int at=FindIBBuffSlot(store,id);if(at<0)for(int i=0;i<48;++i)if(!store->Entry[i].nTimes){at=i;break;}if(at<0)return 0;store->Entry[at].nId=id;store->Entry[at].nTimes++;store->Entry[at].nLevel=level;store->Entry[at].dwExpireTime=questSeconds+seconds;return 1;}
static int RemoveIBBuffFromStore(PHONGTHAN_IBBUFF_STORE* store,int,int id){int at=FindIBBuffSlot(store,id);if(at<0)return 0;ZeroMemory(&store->Entry[at],sizeof(store->Entry[at]));return 1;}
static void PersistPlayerIBBuffStore(int){}

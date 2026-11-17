#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define _SERVER
enum {item_equip=0,magic_requiregdlevel=219,magic_requiregdflag=218,
 TASKVALUE_PT_EQUIP_REALM=4870,TASKVALUE_PT_REALM_SIDE=4868,TASKVALUE_PT_REALM_LEVEL=4869,
 itempart_num=13,pos_equip=2,pos_ptinactiveequip=20,pos_ptrealmescrow=21,pos_equiproom=3,MAX_NPC=8,MAX_PLAYER=3,MAX_ITEM=80,SAVE_REQUEST=1};
struct KMagicAttrib{int nAttribType,nValue[3];};
struct KItem{KMagicAttrib requirements[6];int serial;int GetGenre()const{return item_equip;}int GetWidth(){return 1;}int GetHeight(){return 1;}void* GetRequirement(int n){return n>=0&&n<6?requirements+n:0;}} Item[MAX_ITEM];
struct ItemPos{int nPlace,nX,nY;};
struct PlayerItem{int nIdx,nPlace,nX,nY;};
struct Link {bool used[40];int GetNext(int i){while(++i<40)if(used[i])return i;return 0;}};
struct Task{int values[5000];int GetSaveVal(int n){return values[n];}void SetSaveVal(int n,int v,int){values[n]=v;}};
struct NpcType{int m_CurrentLife;} Npc[MAX_NPC];
struct Lock{int bLock;};
class KItemList {
public:
 int m_PlayerIdx;PlayerItem m_Items[40];Link m_UseIdx;int equipped[itempart_num],failAdd,bagFull;
 int EquipmentRealm()const;BOOL RealmAllows(const KItem*)const;BOOL SwitchEquipmentRealm(int);int ReleaseUnqualifiedRealmGear();
 int Hand(){return 0;}bool Fit(int id,int part){return id>0&&id<MAX_ITEM&&part>=0&&part<itempart_num;}
 bool SearchPosition(int,int,ItemPos* pos,bool){pos->nPlace=pos_equiproom;pos->nX=pos->nY=0;return !bagFull;}
 int FindSame(int id){for(int i=1;i<40;++i)if(m_UseIdx.used[i]&&m_Items[i].nIdx==id)return i;return 0;}
 int Add(int id,int place,int x,int y,bool){if(id==failAdd){failAdd=0;return 0;}if(place==pos_equip&&!RealmAllows(Item+id))return 0;
  for(int i=1;i<40;++i)if(!m_UseIdx.used[i]){m_UseIdx.used[i]=true;m_Items[i].nIdx=id;m_Items[i].nPlace=place;m_Items[i].nX=x;m_Items[i].nY=y;if(place==pos_equip)equipped[x]=id;return i;}return 0;}
 bool Remove(int id){int i=FindSame(id);if(!i)return false;if(m_Items[i].nPlace==pos_equip)equipped[m_Items[i].nX]=0;m_UseIdx.used[i]=false;ZeroMemory(m_Items+i,sizeof(PlayerItem));return true;}
};
struct KPlayer{Task m_cTask;int m_nIndex,m_PlayerDBLoad,m_nPaceBarTime,m_uMustSave;struct{int nTrade;}m_PTrade;KItemList m_ItemList;Lock lock;int locked,trading;
 bool CheckTrading(){return trading!=0;}int GetLockState(){return locked;}Lock* GetLockMove(){return &lock;}void UpdataCurData(){}} Player[MAX_PLAYER];
int appearance=0;void SendEquippedAppearanceNow(int){++appearance;}
#include "../../Sources/Core/Src/PhongThanRealmEquipment.inl"
static void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));Player[1].m_nIndex=1;Player[1].m_PlayerDBLoad=1;Player[1].m_ItemList.m_PlayerIdx=1;Npc[1].m_CurrentLife=100;
 for(int i=0;i<MAX_ITEM;++i)Item[i].serial=10000+i;}
int main(){
 for(int side=0;side<3;++side)for(int mask=1;mask<=7;++mask){Reset();KItemList& b=Player[1].m_ItemList;
  Item[1].requirements[0].nAttribType=magic_requiregdlevel;Item[1].requirements[0].nValue[0]=1;
  Item[1].requirements[1].nAttribType=magic_requiregdflag;Item[1].requirements[1].nValue[0]=mask;
  Player[1].m_cTask.values[4871]=13;Player[1].m_cTask.values[TASKVALUE_PT_REALM_SIDE]=side;Player[1].m_cTask.values[TASKVALUE_PT_REALM_LEVEL]=1;
  assert(!b.RealmAllows(Item+1));assert(b.SwitchEquipmentRealm(1)==(side?TRUE:FALSE));
  assert(b.RealmAllows(Item+1)==(side&&(mask&(1<<side))?TRUE:FALSE));
 }
 for(int failure=0;failure<=26;++failure){Reset();KItemList& b=Player[1].m_ItemList;Player[1].m_cTask.values[4871]=13;Player[1].m_cTask.values[TASKVALUE_PT_REALM_SIDE]=1;Player[1].m_cTask.values[TASKVALUE_PT_REALM_LEVEL]=80;
  for(int i=0;i<13;++i){assert(b.Add(i+1,pos_equip,i,0,false));Item[i+14].requirements[0].nAttribType=magic_requiregdlevel;Item[i+14].requirements[0].nValue[0]=80;assert(b.Add(i+14,pos_ptinactiveequip,i,0,false));}
  b.failAdd=failure;BOOL ok=b.SwitchEquipmentRealm(1);assert(ok==(failure?FALSE:TRUE));
  assert(b.EquipmentRealm()==(failure?0:1));int n=0;for(int j=1;j<40;++j)if(b.m_UseIdx.used[j]){++n;int id=b.m_Items[j].nIdx;assert(Item[id].serial==10000+id);}
  assert(n==26);for(int slot=0;slot<13;++slot)assert(b.equipped[slot]==slot+(failure?1:14));
  if(ok){assert(b.SwitchEquipmentRealm(0));for(slot=0;slot<13;++slot)assert(b.equipped[slot]==slot+1);}
 }
 Reset();KItemList& bag=Player[1].m_ItemList;Player[1].m_cTask.values[4871]=13;Player[1].m_cTask.values[TASKVALUE_PT_REALM_SIDE]=1;Player[1].m_cTask.values[TASKVALUE_PT_REALM_LEVEL]=1;
 Item[1].requirements[0].nAttribType=magic_requiregdlevel;Item[1].requirements[0].nValue[0]=2;
 assert(bag.SwitchEquipmentRealm(1));assert(!bag.RealmAllows(Item+1));assert(!bag.RealmAllows(Item+2));
 Player[1].locked=1;assert(!bag.SwitchEquipmentRealm(0));Player[1].locked=0;Player[1].trading=1;assert(!bag.SwitchEquipmentRealm(0));Player[1].trading=0;Npc[1].m_CurrentLife=0;assert(!bag.SwitchEquipmentRealm(0));
 Reset();KItemList& escrow=Player[1].m_ItemList;Player[1].m_cTask.values[4871]=13;Player[1].m_cTask.values[TASKVALUE_PT_REALM_SIDE]=1;Player[1].m_cTask.values[TASKVALUE_PT_REALM_LEVEL]=1;
 Item[1].requirements[0].nAttribType=magic_requiregdlevel;Item[1].requirements[0].nValue[0]=80;assert(escrow.Add(1,pos_ptinactiveequip,0,0,false));
 escrow.bagFull=1;assert(!escrow.SwitchEquipmentRealm(1)&&escrow.EquipmentRealm()==0&&escrow.FindSame(1));escrow.bagFull=0;
 assert(escrow.SwitchEquipmentRealm(1)&&!escrow.equipped[0]);assert(escrow.m_Items[escrow.FindSame(1)].nPlace==pos_equiproom);
 Reset();KItemList& migrate=Player[1].m_ItemList;
 assert(migrate.Add(1,pos_ptrealmescrow,0,0,false));assert(migrate.Add(2,pos_ptinactiveequip,0,0,false));
 migrate.bagFull=1;assert(migrate.ReleaseUnqualifiedRealmGear()==0&&migrate.FindSame(1)&&migrate.FindSame(2));migrate.bagFull=0;
 migrate.failAdd=1;assert(migrate.ReleaseUnqualifiedRealmGear()==1&&migrate.m_Items[migrate.FindSame(1)].nPlace==pos_ptrealmescrow);
 assert(migrate.ReleaseUnqualifiedRealmGear()==1&&migrate.m_Items[migrate.FindSame(1)].nPlace==pos_equiproom);
 Player[1].m_cTask.values[TASKVALUE_PT_REALM_SIDE]=1;Player[1].m_cTask.values[TASKVALUE_PT_REALM_LEVEL]=1;assert(!migrate.SwitchEquipmentRealm(1));
 puts("PASS production realm banks: 21 realm/side masks, 26 rollback faults, 13-slot roundtrip, serial preservation, locked/trading/dead/level guards, legacy gear escrow, failed migration rollback and full-bag retention");return 0;
}

// Two persisted equipment banks. Only pos_equip contributes stats/appearance.
static int PtEquipmentRealm(const KItem* item)
{
 if(!item||item->GetGenre()!=item_equip)return 0;
 for(int n=0;n<6;++n){const KMagicAttrib* req=(const KMagicAttrib*)((KItem*)item)->GetRequirement(n);
  if(req&&((req->nAttribType==magic_requiregdlevel&&req->nValue[0]>0)||
    (req->nAttribType==magic_requiregdflag&&req->nValue[0]>0&&!(req->nValue[0]&1))))return 1;}
 return 0;
}
int KItemList::EquipmentRealm() const
{
 return Player[m_PlayerIdx].m_cTask.GetSaveVal(TASKVALUE_PT_EQUIP_REALM)==1?1:0;
}
BOOL KItemList::RealmAllows(const KItem* item) const
{
 int realm=PtEquipmentRealm(item);
 if(realm!=EquipmentRealm())return FALSE;
 int side=Player[m_PlayerIdx].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE);
 if(!realm)side=0;
 for(int rq=0;rq<6;++rq){const KMagicAttrib* flag=(const KMagicAttrib*)((KItem*)item)->GetRequirement(rq);
  if(flag&&flag->nAttribType==magic_requiregdflag &&
    (side<0||side>2||!(flag->nValue[0]&(1<<side))))return FALSE;}
 if(!realm)return TRUE;
 if(Player[m_PlayerIdx].m_cTask.GetSaveVal(4871)!=13)return FALSE;
 if(Player[m_PlayerIdx].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE)<1 ||
    Player[m_PlayerIdx].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE)>2)return FALSE;
 for(int n=0;n<6;++n){const KMagicAttrib* r=(const KMagicAttrib*)((KItem*)item)->GetRequirement(n);
  if(r&&r->nAttribType==magic_requiregdlevel &&
    Player[m_PlayerIdx].m_cTask.GetSaveVal(TASKVALUE_PT_REALM_LEVEL)<r->nValue[0])return FALSE;}
 return TRUE;
}
#ifdef _SERVER
int KItemList::ReleaseUnqualifiedRealmGear()
{
 KPlayer& owner=Player[m_PlayerIdx];int side=owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE);
 bool unlocked=owner.m_cTask.GetSaveVal(4871)==13&&(side==1||side==2)&&owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_LEVEL)>0;
 if(owner.CheckTrading()||owner.GetLockState()||Hand())return 0;
 int moved=0;
 for(int slot=m_UseIdx.GetNext(0);slot;){int next=m_UseIdx.GetNext(slot);PlayerItem saved=m_Items[slot];
  if((saved.nPlace==pos_ptrealmescrow||!unlocked&&saved.nPlace==pos_ptinactiveequip)&&saved.nIdx>0&&saved.nIdx<MAX_ITEM){
   ItemPos place;if(SearchPosition(Item[saved.nIdx].GetWidth(),Item[saved.nIdx].GetHeight(),&place,true)){
    if(Remove(saved.nIdx)){if(Add(saved.nIdx,place.nPlace,place.nX,place.nY,false))++moved;else Add(saved.nIdx,saved.nPlace,saved.nX,saved.nY,false);}
   }
  }slot=next;
 }
 return moved;
}
BOOL KItemList::SwitchEquipmentRealm(int realm)
{
 KPlayer& owner=Player[m_PlayerIdx];int actor=owner.m_nIndex;
 if(realm<0||realm>1||actor<=0||actor>=MAX_NPC||!owner.m_PlayerDBLoad ||
   owner.CheckTrading()||owner.GetLockState()||owner.GetLockMove()->bLock||Hand()||
   owner.m_nPaceBarTime||owner.m_PTrade.nTrade||Npc[actor].m_CurrentLife<=0)return FALSE;
 if(realm && (owner.m_cTask.GetSaveVal(4871)!=13 || owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE)<1 ||
   owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_SIDE)>2 ||owner.m_cTask.GetSaveVal(TASKVALUE_PT_REALM_LEVEL)<1))return FALSE;
 if(realm==EquipmentRealm())return TRUE;
 PlayerItem saved[itempart_num*2];int count=0;bool slots[2][itempart_num];ZeroMemory(slots,sizeof(slots));
 for(int i=m_UseIdx.GetNext(0);i;i=m_UseIdx.GetNext(i)){
  PlayerItem p=m_Items[i];if(p.nPlace!=pos_equip&&p.nPlace!=pos_ptinactiveequip)continue;
  int bank=p.nPlace==pos_equip?0:1;
  if(count>=itempart_num*2||p.nX<0||p.nX>=itempart_num||slots[bank][p.nX])return FALSE;
  if(p.nIdx<=0||p.nIdx>=MAX_ITEM||!Fit(p.nIdx,p.nX))return FALSE;
  if(PtEquipmentRealm(&Item[p.nIdx])!=(p.nPlace==pos_equip?EquipmentRealm():realm))return FALSE;
  slots[bank][p.nX]=true;saved[count++]=p;
 }
 int old=EquipmentRealm();
 for(int j=0;j<count;++j)if(!Remove(saved[j].nIdx)){
  for(int restore=0;restore<j;++restore)Add(saved[restore].nIdx,saved[restore].nPlace,saved[restore].nX,saved[restore].nY,false);
  owner.UpdataCurData();SendEquippedAppearanceNow(m_PlayerIdx);return FALSE;
 }
 owner.m_cTask.SetSaveVal(TASKVALUE_PT_EQUIP_REALM,realm,TRUE);
 bool ok=true;
 // Inactive first; the active set is then validated through Equip.
 for(int pass=0;pass<2&&ok;++pass)for(int k=0;k<count;++k){
  int dest=saved[k].nPlace==pos_equip?pos_ptinactiveequip:pos_equip;
  if((dest==pos_equip?1:0)!=pass)continue;
  int x=saved[k].nX,y=0;
  if(dest==pos_equip&&!RealmAllows(&Item[saved[k].nIdx])){
   ItemPos bag;if(!SearchPosition(Item[saved[k].nIdx].GetWidth(),Item[saved[k].nIdx].GetHeight(),&bag,true)){ok=false;break;}
   dest=bag.nPlace;x=bag.nX;y=bag.nY;
  }
  if(!Add(saved[k].nIdx,dest,x,y,false))ok=false;
 }
 if(!ok){
  for(int q=0;q<count;++q)if(FindSame(saved[q].nIdx))Remove(saved[q].nIdx);
  owner.m_cTask.SetSaveVal(TASKVALUE_PT_EQUIP_REALM,old,TRUE);
  for(int z=0;z<count;++z)Add(saved[z].nIdx,saved[z].nPlace,saved[z].nX,0,false);
 }
 owner.UpdataCurData();owner.m_uMustSave=SAVE_REQUEST;
 SendEquippedAppearanceNow(m_PlayerIdx);return ok?TRUE:FALSE;
}
#endif

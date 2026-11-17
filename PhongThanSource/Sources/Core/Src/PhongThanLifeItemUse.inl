#include "../../../Headers/PhongThanLifeItemUseData.h"
int PhongThanUseLifeItem(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_ibitem)return -1;
 const PTLifeItemUse* d=PtLifeItem(Item[item].GetDetailType());if(!d||Item[item].GetParticular()!=d->part)return -1;
 if(p<=0||p>=MAX_PLAYER||!PhongThanIsLivePlayer(p)||Player[p].GetLockState()||Player[p].CheckTrading()||!PtPetCarried(p,item))return 0;
 KNpc& n=Npc[Player[p].m_nIndex];if(n.m_CurrentLife<=0||n.m_Level<d->level||(Item[item].m_CommonAttrib.nExpireTime>0&&Item[item].m_CommonAttrib.nExpireTime<=(int)KSG_GetCurSec()))return 0;
 if(d->kind==1){PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);if(!store)return 0;PHONGTHAN_IBBUFF_STORE previous=*store;
  if(!AddIBBuffToStore(store,Player[p].m_nIndex,d->buff,d->seconds,0))return 0;int slot=FindIBBuffSlot(store,d->buff);if(slot<0){*store=previous;return 0;}store->Entry[slot].nTimes=1;
  if(!Player[p].m_ItemList.RemoveItem(item,1)){*store=previous;return 0;}PersistPlayerIBBuffStore(p);Player[p].UpdataCurData();return 1;
 }
 if(d->kind==3){if(n.m_FreezeState.nTime<=0)return 0;if(!Player[p].m_ItemList.RemoveItem(item,1))return 0;ZeroMemory(&n.m_FreezeState,sizeof(n.m_FreezeState));n.SendSyncData(0,TRUE);return 1;}
 if(d->kind==4){PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);if(!store||FindIBBuffSlot(store,1217)<0)return 0;if(!Player[p].m_ItemList.RemoveItem(item,1))return 0;RemoveIBBuffFromStore(store,Player[p].m_nIndex,1217);PersistPlayerIBBuffStore(p);Player[p].UpdataCurData();return 1;}
 if(d->kind==2){bool useful=false;for(int a=0;a<12;++a){if(d->attrs[a][0]==magic_life_v&&n.m_CurrentLife<n.m_CurrentLifeMax)useful=true;if(d->attrs[a][0]==magic_mana_v&&n.m_CurrentMana<n.m_CurrentManaMax)useful=true;}if(!useful||!Player[p].m_ItemList.RemoveItem(item,1))return 0;
  for(a=0;a<12;++a)if(d->attrs[a][0]){KMagicAttrib v;ZeroMemory(&v,sizeof(v));v.nAttribType=d->attrs[a][0];v.nValue[0]=d->attrs[a][1];n.ModifyAttrib(n.m_Index,&v);}n.m_CurrentLife=min(n.m_CurrentLife,n.m_CurrentLifeMax);n.m_CurrentMana=min(n.m_CurrentMana,n.m_CurrentManaMax);n.SendSyncData(0,TRUE);return 1;
 }return 0;
}

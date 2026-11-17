// The saved original vault password does not lock movement, equipment or trade.
static DWORD ptVaultEntity[MAX_PLAYER];
static bool ptVaultUnlocked[MAX_PLAYER];
bool PhongThanBankLocked(int p){
 if(p<=0||p>=MAX_PLAYER)return true;
 KPlayer& owner=Player[p];if(!owner.m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD)[0])return false;
 int a=owner.m_nIndex;if(a<=0||a>=MAX_NPC)return true;
 return ptVaultEntity[p]!=Npc[a].m_dwID||!ptVaultUnlocked[p];
}
void PhongThanBankSetLocked(int p,bool locked){
 if(p<=0||p>=MAX_PLAYER)return;int a=Player[p].m_nIndex;if(a<=0||a>=MAX_NPC)return;
 ptVaultEntity[p]=Npc[a].m_dwID;ptVaultUnlocked[p]=!locked;
}

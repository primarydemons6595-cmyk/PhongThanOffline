#ifdef _SERVER
// Independent bank entitlements. Stored items are never removed on expiry.
static bool PTBankScript(DWORD id){
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\263\347\263\307\264\363\323\252\134\262\326\277\342\271\334\300\355\324\261\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\263\257\270\350\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\263\347\263\307\264\363\323\252\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\266\253\322\304\134\262\326\277\342\271\334\300\355\324\261\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\266\253\322\304\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\267\342\311\361\314\250\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\274\264\312\261\271\372\325\275\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\314\354\300\316\134\262\326\277\342\271\334\300\355\324\261\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\314\354\300\316\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\316\367\341\252\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\316\367\341\252\134\326\334\276\374\320\243\316\276\061\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\316\367\341\252\134\326\334\276\374\320\243\316\276\062\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\316\367\341\252\134\326\334\276\374\320\243\316\276\063\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\316\367\341\252\134\326\334\276\374\320\243\316\276\064\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\320\302\271\305\325\275\263\241\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\321\376\263\330\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\362\277\323\310\304\271\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\323\361\320\351\271\254\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\324\266\271\305\134\262\326\277\342\271\334\300\355\324\261\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\324\266\271\305\134\264\242\316\357\317\344\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\321\376\263\330\134\262\326\277\342\271\334\300\355\324\261\056\154\165\141"))return true;
 if(id==g_FileName2Id("\134\163\143\162\151\160\164\134\316\367\341\252\134\262\326\277\342\271\334\300\355\324\261\056\154\165\141"))return true;
 return false;
}

static bool PTBankNpc(int n,int actor){
 if(n<=0||n>=MAX_NPC||Npc[n].IsPlayer()||Npc[n].m_Kind!=kind_dialoger||!Npc[n].m_dwID||Npc[n].m_RegionIndex<0)return false;
 if(Npc[actor].m_SubWorldIndex<0||Npc[actor].m_SubWorldIndex!=Npc[n].m_SubWorldIndex)return false;
 // Template768 is the original treasury keeper, including restored/local scripts.
 // m_ActionScriptID is the currently executing script and can be cleared
 // after a dialogue. The placement's ActionScript remains the service identity.
 bool nativeKeeper=Npc[n].m_NpcSettingIdx==151&&!strcmp(Npc[n].Name,"\124\150\361\040\113\150\350");
 if(!nativeKeeper&&Npc[n].m_NpcSettingIdx!=768&&!PTBankScript(Npc[n].m_ActionScriptID)&&!PTBankScript(g_FileName2Id(Npc[n].ActionScript)))return false;
 int distance=NpcSet.GetDistance(actor,n);return distance>=0&&distance<=384;
}
static bool PTBankObject(int n,int actor){
 if(n<=0||n>=MAX_OBJECT||Object[n].m_nIndex!=n||Object[n].m_nRegionIdx<0)return false;
 if(Object[n].m_nSubWorldID!=Npc[actor].m_SubWorldIndex||!PTBankScript(Object[n].m_dwScriptID))return false;
 if(Object[n].m_nKind!=Obj_Kind_Box&&Object[n].m_nKind!=Obj_Kind_Prop)return false;
 int distance=Object[n].GetDistanceSquare(actor);return distance>=0&&distance<=384*384;
}
bool PhongThanBankNearTreasury(int p){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad)return false;
 KPlayer& owner=Player[p];int actor=owner.m_nIndex,n=owner.m_nLastNpcIndex;
 if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsAlive())return false;
 if(PTBankNpc(n,actor))return true;
 for(n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(PTBankNpc(n,actor))return true;
 for(int o=1;o<MAX_OBJECT;++o)if(PTBankObject(o,actor))return true;
 FILE* audit=fopen("bank-near-failures.tsv","ab");if(audit){if(ftell(audit)<256*1024){for(n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n)){if(Npc[n].m_SubWorldIndex!=Npc[actor].m_SubWorldIndex||Npc[n].m_RegionIndex<0||Npc[n].IsPlayer())continue;int d=NpcSet.GetDistance(actor,n);if(d>=0&&d<=512)fprintf(audit,"%lu\t%d\t%d\t%d\t%d\t%d\t%lu\t%lu\t%d\n",(unsigned long)time(NULL),p,n,Npc[n].m_NpcSettingIdx,Npc[n].m_Kind,d,Npc[n].m_ActionScriptID,g_FileName2Id(Npc[n].ActionScript),PTBankScript(g_FileName2Id(Npc[n].ActionScript))?1:0);}}fclose(audit);}
 return false;
}
void PhongThanBankSync(int p){
 if(p<=0||p>=MAX_PLAYER)return;
 KPlayer& owner=Player[p];
 if(!owner.m_cTask.GetSaveVal(PT_BANK_MIGRATED_TASK)){
  for(int i=0;i<4;++i)if(i<owner.m_btRepositoryNum)owner.m_cTask.SetSaveVal(PT_BANK_FIRST_TASK+i,PT_BANK_PERMANENT,TRUE);
  owner.m_cTask.SetSaveVal(PT_BANK_MIGRATED_TASK,1,TRUE);owner.Save();
 }
 for(int i=0;i<4;++i)owner.m_cTask.SetSaveVal(PT_BANK_FIRST_TASK+i,owner.m_cTask.GetSaveVal(PT_BANK_FIRST_TASK+i),TRUE);
 owner.m_cTask.SetSaveVal(PT_BANK_MIGRATED_TASK,1,TRUE);
}
bool PhongThanBankAccess(int p,int page){
 if(p<=0||p>=MAX_PLAYER||page<1||page>6)return false;
 if(PhongThanBankLocked(p))return false;
 if(page==1)return true;
 KPlayer& owner=Player[p];
 if(page==6 || !owner.m_cTask.GetSaveVal(PT_BANK_MIGRATED_TASK))return page-2<owner.m_btRepositoryNum;
 return PTBankActive(owner.m_cTask.GetSaveVal(PT_BANK_FIRST_TASK+page-2),(int)time(NULL));
}
int LuaBankInfo(Lua_State* L){
 int p=GetPlayerIndex(L),page=(int)Lua_ValueToNumber(L,1),permanent=(int)Lua_ValueToNumber(L,2);
 int valid=p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&page>=2&&page<=5&&(permanent==0||permanent==1);
 FILE* trace=fopen("bank-rental-requests.tsv","ab");if(trace){if(ftell(trace)<131072)fprintf(trace,"%lu\t%d\t%d\t%d\t%d\n",(unsigned long)time(NULL),p,page,permanent,valid);fclose(trace);}
 if(valid)PhongThanBankSync(p);
 Lua_PushNumber(L,valid?PTBankBasePrice(page,permanent):0);
 Lua_PushNumber(L,valid?PTBankPrice(page,permanent):0);return 2;
}
int LuaBankPurchase(Lua_State* L){
 int p=GetPlayerIndex(L),page=(int)Lua_ValueToNumber(L,1),permanent=(int)Lua_ValueToNumber(L,2),result=0;
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].m_nIndex<=0||Player[p].m_nIndex>=MAX_NPC||!Npc[Player[p].m_nIndex].IsAlive()||page<2||page>5||(permanent!=0&&permanent!=1))return Lua_PushNumber(L,0),1;
 KPlayer& owner=Player[p];
 if(owner.GetLockState()||owner.CheckTrading()||owner.GetLockMove()->bLock||owner.m_ItemList.Hand()||owner.m_nPaceBarTime)return Lua_PushNumber(L,0),1;
 if(owner.m_cTask.GetSaveVal(13)!=2&&strcmp(owner.m_cTask.GetSaveStr(13),"2")&&owner.m_cTask.GetSaveVal(23)!=2&&strcmp(owner.m_cTask.GetSaveStr(23),"2")&&owner.m_cTask.GetSaveVal(33)!=2&&strcmp(owner.m_cTask.GetSaveStr(33),"2"))return Lua_PushNumber(L,0),1;
 PhongThanBankSync(p);
 int task=PT_BANK_FIRST_TASK+page-2,old=owner.m_cTask.GetSaveVal(task),next=PTBankExtend(old,(int)time(NULL),permanent);
 PhongThanQuestExchangeRow coin;
 if(next && PhongThanDecodeQuestItemTuple(4,47,0,0,0,0,coin.item)){
  coin.count=PTBankPrice(page,permanent);
  if(PhongThanQuestExchange(p,task,old,next,&coin,1,0,0)){
   if(owner.m_btRepositoryNum<page-1)owner.SetExpandBoxNum(page-1);
   owner.Save();result=1;
  }
 }
 Lua_PushNumber(L,result);return 1;
}
#endif

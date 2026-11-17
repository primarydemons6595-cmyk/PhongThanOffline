// Vault-only password gate. Native character lock remains independent.
static void PTBankPublishState(int p,int result){
 KPlayer& player=Player[p];int a=player.m_nIndex;if(a<=0||a>=MAX_NPC||Npc[a].m_SubWorldIndex<0||Npc[a].m_SubWorldIndex>=MAX_SUBWORLD)return;
 PTBankSecurityState state;ZeroMemory(&state,sizeof(state));PhongThanInitializeWireHeader(&state.Header,PT_BANK_SECURITY_RESPONSE,sizeof(state),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
 state.MapId=SubWorld[Npc[a].m_SubWorldIndex].m_SubWorldID;state.EntityId=Npc[a].m_dwID;state.Result=result;
 state.Configured=player.m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD)[0]!=0;state.Locked=PhongThanBankLocked(p)?1:0;
 g_pServer->PackDataToClient(player.m_nNetConnectIdx,&state,sizeof(state));
}
void PhongThanBankOpen(int p,int page){
 if(p<=0||p>=MAX_PLAYER)return;
 if(!PhongThanBankNearTreasury(p)){PTF4Notice(p,"\110\267\171\040\256\325\156\040\147\307\156\040\124\150\361\040\113\150\350\056");return;}
 if(PhongThanBankLocked(p)){PTBankPublishState(p,0);return;}
 PTBankPublishState(p,2);SOME_BOX_SYNC open;ZeroMemory(&open,sizeof(open));open.ProtocolType=(BYTE)s2c_opensomebox;open.bEquipEx=FALSE;open.nBoxIndex=page;
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,&open,sizeof(open));
}
static void PTBankSecurityProcess(int p,const PTBankSecurityRequest& q){
 if(p<=0||p>=MAX_PLAYER)return;KPlayer& player=Player[p];int a=player.m_nIndex;
 if(!player.m_PlayerDBLoad||a<=0||a>=MAX_NPC||Npc[a].m_SubWorldIndex<0||Npc[a].m_SubWorldIndex>=MAX_SUBWORLD)return;
 if(q.EntityId!=Npc[a].m_dwID||q.MapId!=(unsigned int)SubWorld[Npc[a].m_SubWorldIndex].m_SubWorldID)return;
 PTBankSecurityState state;ZeroMemory(&state,sizeof(state));PhongThanInitializeWireHeader(&state.Header,PT_BANK_SECURITY_RESPONSE,sizeof(state),PHONGTHAN_WIRE_FLAG_RESPONSE,0);state.MapId=q.MapId;state.EntityId=q.EntityId;
 bool configured=player.m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD)[0]!=0;
 state.Result=0;static DWORD last[MAX_PLAYER],owners[MAX_PLAYER];DWORD now=GetTickCount();
 bool valid=PhongThanBankNearTreasury(p)&&!player.CheckTrading()&&!player.m_PTrade.nTrade&&!player.m_ItemList.Hand()&&!player.GetLockMove()->bLock&&!player.m_nPaceBarTime;
 if(q.Action){
  if(owners[p]!=player.m_dwID){owners[p]=player.m_dwID;last[p]=0;}
  if(q.Action==3){PhongThanBankSetLocked(p,true);state.Result=1;}
  else if(!valid||(last[p]&&now-last[p]<1000))state.Result=-1;
  else {
   last[p]=now;bool password=q.Password[0]&&strlen(q.Password)<=15&&!strcmp(q.Password,q.Confirm);
   for(int k=0;q.Password[k];++k)if((unsigned char)q.Password[k]<33||(unsigned char)q.Password[k]>126)password=false;
   if(q.Action==1&&!configured&&password){char value[16];memcpy(value,q.Password,sizeof(value));player.SetSavePw(value,FALSE);ZeroMemory(value,sizeof(value));state.Result=1;}
   else if(q.Action==2&&configured){if(!strcmp(player.m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD),q.Old)){PhongThanBankSetLocked(p,false);state.Result=1;}else state.Result=-2;}
   else if(q.Action==3&&configured){PhongThanBankSetLocked(p,true);state.Result=1;}
   else if(q.Action==4&&configured&&password&&!strcmp(player.m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD),q.Old)){char value[16];memcpy(value,q.Password,sizeof(value));player.SetSavePw(value,FALSE);ZeroMemory(value,sizeof(value));state.Result=1;}
   else state.Result=-2;
   if(state.Result==1)player.Save();
  }
 }else if(!valid)state.Result=-1;
 state.Configured=player.m_cTask.GetSaveStr(TASKVALUE_BASEDATA_PASSWORD)[0]!=0;state.Locked=PhongThanBankLocked(p)?1:0;
 g_pServer->PackDataToClient(player.m_nNetConnectIdx,&state,sizeof(state));
 if(q.Action==2&&state.Result==1)PhongThanBankOpen(p,-1);
 if(state.Result==-1)PTF4Notice(p,"\110\267\171\040\256\370\156\147\040\147\307\156\040\124\150\361\040\113\150\350\054\040\153\325\164\040\164\150\363\143\040\147\151\141\157\040\144\336\143\150\040\166\265\040\164\150\366\040\154\271\151\040\163\141\165\040\155\351\164\040\143\150\363\164\056");

}

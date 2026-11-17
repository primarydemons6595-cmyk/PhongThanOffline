namespace PtDestroyItems {
static const char* Script="\\script\\phongthan\\items\\destroy.lua";
struct Session {DWORD owner,actor,npc,opened;};
static Session sessions[MAX_PLAYER];
static bool Near(int p){
 if(p<=0||p>=MAX_PLAYER||!PhongThanIsLivePlayer(p)||Player[p].CheckTrading()||Player[p].GetLockState())return false;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex;
 if(a<=0||a>=MAX_NPC||n<=0||n>=MAX_NPC||Npc[a].m_CurrentLife<=0||Npc[n].m_Kind!=kind_dialoger)return false;
 int w=Npc[a].m_SubWorldIndex;
 if(w<0||w>=MAX_SUBWORLD||Npc[n].m_SubWorldIndex!=w||SubWorld[w].m_SubWorldID!=1021||Npc[n].m_ActionScriptID!=g_FileName2Id((char*)Script))return false;
 int x,y,nx,ny;Npc[a].GetMpsPos(&x,&y);Npc[n].GetMpsPos(&nx,&ny);
 return abs(x-nx)<=512&&abs(y-ny)<=512;
}
static int Run(int p,int op){
 if(!Near(p))return 0;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex;Session& s=sessions[p];
 if(op==0){s.owner=Player[p].m_dwID;s.actor=Npc[a].m_dwID;s.npc=Npc[n].m_dwID;s.opened=GetTickCount();return 1;}
 if(op!=1||s.owner!=Player[p].m_dwID||s.actor!=Npc[a].m_dwID||s.npc!=Npc[n].m_dwID||GetTickCount()-s.opened>300000||
    Player[p].m_dwTaskExcuteScriptId!=g_FileName2Id((char*)Script)||strcmp(Player[p].m_szTaskExcuteFun,"pt_destroy_finish"))return 0;
 // Only items explicitly placed into this NPC's give room are destroyed.
 // Personal/permanent insurance and template trade/drop locks are deliberate
 // exceptions here. No other container or trade policy is changed.
 int count=0;for(PlayerItem* item=Player[p].m_ItemList.GetFirstItem();item;item=Player[p].m_ItemList.GetNextItem()){
  if(item->nPlace!=pos_give)continue;
  if(item->nIdx<=0||item->nIdx>=MAX_ITEM||!Item[item->nIdx].GetID()||Player[p].m_ItemList.FindSame(item->nIdx)<=0)return 0;
  ++count;
 }
 if(!count)return 0;
 ZeroMemory(&s,sizeof(s));Player[p].m_szTaskExcuteFun[0]=0;
 Player[p].m_ItemList.RemoveRoom(room_give);Player[p].m_uMustSave=SAVE_REQUEST;
 return count;
}
}
int LuaDestroyItems(Lua_State* L){
 int result=0;
 if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){
  double op=Lua_ValueToNumber(L,1);if(op==0||op==1)result=PtDestroyItems::Run(GetPlayerIndex(L),(int)op);
 }
 Lua_PushNumber(L,result);return 1;
}

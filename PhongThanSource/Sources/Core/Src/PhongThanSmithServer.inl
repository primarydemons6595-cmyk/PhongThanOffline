#include "../../../Headers/PhongThanSmithFeedback.h"
#include "../../../Headers/PhongThanCompose.h"
namespace PtSmith {
struct Session {
 DWORD owner,actor,npc,script,nonce,until,next,quoteItem,quoteHash;int npcIndex,map,quoteStone;
 Session(){memset(this,0,sizeof(*this));}
};
static Session sessions[MAX_PLAYER];static DWORD serial=0;
static bool Near(int p){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].GetLockState()||Player[p].CheckTrading())return false;
 int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex;
 if(a<=0||a>=MAX_NPC||n<=0||n>=MAX_NPC||Npc[a].m_CurrentLife<=0||Npc[n].m_Kind!=kind_dialoger||Npc[n].m_NpcSettingIdx!=777)return false;
 int w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD||Npc[n].m_SubWorldIndex!=w)return false;
 int map=SubWorld[w].m_SubWorldID;if(map!=1001&&map!=1002&&map!=1003&&map!=1004)return false;
 return NpcSet.GetDistance(a,n)<=256;
}
static bool Valid(int p){
 if(!Near(p))return false;Session& s=sessions[p];
 return s.owner==Player[p].m_dwID&&s.actor==Npc[Player[p].m_nIndex].m_dwID&&s.npcIndex==Player[p].m_nLastNpcIndex&&
  s.npc==Npc[s.npcIndex].m_dwID&&s.script==Npc[s.npcIndex].m_ActionScriptID&&
  s.map==SubWorld[Npc[Player[p].m_nIndex].m_SubWorldIndex].m_SubWorldID&&(int)(GetTickCount()-s.until)<0;
}
static void ClearQuote(Session& s){s.quoteItem=s.quoteHash=0;s.quoteStone=0;}
static DWORD Fingerprint(const KItem& item){
 const unsigned char* bytes=(const unsigned char*)&item;DWORD hash=2166136261UL;
 for(unsigned int i=0;i<sizeof(item);++i){hash^=bytes[i];hash*=16777619UL;}return hash;
}
static int FindCarried(int p,DWORD id,bool& equipped){
 equipped=false;
 for(PlayerItem* b=Player[p].m_ItemList.GetFirstItem();b;b=Player[p].m_ItemList.GetNextItem()){
  int i=b->nIdx;if(i<=0||i>=MAX_ITEM||!Item[i].GetID()||Item[i].GetID()!=id)continue;
  if(b->nPlace==pos_equip){equipped=true;return 0;}
  if(b->nPlace==pos_equiproom||b->nPlace==pos_equiproomex||b->nPlace==pos_ptbag2||b->nPlace==pos_ptbag3)return i;
 }return 0;
}
static int Stone(KItem& item){
 if(item.GetGenre()!=item_equip||item.GetLock()->IsLock())return 0;
 int quality=item.GetPhongThanQuality(),detail=item.GetDetailType(),level=item.m_CommonAttrib.nLevel;
 bool gold=quality==2&&(detail==equip_meleeweapon||detail==equip_rangeweapon);
 if((!gold&&quality!=1&&quality!=4)||level<1||level>10)return 0;
 const KBASICPROP_EQUIPMENT* data=ItemGen.CatalogEquipment(detail,item.GetRow());
 if(!data)return 0;for(int r=0;r<6;++r)if(data->m_aryPropReq[r].nType==219)return 0;
 return (gold?236:239)+(level<=6?0:level<=8?1:2);
}
static bool Reply(int p,const PHONGTHAN_COMPOSE_COMMAND* c,int code,int stone=0){
 if(p>0&&p<MAX_PLAYER&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC){
  int world=Npc[Player[p].m_nIndex].m_SubWorldIndex;
  if(world>=0&&world<MAX_SUBWORLD){
   PHONGTHAN_COMPOSE_RESULT r;ZeroMemory(&r,sizeof(r));PhongThanInitializeWireHeader(&r.Header,PHONGTHAN_COMPOSE_RESPONSE,sizeof(r),PHONGTHAN_WIRE_FLAG_RESPONSE,c->Header.Sequence);
   r.MapId=SubWorld[world].m_SubWorldID;r.EntityId=Npc[Player[p].m_nIndex].m_dwID;r.Result=code;r.OutputDetail=stone;
   g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,(BYTE*)&r,sizeof(r));
  }
 }return true;
}
static int Open(int p){
 if(!Near(p)||Npc[Player[p].m_nIndex].m_ActionScriptID!=Npc[Player[p].m_nLastNpcIndex].m_ActionScriptID)return 0;
 Session& s=sessions[p];s=Session();s.owner=Player[p].m_dwID;s.actor=Npc[Player[p].m_nIndex].m_dwID;
 s.npcIndex=Player[p].m_nLastNpcIndex;s.npc=Npc[s.npcIndex].m_dwID;s.script=Npc[s.npcIndex].m_ActionScriptID;
 s.map=SubWorld[Npc[Player[p].m_nIndex].m_SubWorldIndex].m_SubWorldID;s.nonce=(++serial+GetTickCount())&0x7fffffff;if(!s.nonce)s.nonce=1;s.until=GetTickCount()+120000;
 PHONGTHAN_PLAYER_EVENT e;ZeroMemory(&e,sizeof(e));PhongThanInitializeWireHeader(&e.Header,PHONGTHAN_MSG_UI_PLAYER_EVENT,sizeof(e),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
 e.MapId=s.map;e.EntityId=s.actor;e.Operation=PHONGTHAN_PLAYER_SMITH;e.Value=s.nonce;
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,(BYTE*)&e,sizeof(e));return 1;
}
static int QuoteIndex(int p){
 if(!Valid(p))return 0;Session& s=sessions[p];if(!s.quoteItem)return 0;bool equipped=false;int i=FindCarried(p,s.quoteItem,equipped);
 return i&&Fingerprint(Item[i])==s.quoteHash&&Stone(Item[i])==s.quoteStone?i:0;
}
static int Commit(int p,int& stone){
 stone=0;if(p<=0||p>=MAX_PLAYER)return PT_SMITH_RESULT_UNAVAILABLE;
 Session& s=sessions[p];int i=QuoteIndex(p);stone=s.quoteStone;ClearQuote(s);
 if(!i)return PT_SMITH_RESULT_UNAVAILABLE;
 KItem improved=Item[i];if(!improved.ImprovePhongThanProperty((int)g_Random(10000)))return PT_SMITH_RESULT_MAXIMUM;
 PhongThanQuestExchangeRow need;PhongThanDecodeQuestItemTuple(8,stone,2,0,0,0,need.item);need.count=1;
 int task=TASKVALUE_PT_HANDBOOK_RECEIPT,value=Player[p].m_cTask.GetSaveVal(task);
 if(PhongThanQuestExchange(p,task,value,value==1?0:1,&need,1,0,0,true,0,true)!=1)return PT_SMITH_RESULT_NO_GEM;
 bool success=g_Random(100)<50;
 if(success){Item[i]=improved;Player[p].m_ItemList.SyncItemMagicAttrib(i);}
 Player[p].m_uMustSave=SAVE_REQUEST;return success?PT_SMITH_RESULT_SUCCESS:PT_SMITH_RESULT_FAILED;
}
}
bool PhongThanSmithCommand(int p,const PHONGTHAN_COMPOSE_COMMAND* c){
 if(!c||!c->Header.Sequence)return false;if(p<=0||p>=MAX_PLAYER)return true;
 if(!PtSmith::Valid(p)||PtSmith::sessions[p].nonce!=c->Header.Sequence||PtSmith::sessions[p].map!=c->MapId)return PtSmith::Reply(p,c,PT_SMITH_RESULT_UNAVAILABLE);
 PtSmith::Session& s=PtSmith::sessions[p];DWORD now=GetTickCount();
 if(s.next&&(int)(now-s.next)<0)return PtSmith::Reply(p,c,PT_SMITH_RESULT_WAIT);s.next=now+300;
 PtSmith::ClearQuote(s);
 for(int k=1;k<9;++k)if(c->Items[k]||c->Quantities[k])return PtSmith::Reply(p,c,PT_SMITH_RESULT_WRONG_ITEM);
 if(!c->Items[0]||c->Quantities[0]!=1||c->SocketPosition)return PtSmith::Reply(p,c,PT_SMITH_RESULT_WRONG_ITEM);
 bool equipped=false;int i=PtSmith::FindCarried(p,c->Items[0],equipped);
 if(!i)return PtSmith::Reply(p,c,equipped?PT_SMITH_RESULT_EQUIPPED:PT_SMITH_RESULT_WRONG_ITEM);
 int stone=PtSmith::Stone(Item[i]);if(!stone)return PtSmith::Reply(p,c,PT_SMITH_RESULT_WRONG_ITEM);
 if(!Item[i].ImprovePhongThanProperty(-1))return PtSmith::Reply(p,c,PT_SMITH_RESULT_MAXIMUM);
 s.quoteItem=Item[i].GetID();s.quoteHash=PtSmith::Fingerprint(Item[i]);s.quoteStone=stone;s.until=now+120000;
 if(!Player[p].ExecuteScript(s.script,"pt_smith_confirm",0)){PtSmith::ClearQuote(s);return PtSmith::Reply(p,c,PT_SMITH_RESULT_UNAVAILABLE);}
 return PtSmith::Reply(p,c,PT_SMITH_RESULT_QUOTE,stone);
}
int LuaSmithPropertyOpen(Lua_State* L){Lua_PushNumber(L,Lua_GetTopIndex(L)==0?PtSmith::Open(GetPlayerIndex(L)):0);return 1;}
int LuaSmithPropertyAction(Lua_State* L){
 int p=GetPlayerIndex(L),result=PT_SMITH_RESULT_UNAVAILABLE,stone=0;
 bool valid=p>0&&p<MAX_PLAYER&&Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1);
 if(valid){double op=Lua_ValueToNumber(L,1);PtSmith::Session& s=PtSmith::sessions[p];
  if(op==2){PtSmith::ClearQuote(s);Lua_PushNumber(L,1);return 1;}
  if(PtSmith::Valid(p)&&Npc[Player[p].m_nIndex].m_ActionScriptID==s.script){
   if(op==0){int i=PtSmith::QuoteIndex(p);Lua_PushNumber(L,i?1:0);Lua_PushString(L,i?Item[i].GetName():"");Lua_PushNumber(L,i?s.quoteStone:0);return 3;}
   if(op==1)result=PtSmith::Commit(p,stone);
  }
 }
 Lua_PushNumber(L,result);Lua_PushNumber(L,stone);return 2;
}

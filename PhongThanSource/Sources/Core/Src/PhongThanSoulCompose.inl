#include "PhongThanSoulRules.h"
static int PtSoulCompose(int player,const int* indexes,int* selected,int mode,int& recipe,int& money,bool preview){
 PtSoulRules::Plan plan;int result=PtSoulRules::Quote(indexes,selected,!preview,plan,mode);if(result!=PT_COMPOSE_OK)return result;
 recipe=plan.Recipe;money=plan.Money;if(preview){memcpy(selected,plan.Used,sizeof(plan.Used));return PT_COMPOSE_OK;}
#ifndef PHONGTHAN_COMPOSE_QUOTE_ONLY
 if(Player[player].m_ItemList.GetEquipmentMoney()<money)return PT_COMPOSE_NO_MONEY;
 KItem original=Item[indexes[plan.Main]],updated=original;PT_SOUL_STATE after=plan.Before;
 const PTSoulFormula* f=PtSoulFormula(plan.Formula);if(!f)return PT_COMPOSE_TABLE_ERROR;
 if(plan.Refill)after.Aura=min(f->capacity,after.Aura+200);else{after.Formula=f->id;after.Aura=f->capacity;if(!PtSoulRollMagic(after,g_Random(100),g_Random(1000001)))return PT_COMPOSE_TABLE_ERROR;}
 int param;if(!PtSoulReplace(original.GetParam(),after,param))return PT_COMPOSE_TABLE_ERROR;updated.SetParam(param);if(updated.GetParam()!=param)return PT_COMPOSE_TABLE_ERROR;
 ItemPos destination;if(!Player[player].m_ItemList.SearchPosition(original.GetWidth(),original.GetHeight(),&destination,true))return PT_COMPOSE_NO_ROOM;
 bool detached[9],remainder[9];int counts[9];ZeroMemory(detached,sizeof(detached));ZeroMemory(remainder,sizeof(remainder));
 for(int c=0;c<9;++c)counts[c]=indexes[c]?Item[indexes[c]].GetStackNum():0;
 bool valid=true,placed=false;result=PT_COMPOSE_TABLE_ERROR;
 for(int slot=0;slot<9;++slot)if(plan.Used[slot]){if(!Player[player].m_ItemList.Remove(indexes[slot])){valid=false;break;}detached[slot]=true;}
 if(valid){Item[indexes[plan.Main]]=updated;placed=Player[player].m_ItemList.Add(indexes[plan.Main],destination.nPlace,destination.nX,destination.nY,false)!=0;if(!placed){valid=false;result=PT_COMPOSE_NO_ROOM;}}
 for(int s=0;s<9&&valid;++s)if(detached[s]&&s!=plan.Main&&counts[s]>plan.Used[s]){Item[indexes[s]].SetStackNum(counts[s]-plan.Used[s]);remainder[s]=Player[player].m_ItemList.Add(indexes[s],pos_builditem,s,0,false)!=0;if(!remainder[s])valid=false;}
 if(valid&&!Player[player].Pay(money)){valid=false;result=PT_COMPOSE_NO_MONEY;}
 if(valid){for(int used=0;used<9;++used)if(detached[used]&&used!=plan.Main&&!remainder[used])ItemSet.Remove(indexes[used]);Player[player].m_uMustSave=SAVE_REQUEST;return PT_COMPOSE_OK;}
 if(placed)Player[player].m_ItemList.Remove(indexes[plan.Main]);for(int r=0;r<9;++r)if(remainder[r])Player[player].m_ItemList.Remove(indexes[r]);Item[indexes[plan.Main]]=original;
 for(int back=0;back<9;++back)if(detached[back]){Item[indexes[back]].SetStackNum(counts[back]);if(!Player[player].m_ItemList.Add(indexes[back],pos_builditem,back,0,false))g_DebugLog("[Soul] restore invariant player=%d slot=%d",player,back);}
 return result;
#else
 return PT_COMPOSE_NO_RECIPE;
#endif
}
#ifdef _SERVER
namespace PtSoulService {
struct Session {DWORD Owner,Actor,Nonce,Last,Until,Next,Stone;int Map,Mode,Result,Recipe;};
static Session sessions[MAX_PLAYER];static DWORD serial=0;
static bool Actor(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].m_CurrentLife>0&&Npc[Player[p].m_nIndex].m_RegionIndex>=0&&!Player[p].GetLockState()&&!Player[p].CheckTrading();}
static bool Valid(int p){if(!Actor(p))return false;Session& s=sessions[p];int a=Player[p].m_nIndex,w=Npc[a].m_SubWorldIndex;return w>=0&&w<MAX_SUBWORLD&&s.Owner==Player[p].m_dwID&&s.Actor==Npc[a].m_dwID&&s.Map==SubWorld[w].m_SubWorldID&&(int)(GetTickCount()-s.Until)<0;}
static void Reply(int p,int result,int recipe,DWORD nonce){Session& s=sessions[p];PHONGTHAN_COMPOSE_RESULT reply;ZeroMemory(&reply,sizeof(reply));PhongThanInitializeWireHeader(&reply.Header,PT_SOUL_COMPOSE_REPLY,sizeof(reply),PHONGTHAN_WIRE_FLAG_RESPONSE,nonce);reply.MapId=s.Map;reply.EntityId=s.Actor;reply.Result=result;reply.RecipeId=recipe;g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,(BYTE*)&reply,sizeof(reply));}
}
int PhongThanSoulOpen(int p,int mode,unsigned long stone){
 if(!PtSoulService::Actor(p)||(mode!=8&&mode!=9)||(mode==9&&!stone))return 0;int a=Player[p].m_nIndex,w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD)return 0;
 PtSoulService::Session& s=PtSoulService::sessions[p];ZeroMemory(&s,sizeof(s));s.Owner=Player[p].m_dwID;s.Actor=Npc[a].m_dwID;s.Map=SubWorld[w].m_SubWorldID;s.Mode=mode;s.Stone=stone;s.Nonce=(++PtSoulService::serial+GetTickCount())&0x7fffffff;if(!s.Nonce)s.Nonce=1;s.Until=GetTickCount()+120000;
 PHONGTHAN_PLAYER_EVENT event;ZeroMemory(&event,sizeof(event));PhongThanInitializeWireHeader(&event.Header,PHONGTHAN_MSG_UI_PLAYER_EVENT,sizeof(event),PHONGTHAN_WIRE_FLAG_RESPONSE,s.Nonce);event.MapId=s.Map;event.EntityId=s.Actor;event.Operation=PHONGTHAN_PLAYER_ENCHASE_PANEL;event.Value=mode;
 return SUCCEEDED(g_pServer->SendData(Player[p].m_nNetConnectIdx,&event,sizeof(event)))?1:0;
}
static void PhongThanSoulComposeCommand(int p,const PHONGTHAN_COMPOSE_COMMAND* command){
 if(!PtSoulService::Valid(p))return;PtSoulService::Session& s=PtSoulService::sessions[p];if(command->MapId!=(unsigned)s.Map)return;
 if(command->Header.Sequence==s.Last){PtSoulService::Reply(p,s.Result,s.Recipe,s.Nonce);return;}
 if(command->Header.Sequence!=s.Nonce){PtSoulService::Reply(p,PT_COMPOSE_STALE,0,s.Nonce);return;}
 DWORD now=GetTickCount();if(s.Next&&(int)(now-s.Next)<0)return;s.Next=now+300;
 int indexes[9],selected[9];bool valid=true,stone=!s.Stone;
 for(int slot=0;slot<9;++slot){indexes[slot]=Player[p].m_ItemList.GetBuildItem(slot);selected[slot]=0;
  int i=indexes[slot];if(!i){if(command->Items[slot])valid=false;continue;}if(i<=0||i>=MAX_ITEM){valid=false;continue;}
  int at=Player[p].m_ItemList.FindSame(i);if(!at||at>=MAX_PLAYER_ITEM||Player[p].m_ItemList.m_Items[at].nPlace!=pos_builditem||Player[p].m_ItemList.m_Items[at].nX!=slot)valid=false;
  KItem& item=Item[i];int quantity=command->Quantities[slot],count=item.GetStackNum();
  if(!item.GetID()||item.GetID()!=command->Items[slot]||count<1||count>10000||quantity<-1||quantity>count||item.GetLock()->IsLock()||item.IsTemp()||(item.GetExpireTime()>0&&(unsigned)item.GetExpireTime()<=KSG_GetCurSec()))valid=false;
  selected[slot]=quantity==-1?0:quantity?quantity:count;if(selected[slot]>0&&item.GetID()==s.Stone&&PtSoulRules::Stone(item))stone=true;
 }
 int recipe=0,money=0;int result=valid&&stone?PtSoulCompose(p,indexes,selected,s.Mode,recipe,money,false):PT_COMPOSE_STALE;
 s.Last=s.Nonce;if(++s.Nonce==0)s.Nonce=1;s.Result=result;s.Recipe=recipe;s.Until=now+120000;PtSoulService::Reply(p,result,recipe,s.Nonce);
}
#endif

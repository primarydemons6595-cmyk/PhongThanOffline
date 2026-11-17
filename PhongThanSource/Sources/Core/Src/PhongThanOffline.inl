#ifdef _SERVER
#include "PhongThanOffline.h"
#include "PhongThanOfflineMail.h"
#include "../../../Headers/PhongThanOfflineContract.h"
#include "../../../Headers/PhongThanOfflineProtocol.h"
#include <time.h>
namespace PTOffline {
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&!Player[p].m_pStatusLoadPlayerInfo&&!Player[p].m_bIsBot&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC;}
static int Get(int p,int task){return Player[p].m_cTask.GetSaveVal(task);}static void Set(int p,int task,int value){Player[p].m_cTask.SetSaveVal(task,value,TRUE);}
static bool Carried(PlayerItem* p){return p->nPlace==pos_equiproom||p->nPlace==pos_equiproomex||p->nPlace==pos_ptbag2||p->nPlace==pos_ptbag3;}
static bool Usable(int id,int now){return id>0&&id<MAX_ITEM&&Item[id].GetID()&&!Item[id].GetLock()->IsLock()&&(!Item[id].GetExpireTime()||Item[id].GetExpireTime()>now);}
static bool ActiveSunMoon(int p,int now){for(PlayerItem* it=Player[p].m_ItemList.GetFirstItem();it;it=Player[p].m_ItemList.GetNextItem()){int id=it->nIdx;if(Carried(it)&&Usable(id,now)&&Item[id].GetGenre()==8&&(Item[id].GetDetailType()==164||Item[id].GetDetailType()==252)&&Item[id].GetParticular()==0&&Item[id].GetParam()==0x2d000164&&Item[id].GetExpireTime()>now)return true;}return false;}
static int Policy(const char* key,int fallback,int high){int value=GetPrivateProfileIntA("Loose",key,fallback,".\\settings\\phongthan\\OfflinePolicy.ini");return value>=0&&value<=high?value:fallback;}
static int Day(int now){return (now+7*3600)/86400;}
static void DayReset(int p,int now){int day=Day(now);if(Get(p,PtOfflineTask::Day)!=day){Set(p,PtOfflineTask::Day,day);Set(p,PtOfflineTask::ExpHours,0);}}
static int Skill(int p){int skill=Get(p,PtOfflineTask::Skill);KSkillList& list=Npc[Player[p].m_nIndex].m_SkillList;int level=list.GetCurrentLevel(skill),next=list.GetNextExp(skill),current=list.GetExp(skill);if(level<1||level>=10||list.IsTempSkill(skill)||next<=current)return 0;return next-current;}
static int Configure(int p,int mask){if(!Live(p)||mask<0||mask>7||(mask&&Get(p,PtOfflineTask::Pending))||Player[p].CheckTrading()||Player[p].GetLockState())return 0;
 int now=(int)time(0);if((mask&1)&&!ActiveSunMoon(p,now))return 0;
 if((mask&4)&&!Skill(p))return 0;
 Set(p,PtOfflineTask::Mode,mask);Set(p,PtOfflineTask::Start,0);Player[p].Save();Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}
static int Claim(int p,int mode){if(!Live(p)||mode<0||mode>2||Player[p].CheckTrading()||Player[p].GetLockState())return 0;
 int now=(int)time(0),packed=Get(p,PtOfflineTask::Pending),hours=PhongThanOfflinePending(packed,mode);if(hours<1||hours>4)return 0;DayReset(p,now);
 PhongThanQuestExchangeRow need,reward;memset(&need,0,sizeof(need));memset(&reward,0,sizeof(reward));int needs=0,rewards=0,exp=0,skillExp=0;
 if(mode==0){if(!ActiveSunMoon(p,now)||Get(p,PtOfflineTask::ExpHours)>=4||Npc[Player[p].m_nIndex].m_Level>=200)return 0;exp=Npc[Player[p].m_nIndex].m_Level*1200;}
 if(mode==1){PhongThanDecodeQuestItemTuple(8,166,2,0,0,0,need.item);need.count=1;needs=1;int detail=(Get(p,PtOfflineTask::GemRolls)&1)?80:78;PhongThanDecodeQuestItemTuple(3,detail,0,0,0,0,reward.item);reward.count=1;rewards=1;}
 if(mode==2){int left=Skill(p);if(left<=0)return 0;skillExp=Policy("SkillExperiencePerHour",1200,100000000);if(skillExp<1)return 0;if(skillExp>left)skillExp=left;PhongThanDecodeQuestItemTuple(8,165,2,0,0,0,need.item);need.count=1;needs=1;}
 int receipt=Get(p,PtOfflineTask::Receipt);if(receipt<0||receipt>=1000000000)return 0;
 bool paid=PhongThanQuestExchange(p,PtOfflineTask::Receipt,receipt,receipt+1,&need,needs,&reward,rewards,true,0,true,0,true);
 if(!paid&&mode==1&&PhongThanOfflineMailPrepare(p,receipt,receipt+1,reward.item.detail)){
  paid=PhongThanQuestExchange(p,PtOfflineTask::Receipt,receipt,receipt+1,&need,needs,0,0,true,0,true,0,true);
  if(!paid)PhongThanOfflineMailReconcile(p);
 }
 if(!paid)return 0;
 Set(p,PtOfflineTask::Pending,PhongThanOfflineSetPending(packed,mode,hours-1));
 if(mode==0){Set(p,PtOfflineTask::ExpHours,Get(p,PtOfflineTask::ExpHours)+1);Player[p].DirectAddExp(exp);}
 else if(mode==1)Set(p,PtOfflineTask::GemRolls,Get(p,PtOfflineTask::GemRolls)>>1);
 else Player[p].IncSkillExp(Get(p,PtOfflineTask::Skill),skillExp);
 // Clear hours rendered unusable by this award immediately, without charging
 // extra pills or forcing another tick before the original exit dialog works.
 int pending=Get(p,PtOfflineTask::Pending);
 if(mode==0&&(Get(p,PtOfflineTask::ExpHours)>=4||Npc[Player[p].m_nIndex].m_Level>=200))pending=PhongThanOfflineSetPending(pending,0,0);
 if(mode==2&&!Skill(p))pending=PhongThanOfflineSetPending(pending,2,0);
 Set(p,PtOfflineTask::Pending,pending);
 Player[p].Save();Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}
}
int PhongThanUseOfflinePill(int p,int item){if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=8)return -1;int detail=Item[item].GetDetailType();if(detail!=164&&detail!=252)return -1;
 if(!PTOffline::Live(p)||Player[p].CheckTrading()||Player[p].GetLockState()||Item[item].GetParticular()!=0||!PTOffline::Usable(item,(int)time(0)))return 0;
 bool carried=false;for(PlayerItem* it=Player[p].m_ItemList.GetFirstItem();it;it=Player[p].m_ItemList.GetNextItem())if(it->nIdx==item&&PTOffline::Carried(it))carried=true;if(!carried)return 0;
 if(Item[item].GetParam()!=0x2d000164){int expiry=Item[item].GetExpireTime(),end=(int)time(0)+(detail==164?2592000:604800);if(expiry>0&&expiry<end)end=expiry;Item[item].SetExpireTime(end);Item[item].SetParam(0x2d000164);Player[p].m_ItemList.SyncItem(item);Player[p].Save();Player[p].m_uMustSave=SAVE_REQUEST;}
 return 1;
}
void PhongThanOfflineLeaving(int p){if(!PTOffline::Live(p)||PTOffline::Get(p,PtOfflineTask::Pending)||PTOffline::Get(p,PtOfflineTask::Start))return;int mask=PTOffline::Get(p,PtOfflineTask::Mode),now=(int)time(0);if(mask<0||mask>7)return;if((mask&1)&&!PTOffline::ActiveSunMoon(p,now))mask&=~1;if((mask&4)&&!PTOffline::Skill(p))mask&=~4;PTOffline::Set(p,PtOfflineTask::Mode,mask);PTOffline::Set(p,PtOfflineTask::Start,mask?now:0);}
void PhongThanOfflineLogin(int p){if(!PTOffline::Live(p))return;int start=PTOffline::Get(p,PtOfflineTask::Start),now=(int)time(0);if(!start)return;
 int hours=PhongThanOfflineHours(start,now),mask=PTOffline::Get(p,PtOfflineTask::Mode),pending=0,rolls=0;if((mask&1)&&!PTOffline::ActiveSunMoon(p,now))mask&=~1;
 for(int mode=0;mode<3;++mode)if(mask&(1<<mode))pending=PhongThanOfflineSetPending(pending,mode,hours);
 for(int h=0;h<hours;++h)if(g_Random(10000)<PTOffline::Policy("WholeCrystalChancePer10000",1000,10000))rolls|=1<<h;
 PTOffline::Set(p,PtOfflineTask::Pending,pending);PTOffline::Set(p,PtOfflineTask::GemRolls,rolls);PTOffline::Set(p,PtOfflineTask::Start,0);PTOffline::DayReset(p,now);Player[p].Save();Player[p].m_uMustSave=SAVE_REQUEST;
}
void PhongThanOfflineTick(int p){
 if(!PTOffline::Live(p))return;struct State {BYTE Ticket[16];DWORD At;bool Checked;};static State states[MAX_PLAYER];State& s=states[p];
 if(memcmp(s.Ticket,Player[p].m_SessionTicket,16)){memset(&s,0,sizeof(s));memcpy(s.Ticket,Player[p].m_SessionTicket,16);}
 DWORD now=GetTickCount();if(s.At&&now-s.At<2000)return;s.At=now;
 if(s.Checked&&!PTOffline::Get(p,PtOfflineTask::Start)&&!PTOffline::Get(p,PtOfflineTask::Pending))return;
 if(!PhongThanOfflineMailReconcile(p))return;s.Checked=true;PhongThanOfflineLogin(p);
 int packed=PTOffline::Get(p,PtOfflineTask::Pending);
 // Ineligible EXP/skill hours cannot award beyond original limits and do not
 // consume pills. Mailbox/pill shortages retain their hours for automatic retry.
 if(PhongThanOfflinePending(packed,0)&&(!PTOffline::ActiveSunMoon(p,(int)time(0))||Npc[Player[p].m_nIndex].m_Level>=200||PTOffline::Get(p,PtOfflineTask::ExpHours)>=4))packed=PhongThanOfflineSetPending(packed,0,0);
 if(PhongThanOfflinePending(packed,2)&&!PTOffline::Skill(p))packed=PhongThanOfflineSetPending(packed,2,0);
 if(packed!=PTOffline::Get(p,PtOfflineTask::Pending)){PTOffline::Set(p,PtOfflineTask::Pending,packed);Player[p].Save();Player[p].m_uMustSave=SAVE_REQUEST;}
 for(int mode=0;mode<3;++mode)for(int hour=0;hour<4;++hour){if(!PTOffline::Claim(p,mode))break;s.Checked=false;}
}
static int LuaOfflineConfigure(Lua_State* L){Lua_PushNumber(L,PTOffline::Configure(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1)));return 1;}
static int LuaOfflineClaim(Lua_State* L){Lua_PushNumber(L,PTOffline::Claim(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1)));return 1;}
static int LuaOfflineState(Lua_State* L){int p=GetPlayerIndex(L),mode=(int)Lua_ValueToNumber(L,1);if(!PTOffline::Live(p)){Lua_PushNumber(L,0);return 1;}Lua_PushNumber(L,mode==3?PTOffline::Get(p,PtOfflineTask::Mode):PhongThanOfflinePending(PTOffline::Get(p,PtOfflineTask::Pending),mode));return 1;}
static int LuaOfflineDiscard(Lua_State* L){int p=GetPlayerIndex(L);if(!PTOffline::Live(p)||Player[p].CheckTrading()||Player[p].GetLockState()){Lua_PushNumber(L,0);return 1;}PTOffline::Set(p,PtOfflineTask::Pending,0);PTOffline::Set(p,PtOfflineTask::GemRolls,0);Player[p].Save();Player[p].m_uMustSave=SAVE_REQUEST;Lua_PushNumber(L,1);return 1;}
bool PhongThanProcessOffline(int p,const char* bytes,int size){
 if(!bytes||size<(int)sizeof(tagExtendProtoHeader)+1||((BYTE*)bytes)[0]!=c2s_extend||((BYTE*)bytes)[sizeof(tagExtendProtoHeader)]!=PT_OFFLINE_FAMILY)return false;
 if(size!=sizeof(tagExtendProtoHeader)+sizeof(PTOfflineRequest)||!PTOffline::Live(p))return true;
 const tagExtendProtoHeader* header=(const tagExtendProtoHeader*)bytes;const PTOfflineRequest* q=(const PTOfflineRequest*)(bytes+sizeof(*header));
 if(header->wLength+1!=size||q->Version!=2||q->Skill>MAX_SKILL||!q->Sequence||q->Mode>7||q->Action>1||q->Reserved)return true;
 struct Session {BYTE Ticket[16];unsigned int Sequence;DWORD Last;};static Session sessions[MAX_PLAYER];Session& s=sessions[p];
 if(memcmp(s.Ticket,Player[p].m_SessionTicket,16)){memset(&s,0,sizeof(s));memcpy(s.Ticket,Player[p].m_SessionTicket,16);}
 DWORD now=GetTickCount();if(q->Sequence<=s.Sequence||(s.Last&&now-s.Last<250))return true;s.Sequence=q->Sequence;s.Last=now;
 if(!PTOffline::Get(p,PtOfflineTask::Pending))PTOffline::Set(p,PtOfflineTask::Skill,q->Skill);
 PTOfflineResponse response;memset(&response,0,sizeof(response));response.Family=PT_OFFLINE_FAMILY;response.Version=2;response.Sequence=q->Sequence;
 // The original exit dialog is the sole selection UI. Claiming is automatic.
 response.Result=q->Action==1?PTOffline::Configure(p,q->Mode):0;
 char packet[sizeof(tagExtendProtoHeader)+sizeof(response)];tagExtendProtoHeader* out=(tagExtendProtoHeader*)packet;memset(out,0,sizeof(*out));out->ProtocolType=s2c_extend;out->wLength=sizeof(packet)-1;memcpy(packet+sizeof(*out),&response,sizeof(response));
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,packet,sizeof(packet));return true;
}
#endif

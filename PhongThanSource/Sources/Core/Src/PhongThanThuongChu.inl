#include "PhongThanThuongChu.h"
extern int GetPlayerIndex(Lua_State*);
extern int PhongThanThuongChuEconomy(int,int,int);
extern int PhongThanThuongChuQuestPayment(int,int,int);
extern void PhongThanThuongChuBuffTick(int,unsigned long);
#include "../../../Headers/PhongThanThuongChuPolicy.h"
// Reconstructed gameplay from VNG's documented rules; the old guide NPCs are
// not treated as combatants. Player inventories, payments and role saves remain
// normal engine transactions. Sessions never adopt arbitrary map NPCs.
namespace PtThuongChu {
enum { EventId=2101,TaskCamp=5070,TaskPk=5071,TaskReward=5072,TaskScore=5073,TaskSession=5074,TaskSide=5075,TaskRevMap=5076,TaskRevX=5077,TaskRevY=5078,TaskGranted=5079 };
struct Member {DWORD id;int side,points,quest,kills,marshal,done,carried,flagSlot;bool left,entered;char key[160];Member():id(0),side(0),points(0),quest(0),kills(0),marshal(0),done(0),carried(0),flagSlot(-1),left(false),entered(false){key[0]=0;}};
struct Unit {int index,side,slot,kind,x,y;DWORD id;unsigned long respawn;bool neutral,dead;Unit():index(0),side(0),slot(0),kind(0),x(0),y(0),id(0),respawn(0),neutral(false),dead(false){}};
static PTThuongChuMatch match;static Member members[MAX_PLAYER];static Unit units[32];static int Map=1071;static bool modernStarted=false;static int pendingGenerals[3];static unsigned long generation=0,nextTick=0;static int minimumLevel=120;
static bool Near(int,int,int,int);static void DropFlag(int);
static std::map<std::string,Member> archived;
static unsigned long pendingCheck[MAX_PLAYER];static DWORD pendingIdentity[MAX_PLAYER];
static bool Key(int p,char* key){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||!Player[p].m_SaveBuffer)return false;
 const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)Player[p].m_SaveBuffer;
 if(!PhongThanValidateCharacterState(s,s->StateSize))return false;
 int at=0;for(int part=0;part<2;++part){const char* name=part?Player[p].Name:Player[p].AccountName;for(int n=0;name[n]&&n<32;++n)at+=sprintf(key+at,"%02x",(unsigned char)name[n]);key[at++]='-';}sprintf(key+at,"%lu",s->RoleTime);return true;
}
static void Archive(int p){if(members[p].key[0])archived[members[p].key]=members[p];}
static void ApplyPending(int p){char key[160];if(!Key(p,key))return;
 char path[240];sprintf(path,"EventService\\thuong_chu\\%s.ini",key);int session=GetPrivateProfileIntA("Reward","Session",0,path);
 if(session>0&&Player[p].m_cTask.GetSaveVal(TaskGranted)!=session){int xp=GetPrivateProfileIntA("Reward","Experience",0,path),points=GetPrivateProfileIntA("Reward","Points",0,path);
  if(xp>0&&xp<=4000000&&points>=0&&points<=2000000000){Player[p].m_cTask.SetSaveVal(TaskReward,xp,TRUE);Player[p].m_cTask.SetSaveVal(TaskScore,Player[p].m_cTask.GetSaveVal(TaskScore)+points,TRUE);Player[p].m_cTask.SetSaveVal(5082,PTThuongChuMatch::Points(Player[p].m_cTask.GetSaveVal(5082),GetPrivateProfileIntA("Reward","Merit",0,path)),TRUE);Player[p].m_cTask.SetSaveVal(TaskGranted,session,TRUE);Player[p].Save();}}
}
static bool Valid(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC;}
static int World(int p){return Valid(p)&&Npc[Player[p].m_nIndex].m_SubWorldIndex>=0?SubWorld[Npc[Player[p].m_nIndex].m_SubWorldIndex].m_SubWorldID:0;}
static bool Own(int p){return Valid(p)&&members[p].id==Player[p].m_dwID&&members[p].side>0;}
static int Count(int side){int n=0;for(int p=1;p<MAX_PLAYER;++p)if(Own(p)&&members[p].side==side&&!members[p].left)++n;return n;}
static void Score(int p,int delta){if(!Own(p))return;members[p].points=PTThuongChuMatch::Points(members[p].points,delta);Archive(p);}
static void Restore(int p){if(!Valid(p))return;KPlayer& player=Player[p];int oldMap=player.m_cTask.GetSaveVal(TaskRevMap);if(oldMap>0){PLAYER_REVIVAL_POS* pos=player.GetDeathRevivalPos();pos->m_nSubWorldID=oldMap;pos->m_nMpsX=player.m_cTask.GetSaveVal(TaskRevX);pos->m_nMpsY=player.m_cTask.GetSaveVal(TaskRevY);player.m_cTask.SetSaveVal(TaskRevMap,0,TRUE);}int saved=player.m_cTask.GetSaveVal(TaskCamp);if(saved>0){Npc[player.m_nIndex].SetCurrentCamp(saved-1);player.m_cTask.SetSaveVal(TaskCamp,0,TRUE);int pk=player.m_cTask.GetSaveVal(TaskPk);player.m_cPK.SetLockPKState((BYTE)pk,0);player.m_cTask.SetSaveVal(TaskPk,0,TRUE);}Npc[player.m_nIndex].SetFightMode(0);}
static bool Exit(int p){if(!Valid(p))return false;int side=Own(p)?members[p].side:Player[p].m_cTask.GetSaveVal(TaskSide);int map=side==1?1021:1020,x=side==1?237:159,y=side==1?180:189;
 if((World(p)==Map||(World(p)==1064&&Player[p].m_cTask.GetSaveVal(TaskCamp)>0))&&!Npc[Player[p].m_nIndex].ChangeWorld(map,x*256,y*512))return false;Restore(p);if(Own(p)){DropFlag(p);members[p].left=true;members[p].quest=members[p].kills=members[p].marshal=0;Archive(p);}Player[p].Save();return true;}
static void Remove(Unit& u){if(u.index>0&&u.index<MAX_NPC&&Npc[u.index].m_dwID==u.id&&!Npc[u.index].IsPlayer()){KNpc& n=Npc[u.index];int w=n.m_SubWorldIndex,r=n.m_RegionIndex;if(w>=0&&w<MAX_SUBWORLD&&r>=0&&r<SubWorld[w].m_nTotalRegion){SubWorld[w].m_Region[r].RemoveNpc(u.index);SubWorld[w].m_Region[r].DecRef(n.m_MapX,n.m_MapY,obj_npc);}NpcSet.Remove(u.index);}u=Unit();}
static bool Spawn(int slot,int side,bool neutral){int world=g_SubWorldSet.SearchWorld(Map);if(world<0||slot<0||slot>2)return false;
 static const int neutralPos[3][2]={{237,190},{237,200},{237,220}},shang[3][2]={{231,191},{231,201},{231,219}},zhou[3][2]={{242,190},{242,204},{242,224}};
 Unit& u=units[neutral?slot:3+slot];Remove(u);const int* pos=neutral?neutralPos[slot]:side==1?shang[slot]:zhou[slot];int tid=side==2?580:579;
 int n=NpcSet.Add(MAKELONG(120,tid),world,pos[0]*256,pos[1]*512,TRUE);if(n<=0||n>=MAX_NPC)return false;
 KNpc& npc=Npc[n];npc.SetCurrentCamp(neutral?camp_free:side==1?camp_justice:camp_evil);npc.SetFightMode(enumFightActive);npc.m_btSpecial=npc_gold;
 lstrcpynA(npc.Name,neutral?"\116\147\165\171\252\156\040\123\157\270\151":side==1?"\124\255\355\156\147\040\114\334\156\150\040\124\150\255\254\156\147":"\124\255\355\156\147\040\114\334\156\150\040\103\150\165",sizeof(npc.Name));u.index=n;u.id=npc.m_dwID;u.side=side;u.slot=slot;u.kind=neutral?0:1;u.neutral=neutral;return true;}
static void Finish(unsigned long now,int winner){if(match.phase==3||!match.phase)return;match.Finish(now,winner);
 for(int p=1;p<MAX_PLAYER;++p)if(Own(p))Archive(p);
 CreateDirectoryA("EventService\\thuong_chu",0);
 for(std::map<std::string,Member>::iterator it=archived.begin();it!=archived.end();++it){Member& m=it->second;if(!m.entered)continue;int xp=PTThuongChuMatch::Reward(m.side,winner,m.done>0);if(xp<=0)continue;
  char path[240],text[240];sprintf(path,"EventService\\thuong_chu\\%s.ini",it->first.c_str());sprintf(text,"[Reward]\nSession=%lu\nExperience=%d\nPoints=%d\nMerit=%d\n",generation,xp,m.points,m.points);
  PtEvents::Write(path,text);
 }
 for(int role=1;role<MAX_PLAYER;++role)if(Own(role)){ApplyPending(role);pendingCheck[role]=0;}

}
static bool Near(int p,int map,int x,int y){if(World(p)!=map)return false;int px,py;Npc[Player[p].m_nIndex].GetMpsPos(&px,&py);return abs(px-x*256)<=768&&abs(py-y*512)<=768;}
static bool Join(int p,int side){if(Valid(p))ApplyPending(p);if(!Valid(p)||!Near(p,1001,side==1?189:212,side==1?191:205)||Npc[Player[p].m_nIndex].m_Level<minimumLevel||Player[p].CheckTrading()||Player[p].GetLockState()||Player[p].m_cTask.GetSaveVal(TaskReward)>0)return false;
 if(Own(p)){if(members[p].side!=side)return false;members[p].left=false;return true;}
 if(!match.Admit(side,Count(1),Count(2)))return false;Member& m=members[p];m=Member();m.id=Player[p].m_dwID;m.side=side;if(!Key(p,m.key)){m=Member();return false;}Archive(p);
 Player[p].m_cTask.SetSaveVal(TaskSession,generation,TRUE);Player[p].m_cTask.SetSaveVal(TaskSide,side,TRUE);return Player[p].Save()!=FALSE;}
static bool Enter(int p){if(!Own(p)||members[p].left||match.phase!=2||!Near(p,1001,members[p].side==1?189:212,members[p].side==1?191:205)||Player[p].CheckTrading())return false;Member& m=members[p];KNpc& n=Npc[Player[p].m_nIndex];KPlayer& player=Player[p];
 int x,y;if(Map==1071){x=m.side==1?178:290;y=m.side==1?203:206;}else{x=m.side==1?176:216;y=195;}int previousCamp=n.m_CurrentCamp;if(!n.ChangeWorld(Map,x*256,y*512))return false;
 if(!player.m_cTask.GetSaveVal(TaskCamp)){player.m_cTask.SetSaveVal(TaskCamp,previousCamp+1,TRUE);player.m_cTask.SetSaveVal(TaskPk,player.m_cPK.GetNormalPKState(),TRUE);}
 PLAYER_REVIVAL_POS* revive=player.GetDeathRevivalPos();if(!player.m_cTask.GetSaveVal(TaskRevMap)){player.m_cTask.SetSaveVal(TaskRevMap,revive->m_nSubWorldID,TRUE);player.m_cTask.SetSaveVal(TaskRevX,revive->m_nMpsX,TRUE);player.m_cTask.SetSaveVal(TaskRevY,revive->m_nMpsY,TRUE);}revive->m_nSubWorldID=Map;revive->m_nMpsX=x*256;revive->m_nMpsY=y*512;m.entered=true;Archive(p);n.SetCurrentCamp(m.side==1?camp_justice:camp_evil);player.m_cPK.SetLockPKState(enumPKWar,1);n.SetFightMode(enumFightActive);return player.Save()!=FALSE;}
static int Claim(int p){if(!Valid(p))return 0;int side=Player[p].m_cTask.GetSaveVal(TaskSide);if(!Near(p,side==1?1021:1020,side==1?237:159,side==1?180:189))return 0;int xp=Player[p].m_cTask.GetSaveVal(TaskReward);if(xp<=0)return 0;
 Player[p].m_cTask.SetSaveVal(TaskReward,0,TRUE);Player[p].DirectAddExp(xp);Player[p].Save();return xp;}
#include "PhongThanThuongChuTgpt.inl"
}
bool PhongThanThuongChuOpen(int registration,int duration){using namespace PtThuongChu;if(match.phase||registration<1||duration<1)return false;int selected=GetRandomNumber(1,3);int forced=Config("Mode",0);if(forced>=1&&forced<=3)selected=forced;Map=selected==1?1071:1064;if(g_SubWorldSet.SearchWorld(Map)<0)return false;modernStarted=false;
 archived.clear();for(int p=1;p<MAX_PLAYER;++p)members[p]=Member();for(int i=0;i<32;++i)Remove(units[i]);ZeroMemory(pendingGenerals,sizeof(pendingGenerals));generation=(unsigned long)time(NULL);match.Open(generation,registration,duration,selected);minimumLevel=PhongThanEventValue(EventId,10);if(minimumLevel<1)minimumLevel=120;return true;}
void PhongThanThuongChuClose(){using namespace PtThuongChu;Finish((unsigned long)time(NULL),0);}
int PhongThanThuongChuPhase(){return PtThuongChu::match.phase;}
void PhongThanThuongChuTick(){using namespace PtThuongChu;unsigned long now=(unsigned long)time(NULL);if(now==nextTick)return;nextTick=now;
 for(int p=1;p<MAX_PLAYER;++p)if(Valid(p)){
  PhongThanThuongChuBuffTick(p,now);
  if(pendingIdentity[p]!=Player[p].m_dwID){pendingIdentity[p]=Player[p].m_dwID;pendingCheck[p]=0;char key[160];if(match.phase&&Key(p,key)){std::map<std::string,Member>::iterator old=archived.find(key);if(old!=archived.end()){members[p]=old->second;members[p].id=Player[p].m_dwID;}}}
  if(now>=pendingCheck[p]){ApplyPending(p);pendingCheck[p]=now+30;}

  if(Player[p].m_cTask.GetSaveVal(TaskCamp)>0&&(!Own(p)||World(p)!=Map)){if(World(p)==Map)Exit(p);else Restore(p);}
  if(Own(p)&&World(p)!=Map&&match.phase==2&&Player[p].m_cTask.GetSaveVal(TaskCamp)>0){members[p].quest=members[p].kills=members[p].marshal=0;Restore(p);}
  if(Own(p)&&World(p)==Map&&match.phase==2&&Npc[Player[p].m_nIndex].m_CurrentLife>0&&Npc[Player[p].m_nIndex].m_FightMode!=enumFightActive)Npc[Player[p].m_nIndex].SetFightMode(enumFightActive);
 }
 if(!match.phase)return;
 if(match.phase==1&&now>=match.fightAt){int minimum=GetPrivateProfileIntA("Battle","MinimumPerSide",5,"settings\\phongthan\\ThuongChu.ini");if(minimum<1||minimum>100)minimum=5;
  if(Count(1)<minimum||Count(2)<minimum)Finish(now,0);else match.Begin(now,Count(1),Count(2),minimum);}
 if(match.phase==2){
  ModernTick(now);
  if(match.mode==1)for(int captured=0;captured<3;++captured)if(pendingGenerals[captured]&&Spawn(captured,pendingGenerals[captured],false))pendingGenerals[captured]=0;
  if(match.mode==1)for(int slot=0;slot<3;++slot)if(match.marshal[slot]==-1||(!match.marshal[slot]&&now>=match.fightAt+(unsigned long)(600*(slot+1)))){if(Spawn(slot,0,true))match.marshal[slot]=1;}
  if(now>=match.endAt)Finish(now,match.mode==1?match.Decide(GetRandomNumber(1,2)):match.score[1]==match.score[2]?0:match.score[1]>match.score[2]?1:2);
 }
 if(match.phase==3&&now>=match.exitAt){bool pending=false;for(int p=1;p<MAX_PLAYER;++p)if(Own(p)&&World(p)==Map&&!Exit(p))pending=true;if(!pending){for(int i=0;i<32;++i)Remove(units[i]);match.Reset();}}
}
int PhongThanThuongChuRelation(int a,int b){using namespace PtThuongChu;if(a<=0||a>=MAX_NPC||b<=0||b>=MAX_NPC)return -1;int wa=Npc[a].m_SubWorldIndex,wb=Npc[b].m_SubWorldIndex;if(wa<0||wb<0||wa!=wb||SubWorld[wa].m_SubWorldID!=Map)return -1;
 if(!match.phase)return -1;int pa=Npc[a].IsPlayer()?Npc[a].GetPlayerIdx():0,pb=Npc[b].IsPlayer()?Npc[b].GetPlayerIdx():0;
 int sa=Own(pa)&&!members[pa].left?members[pa].side:0,sb=Own(pb)&&!members[pb].left?members[pb].side:0;bool na=false,nb=false;
 bool bound=sa||sb;for(int i=0;i<32;++i){Unit& u=units[i];if(u.kind==UnitService)continue;if(u.index==a&&u.id==Npc[a].m_dwID){sa=u.side;na=u.neutral;bound=true;}if(u.index==b&&u.id==Npc[b].m_dwID){sb=u.side;nb=u.neutral;bound=true;}}
 if(!bound)return -1;if(match.phase!=2)return relation_none;
 if(!Npc[a].IsAlive()||!Npc[b].IsAlive())return relation_none;if((na&&sb)||(nb&&sa))return relation_enemy;if(!sa||!sb)return relation_none;return sa==sb?relation_ally:relation_enemy;
}
bool PhongThanThuongChuDeath(int victim,int killer){using namespace PtThuongChu;if(victim<=0||victim>=MAX_NPC||Npc[victim].m_SubWorldIndex<0||SubWorld[Npc[victim].m_SubWorldIndex].m_SubWorldID!=Map)return false;
 int p=killer>0&&killer<MAX_NPC&&Npc[killer].IsPlayer()?Npc[killer].GetPlayerIdx():0;
 if(Npc[victim].IsPlayer()){int dead=Npc[victim].GetPlayerIdx();if(match.phase==2&&Own(dead)){DropFlag(dead);Score(dead,-1);if(Own(p)&&members[p].side!=members[dead].side&&!members[p].left){Score(p,2);match.score[members[p].side]+=2;ShareQuest(p,1);Archive(p);}Archive(dead);return true;}return false;}
 for(int i=0;i<32;++i){Unit& u=units[i];if(u.index!=victim||u.id!=Npc[victim].m_dwID)continue;if(u.kind>=UnitFlag)return ModernDeath(u,p);if(u.dead)return true;u.dead=true;
  if(match.phase==2&&Own(p)&&!members[p].left&&(u.neutral||u.side!=members[p].side)){
   bool neutral=u.neutral;int slot=u.slot,side=members[p].side;Score(p,neutral?100:50);
   bool accepted=match.Capture(slot,side,neutral,(unsigned long)time(NULL));if(accepted){if(match.phase==3){match.phase=2;Finish((unsigned long)time(NULL),side);}else pendingGenerals[slot]=side;}
  }return true;
 }return false;
}
int LuaThuongChu(Lua_State* L){using namespace PtThuongChu;int p=GetPlayerIndex(L),op=(int)Lua_ValueToNumber(L,1),arg=(int)Lua_ValueToNumber(L,2),value=0;
 if(Valid(p))switch(op){case 0:value=match.phase;break;case 1:value=Join(p,arg);break;case 2:value=Enter(p);break;case 3:value=Exit(p);break;
 case 4:value=TakeQuest(p,arg);break;
 case 5:value=CompleteQuest(p);break;
 case 6:value=Own(p)?members[p].points:0;break;case 7:value=match.score[arg==1?1:2];break;case 8:value=Claim(p);break;
 case 9:if(Own(p)&&World(p)==Map&&match.phase==2&&arg>=1&&arg<=3){Member& m=members[p];static const int a[3][2]={{225,191},{225,201},{225,219}},b[3][2]={{248,190},{248,204},{248,224}};const int* pos=m.side==1?a[arg-1]:b[arg-1];int x,y;KNpc& actor=Npc[Player[p].m_nIndex];actor.GetMpsPos(&x,&y);if(abs(x-(m.side==1?178:290)*256)<=768&&abs(y-(m.side==1?203:206)*512)<=768&&Player[p].m_ItemList.GetEquipmentMoney()>=10000&&Player[p].m_ItemList.CostMoney(10000)){if(actor.ChangeWorld(Map,pos[0]*256,pos[1]*512)){Player[p].Save();value=1;}else Player[p].Earn(10000);}}break;
 case 13:if(Transaction(p))value=PhongThanThuongChuEconomy(p,1,arg);break;
 case 10:value=Own(p)?members[p].side:0;break;
 case 11:value=ActiveQuest(p);break;case 12:value=Player[p].m_cTask.GetSaveVal(TaskScore);break;
 case 14:value=Player[p].m_cTask.GetSaveVal(RepTask(arg==1?1:2));break;
 case 15:value=Player[p].m_cTask.GetSaveVal(TaskMerit);break;
 case 16:value=CompleteQuest(p,true);break;case 17:value=TakeQuest(p,arg,true);break;
 case 18:value=Deliver(p);break;case 19:value=match.phase?match.mode:0;break;
 case 20:value=Player[p].m_cTask.GetSaveVal(TaskQuestProgress);break;
 case 21:value=Player[p].m_cTask.GetSaveVal(TaskQuestDay)==Day()?Player[p].m_cTask.GetSaveVal(TaskQuestRuns):0;break;
 case 22:case 23:case 24:case 25:if(Transaction(p))value=PhongThanThuongChuEconomy(p,op-20,arg);break;
 case 26:value=match.flags[arg==1?1:2];break;
 case 27:value=Own(p)?members[p].carried:0;break;
 }

 Lua_PushNumber(L,value);return 1;
}

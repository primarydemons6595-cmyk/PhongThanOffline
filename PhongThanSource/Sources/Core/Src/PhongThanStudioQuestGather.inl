#ifdef _SERVER
#include "PhongThanStudioQuestGatherText.h"
namespace PtStudioGather {
struct Spawn {int slot,id;Spawn():slot(0),id(0){}};
struct Session {DWORD role,actor;int quest,node,slot,objectId,world,x,y,raw,deadline,nextSync;Session(){memset(this,0,sizeof(*this));}};
static std::map<int,Spawn> spawned;static Session sessions[MAX_PLAYER];
static PtStudioQuest::Node* Node(int id){for(unsigned i=0;i<PtStudioQuest::nodes.size();++i)if(PtStudioQuest::nodes[i].id==id)return &PtStudioQuest::nodes[i];return 0;}
static void Script(int id,char* path){sprintf(path,"\\script\\phongthan\\studio_quests\\node_%d.lua",id);}
static bool ObjectLive(int node){
 PtStudioQuest::Node* n=Node(node);std::map<int,Spawn>::iterator found=spawned.find(node);if(!n||found==spawned.end())return false;
 int slot=found->second.slot;if(slot<=0||slot>=MAX_OBJECT)return false;char script[160];Script(node,script);
 return Object[slot].m_nID==found->second.id&&Object[slot].m_nDataID==n->type&&Object[slot].m_dwScriptID==g_FileName2Id(script);
}
static PtStudioQuest::Step* Step(int p,int quest,int node){
 if(!PtStudioQuest::Live(p))return 0;PtStudioQuest::Quest* q=PtStudioQuest::Find(quest);if(!q||!(q->classes&PtStudioQuest::Class(p)))return 0;
 int stage=PtStudioQuest::Status(p,*q);if(stage<1||stage>(int)q->steps.size())return 0;PtStudioQuest::Step& s=q->steps[stage-1];return s.kind==4&&s.node==node?&s:0;
}
static void Clear(int p){
 Session& s=sessions[p];if(PtStudioQuest::Live(p)&&s.role==Player[p].m_dwID&&s.actor==Npc[Player[p].m_nIndex].m_dwID){
  char script[160];Script(s.node,script);
  if(Player[p].m_dwTaskExcuteScriptId==g_FileName2Id(script)&&!strcmp(Player[p].m_szTaskExcuteFun,"gather_finish")){
   Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=0;Player[p].m_szTaskExcuteFun[0]=0;Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);
  }
 }s=Session();
}
static bool Valid(int p){
 if(!PtStudioQuest::Live(p))return false;Session& s=sessions[p];int a=Player[p].m_nIndex;
 PtStudioQuest::Quest* q=PtStudioQuest::Find(s.quest);if(!s.role||s.role!=Player[p].m_dwID||s.actor!=Npc[a].m_dwID||!q||PtStudioQuest::Raw(p,*q)!=s.raw||!Step(p,s.quest,s.node)||!ObjectLive(s.node))return false;
 if(spawned[s.node].slot!=s.slot||spawned[s.node].id!=s.objectId||Npc[a].m_SubWorldIndex!=s.world||Npc[a].m_CurrentLife<=0||Player[p].GetLockState()||Player[p].CheckTrading()||Object[s.slot].m_nState!=OBJ_PROP_STATE_DISPLAY)return false;
 char script[160];Script(s.node,script);if(Player[p].m_dwTaskExcuteScriptId!=g_FileName2Id(script)||strcmp(Player[p].m_szTaskExcuteFun,"gather_finish"))return false;
 int x,y;Npc[a].GetMpsPos(&x,&y);return x==s.x&&y==s.y;
}
static bool Keys(int p,const PtStudioQuest::Step& step,std::string& text){
 if(!PtStudioQuest::SealOps::Has(p,step.requiredSeal)){PtStudioQuest::Seal* required=PtStudioQuest::SealOps::Find(step.requiredSeal);text=required?std::string(PTSQ_SEAL_MISSING)+required->name:std::string(PTSQ_NOT_READY);return false;}
 for(unsigned i=0;i<step.keys.size();++i)if(PtStudioQuest::Owned(p,step.keys[i].row)<step.keys[i].row.count){char count[70];sprintf(count," (%d/%d)",PtStudioQuest::Owned(p,step.keys[i].row),step.keys[i].row.count);text=PTSQ_NEED+step.keys[i].name+count;return false;}return true;
}
static int Start(int p,int quest,int node,int object,std::string& text){
 PtStudioQuest::Step* step=Step(p,quest,node);PtStudioQuest::Node* n=Node(node);if(!step||!n||!ObjectLive(node)){text=PTSQ_NOT_READY;return 0;}
 if(Player[p].m_nPaceBarTime||Player[p].GetLockState()||Player[p].CheckTrading()||sessions[p].role){text=PTSQ_BUSY;return 0;}
 int a=Player[p].m_nIndex;if(Npc[a].m_CurrentLife<=0||object!=spawned[node].slot||Object[object].m_nSubWorldID!=Npc[a].m_SubWorldIndex||Object[object].m_nState!=OBJ_PROP_STATE_DISPLAY){text=PTSQ_NOT_READY;return 0;}
 int x,y,px,py;Npc[a].GetMpsPos(&x,&y);Object[object].GetMpsPos(&px,&py);__int64 dx=(__int64)x-px,dy=(__int64)y-py;
 if(dx*dx+dy*dy>192*192){text=PTSQ_TOO_FAR;return 0;}
 if(!Keys(p,*step,text))return 0;
 char script[160];Script(node,script);if(!g_GetScript(script)&&!ReLoadScript(script)){text=PTSQ_NOT_READY;return 0;}
 Session s;s.role=Player[p].m_dwID;s.actor=Npc[a].m_dwID;s.quest=quest;s.node=node;s.slot=object;s.objectId=Object[object].m_nID;s.world=Npc[a].m_SubWorldIndex;s.x=x;s.y=y;s.raw=PtStudioQuest::Raw(p,*PtStudioQuest::Find(quest));s.deadline=g_SubWorldSet.GetGameTime()+step->seconds*GAME_FPS;s.nextSync=g_SubWorldSet.GetGameTime()+3;sessions[p]=s;
 Player[p].m_dwTaskExcuteScriptId=g_FileName2Id(script);g_StrCpyLen(Player[p].m_szTaskExcuteFun,"gather_finish",sizeof(Player[p].m_szTaskExcuteFun));
 Player[p].m_nPaceBarTime=Player[p].m_nPaceBarTimeMax=step->seconds*GAME_FPS;Npc[a].SendSyncData(0,TRUE);text="";return 1;
}
static int Finish(int p,int node,std::string& text){
 if(!PtStudioQuest::Live(p)||sessions[p].node!=node||!Valid(p)){if(p>0&&p<MAX_PLAYER)Clear(p);text=PTSQ_CANCELLED;return 0;}
 Session s=sessions[p];if(g_SubWorldSet.GetGameTime()+1<s.deadline||Player[p].m_nPaceBarTime!=0){text=PTSQ_NOT_FINISHED;return 0;}
 PtStudioQuest::Quest& q=*PtStudioQuest::Find(s.quest);PtStudioQuest::Step& step=*Step(p,s.quest,node);PtStudioQuest::Node* n=Node(node);
 if(!n||step.needs.empty()||!Keys(p,step,text)){Clear(p);return 0;}
 PhongThanQuestExchangeRow reward=step.needs[0].row;reward.count=1;PhongThanQuestExchangeRow needs[16];int needCount=0;
 if(step.keyConsume)for(unsigned i=0;i<step.keys.size()&&needCount<16;++i)needs[needCount++]=step.keys[i].row;
 int progress=(s.raw&65535)+1,stage=PtStudioQuest::Status(p,q);if(progress>=step.count){stage=PtStudioQuest::Advance(p,q,stage);progress=0;}
 int result=PtStudioQuest::SealOps::Exchange(p,step.requiredSeal,step.consumeSeal!=0,PtStudioQuest::FirstTask+q.id-1,s.raw,PtStudioQuest::Pack(q,stage,progress),needs,needCount,&reward,1);
 if(result){Object[s.slot].m_nLifeTime=n->respawn*GAME_FPS;Object[s.slot].SetState(OBJ_PROP_STATE_HIDE);Player[p].Save();PtStudioQuest::Journal(p,q);text=PTSQ_RECEIVED;}else text=PTSQ_BAG_FULL;
 Clear(p);return result;
}
static void SpawnNodes(){
 for(unsigned i=0;i<PtStudioQuest::nodes.size();++i){PtStudioQuest::Node& node=PtStudioQuest::nodes[i];if(ObjectLive(node.id))continue;
  int world=g_SubWorldSet.SearchWorld(node.map);if(world<0)continue;char script[160];Script(node.id,script);if(!g_GetScript(script)&&!ReLoadScript(script))continue;
  KMapPos pos;ZeroMemory(&pos,sizeof(pos));pos.nSubWorld=world;SubWorld[world].Mps2Map(node.x,node.y,&pos.nRegion,&pos.nMapX,&pos.nMapY,&pos.nOffX,&pos.nOffY);if(pos.nRegion<0)continue;
  KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nMovieFlag=1;int object=ObjSet.Add(node.type,pos,info);
  if(object>0){Object[object].SetScriptFile(script);Object[object].m_nLifeTime=node.respawn*GAME_FPS;g_StrCpyLen(Object[object].m_szName,node.name.c_str(),sizeof(Object[object].m_szName));Spawn s;s.slot=object;s.id=Object[object].m_nID;spawned[node.id]=s;}
 }
}
}
void PhongThanStudioQuestNodesChanged(){
 for(int p=1;p<MAX_PLAYER;++p)if(PtStudioGather::sessions[p].role)PtStudioGather::Clear(p);
 for(std::map<int,PtStudioGather::Spawn>::iterator i=PtStudioGather::spawned.begin();i!=PtStudioGather::spawned.end();++i){int slot=i->second.slot;char script[160];PtStudioGather::Script(i->first,script);if(slot>0&&slot<MAX_OBJECT&&Object[slot].m_nID==i->second.id&&Object[slot].m_dwScriptID==g_FileName2Id(script))ObjSet.Remove(slot);}
 PtStudioGather::spawned.clear();
}
void PhongThanStudioQuestGatherDamaged(int actor){if(actor<=0||actor>=MAX_NPC)return;int p=Npc[actor].GetPlayerIdx();if(p>0&&p<MAX_PLAYER&&PtStudioGather::sessions[p].role)PtStudioGather::Clear(p);}
void PhongThanStudioQuestTick(){
 static DWORD last=0;DWORD now=GetTickCount();if(!last||now-last>=1000){last=now;PtStudioQuest::Load();PtStudioQuest::BossPulse();if(!PtStudioQuest::quests.empty())PtStudioGather::SpawnNodes();}
 for(int p=1;p<MAX_PLAYER;++p)if(PtStudioGather::sessions[p].role){
  if(!PtStudioGather::Valid(p)||g_SubWorldSet.GetGameTime()>PtStudioGather::sessions[p].deadline+GAME_FPS)PtStudioGather::Clear(p);
  else if(g_SubWorldSet.GetGameTime()>=PtStudioGather::sessions[p].nextSync){PtStudioGather::sessions[p].nextSync=g_SubWorldSet.GetGameTime()+3;Npc[Player[p].m_nIndex].SendSyncData(0,TRUE);}
 }
}
int LuaStudioQuestCanGather(Lua_State* L){int p=GetPlayerIndex(L);PtStudioQuest::Load();Lua_PushNumber(L,PtStudioGather::Step(p,(int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2))?1:0);return 1;}
int LuaStudioQuestGather(Lua_State* L){
 int p=GetPlayerIndex(L),result=0;std::string text=PTSQ_NOT_READY;PtStudioQuest::Load();PtStudioQuest::Operation operation;
 int node=(int)Lua_ValueToNumber(L,2),op=(int)Lua_ValueToNumber(L,3);
 if(p>0&&p<MAX_PLAYER){if(op==0)result=PtStudioGather::Start(p,(int)Lua_ValueToNumber(L,1),node,GetObjIndex(L),text);else if(op==1)result=PtStudioGather::Finish(p,node,text);}
 Lua_PushNumber(L,result);Lua_PushString(L,text.c_str());return 2;
}

#endif

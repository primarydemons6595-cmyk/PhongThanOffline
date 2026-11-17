// Authoritative, stationary Sieu Do circles. Never restore a circle from a
// saved expiry alone: the matching live NPC must exist in this world session.
namespace PtDailySoul {
enum {Phase=4388,QuestEnd=4391,CircleEnd=4395,Sequence=4490,Seconds=300,Radius=600,PlacementRadius=600};
struct Circle {
 DWORD owner,actor,entity;int npc,world;time_t expires;
 Circle():owner(0),actor(0),entity(0),npc(0),world(-1),expires(0){}
};
static Circle circles[MAX_PLAYER];
static bool Entity(const Circle& c){return c.npc>0&&c.npc<MAX_NPC&&Npc[c.npc].m_dwID==c.entity&&Npc[c.npc].m_Kind==kind_dialoger&&Npc[c.npc].m_NpcSettingIdx>=551&&Npc[c.npc].m_NpcSettingIdx<=554;}
static void ClearState(int p){
 if(!PtDaily::Live(p))return;
 PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);bool changed=false;
 for(int id=262;id<=265;++id)if(RemoveIBBuffFromStore(store,Player[p].m_nIndex,id))changed=true;
 if(changed)PersistPlayerIBBuffStore(p);
 if(Player[p].m_cTask.GetSaveVal(CircleEnd))Player[p].m_cTask.SetSaveVal(CircleEnd,0,TRUE);
}
static void Clear(int p){Circle& c=circles[p];if(Entity(c))PhongThanRemoveNpc(c.npc);
 if(!c.owner||c.owner==Player[p].m_dwID)ClearState(p);c=Circle();}
static bool Active(int p){
 if(p<=0||p>=MAX_PLAYER)return false;Circle& c=circles[p];
 if(!PtDaily::Live(p)||c.owner!=Player[p].m_dwID||c.actor!=Npc[Player[p].m_nIndex].m_dwID||
  Player[p].m_cTask.GetSaveVal(Phase)!=1||Player[p].m_cTask.GetSaveVal(QuestEnd)<time(NULL)||
  c.expires<=time(NULL)||!Entity(c)||Npc[c.npc].m_SubWorldIndex!=c.world||Npc[Player[p].m_nIndex].m_SubWorldIndex!=c.world){Clear(p);return false;}
 return true;
}
static int Create(int p,int variant,const char* name,int map,int targetX,int targetY){
 if(!PtDaily::Live(p)||variant<1||variant>4||!name||strlen(name)>=32||Player[p].GetLockState()||Player[p].CheckTrading()||
  Player[p].m_cTask.GetSaveVal(Phase)!=1||Player[p].m_cTask.GetSaveVal(QuestEnd)<=time(NULL)||Active(p))return 0;
 KNpc& actor=Npc[Player[p].m_nIndex];int x=0,y=0;actor.GetMpsPos(&x,&y);
 // Validate before spawning, taking an item, or publishing any buff.
 if(actor.m_SubWorldIndex<0||actor.m_SubWorldIndex>=MAX_SUBWORLD||SubWorld[actor.m_SubWorldIndex].m_SubWorldID!=map)return -1;
 __int64 dx=(__int64)x-targetX,dy=(__int64)y-targetY;
 if(dx*dx+dy*dy>(__int64)PlacementRadius*PlacementRadius)return -1;
 int n=PhongThanSpawnNpc(550+variant,1,actor.m_SubWorldIndex,x,y,FALSE);if(n<=0)return 0;
 Circle c;c.owner=Player[p].m_dwID;c.actor=actor.m_dwID;c.entity=Npc[n].m_dwID;c.npc=n;c.world=actor.m_SubWorldIndex;c.expires=time(NULL)+Seconds;
 Npc[n].m_Kind=kind_dialoger;Npc[n].m_AiMode=0;Npc[n].m_DeathScriptID=0;Npc[n].m_TimerScriptID=0;
 Npc[n].m_nNpcTimeout=g_SubWorldSet.GetGameTime()+Seconds*18;g_StrCpyLen(Npc[n].Name,name,sizeof(Npc[n].Name));
 PhongThanQuestExchangeRow need;ZeroMemory(&need,sizeof(need));PhongThanDecodeQuestItemTuple(6,1,187,0,0,0,need.item);need.count=1;
 // Circle lifetime and the visible buff are one native transaction.
 PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);
 if(!AddIBBuffToStore(store,Player[p].m_nIndex,261+variant,Seconds,0)){PhongThanRemoveNpc(n);return 0;}
 int serial=Player[p].m_cTask.GetSaveVal(Sequence);
 if(serial<0||serial>=1000000000||PhongThanQuestExchange(p,Sequence,serial,serial+1,&need,1,0,0,true)!=1){if(Entity(c))PhongThanRemoveNpc(n);ClearState(p);return 0;}
 circles[p]=c;PersistPlayerIBBuffStore(p);Player[p].m_cTask.SetSaveVal(CircleEnd,(int)c.expires,TRUE);
 // Publish the final stationary NPC kind and quest name together.
 Npc[n].SendSyncData(0,TRUE);return n;
}
static bool Contains(int p,int dead){
 if(!Active(p)||dead<=0||dead>=MAX_NPC)return false;Circle& c=circles[p];KNpc& n=Npc[dead];
 if(n.m_SubWorldIndex!=c.world||n.m_Kind!=kind_normal||n.m_CurrentLife>0)return false;
 int x,y,cx,cy;n.GetMpsPos(&x,&y);Npc[c.npc].GetMpsPos(&cx,&cy);int dx=x-cx,dy=y-cy;
 return dx>=-Radius&&dx<=Radius&&dy>=-Radius&&dy<=Radius&&dx*dx+dy*dy<=Radius*Radius;
}
static void Tick(){for(int p=1;p<MAX_PLAYER;++p){
 if(circles[p].npc)Active(p);
 else if(PtDaily::Live(p))ClearState(p); // remove persisted orphan icons after relog/world restart
}}
}
int LuaDailySoulCircle(Lua_State* L){
 int result=0;
 if(Lua_GetTopIndex(L)==5&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,3)&&Lua_IsNumber(L,4)&&Lua_IsNumber(L,5)){
  double v=Lua_ValueToNumber(L,1),map=Lua_ValueToNumber(L,3),x=Lua_ValueToNumber(L,4),y=Lua_ValueToNumber(L,5);
  if(v>=1&&v<=4&&v==(int)v&&map>=1&&map<=100000&&map==(int)map&&x>=0&&x<=10000000&&x==(int)x&&y>=0&&y<=10000000&&y==(int)y)
   result=PtDailySoul::Create(GetPlayerIndex(L),(int)v,Lua_ValueToString(L,2),(int)map,(int)x,(int)y);
 }
 Lua_PushNumber(L,result);return 1;
}

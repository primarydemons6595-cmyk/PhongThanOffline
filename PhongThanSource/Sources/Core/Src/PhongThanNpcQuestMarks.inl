#ifndef PHONGTHAN_NPC_QUEST_MARKS_INL
#define PHONGTHAN_NPC_QUEST_MARKS_INL
#include "../../../Headers/PhongThanNpcQuestMark.h"
#include <stdio.h>

#ifdef _SERVER
#include "../../../Headers/PhongThanServerPerformance.h"
// Read-only GetPlayerTaskState callbacks run in the requesting player's scope.
// In particular, do not use ExecuteScript: it replaces the dialog callback.
class PTQuestMarkLuaScope
{
    Lua_State* state; int top,refs[4],lastNpc; KPlayer& player;
    const char* Name(int i) { return i==0?SCRIPT_PLAYERINDEX:i==1?SCRIPT_PLAYERID:i==2?SCRIPT_SUBWORLDINDEX:"NpcIndex"; }
public:
    PTQuestMarkLuaScope(Lua_State* s,KPlayer& p,int npc):state(s),top(lua_gettop(s)),player(p),lastNpc(p.m_nLastNpcIndex)
    {
        for(int i=0;i<4;++i){lua_getglobal(state,Name(i));refs[i]=lua_ref(state,1);}
        const double values[4]={p.GetPlayerIndex(),p.m_dwID,Npc[p.m_nIndex].m_SubWorldIndex,npc};
        for(i=0;i<4;++i){lua_pushnumber(state,values[i]);lua_setglobal(state,Name(i));}
        player.m_nLastNpcIndex=npc;
    }
    ~PTQuestMarkLuaScope()
    {
        for(int i=0;i<4;++i){if(refs[i]==LUA_REFNIL)lua_pushnil(state);else lua_getref(state,refs[i]);
            lua_setglobal(state,Name(i));if(refs[i]>=0)lua_unref(state,refs[i]);}
        lua_settop(state,top);player.m_nLastNpcIndex=lastNpc;
    }
};
static int PhongThanReadNpcQuestMark(KPlayer& player,int npc)
{
    KLuaScript* script=(KLuaScript*)g_GetScript(Npc[npc].m_ActionScriptID);
    if(!script || !script->m_LuaState)return -1;
    Lua_State* state=script->m_LuaState;
    PTQuestMarkLuaScope scope(state,player,npc);
    lua_getglobal(state,"GetPlayerTaskState");
    bool exists=lua_type(state,lua_gettop(state))==LUA_TFUNCTION;
    Lua_Pop(state,1);
    if(!exists || !script->CallFunction("GetPlayerTaskState",2,""))return -1;
    if(!lua_isnumber(state,-2))return -1;
    int stage=(int)Lua_ValueToNumber(state,-2);
    int level=lua_isnumber(state,-1)?(int)Lua_ValueToNumber(state,-1):0;
    int variant=PhongThanQuestMarkVariant(stage,level,Npc[player.m_nIndex].m_Level);
    if(variant<0 || stage==2)return variant;
    // Explicit story families. Daily and side quests remain blue regardless
    // of the player's level; level difference never changes quest category.
    const char* path=Npc[npc].ActionScript;
    bool story=strstr(path,"\\giapsi\\") || strstr(path,"\\two_faction\\") || strstr(path,"\\tran_nguyen\\");
    Lua_Pop(state,2);
    lua_getglobal(state,"GetPlayerTaskCategory");
    bool category=lua_type(state,lua_gettop(state))==LUA_TFUNCTION;Lua_Pop(state,1);
    if(category && script->CallFunction("GetPlayerTaskCategory",1,"")) {
        if(lua_isnumber(state,-1))story=(int)Lua_ValueToNumber(state,-1)==1;
        Lua_Pop(state,1);
    } else {
        // Many original logical paths alias a restored story NPC script.
        // Consult the explicit story projection rather than its disk alias.
        char* markers[]={"PTGMarker","PT2Marker"};
        for(int family=0;family<2;++family){
            lua_getglobal(state,markers[family]);bool found=lua_type(state,lua_gettop(state))==LUA_TFUNCTION;Lua_Pop(state,1);
            if(found && script->CallFunction(markers[family],2,"")){
                if(lua_isnumber(state,-2) && (int)Lua_ValueToNumber(state,-2)>=stage)story=true;
                Lua_Pop(state,2);
            }
        }
    }
    return stage==1?(story?0:1):(story?3:4);
}
static void PhongThanNpcQuestMarksTick(KPlayer& player)
{
    // Quest markers are client presentation. Server agents use real NPC dialog
    // and task state directly; scanning every NPC for a nonexistent UI is waste.
    if(player.m_nNetConnectIdx<0)return;
    PTPerfScope profile(PTPERF_QUEST_MARKS);
    struct Sent {DWORD role,actor,revision,tick,sent,hash;int world;};
    static Sent sent[MAX_PLAYER];
    int slot=player.GetPlayerIndex();
    if(slot<=0 || slot>=MAX_PLAYER || player.m_nIndex<=0 || player.m_nIndex>=MAX_NPC)return;
    KNpc& owner=Npc[player.m_nIndex];
    if(owner.m_SubWorldIndex<0 || owner.m_SubWorldIndex>=MAX_SUBWORLD)return;
    DWORD now=GetTickCount();Sent& cache=sent[slot];
    if(cache.role==player.m_dwID && cache.actor==owner.m_dwID && cache.revision==player.m_dwPhongThanTaskRevision && cache.world==owner.m_SubWorldIndex && (DWORD)(now-cache.tick)<1000)return;
    bool newSession=cache.role!=player.m_dwID || cache.actor!=owner.m_dwID || cache.world!=owner.m_SubWorldIndex;
    cache.actor=owner.m_dwID;cache.revision=player.m_dwPhongThanTaskRevision;
    cache.role=player.m_dwID;cache.world=owner.m_SubWorldIndex;cache.tick=now;
    KPhongThanScriptAction action;memset(&action,0,sizeof(action));
    action.Operation=PHONGTHAN_SCRIPT_SHOW;action.View=UI_NOTEINFO;action.ServerOwned=1;
    action.NumberArgument=PT_NPC_QUEST_MARK_NOTE;action.MapId=SubWorld[owner.m_SubWorldIndex].m_SubWorldID;
    strcpy(action.Key,PT_NPC_QUEST_MARK_KEY);
    int px,py;owner.GetMpsPos(&px,&py);int count=0,used=0;
    for(int idx=NpcSet.GetNextIdx(0);idx>0 && count<PT_NPC_QUEST_MARK_MAX;idx=NpcSet.GetNextIdx(idx))
    {
        KNpc& npc=Npc[idx];
        if(npc.m_Kind!=kind_dialoger || npc.m_SubWorldIndex!=owner.m_SubWorldIndex || !npc.m_ActionScriptID || !npc.m_dwID)continue;
        int x,y;npc.GetMpsPos(&x,&y);
        // Whole-map projection: Tab must show available NPCs beyond the streamed neighborhood.
        int variant=PhongThanReadNpcQuestMark(player,idx);
        if(variant<0)continue;
        int n=_snprintf(action.Content+used,sizeof(action.Content)-used-1,"%u:%d:%d:%d;",npc.m_dwID,variant,x,y);
        if(n<=0 || used+n>=(int)sizeof(action.Content)-1)break;
        used+=n;++count;
    }
    action.Content[used]=0;action.ContentLength=used;
    DWORD hash=2166136261U;
    for(int i=0;i<used;++i)hash=(hash^(unsigned char)action.Content[i])*16777619U;
    if(!newSession && cache.hash==hash && (DWORD)(now-cache.sent)<3000)return;
    PHONGTHAN_U8 packet[sizeof(PHONGTHAN_UI_ACTION_HEADER)+PHONGTHAN_UI_MAX_KEY+PHONGTHAN_UI_MAX_CONTENT];
    unsigned int size=PhongThanBuildScriptPacket(action,packet,sizeof(packet));
    if(size && g_pServer && SUCCEEDED(g_pServer->SendData(player.m_nNetConnectIdx,packet,size)))
    {cache.hash=hash;cache.sent=now;}
}
#else
struct PTQuestMarkEntry {DWORD entity;int variant,x,y;};
static PTQuestMarkEntry s_PTQuestMarks[PT_NPC_QUEST_MARK_MAX];
static int s_PTQuestMarkCount=0;
static DWORD s_PTQuestMarkMap=0,s_PTQuestMarkTime=0;
static bool PhongThanReceiveNpcQuestMarks(const KPhongThanScriptAction& action)
{
    if(action.View!=UI_NOTEINFO || action.NumberArgument!=PT_NPC_QUEST_MARK_NOTE ||
       strcmp(action.Key,PT_NPC_QUEST_MARK_KEY))return false;
    if(!action.ServerOwned || !action.MapId || action.ContentLength<0 || action.ContentLength>=(int)sizeof(action.Content))return true;
    PTQuestMarkEntry parsed[PT_NPC_QUEST_MARK_MAX];int count=0,offset=0;
    while(offset<action.ContentLength)
    {
        unsigned int entity=0;int variant=-1,x=0,y=0,n=0;
        if(count>=PT_NPC_QUEST_MARK_MAX || sscanf(action.Content+offset,"%u:%d:%d:%d;%n",&entity,&variant,&x,&y,&n)!=4 ||
           n<=0 || offset+n>action.ContentLength || !entity || variant<0 || variant>4 || x<0 || y<0 || x>10000000 || y>10000000)return true;
        for(int i=0;i<count;++i)if(parsed[i].entity==entity)return true;
        parsed[count].entity=entity;parsed[count].variant=variant;parsed[count].x=x;parsed[count++].y=y;offset+=n;
    }
    memcpy(s_PTQuestMarks,parsed,count*sizeof(parsed[0]));s_PTQuestMarkCount=count;
    s_PTQuestMarkMap=action.MapId;s_PTQuestMarkTime=GetTickCount();return true;
}
int PhongThanGetMapQuestMark(int slot,int map,int& x,int& y) {
    if((DWORD)map!=s_PTQuestMarkMap || slot<0 || slot>=s_PTQuestMarkCount ||
       (DWORD)(GetTickCount()-s_PTQuestMarkTime)>6000)return -1;
    x=s_PTQuestMarks[slot].x;y=s_PTQuestMarks[slot].y;return s_PTQuestMarks[slot].variant;
}
int PhongThanGetNpcQuestMark(DWORD entity,int map)
{
    if((DWORD)map!=s_PTQuestMarkMap || (DWORD)(GetTickCount()-s_PTQuestMarkTime)>6000)return -1;
    for(int i=0;i<s_PTQuestMarkCount;++i)if(s_PTQuestMarks[i].entity==entity)return s_PTQuestMarks[i].variant;
    return -1;
}
#endif
#endif

// Reuse the existing server-owned numeric dialog and its one-use wire token.
struct PHONGTHAN_EVENT_INPUT {
    DWORD playerId,npcId,scriptId,token,created;
    int mapId;
    char confirm[32],cancel[32];
};
static PHONGTHAN_EVENT_INPUT g_PhongThanEventInput[MAX_PLAYER];

static BOOL PhongThanEventCallback(const char *option,char *name)
{
    if(!option)return FALSE;
    const char *start=strrchr(option,'/');start=start?start+1:option;
    const int length=strlen(start);
    if(length<=0 || length>=32)return FALSE;
    for(int i=0;i<length;++i)
        if(!((start[i]>='A' && start[i]<='Z') || (start[i]>='a' && start[i]<='z') ||
            (i>0 && start[i]>='0' && start[i]<='9') || start[i]=='_'))return FALSE;
    strcpy(name,start);return TRUE;
}
int LuaInputDialogCompat(Lua_State *L)
{
    const int top=Lua_GetTopIndex(L);int type=0,result=0;
    if(top==4 && Lua_IsString(L,1) && PhongThanEventInt(L,2,2,2,&type) && Lua_IsString(L,3) && Lua_IsString(L,4))
    {
        PHONGTHAN_EVENT_INPUT pending;memset(&pending,0,sizeof(pending));
        if(PhongThanEventCallback(Lua_ValueToString(L,3),pending.confirm) && PhongThanEventCallback(Lua_ValueToString(L,4),pending.cancel))
        {
            const int player=GetPlayerIndex(L);Lua_SetTopIndex(L,top);
            if(PhongThanIsLivePlayer(player) && !Player[player].CheckTrading())
            {
                KPlayer &owner=Player[player];const int npc=owner.m_nIndex;
                const int world=Npc[npc].m_SubWorldIndex;
                const DWORD scripts[3]={owner.m_SelUiScriptId,Npc[npc].m_ActionScriptID,owner.m_TalkUiScriptId};
                for(int i=0;i<3 && !pending.scriptId;++i)
                {
                    KLuaScript *script=scripts[i]?(KLuaScript*)g_GetScript(scripts[i]):NULL;
                    if(script && script->m_LuaState==L)pending.scriptId=scripts[i];
                }
                if(pending.scriptId && world>=0 && world<MAX_SUBWORLD)
                {
                    pending.playerId=owner.m_dwID;pending.npcId=Npc[npc].m_dwID;
                    pending.mapId=SubWorld[world].m_SubWorldID;pending.created=GetTickCount();
                    KPhongThanScriptAction action;memset(&action,0,sizeof(action));
                    action.Operation=PHONGTHAN_SCRIPT_SHOW;action.View=UI_NUMBER_INPUT;
                    action.ServerOwned=1;action.NumberArgument=1000;
                    strcpy(action.Content,"So luong");action.ContentLength=strlen(action.Content);
                    owner.m_nAvailableAnswerNum=0;owner.m_bWaitingPlayerFeedBack=false;
                    owner.DoScriptAction(&action);
                    if(owner.m_UiDialogToken && owner.m_UiDialogView==PHONGTHAN_VIEW_NUMBER_INPUT)
                    {
                        pending.token=owner.m_UiDialogToken;g_PhongThanEventInput[player]=pending;result=1;
                    }
                }
            }
        }
    }
    Lua_PushNumber(L,result);return 1;
}

static bool PhongThanRouletteInput(KPlayer&,const PHONGTHAN_UI_NUMBER_REQUEST&);
bool PhongThanEventNumberInput(KPlayer &owner,const PHONGTHAN_UI_NUMBER_REQUEST &request)
{
    if(PhongThanRouletteInput(owner,request))return true;
    const int player=owner.GetPlayerIndex();
    if(player<=0 || player>=MAX_PLAYER)return false;
    PHONGTHAN_EVENT_INPUT &state=g_PhongThanEventInput[player];
    if(!state.token || state.token!=request.DialogToken)return false;
    if(owner.m_dwID!=state.playerId || !PhongThanIsLivePlayer(player) ||
        Npc[owner.m_nIndex].m_dwID!=state.npcId || owner.CheckTrading() ||
        owner.m_UiDialogToken!=state.token || owner.m_UiDialogView!=PHONGTHAN_VIEW_NUMBER_INPUT ||
        request.MapId!=(PHONGTHAN_U32)state.mapId ||
        Npc[owner.m_nIndex].m_SubWorldIndex<0 || Npc[owner.m_nIndex].m_SubWorldIndex>=MAX_SUBWORLD ||
        SubWorld[Npc[owner.m_nIndex].m_SubWorldIndex].m_SubWorldID!=state.mapId ||
        (DWORD)(GetTickCount()-state.created)>120000 || request.Value < -1 || request.Value>1000 || request.Value==0)
    {
        if(owner.m_UiDialogToken==state.token){owner.m_UiDialogToken=0;owner.m_UiDialogView=0;}
        memset(&state,0,sizeof(state));return true;
    }
    PHONGTHAN_EVENT_INPUT pending=state;
    memset(&state,0,sizeof(state));owner.m_UiDialogToken=0;owner.m_UiDialogView=0;
    // Consume before Lua; a subsequent input/menu opened by Lua gets a new token.
    owner.ExecuteScript(pending.scriptId,request.Value<0?pending.cancel:pending.confirm,request.Value<0?0:request.Value);
    return true;
}

// The original Lua chooses the result. The client animates it, then acknowledges
// this one-use token; a client response never supplies a prize or outcome.
static PHONGTHAN_EVENT_INPUT g_PhongThanRoulettePending[MAX_PLAYER];
static DWORD g_PhongThanRouletteBusy[MAX_PLAYER];
static Lua_State *g_PhongThanRouletteState[MAX_PLAYER];
static int g_PhongThanRouletteClaimed[MAX_PLAYER];
int LuaConsumeRouletteResultCompat(Lua_State *L)
{
    const int top=Lua_GetTopIndex(L);const int player=GetPlayerIndex(L);Lua_SetTopIndex(L,top);int result=0;
    if(PhongThanIsLivePlayer(player) && g_PhongThanRouletteBusy[player]==Player[player].m_dwID &&
        g_PhongThanRouletteState[player]==L && !g_PhongThanRouletteClaimed[player])
    {g_PhongThanRouletteClaimed[player]=1;result=1;}
    Lua_PushNumber(L,result);return 1;
}
int LuaRouletteCompat(Lua_State *L)
{
    const int top=Lua_GetTopIndex(L);int outcome=0,result=0;
    if(top==1 && PhongThanEventInt(L,1,0,11,&outcome))
    {
        const int player=GetPlayerIndex(L);Lua_SetTopIndex(L,top);
        if(PhongThanIsLivePlayer(player) && !g_PhongThanRouletteBusy[player])
        {
            KPlayer &owner=Player[player];
            PHONGTHAN_EVENT_INPUT pending;memset(&pending,0,sizeof(pending));
            const DWORD scripts[3]={owner.m_SelUiScriptId,Npc[owner.m_nIndex].m_ActionScriptID,owner.m_TalkUiScriptId};
            for(int i=0;i<3 && !pending.scriptId;++i)
            {
                KLuaScript *script=scripts[i]?(KLuaScript*)g_GetScript(scripts[i]):NULL;
                if(script && script->m_LuaState==L)pending.scriptId=scripts[i];
            }
            const int world=Npc[owner.m_nIndex].m_SubWorldIndex;
            if(pending.scriptId && world>=0 && world<MAX_SUBWORLD)
            {
                pending.playerId=owner.m_dwID;pending.npcId=Npc[owner.m_nIndex].m_dwID;
                pending.mapId=SubWorld[world].m_SubWorldID;pending.created=GetTickCount();
                KPhongThanScriptAction action;memset(&action,0,sizeof(action));
                action.Operation=PHONGTHAN_SCRIPT_SHOW;action.View=UI_ROULETTE;
                action.ServerOwned=1;action.NumberArgument=outcome;
                owner.m_nAvailableAnswerNum=0;owner.m_bWaitingPlayerFeedBack=false;
                owner.DoScriptAction(&action);
                if(owner.m_UiDialogToken && owner.m_UiDialogView==PHONGTHAN_VIEW_ROULETTE)
                {
                    pending.token=owner.m_UiDialogToken;g_PhongThanRoulettePending[player]=pending;result=1;
                }
            }
            Lua_SetTopIndex(L,top);
        }
    }
    Lua_PushNumber(L,result);return 1;
}

static bool PhongThanRouletteInput(KPlayer &owner,const PHONGTHAN_UI_NUMBER_REQUEST &request)
{
    const int player=owner.GetPlayerIndex();
    if(player<=0 || player>=MAX_PLAYER)return false;
    PHONGTHAN_EVENT_INPUT &pending=g_PhongThanRoulettePending[player];
    if(!pending.token || pending.token!=request.DialogToken)return false;
    if(owner.m_dwID!=pending.playerId || !PhongThanIsLivePlayer(player) ||
        Npc[owner.m_nIndex].m_dwID!=pending.npcId || owner.m_UiDialogToken!=pending.token ||
        owner.m_UiDialogView!=PHONGTHAN_VIEW_ROULETTE || request.MapId!=(PHONGTHAN_U32)pending.mapId ||
        Npc[owner.m_nIndex].m_SubWorldIndex<0 || Npc[owner.m_nIndex].m_SubWorldIndex>=MAX_SUBWORLD ||
        SubWorld[Npc[owner.m_nIndex].m_SubWorldIndex].m_SubWorldID!=pending.mapId ||
        request.Value!=1)
        return true; // Keep persistent pending spin for the NPC's resume menu.
    const DWORD scriptId=pending.scriptId;
    KLuaScript *script=(KLuaScript*)g_GetScript(scriptId);
    if(!script)return true;
    memset(&pending,0,sizeof(pending));owner.m_UiDialogToken=0;owner.m_UiDialogView=0;
    g_PhongThanRouletteBusy[player]=owner.m_dwID;
    g_PhongThanRouletteState[player]=script->m_LuaState;g_PhongThanRouletteClaimed[player]=0;
    owner.ExecuteScript(scriptId,"Finished",0);
    g_PhongThanRouletteBusy[player]=0;g_PhongThanRouletteState[player]=NULL;g_PhongThanRouletteClaimed[player]=0;
    return true;
}

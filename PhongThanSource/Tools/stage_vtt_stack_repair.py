import json,re,time
from restore_original_world import ROOT,OUT
import build_offline_loose as b
folder=ROOT/'analysis/vtt_open_fix_20261019';folder.mkdir(exist_ok=True)
if (folder/'publication.json').exists():raise ValueError('Already published: preserve journal and do not re-stage the recovery round')
server=ROOT/'PhongThanRuntime-Loose/Server'
p=ROOT/'PhongThanSource/Sources/Core/Src/ScriptFuns.cpp';data=p.read_bytes();start=data.index(b'int LuaInitMission(');end=data.index(b'int LuaRunMission(',start)
function=b'''int LuaInitMission(Lua_State* L)
{
    if (Lua_GetTopIndex(L)<1) return 0;
    const int id=(int)Lua_ValueToNumber(L,1);
    const int world=GetSubWorldIndex(L);
    if (id<0 || world<0 || world>=MAX_SUBWORLD) return 0;
    if (SubWorld[world].m_MissionArray.FindByMissionId(id)) return 0;
    char path[MAX_PATH];
    PhongThanMissionScriptPath(path,id,SubWorld[world].m_SubWorldID);
    KLuaScript* script=(KLuaScript*)g_GetScript(path);
    const bool vtt=SubWorld[world].m_SubWorldID>=1067 && SubWorld[world].m_SubWorldID<=1070 && id==SubWorld[world].m_SubWorldID-1066;
    if (!script || !script->m_LuaState) {
        if (vtt) { FILE* f=fopen("vtt_mission_lifecycle.log","a"); if(f){fprintf(f,"INIT_FAILED utc=%ld map=%d mission=%d reason=SCRIPT_MISSING path=%s\\n",(long)time(NULL),SubWorld[world].m_SubWorldID,id,path);fclose(f);} }
        return 0;
    }
    KMission* mission=SubWorld[world].m_MissionArray.Add();
    if (!mission) return 0;
    mission->m_MissionPlayer.Clear();mission->m_MissionNpc.Clear();mission->SetMissionId(id);
    if (Lua_GetTopIndex(L)>4) {
        int parameters[MAX_GLBMISSION_PARAM];
        for(int i=0;i<MAX_GLBMISSION_PARAM;++i)parameters[i]=(int)Lua_ValueToNumber(L,4+i);
        mission->SetMissionLadder((char*)Lua_ValueToString(L,2),(int)Lua_ValueToNumber(L,3),parameters);
    }
    Lua_PushNumber(script->m_LuaState,world);script->SetGlobalName(SCRIPT_SUBWORLDINDEX);
    int top=0;script->SafeCallBegin(&top);
    const BOOL ok=script->CallFunction("InitMission",0,"d",id);
    script->SafeCallEnd(top);
    if (!ok) {mission->StopMission();SubWorld[world].m_MissionArray.Remove(mission);}
    if(vtt){FILE* f=fopen("vtt_mission_lifecycle.log","a");if(f){fprintf(f,"INIT utc=%ld map=%d mission=%d call=%s state=%d registered=%d\\n",(long)time(NULL),SubWorld[world].m_SubWorldID,id,ok?"PASS":"FAIL",g_TaskGlobalValue[id],SubWorld[world].m_MissionArray.FindByMissionId(id)?1:0);fclose(f);}}
    return 0;
}

'''
data=data[:start]+function+data[end:];p.write_bytes(data)
# A bounded recovery round lets the user test now rather than wait for the next
# daily opening. The ordinary schedule remains unchanged. Expired code is inert.
relative='script/servertimer.lua';payload=(server/relative).read_bytes();expires=int(time.time())+900
payload+=f'''
-- One recovery round for the entry failure reported 2026-10-19; expires UTC {expires}.
pt_vtt_before_recovery=main
pt_vtt_recovery_done=0
function main()
    pt_vtt_before_recovery()
    if pt_vtt_recovery_done==1 or SystemTime()>{expires} then return end
    pt_vtt_recovery_done=1
    local previous=SubWorld
    for id=1,4 do
        SubWorld=SubWorldID2Idx(66+id)
        if SubWorld>=0 and IsMission(id)==0 then OpenMission(id) end
    end
    SubWorld=previous
end
'''.encode('ascii')
stage=folder/'staged/Server'/relative;stage.parent.mkdir(parents=True,exist_ok=True);stage.write_bytes(payload)
plan=dict(ready=[],blocked=[],files=[dict(relative=relative,sha256=b.sha(payload),before_sha256=b.sha((server/relative).read_bytes()))],binaries=[dict(relative='CoreServer.dll',sha256=b.sha((ROOT/'PhongThanSource/Output/Server/CoreServer.dll').read_bytes()),before_sha256=b.sha((server/'CoreServer.dll').read_bytes()))],recovery_round_expires_utc=expires)
(folder/'plan.json').write_text(json.dumps(plan,indent=2),'utf8')
print('STAGED bounded recovery round; native scalar mission lookup; lifecycle diagnostics. Re-run after final build to refresh binary hash.')

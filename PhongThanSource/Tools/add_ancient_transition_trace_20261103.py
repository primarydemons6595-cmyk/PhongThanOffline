from pathlib import Path
r=Path(__file__).resolve().parents[1]/'Sources/Core/Src'
p=r/'KNpc.cpp';d=p.read_bytes();a=b'int KNpc::ChangeWorld(DWORD dwSubWorldID, int nX, int nY)\r\n{';assert d.count(a)==1
code='''
    // Human-only evidence for unexpected Ancient battlefield transitions.
    // Record the requesting role and issued script; no bot/player data edits.
    if(IsPlayer() && m_nPlayerIdx>0 && m_nPlayerIdx<MAX_PLAYER && !Player[m_nPlayerIdx].m_bIsBot && dwSubWorldID==1064){
        WIN32_FILE_ATTRIBUTE_DATA info;if(GetFileAttributesExA("ancient_transition_trace.log",GetFileExInfoStandard,&info)&&info.nFileSizeLow>4194304){DeleteFileA("ancient_transition_trace.previous.log");MoveFileA("ancient_transition_trace.log","ancient_transition_trace.previous.log");}
        FILE* trace=fopen("ancient_transition_trace.log","a");if(trace){fprintf(trace,"utc=%lu player=%d role=%lu entity=%lu from=%d requested=%lu x=%d y=%d action_script=%lu select_script=%lu talk_script=%lu last_npc=%d doing=%d life=%d\\n",(unsigned long)time(NULL),m_nPlayerIdx,Player[m_nPlayerIdx].m_dwID,m_dwID,m_SubWorldIndex>=0?SubWorld[m_SubWorldIndex].m_SubWorldID:0,dwSubWorldID,nX,nY,m_ActionScriptID,Player[m_nPlayerIdx].m_SelUiScriptId,Player[m_nPlayerIdx].m_TalkUiScriptId,Player[m_nPlayerIdx].m_nLastNpcIndex,m_Doing,m_CurrentLife);fclose(trace);}
    }
'''.replace('\n','\r\n').encode();d=d.replace(a,a+code);p.write_bytes(d)
p=r/'PhongThanPlayerDialog.inl';d=p.read_bytes();a=b'const BOOL result = ExecuteScript(scriptId, callback, selection);';assert d.count(a)==1
code='''if(!m_bIsBot){
            KLuaScript* origin=(KLuaScript*)g_GetScript(scriptId);
            if(origin && (strstr(origin->m_szScriptName,"super") || strstr(origin->m_szScriptName,"giapsi") || strstr(origin->m_szScriptName,"thuong_chu"))){FILE* t=fopen("ancient_transition_trace.log","a");if(t){fprintf(t,"utc=%lu player=%d role=%lu selected=%d script=%lu path=%s callback=%s\\n",(unsigned long)time(NULL),m_nPlayerIndex,m_dwID,selection,scriptId,origin->m_szScriptName,callback);fclose(t);}}
        }
        '''.replace('\n','\r\n').encode();d=d.replace(a,code+a);p.write_bytes(d)
print('Added human transition evidence without blocking legitimate Ancient quests')

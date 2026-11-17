void CClientNode::StudioAdminTick(){
    static DWORD last=0;DWORD now=GetTickCount();if((DWORD)(now-last)<500)return;last=now;
    WIN32_FIND_DATAA entry;HANDLE find=FindFirstFileA("CharacterAdmin\\*.ready",&entry);if(find==INVALID_HANDLE_VALUE)return;
    do{
        char id[33];if(strlen(entry.cFileName)!=38||strcmp(entry.cFileName+32,".ready"))continue;
        bool valid=true;for(int i=0;i<32;++i){char c=entry.cFileName[i];if(!((c>='0'&&c<='9')||(c>='a'&&c<='f')))valid=false;id[i]=c;}id[32]=0;if(!valid)continue;
        char base[MAX_PATH],next[MAX_PATH],ready[MAX_PATH],done[MAX_PATH],response[MAX_PATH];
        sprintf(base,"CharacterAdmin\\%s\\base.pthc",id);sprintf(next,"CharacterAdmin\\%s\\next.pthc",id);
        sprintf(ready,"CharacterAdmin\\%s.ready",id);sprintf(done,"CharacterAdmin\\%s.done",id);sprintf(response,"CharacterAdmin\\%s.result",id);
        // Claim once before executing; a crash leaves a visible pending job, never an automatic replay.
        if(!MoveFileA(ready,done))continue;
        int result=0;FILE* f=fopen(base,"rb");PHONGTHAN_CHARACTER_STATE_HEADER state;ZeroMemory(&state,sizeof(state));
        if(f){if(fseek(f,16,SEEK_SET)==0&&fread(&state,1,sizeof(state),f)==sizeof(state)&&memchr(state.RoleName,0,32)&&state.RoleName[0]){
            CCriticalSection::Owner roleLock(m_csCR);bool locked=false;
            for(stdRoleLockMap::iterator it=m_csRoleLock.begin();it!=m_csRoleLock.end();++it)if(!_stricmp(it->first.c_str(),(const char*)state.RoleName)&&it->second!=(size_t)-1)locked=true;
            result=locked?-1:g_PhongThanCharacterStore.AdminReplace(base,next);
        }fclose(f);}
        char temporary[MAX_PATH];sprintf(temporary,"%s.tmp",response);
        f=fopen(temporary,"wb");if(f){fprintf(f,"%d\n",result);fflush(f);fclose(f);MoveFileExA(temporary,response,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);}
    }while(FindNextFileA(find,&entry));FindClose(find);
}

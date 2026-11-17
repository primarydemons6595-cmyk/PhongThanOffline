// Local Studio jobs are serialized with normal role load/save operations.
// The character must be logged out; the world server may remain running.
int CPhongThanCharacterStore::AdminReplace(const char* basePath,const char* nextPath){
    EnterCriticalSection(&m_Lock);
    std::vector<BYTE> base(PHONGTHAN_CHARACTER_MAX_STATE_SIZE),next(PHONGTHAN_CHARACTER_MAX_STATE_SIZE),current(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);
    PHONGTHAN_U32 bs=0,ns=0,cs=0;int result=0;
    if(LoadPathUnlocked(basePath,&base[0],base.size(),&bs)&&LoadPathUnlocked(nextPath,&next[0],next.size(),&ns)){
        PHONGTHAN_CHARACTER_STATE_HEADER* b=(PHONGTHAN_CHARACTER_STATE_HEADER*)&base[0];
        PHONGTHAN_CHARACTER_STATE_HEADER* n=(PHONGTHAN_CHARACTER_STATE_HEADER*)&next[0];
        if(!_stricmp((const char*)b->RoleName,(const char*)n->RoleName)&&!_stricmp((const char*)b->AccountName,(const char*)n->AccountName)&&
           n->Revision==b->Revision+1&&LoadUnlocked((const char*)b->RoleName,&current[0],current.size(),&cs)&&cs==bs&&!memcmp(&current[0],&base[0],bs)){
            char canonical[MAX_PATH],backup[MAX_PATH];
            if(BuildRolePath((const char*)b->RoleName,canonical,sizeof(canonical))){
                _snprintf(backup,sizeof(backup),"%s.studio.bak",canonical);backup[sizeof(backup)-1]=0;
                PtCharacterBundle::Bundle imported;std::string error;
                if(PtCharacterBundle::Read(nextPath,imported)&&(!imported.unified||(!imported.magic.empty()&&PtCharacterBundle::StagePath(canonical,n,imported.magic,error)))&&CopyFileA(canonical,backup,FALSE)&&WriteUnlocked(n,ns,true))result=1;
            }
        }else result=-2;
    }
    LeaveCriticalSection(&m_Lock);return result;
}

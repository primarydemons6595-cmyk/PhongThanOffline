// Two-owner purification uses durable per-role intents and one commit record.
// Before commit neither changes; after commit either owner can finish replay.
struct PT_PET_PAIR_INTENT {
 DWORD Magic,Side;char Token[32],Account[32],Role[32];
 PT_PET_DATA Data;DWORD Checksum;
};
struct PT_PET_PAIR_COMMIT {DWORD Magic,Checksums[2];};
static bool PtPetWriteDurable(const std::string& path,const void* bytes,DWORD size){
 std::string temp=path+".tmp";
 HANDLE h=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
 if(h==INVALID_HANDLE_VALUE)return false;
 DWORD wrote=0;bool ok=WriteFile(h,bytes,size,&wrote,0)&&wrote==size&&FlushFileBuffers(h);CloseHandle(h);
 if(ok)ok=MoveFileExA(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temp.c_str());return ok;
}
static bool PtPetReadExact(const std::string& path,void* bytes,DWORD size){
 HANDLE h=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(h==INVALID_HANDLE_VALUE)return false;
 DWORD got=0;bool ok=GetFileSize(h,0)==size&&ReadFile(h,bytes,size,&got,0)&&got==size;CloseHandle(h);return ok;
}
static bool PtPetRecoverPair(int player,bool* changed){
 if(changed)*changed=false;
 std::string path;if(!PtPetPath(player,path))return false;path+=".pair";
 DWORD attributes=GetFileAttributesA(path.c_str());
 if(attributes==((DWORD)-1))return GetLastError()==ERROR_FILE_NOT_FOUND;
 PT_PET_PAIR_INTENT intent;
 if(!PtPetReadExact(path,&intent,sizeof(intent))||intent.Magic!=0x31525050||intent.Side>1||
    intent.Token[31]||intent.Account[31]||intent.Role[31]||
    _stricmp(intent.Account,Player[player].AccountName)||_stricmp(intent.Role,Player[player].Name)||
    !PtPetValid(intent.Data)||intent.Checksum!=PtPetHash(&intent,sizeof(intent)-4))return false;
 if(strlen(intent.Token)!=24)return false;
 for(int i=0;i<24;++i)if(!((intent.Token[i]>='0'&&intent.Token[i]<='9')||(intent.Token[i]>='a'&&intent.Token[i]<='f')))return false;
 std::string commitPath=std::string("CharacterStore\\PetPairCommits\\")+intent.Token+".commit";
 PT_PET_PAIR_COMMIT commit;
 if(GetFileAttributesA(commitPath.c_str())==((DWORD)-1)){
  if(GetLastError()!=ERROR_FILE_NOT_FOUND)return false;
  // No commit exists: this transaction never changed either pet.
  return DeleteFileA(path.c_str())!=0;
 }
 if(!PtPetReadExact(commitPath,&commit,sizeof(commit))||commit.Magic!=0x31435050||
    commit.Checksums[intent.Side]!=intent.Checksum)return false;
 if(!PtPetSave(player,intent.Data)||!DeleteFileA(path.c_str()))return false;
 if(changed)*changed=true;return true;
}
static bool PtPetCommitPair(int first,int second,const PT_PET_DATA& a,const PT_PET_DATA& b){
 std::string paths[2];if(!PtPetPath(first,paths[0])||!PtPetPath(second,paths[1])||paths[0]==paths[1])return false;
 if(!PtPetValid(a)||!PtPetValid(b))return false;
 if(!CreateDirectoryA("CharacterStore\\PetPairCommits",0)&&GetLastError()!=ERROR_ALREADY_EXISTS)return false;
 static DWORD sequence=0;char token[32];
 // CREATE_NEW reserves a unique transaction token, including after a restart.
 std::string reserved;
 HANDLE reservation=INVALID_HANDLE_VALUE;
 for(int attempt=0;attempt<100&&reservation==INVALID_HANDLE_VALUE;++attempt){
  sprintf(token,"%08x%08x%08x",(DWORD)time(0),GetCurrentProcessId(),++sequence);
  reserved=std::string("CharacterStore\\PetPairCommits\\")+token+".reserve";
  reservation=CreateFileA(reserved.c_str(),GENERIC_WRITE,0,0,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,0);
 }
 if(reservation==INVALID_HANDLE_VALUE)return false;CloseHandle(reservation);
 PT_PET_PAIR_INTENT intents[2];ZeroMemory(intents,sizeof(intents));int players[2]={first,second};
 for(int i=0;i<2;++i){
  intents[i].Magic=0x31525050;intents[i].Side=i;lstrcpyn(intents[i].Token,token,32);
  lstrcpyn(intents[i].Account,Player[players[i]].AccountName,32);lstrcpyn(intents[i].Role,Player[players[i]].Name,32);
  intents[i].Data=i?b:a;intents[i].Checksum=PtPetHash(&intents[i],sizeof(intents[i])-4);
  if(!PtPetWriteDurable(paths[i]+".pair",&intents[i],sizeof(intents[i])))return false;
 }
 PT_PET_PAIR_COMMIT commit;commit.Magic=0x31435050;
 commit.Checksums[0]=intents[0].Checksum;commit.Checksums[1]=intents[1].Checksum;
 std::string commitPath=std::string("CharacterStore\\PetPairCommits\\")+token+".commit";
 if(!PtPetWriteDurable(commitPath,&commit,sizeof(commit)))return false;
 // The commit is final. On write failure the next pet load replays its intent.
 PtPetRecoverPair(first,0);PtPetRecoverPair(second,0);
 ptPets[first].Loaded=ptPets[second].Loaded=0;
 return true;
}

// Bounded companion snapshots. Canonical .pthc and item/wire schemas stay unchanged.
#ifndef PHONGTHAN_MAGIC_RETENTION_H
#define PHONGTHAN_MAGIC_RETENTION_H
#include <windows.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "PhongThanCharacter.h"
#include "PhongThanAccountStorage.h"
namespace PtMagicRetention {
inline bool Sha256(const std::string& bytes,std::string& hex){
    hex="";HMODULE lib=LoadLibraryA("advapi32.dll");if(!lib)return false;
    typedef BOOL (WINAPI *Acquire)(DWORD*,LPCSTR,LPCSTR,DWORD,DWORD);
    typedef BOOL (WINAPI *Create)(DWORD,DWORD,DWORD,DWORD,DWORD*);
    typedef BOOL (WINAPI *Hash)(DWORD,const BYTE*,DWORD,DWORD);
    typedef BOOL (WINAPI *Get)(DWORD,DWORD,BYTE*,DWORD*,DWORD);
    typedef BOOL (WINAPI *Destroy)(DWORD);typedef BOOL (WINAPI *Release)(DWORD,DWORD);
    Acquire acquire=(Acquire)GetProcAddress(lib,"CryptAcquireContextA");Create create=(Create)GetProcAddress(lib,"CryptCreateHash");
    Hash hash=(Hash)GetProcAddress(lib,"CryptHashData");Get get=(Get)GetProcAddress(lib,"CryptGetHashParam");
    Destroy destroy=(Destroy)GetProcAddress(lib,"CryptDestroyHash");Release release=(Release)GetProcAddress(lib,"CryptReleaseContext");
    DWORD provider=0;DWORD digest=0;BYTE output[32];DWORD size=sizeof(output);bool ok=false;
    if(acquire&&create&&hash&&get&&destroy&&release&&acquire(&provider,0,0,24,0xf0000000UL)){
        if(create(provider,0x800c,0,0,&digest)){
            ok=hash(digest,(const BYTE*)bytes.c_str(),bytes.size(),0)&&get(digest,2,output,&size,0)&&size==32;
            destroy(digest);
        }release(provider,0);
    }
    FreeLibrary(lib);if(!ok)return false;char b[3];for(int i=0;i<32;++i){sprintf(b,"%02x",output[i]);hex+=b;}return true;
}
enum {HistoryLimit=3,PendingLimit=8};
#pragma pack(push,1)
struct Ref{char key[64];DWORD revision;};
struct Journal{DWORD magic,version,committed,histories,pendings;char role[64];Ref history[HistoryLimit],pending[PendingLimit];DWORD checksum;};
#pragma pack(pop)
inline DWORD Hash(const void* bytes,size_t size){const BYTE* p=(const BYTE*)bytes;DWORD h=2166136261UL;for(size_t i=0;i<size;++i)h=(h^p[i])*16777619UL;return h;}
inline bool Key(const char* key){for(int i=0;i<64;++i)if(!((key[i]>='0'&&key[i]<='9')||(key[i]>='a'&&key[i]<='f')))return false;return true;}
inline std::string Text(const Ref& r){return std::string(r.key,64);}
inline bool Role(const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& role){
 std::string names((const char*)state->RoleName);names+='\0';names+=(const char*)state->AccountName;
 for(size_t i=0;i<names.size();++i)if(names[i]>='A'&&names[i]<='Z')names[i]+='a'-'A';return Sha256(names,role);
}
inline bool StateKey(const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& key){
 return state&&PhongThanValidateCharacterState(state,state->StateSize)&&Sha256(std::string((const char*)state,state->StateSize),key);
}
inline bool Regular(const std::string& path){DWORD a=GetFileAttributesA(path.c_str());return a!=(DWORD)-1&&!(a&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT));}
inline bool Read(const std::string& path,std::string& bytes,DWORD limit=1024*1024){
 bytes="";if(!Regular(path))return false;HANDLE f=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);if(f==INVALID_HANDLE_VALUE)return false;
 DWORD size=GetFileSize(f,0),got=0;bool ok=size!=INVALID_FILE_SIZE&&size<=limit;
 if(ok){bytes.resize(size);ok=ReadFile(f,size?&bytes[0]:0,size,&got,0)&&got==size;}CloseHandle(f);return ok;
}
inline bool Atomic(const std::string& path,const std::string& bytes){
 char suffix[64];sprintf(suffix,".tmp.%lu.%lu",GetCurrentProcessId(),GetCurrentThreadId());std::string temp=path+suffix;
 HANDLE f=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
 DWORD got=0;bool ok=WriteFile(f,bytes.c_str(),bytes.size(),&got,0)&&got==bytes.size()&&FlushFileBuffers(f);CloseHandle(f);
 if(ok)ok=MoveFileExA(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temp.c_str());return ok;
}
class Guard{HANDLE mutex;bool held;public:Guard(const std::string& role):held(false){std::string name="Local\\PhongThanMagicRetention-"+role;mutex=CreateMutexA(0,FALSE,name.c_str());if(mutex){DWORD r=WaitForSingleObject(mutex,5000);held=r==WAIT_OBJECT_0||r==WAIT_ABANDONED;}}~Guard(){if(held)ReleaseMutex(mutex);if(mutex)CloseHandle(mutex);}bool Good(){return held;}};
inline bool ReadJournal(const std::string& path,const std::string& role,Journal& j){
 ZeroMemory(&j,sizeof(j));j.magic=0x3152474d;j.version=1;memcpy(j.role,role.c_str(),64);
 if(GetFileAttributesA(path.c_str())==(DWORD)-1)return GetLastError()==ERROR_FILE_NOT_FOUND||GetLastError()==ERROR_PATH_NOT_FOUND;
 std::string bytes;if(!Read(path,bytes,sizeof(j))||bytes.size()!=sizeof(j))return false;memcpy(&j,bytes.c_str(),sizeof(j));
 if(j.magic!=0x3152474d||j.version!=1||memcmp(j.role,role.c_str(),64)||j.histories>HistoryLimit||j.pendings>PendingLimit||j.checksum!=Hash(&j,sizeof(j)-4))return false;
 for(DWORD i=0;i<j.histories;++i)if(!Key(j.history[i].key)||!j.history[i].revision)return false;
 for(i=0;i<j.pendings;++i)if(!Key(j.pending[i].key)||!j.pending[i].revision)return false;return true;
}
inline bool WriteJournal(const std::string& path,Journal& j){j.checksum=Hash(&j,sizeof(j)-4);return Atomic(path,std::string((const char*)&j,sizeof(j)));}
inline bool Held(const Journal& j,const Ref& r){for(DWORD i=0;i<j.histories;++i)if(!memcmp(j.history[i].key,r.key,64))return true;return false;}
inline bool AddPending(Journal& j,const Ref& r){
 for(DWORD i=0;i<j.pendings;++i)if(!memcmp(j.pending[i].key,r.key,64))return true;
 if(j.pendings==PendingLimit)return false;j.pending[j.pendings++]=r;return true;
}
inline bool RemoveFile(const std::string& path){DWORD a=GetFileAttributesA(path.c_str());if(a==(DWORD)-1)return GetLastError()==ERROR_FILE_NOT_FOUND;
 return !(a&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))&&DeleteFileA(path.c_str());}
// Explicit canonical/Studio backup states remain usable. Inspect only the small
// account directory, never the hundreds of thousands of companion generations.
inline int Pinned(const std::string& root,const Ref& ref){
 size_t slash=root.find_last_of("\\/");if(slash==std::string::npos)return -1;std::string parent=root.substr(0,slash);
 WIN32_FIND_DATAA found;HANDLE search=FindFirstFileA((parent+"\\*.pthc*").c_str(),&found);
 if(search==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_NOT_FOUND?0:-1;
 int result=0;do{
  if(found.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))continue;
  if(found.nFileSizeHigh||found.nFileSizeLow<424||found.nFileSizeLow>65552)continue;
  std::string bytes,key;if(!Read(parent+"\\"+found.cFileName,bytes,65552)){result=-1;break;}
  if(bytes.size()<424||memcmp(bytes.c_str(),"PHTC",4))continue;
  const DWORD* h=(const DWORD*)bytes.c_str();const PHONGTHAN_CHARACTER_STATE_HEADER* state=(const PHONGTHAN_CHARACTER_STATE_HEADER*)(bytes.c_str()+16);
  if(h[2]!=bytes.size()-16||h[3]!=Hash(bytes.c_str()+16,bytes.size()-16)||!StateKey(state,key)){result=-1;break;}
  if(!memcmp(key.c_str(),ref.key,64)){result=1;break;}
 }while(FindNextFileA(search,&found));FindClose(search);return result;
}
inline void Collect(const std::string& root,Journal& j){
 DWORD n=0;for(DWORD i=0;i<j.pendings;++i){Ref r=j.pending[i];
  if(Held(j,r))continue;
  if(r.revision>j.committed){j.pending[n++]=r;continue;}
  int pinned=Pinned(root,r);if(pinned>0)continue;if(pinned<0){j.pending[n++]=r;continue;}
  std::string base=root+"\\"+Text(r);
  // Leave failed removals recorded for retry; never scan the account directory.
  bool a=RemoveFile(base+".magic-v1"),b=RemoveFile(base+".required");if(!a||!b)j.pending[n++]=r;
 }j.pendings=n;for(;n<PendingLimit;++n)ZeroMemory(&j.pending[n],sizeof(Ref));
}
inline bool Pair(const std::string& root,const PHONGTHAN_CHARACTER_STATE_HEADER* state,const std::string& key){
 std::string bytes,required,digest;if(!Read(root+"\\"+key+".magic-v1",bytes)||bytes.size()<144||!Read(root+"\\"+key+".required",required,64)||required.size()!=64||!Sha256(bytes,digest)||required!=digest)return false;
 const DWORD* h=(const DWORD*)bytes.c_str();if(h[0]!=0x474d3050||(h[1]!=1&&h[1]!=2)||h[2]!=748||h[3]!=state->ItemCount||bytes.size()!=144+h[2]*h[3]||memcmp(bytes.data()+16,key.c_str(),64))return false;
 return Sha256(bytes.substr(144),digest)&&!memcmp(bytes.data()+80,digest.c_str(),64);
}
inline bool Begin(const std::string& root,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& error){
 std::string key,role;if(!StateKey(state,key)||!Role(state,role)||!state->Revision){error="MAGIC_RETENTION_INVALID_STATE";return false;}
 Guard lock(role);if(!lock.Good()){error="MAGIC_RETENTION_BUSY";return false;}
 std::string path=root+"\\role-"+role+".retention-v1";Journal j;
 if(!ReadJournal(path,role,j)){error="MAGIC_RETENTION_JOURNAL_CORRUPT";return false;}
 Ref r;memcpy(r.key,key.c_str(),64);r.revision=state->Revision;
 if(Held(j,r))return true;
 if(r.revision<=j.committed){error="MAGIC_RETENTION_STALE_SAVE";return false;}
 Collect(root,j);
 if(!AddPending(j,r)){error="MAGIC_RETENTION_PENDING_LIMIT_SAVE_NOT_ACKNOWLEDGED";return false;}
 if(!WriteJournal(path,j)){error="MAGIC_RETENTION_JOURNAL_WRITE_FAILED";return false;}return true;
}
// Call only AFTER the canonical state has been atomically committed, never on a failed save.
inline bool Commit(const std::string& root,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& error){
 std::string key,role;if(!StateKey(state,key)||!Role(state,role)){error="MAGIC_RETENTION_INVALID_COMMIT";return false;}
 DWORD attr=GetFileAttributesA(root.c_str());if(attr==(DWORD)-1)return true;
 if(!(attr&FILE_ATTRIBUTE_DIRECTORY)||(attr&FILE_ATTRIBUTE_REPARSE_POINT))return false;
 if(!Pair(root,state,key)){error="MAGIC_RETENTION_COMMITTED_PAIR_INVALID";return false;}
 Guard lock(role);if(!lock.Good())return false;std::string path=root+"\\role-"+role+".retention-v1";Journal j;
 if(!ReadJournal(path,role,j)||state->Revision<j.committed){error="MAGIC_RETENTION_INVALID_JOURNAL_OR_STALE_COMMIT";return false;}
 Journal next=j;next.histories=0;Ref current;memcpy(current.key,key.c_str(),64);current.revision=state->Revision;next.history[next.histories++]=current;
 for(DWORD i=0;i<j.histories&&next.histories<HistoryLimit;++i)if(memcmp(j.history[i].key,current.key,64))next.history[next.histories++]=j.history[i];
 DWORD pendingCount=0;for(i=0;i<next.pendings;++i)if(!Held(next,next.pending[i]))next.pending[pendingCount++]=next.pending[i];next.pendings=pendingCount;
 for(i=0;i<j.histories;++i)if(!Held(next,j.history[i])&&!AddPending(next,j.history[i])){error="MAGIC_RETENTION_PENDING_LIMIT";return false;}
 next.committed=state->Revision;
 // Journal the new committed generation before touching any obsolete pair.
 if(!WriteJournal(path,next)){error="MAGIC_RETENTION_COMMIT_WRITE_FAILED";return false;}
 Collect(root,next);if(!WriteJournal(path,next)){error="MAGIC_RETENTION_CLEANUP_WRITE_FAILED";return false;}return true;
}
inline bool CommitAccount(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& error){
 std::string directory;if(!PtAccountDirectory(store,(const char*)state->AccountName,directory,false))return false;
 return Commit(directory+"\\NativeDropPersistenceV1",state,error);
}
// Normal network retries may reuse a durable, unacknowledged serialization.
// This never authorizes logout/shutdown to skip capturing the latest state.
inline bool IsUnacknowledgedAccount(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state){
 std::string directory,key,role;if(!StateKey(state,key)||!Role(state,role)||!PtAccountDirectory(store,(const char*)state->AccountName,directory,false))return false;
 Guard lock(role);if(!lock.Good())return false;std::string root=directory+"\\NativeDropPersistenceV1";Journal j;
 if(!ReadJournal(root+"\\role-"+role+".retention-v1",role,j))return false;
 for(DWORD i=0;i<j.pendings;++i)if(!memcmp(j.pending[i].key,key.c_str(),64))return Pair(root,state,key);
 return false;
}
inline bool BeforeCommitAccount(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& error){
 std::string directory,key,role;if(!PtAccountDirectory(store,(const char*)state->AccountName,directory,false)||!StateKey(state,key)||!Role(state,role))return false;
 std::string root=directory+"\\NativeDropPersistenceV1",prefix="\\role-"+role;
 bool managed=Regular(root+prefix+".managed-v1")||Regular(root+prefix+".retention-v1")||Regular("NativeDropPersistenceV1"+prefix+".managed-v1");
 if(!managed)return true;
 if(Pair(root,state,key)||Pair("NativeDropPersistenceV1",state,key))return true;
 error="MAGIC_RETENTION_PRECOMMIT_PAIR_INVALID";return false;
}
}
#endif

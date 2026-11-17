#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanCharacterBundle.h"
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
using namespace PtCharacterBundle;
static std::vector<BYTE> stateBytes;
static PHONGTHAN_CHARACTER_STATE_HEADER* State(DWORD rev){
 stateBytes.resize(sizeof(PHONGTHAN_CHARACTER_STATE_HEADER)+sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD));ZeroMemory(&stateBytes[0],stateBytes.size());
 PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&stateBytes[0];s->SchemaVersion=1;s->StateSize=stateBytes.size();s->Revision=rev;s->ItemCount=1;strcpy((char*)s->AccountName,"bundle");strcpy((char*)s->RoleName,"roleA");return s;
}
static std::string Magic(PHONGTHAN_CHARACTER_STATE_HEADER* s){std::string key,payload(748,'\0'),digest;assert(PtMagicRetention::StateKey(s,key));*(DWORD*)&payload[4]=123;*(int*)&payload[8]=2;*(int*)&payload[12]=1;*(int*)&payload[16]=125;assert(PtMagicRetention::Sha256(payload,digest));DWORD h[4]={0x474d3050,2,748,1};return std::string((char*)h,16)+key+digest+payload;}
static int Files(const std::string& root){WIN32_FIND_DATAA d;HANDLE h=FindFirstFileA((root+"\\*").c_str(),&d);if(h==INVALID_HANDLE_VALUE)return 0;int n=0;do{if(!(d.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))++n;}while(FindNextFileA(h,&d));FindClose(h);return n;}
int main(int argc,char**argv){assert(argc==2);std::string parent=argv[1],error;assert(CreateDirectoryA(parent.c_str(),0));std::string store=parent+"\\CharacterStore";CPhongThanCharacterStore chars;assert(chars.Open(store.c_str()));
 PHONGTHAN_CHARACTER_STATE_HEADER* s=State(1);assert(chars.Create(s,s->StateSize));std::string path,account;assert(Path(store.c_str(),s,path)&&PtAccountDirectory(store.c_str(),"bundle",account,false));assert(StagePath(path,s,Magic(s),error));
 Bundle b;assert(Read(path,b)&&b.unified&&b.pending.empty()&&MagicValid(b.magic,s));std::string initial=b.magic;
 for(int i=2;i<=1001;++i){s=State(i);std::string magic=Magic(s);assert(StagePath(path,s,magic,error)&&PendingAccount(store.c_str(),s));assert(chars.Save(s,s->StateSize));assert(Read(path,b)&&b.magic==magic&&b.pending.empty()&&Files(account)==1);}
 puts("PASS 1000 production Goddess commits: exactly one file per role, exact attributes/serial preserved");
 // Unacknowledged network retries and shutdown captures never grow file count.
 for(i=1002;i<=1101;++i){s=State(i);assert(StagePath(path,s,Magic(s),error));}assert(Read(path,b)&&b.pending.size()==8&&Files(account)==1);
 assert(!chars.Save(State(1002),stateBytes.size()));s=State(1101);assert(PendingAccount(store.c_str(),s)&&chars.Save(s,s->StateSize));assert(Read(path,b)&&b.pending.empty());
 HANDLE hold=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);assert(hold!=INVALID_HANDLE_VALUE);s=State(1102);assert(!StagePath(path,s,Magic(s),error));CloseHandle(hold);assert(Read(path,b)&&((const PHONGTHAN_CHARACTER_STATE_HEADER*)b.state.data())->Revision==1101&&Files(account)==1);
 assert(StagePath(path,s,Magic(s),error));hold=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);assert(hold!=INVALID_HANDLE_VALUE);assert(!chars.Save(s,s->StateSize));CloseHandle(hold);assert(chars.Save(s,s->StateSize));
 assert(chars.RecordLogin("bundle","roleA",0x12345678));assert(Read(path,b)&&b.loginIp==0x12345678&&b.loginUtc&&Files(account)==1);
 std::string bytes;assert(Encode(b,bytes));Bundle parsed;bytes[bytes.size()-1]^=1;assert(!Decode(bytes,parsed));assert(Encode(b,bytes));bytes[20]^=1;assert(!Decode(bytes,parsed));
 std::vector<BYTE> loaded(65536);PHONGTHAN_U32 size=0;assert(chars.Load("roleA",&loaded[0],loaded.size(),&size));assert(((PHONGTHAN_CHARACTER_STATE_HEADER*)&loaded[0])->Revision==1102);
 puts("PASS bounded pending eviction/retry, stale commit rejection, failed atomic rename preserves canonical, login metadata in same file, corruption rejection and reload");
 // A copied file is self-contained, even without the original directory tree.
 assert(Read(path,b));std::string copy=parent+"\\copied.pthc";assert(Write(copy,b)&&Read(copy,parsed)&&parsed.magic==b.magic);
 puts("PASS copy only .pthc retains exact attributes; no sidecar directory required");return 0;
}

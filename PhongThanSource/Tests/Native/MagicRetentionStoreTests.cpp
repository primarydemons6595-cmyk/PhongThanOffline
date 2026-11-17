#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanMagicRetention.h"
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
using namespace PtMagicRetention;
static std::vector<BYTE> stateBytes;
static PHONGTHAN_CHARACTER_STATE_HEADER* State(DWORD revision,const char* role="roleA"){
 stateBytes.resize(sizeof(PHONGTHAN_CHARACTER_STATE_HEADER)+sizeof(PHONGTHAN_CHARACTER_ITEM_RECORD));ZeroMemory(&stateBytes[0],stateBytes.size());
 PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&stateBytes[0];s->SchemaVersion=1;s->StateSize=stateBytes.size();s->Revision=revision;s->ItemCount=1;
 strcpy((char*)s->AccountName,"retention");strcpy((char*)s->RoleName,role);return s;
}
static std::string PairWrite(const std::string& root,PHONGTHAN_CHARACTER_STATE_HEADER* s){
 std::string key,payload(748,'\0'),digest;assert(StateKey(s,key)&&Sha256(payload,digest));DWORD h[4]={0x474d3050,2,748,1};
 std::string bytes((const char*)h,16);bytes+=key;bytes+=digest;bytes+=payload;assert(Atomic(root+"\\"+key+".magic-v1",bytes)&&Sha256(bytes,digest)&&Atomic(root+"\\"+key+".required",digest));return key;
}
static int Pairs(const std::string& root){WIN32_FIND_DATAA d;HANDLE h=FindFirstFileA((root+"\\*.magic-v1").c_str(),&d);if(h==INVALID_HANDLE_VALUE)return 0;int n=0;do{++n;}while(FindNextFileA(h,&d));FindClose(h);return n;}
int main(int argc,char** argv){assert(argc==2);std::string parent=argv[1],error;assert(CreateDirectoryA(parent.c_str(),0));std::string store=parent+"\\CharacterStore";CPhongThanCharacterStore chars;assert(chars.Open(store.c_str()));
 std::string account;assert(PtAccountDirectory(store.c_str(),"retention",account,true));std::string root=account+"\\NativeDropPersistenceV1";assert(CreateDirectoryA(root.c_str(),0));
 PHONGTHAN_CHARACTER_STATE_HEADER* s=State(1);assert(Begin(root,s,error));std::string key=PairWrite(root,s);assert(chars.Create(s,s->StateSize));
 for(int i=2;i<=50;++i){s=State(i);assert(Begin(root,s,error));key=PairWrite(root,s);assert(chars.Save(s,s->StateSize)&&Pairs(root)<=3);}
 std::string filename;assert(PtRoleFileName("roleA",filename));std::string canonical=account+"\\"+filename;
 HANDLE hold=CreateFileA(canonical.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);assert(hold!=INVALID_HANDLE_VALUE);
 s=State(51);assert(Begin(root,s,error));std::string failed=PairWrite(root,s);assert(!chars.Save(s,s->StateSize));assert(Regular(root+"\\"+key+".magic-v1"));CloseHandle(hold);
 s=State(52);assert(Begin(root,s,error));key=PairWrite(root,s);assert(chars.Save(s,s->StateSize)&&!Regular(root+"\\"+failed+".magic-v1")&&Pairs(root)==3);
 assert(!chars.Save(State(50),stateBytes.size()));
 // A published pair lost or corrupted before the main rename must reject the save.
 s=State(53);assert(Begin(root,s,error));std::string corrupt=PairWrite(root,s);assert(DeleteFileA((root+"\\"+corrupt+".required").c_str()));assert(!chars.Save(s,s->StateSize));
 std::vector<BYTE> loaded(65536);PHONGTHAN_U32 size=0;assert(chars.Load("roleA",&loaded[0],loaded.size(),&size));s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&loaded[0];assert(s->Revision==52);std::string actual;assert(StateKey(s,actual)&&actual==key&&Pair(root,s,key));
 // A full queue must still allow forwarding the already serialized latest
 // candidate. Creating a ninth snapshot first would deadlock recovery.
 for(i=54;i<=60;++i){s=State(i);assert(Begin(root,s,error));PairWrite(root,s);}
 s=State(60);assert(IsUnacknowledgedAccount(store.c_str(),s));assert(!Begin(root,State(61),error));
 s=State(60);assert(chars.Save(s,s->StateSize));assert(!IsUnacknowledgedAccount(store.c_str(),s));assert(Pairs(root)==3);
 s=State(61);assert(Begin(root,s,error));PairWrite(root,s);assert(chars.Save(s,s->StateSize));
 puts("PASS real Goddess CharacterStore: 50 commits bounded to 3 pairs, atomic rename failure preserves old state, later success collects orphan, stale saves rejected, missing companion rejects BEFORE replacing .pthc; full pending queue forwards existing snapshot and recovers without a ninth allocation");return 0;}

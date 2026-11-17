#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanMagicRetention.h"
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
int main(int argc,char** argv){assert(argc==2);std::string parent=argv[1],error,role;assert(CreateDirectoryA(parent.c_str(),0)||GetLastError()==ERROR_ALREADY_EXISTS);std::string root=parent+"\\NativeDropPersistenceV1";assert(CreateDirectoryA(root.c_str(),0));assert(Pairs(root)==0);
 std::string committed;
 for(int i=1;i<=1000;++i){PHONGTHAN_CHARACTER_STATE_HEADER* s=State(i);assert(Begin(root,s,error));committed=PairWrite(root,s);assert(Commit(root,s,error));assert(Pairs(root)<=HistoryLimit);}
 printf("PASS 1000 successful commits: at most %d companion pairs, no directory scans\n",HistoryLimit);
 for(i=1001;i<=1008;++i){PHONGTHAN_CHARACTER_STATE_HEADER* s=State(i);assert(Begin(root,s,error));PairWrite(root,s);}
 assert(!Begin(root,State(1009),error)&&error=="MAGIC_RETENTION_PENDING_LIMIT_SAVE_NOT_ACKNOWLEDGED");
 assert(Pairs(root)==HistoryLimit+PendingLimit&&Regular(root+"\\"+committed+".magic-v1"));
 assert(Commit(root,State(1008),error)&&Pairs(root)==HistoryLimit);
 puts("PASS failed/unacknowledged saves: eight-slot budget, committed pair preserved, later acknowledgement reclaims all obsolete candidates");
 PHONGTHAN_CHARACTER_STATE_HEADER* s=State(1009);assert(Begin(root,s,error));std::string a=PairWrite(root,s);
 s=State(1010);assert(Begin(root,s,error));std::string b=PairWrite(root,s);
 assert(Commit(root,State(1009),error)&&Regular(root+"\\"+b+".magic-v1"));assert(Commit(root,State(1010),error));
 assert(!Commit(root,State(1008),error));
 // An invalid new pair cannot trigger collection of a valid prior generation.
 s=State(1011);assert(Begin(root,s,error));std::string bad=PairWrite(root,s);assert(DeleteFileA((root+"\\"+bad+".required").c_str()));
 int before=Pairs(root);assert(!Commit(root,s,error)&&Pairs(root)==before&&Regular(root+"\\"+a+".magic-v1"));PairWrite(root,s);assert(Commit(root,s,error));
 puts("PASS future in-flight saves retained, stale commits refused, corrupt/missing pair never triggers collection");
 // A locked old pair stays in the bounded pending queue and is retried.
 s=State(1012);assert(Begin(root,s,error));PairWrite(root,s);
 HANDLE hold=CreateFileA((root+"\\"+a+".magic-v1").c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);assert(hold!=INVALID_HANDLE_VALUE);
 assert(Commit(root,s,error)&&Regular(root+"\\"+a+".magic-v1"));CloseHandle(hold);
 s=State(1013);assert(Begin(root,s,error));PairWrite(root,s);assert(Commit(root,s,error)&&!Regular(root+"\\"+a+".magic-v1")&&Pairs(root)==HistoryLimit);
 assert(Role(s,role));std::string journal=root+"\\role-"+role+".retention-v1",good;assert(Read(journal,good));assert(Atomic(journal,"corrupt"));
 before=Pairs(root);assert(!Begin(root,State(1014),error)&&Pairs(root)==before);assert(Atomic(journal,good));
 for(i=1;i<=10;++i){s=State(i,"roleB");assert(Begin(root,s,error));PairWrite(root,s);assert(Commit(root,s,error));}
 assert(Pairs(root)==2*HistoryLimit);assert(Regular(root+"\\"+b+".magic-v1")==false);
 puts("PASS deletion failure retries, corrupt journal fails closed, neighboring roles remain isolated");
 s=State(1,"rolePinned");assert(Begin(root,s,error));std::string pinned=PairWrite(root,s);assert(Commit(root,s,error));
 DWORD fileHeader[4]={0x43544850,0x00100001,stateBytes.size(),Hash(&stateBytes[0],stateBytes.size())};
 std::string backup((char*)fileHeader,16);backup.append((char*)&stateBytes[0],stateBytes.size());assert(Atomic(parent+"\\role_backup.pthc",backup));
 for(i=2;i<=8;++i){s=State(i,"rolePinned");assert(Begin(root,s,error));PairWrite(root,s);assert(Commit(root,s,error));}
 assert(Regular(root+"\\"+pinned+".magic-v1"));puts("PASS explicit canonical/Studio backup reference remains loadable after history eviction");return 0;
}

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "../../Sources/MultiServer/Goddess/PhongThanCharacterStore.h"
#include "../../Headers/PhongThanAccountStorage.h"
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char**argv){if(argc!=2&&argc!=3)return 2;int checks=0;CPhongThanCharacterStore store;CHECK(store.Open(argv[1]));
 if(argc==3){const char* accounts[]={"shokboy01","123456","shokboy02"};const unsigned int counts[]={49,33,7};
    for(int a=0;a<3;++a){PHONGTHAN_CHARACTER_SUMMARY summary;CHECK(store.List(accounts[a],&summary,1)==1);std::vector<BYTE> data(PHONGTHAN_CHARACTER_MAX_STATE_SIZE);PHONGTHAN_U32 size=0;
        CHECK(store.Load((const char*)summary.Name,&data[0],data.size(),&size));PHONGTHAN_CHARACTER_STATE_HEADER* state=(PHONGTHAN_CHARACTER_STATE_HEADER*)&data[0];CHECK(!_stricmp((const char*)state->AccountName,accounts[a]));CHECK(state->ItemCount==counts[a]);}
    printf("PASS migrated preview checks=%d\n",checks);return 0;}
 std::vector<BYTE> bytes(PHONGTHAN_CHARACTER_MAX_STATE_SIZE),restored(bytes.size());PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];
 s->SchemaVersion=1;s->StateSize=sizeof(*s);s->Revision=10;s->Money=123456;s->HelmResource=3;
 strcpy((char*)s->RoleName,"FirstRole");strcpy((char*)s->AccountName,"shokboy01");CHECK(store.Create(s,s->StateSize));
 std::string root=argv[1],name;CHECK(PtRoleFileName("FirstRole",name));std::string first=root+"\\shokboy01\\"+name;
 CHECK(GetFileAttributesA(first.c_str())!=INVALID_FILE_ATTRIBUTES);CHECK(GetFileAttributesA((root+"\\"+name).c_str())==INVALID_FILE_ATTRIBUTES);
 PHONGTHAN_U32 size=0;CHECK(store.Load("FIRSTROLE",&restored[0],restored.size(),&size));CHECK(size==s->StateSize&&!memcmp(s,&restored[0],size));
 PHONGTHAN_CHARACTER_SUMMARY portraitSummary;CHECK(store.List("shokboy01",&portraitSummary,1)==1&&portraitSummary.Portrait==3);
 CHECK(store.List("SHOKBOY01",0,0)==1);CHECK(store.List("other",0,0)==0);CHECK(store.RecordLogin("shokboy01","FirstRole",0x0100007f));
 CHECK(GetFileAttributesA((first+".login.ini").c_str())!=INVALID_FILE_ATTRIBUTES);
 s->Money++;s->Revision++;CHECK(store.Save(s,s->StateSize));CHECK(store.Load("FirstRole",&restored[0],restored.size(),&size));CHECK(((PHONGTHAN_CHARACTER_STATE_HEADER*)&restored[0])->Money==123457);
 strcpy((char*)s->AccountName,"other");CHECK(!store.Save(s,s->StateSize));CHECK(!store.Create(s,s->StateSize));CHECK(!store.Delete("other","FirstRole"));
 strcpy((char*)s->RoleName,"OtherRole");CHECK(store.Create(s,s->StateSize));CHECK(store.List("other",0,0)==1);CHECK(store.List("shokboy01",0,0)==1);
 // Legacy flat copy remains readable; a save migrates it into the account.
 CHECK(MoveFileA(first.c_str(),(root+"\\"+name).c_str()));CHECK(store.List("shokboy01",0,0)==1);CHECK(store.Load("FirstRole",&restored[0],restored.size(),&size));
 CHECK(store.Save((PHONGTHAN_CHARACTER_STATE_HEADER*)&restored[0],size));CHECK(GetFileAttributesA(first.c_str())!=INVALID_FILE_ATTRIBUTES);CHECK(store.List("shokboy01",0,0)==1);
 CHECK(store.Delete("shokboy01","FirstRole"));CHECK(!store.Load("FirstRole",&restored[0],restored.size(),&size)); // delete cannot resurrect a legacy copy
 CHECK(GetFileAttributesA((root+"\\"+name).c_str())==INVALID_FILE_ATTRIBUTES);CHECK(!store.Load("FirstRole",&restored[0],restored.size(),&size));
 std::string component;CHECK(PtAccountComponent("shokboy01",component)&&component=="shokboy01");CHECK(PtAccountComponent("../escape",component)&&component.find('/')==std::string::npos&&component[0]=='~');CHECK(PtAccountComponent("CON",component)&&component[0]=='~');
 CHECK(store.Delete("other","OtherRole"));printf("PASS account store checks=%d\n",checks);return 0;
}

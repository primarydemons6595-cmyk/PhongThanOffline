#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "P0ItemMagicPersistence.h"
#include "PhongThanConsumableData.h"
KPakList* g_pPakList=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d: %s error=%s\n",__LINE__,#x,error.c_str());return 1;}}while(0)
int main(int argc,char**argv){if(argc!=4||!SetCurrentDirectoryA(argv[1]))return 2;int checks=0;std::string error,raw,comp;
 SetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",0);
 CHECK(P0ItemMagic::ReadFile(argv[2],raw));CHECK(P0ItemMagic::ReadFile(argv[3],comp));
 PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&raw[16];
 CHECK(PhongThanValidateCharacterState(s,raw.size()-16));std::vector<P0ItemMagic::Entry> entries(s->ItemCount);
 memcpy(&entries[0],comp.data()+144,entries.size()*sizeof(entries[0]));
 ++s->Revision;std::string accountRoot;CHECK(P0ItemMagic::AccountRoot(s,accountRoot,error,true)==1);
 CHECK(accountRoot.find("CharacterStore\\")!=std::string::npos);printf("account_root=%s\n",accountRoot.c_str());CHECK(P0ItemMagic::Save(s,entries,error));
 std::string key;CHECK(P0ItemMagic::StateHash(s,key,error));CHECK(GetFileAttributesA((accountRoot+"\\"+key+".magic-v1").c_str())!=(DWORD)-1);
 CHECK(GetFileAttributesA((std::string("NativeDropPersistenceV1\\")+key+".magic-v1").c_str())==(DWORD)-1);
 std::vector<P0ItemMagic::Entry> read;bool present=false;CHECK(P0ItemMagic::Load(s,read,present,error)&&present);CHECK(read.size()==entries.size()&&!memcmp(&read[0],&entries[0],entries.size()*sizeof(entries[0])));
 std::string required=accountRoot+"\\"+key+".required",old;CHECK(P0ItemMagic::ReadFile(required,old));CHECK(DeleteFileA(required.c_str()));CHECK(!P0ItemMagic::Load(s,read,present,error));CHECK(P0ItemMagic::ImmutableWrite(required,old,error));
 // Existing flat snapshots remain readable until the offline migration moves them.
 std::string originalKey(comp.data()+16,64);--s->Revision;
 CHECK(P0ItemMagic::ImmutableWrite("NativeDropPersistenceV1\\"+originalKey+".magic-v1",comp,error));std::string digest;CHECK(BlueMagic::Sha256(comp,digest));CHECK(P0ItemMagic::ImmutableWrite("NativeDropPersistenceV1\\"+originalKey+".required",digest,error));
 CHECK(P0ItemMagic::Load(s,read,present,error)&&present);CHECK(!memcmp(&read[0],&entries[0],entries.size()*sizeof(entries[0])));
 printf("PASS account magic checks=%d entries=%u\n",checks,s->ItemCount);return 0;
}

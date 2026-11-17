// Read-only audit of Python-seeded journals using the production native reader.
#include "../../Headers/PhongThanMagicRetention.h"
#include <assert.h>
int main(int argc,char** argv){
 if(argc!=2)return 2;FILE* f=fopen(argv[1],"rb");if(!f)return 3;char line[1200];int count=0;
 while(fgets(line,sizeof(line),f)){
  char* root=strchr(line,'\t');assert(root);*root++=0;char* role=strchr(root,'\t');assert(role);*role++=0;role[strcspn(role,"\r\n")]=0;
  std::string bytes,error,key;assert(PtMagicRetention::Read(line,bytes,65552)&&bytes.size()>=424);
  const DWORD* h=(const DWORD*)bytes.c_str();assert(h[0]==0x43544850&&h[2]==bytes.size()-16&&h[3]==PtMagicRetention::Hash(bytes.data()+16,bytes.size()-16));
  const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)(bytes.data()+16);
  assert(PtMagicRetention::StateKey(s,key)&&PtMagicRetention::Pair(root,s,key));
  PtMagicRetention::Journal journal;assert(PtMagicRetention::ReadJournal(std::string(root)+"\\role-"+role+".retention-v1",role,journal));
  PtMagicRetention::Ref ref;memcpy(ref.key,key.c_str(),64);ref.revision=s->Revision;assert(PtMagicRetention::Held(journal,ref));++count;
 }
 fclose(f);printf("PASS native read-only audit: %d canonical states retain exact bound item attributes and valid bounded journals\n",count);return count?0:4;
}

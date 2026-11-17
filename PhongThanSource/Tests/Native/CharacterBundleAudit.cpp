#include "../../Headers/PhongThanCharacterBundle.h"
#include <assert.h>
int main(int argc,char**argv){if(argc!=2&&argc!=3)return 2;if(argc==3&&!SetCurrentDirectoryA(argv[2]))return 5;FILE* f=fopen(argv[1],"rb");if(!f)return 3;char line[1200];int count=0,items=0;
 while(fgets(line,sizeof(line),f)){line[strcspn(line,"\r\n")]=0;if(!line[0])continue;PtCharacterBundle::Bundle b;assert(PtCharacterBundle::Read(line,b)&&b.unified&&b.pending.empty());const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)b.state.data();if(!b.magic.empty()){assert(PtCharacterBundle::MagicValid(b.magic,s));items+=s->ItemCount;}if(argc==3){std::string path;assert(PtCharacterBundle::Path("CharacterStore",s,path));char expected[MAX_PATH],actual[MAX_PATH];assert(GetFullPathNameA(line,MAX_PATH,expected,0)&&GetFullPathNameA(path.c_str(),MAX_PATH,actual,0)&&!_stricmp(expected,actual));}++count;}
 fclose(f);printf("PASS production native PTHC v2 audit: files=%d exact-item-records=%d\n",count,items);return count?0:4;
}

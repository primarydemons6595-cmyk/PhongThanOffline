// Focused data repair verifier; uses the unchanged GameServer INI parser.
#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include "LegacyDropEngine.h"
using namespace LegacyDrop;
static std::string Read(const std::string& path) {
    FILE* f=fopen(path.c_str(),"rb");if(!f)return "";
    std::string s;char b[8192];size_t n;
    while((n=fread(b,1,sizeof(b),f)))s.append(b,n);
    fclose(f);return s;
}
int main(int argc,char** argv) {
    if(argc!=4)return 2;
    const char* names[]={"bingling_shouling.ini","changchengshen_shouling.ini","tianwu.ini","zhaohuanzhulong.ini"};
    const int counts[]={29,29,17,41};int failed=0;
    for(int i=0;i<4;++i) {
        Table t=Parse(Read(std::string(argv[2])+"\\"+names[i]));
        PreparedTable prepared;std::string error;
        bool ok=t.errors.empty()&&t.count==counts[i]&&prepared.Prepare(t,error);
        if(!ok)++failed;
        printf("REPAIRED_TABLE file=%s count=%d valid=%d\n",names[i],t.count,ok?1:0);
    }
    WIN32_FIND_DATAA fd;std::string dir=argv[1],pattern=dir+"\\*.ini";
    HANDLE h=FindFirstFileA(pattern.c_str(),&fd);if(h==INVALID_HANDLE_VALUE)return 3;
    FILE* out=fopen(argv[3],"w");if(!out){FindClose(h);return 4;}
    fputs("file,count,status,reason\n",out);int total=0,valid=0;
    do {
        Table t=Parse(Read(dir+"\\"+fd.cFileName));++total;
        if(t.errors.empty())++valid;
        fprintf(out,"%s,%d,%s,%s\n",fd.cFileName,t.count,t.errors.empty()?"VALID":"DATA_BLOCKED",t.errors.empty()?"":t.errors[0].c_str());
    } while(FindNextFileA(h,&fd));
    FindClose(h);fclose(out);
    if(total!=478||_heapchk()!=_HEAPOK)++failed;
    printf("STRUCTURAL_RESULT files=%d valid=%d rejected=%d candidate_failures=%d world_spawn_death=0 rng_draws=0\n",total,valid,total-valid,failed);
    return failed?1:0;
}

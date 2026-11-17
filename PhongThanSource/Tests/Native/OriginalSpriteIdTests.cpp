#include "KWin32.h"
#include "KPakList.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char** argv){
    if(argc!=2)return 2;
    FILE* file=fopen(argv[1],"r");if(!file)return 3;
    KPakList native;KPakList* object=&native;
    FARPROC hash=GetProcAddress(GetModuleHandleA("Engine.dll"),"?FileNameToId@KPakList@@AAEKPBD@Z");
    if(!hash)return 4;
    unsigned long expected;char hex[4096],raw[2048];int checked=0,failed=0;
    while(fscanf(file,"%lx %4095s",&expected,hex)==2){
        size_t length=strlen(hex);if(length%2||length/2>=sizeof(raw)){++failed;continue;}
        bool valid=true;
        for(size_t i=0;i<length/2;++i){unsigned value;char pair[3]={hex[2*i],hex[2*i+1],0};if(sscanf(pair,"%x",&value)!=1){valid=false;break;}raw[i]=(char)value;}
        raw[length/2]=0;
        unsigned long actual;const char* path=raw;
        // Invoke the existing exported x86 thiscall without changing its access
        // declaration, source algorithm, runtime DLL or decorated symbol.
        __asm { mov ecx, object
                push path
                call hash
                mov actual, eax }
        ++checked;
        if(!valid||actual!=expected){++failed;printf("MISMATCH expected=%08lX actual=%08lX\n",expected,actual);}
    }
    fclose(file);printf("NATIVE_FILENAME_ID checked=%d failed=%d\n",checked,failed);return failed?1:0;
}
